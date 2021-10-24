// ashi

#include "renderer.hpp"

#include <stdexcept>

namespace ashi 
{
    AshiRenderer::AshiRenderer
    (
        AshiWindow &window,
        AshiDevice &device
    ): ashiWindow{window}, ashiDevice{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    AshiRenderer::~AshiRenderer()
    {
        freeCommandBuffers();
    }

    void AshiRenderer::recreateSwapChain()
    {
        auto extent = ashiWindow.getExtent();

        while (extent.width == 0 || extent.height == 0)
        {
            extent = ashiWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(ashiDevice.device());

        if (ashiSwapChain == nullptr)
        {
            ashiSwapChain = std::make_unique<AshiSwapChain>(ashiDevice, extent);
        }
        else
        {
            std::shared_ptr<AshiSwapChain> oldSwapChain = std::move(ashiSwapChain);
            ashiSwapChain = std::make_unique<AshiSwapChain>(ashiDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*ashiSwapChain.get()))
            {
                throw std::runtime_error("swap chain image (or depth) format has changed");
            }
        }
    }

    void AshiRenderer::createCommandBuffers()
    {
        commandBuffers.resize(AshiSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = ashiDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if
        (
            vkAllocateCommandBuffers
            (
                ashiDevice.device(),
                &allocInfo,
                commandBuffers.data()
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void AshiRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers
        (
            ashiDevice.device(),
            ashiDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data()
        );
        commandBuffers.clear();
    }

    VkCommandBuffer AshiRenderer::beginFrame()
    {
        assert(!isFrameStarted && "can't call beginFrame while already in progress");

        auto result = ashiSwapChain->acquireNextImage(&currentImageIndex);

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

    void AshiRenderer::endFrame()
    {
        assert(isFrameStarted && "can't call endFrame while frame is not in progress");
        
        auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = ashiSwapChain->submitCommandBuffers
        (
            &commandBuffer,
            &currentImageIndex
        );

        if
        (
            result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR ||
            ashiWindow.wasWindowResized()
        )
        {
            ashiWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % AshiSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void AshiRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = ashiSwapChain->getRenderPass();
        renderPassInfo.framebuffer = ashiSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = ashiSwapChain->getSwapChainExtent();

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
        viewport.width = static_cast<float>(ashiSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(ashiSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, ashiSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void AshiRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }
}
