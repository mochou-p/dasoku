// hebi

#include "renderer.hpp"

#include <stdexcept>

namespace hebi 
{
    HebiRenderer::HebiRenderer
    (
        HebiWindow &window,
        HebiDevice &device
    ): hebiWindow{window}, hebiDevice{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    HebiRenderer::~HebiRenderer()
    {
        freeCommandBuffers();
    }

    void HebiRenderer::recreateSwapChain()
    {
        auto extent = hebiWindow.getExtent();

        while (extent.width == 0 || extent.height == 0)
        {
            extent = hebiWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(hebiDevice.device());

        if (hebiSwapChain == nullptr)
        {
            hebiSwapChain = std::make_unique<HebiSwapChain>(hebiDevice, extent);
        }
        else
        {
            std::shared_ptr<HebiSwapChain> oldSwapChain = std::move(hebiSwapChain);
            hebiSwapChain = std::make_unique<HebiSwapChain>(hebiDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*hebiSwapChain.get()))
            {
                throw std::runtime_error("swap chain image (or depth) format has changed");
            }
        }
    }

    void HebiRenderer::createCommandBuffers()
    {
        commandBuffers.resize(HebiSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = hebiDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if
        (
            vkAllocateCommandBuffers
            (
                hebiDevice.device(),
                &allocInfo,
                commandBuffers.data()
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void HebiRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers
        (
            hebiDevice.device(),
            hebiDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data()
        );
        commandBuffers.clear();
    }

    VkCommandBuffer HebiRenderer::beginFrame()
    {
        assert(!isFrameStarted && "can't call beginFrame while already in progress");

        auto result = hebiSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if
        (
            vkBeginCommandBuffer
            (
                commandBuffer,
                &beginInfo
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        return commandBuffer;
    }

    void HebiRenderer::endFrame()
    {
        assert(isFrameStarted && "can't call endFrame while frame is not in progress");
        
        auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = hebiSwapChain->submitCommandBuffers
        (
            &commandBuffer,
            &currentImageIndex
        );

        if
        (
            result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR ||
            hebiWindow.wasWindowResized()
        )
        {
            hebiWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % HebiSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void HebiRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = hebiSwapChain->getRenderPass();
        renderPassInfo.framebuffer = hebiSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = hebiSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f }; // background
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass
        (
            commandBuffer,
            &renderPassInfo,
            VK_SUBPASS_CONTENTS_INLINE
        );

        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(hebiSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(hebiSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, hebiSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void HebiRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }
}
