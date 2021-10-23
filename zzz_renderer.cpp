// zzz

#include "zzz_renderer.hpp"

#include <stdexcept>

namespace zzz 
{
    ZzzRenderer::ZzzRenderer
    (
        ZzzWindow &window,
        ZzzDevice &device
    ): zzzWindow{window}, zzzDevice{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    ZzzRenderer::~ZzzRenderer()
    {
        freeCommandBuffers();
    }

    void ZzzRenderer::recreateSwapChain()
    {
        auto extent = zzzWindow.getExtent();

        while (extent.width == 0 || extent.height == 0)
        {
            extent = zzzWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(zzzDevice.device());

        if (zzzSwapChain == nullptr)
        {
            zzzSwapChain = std::make_unique<ZzzSwapChain>(zzzDevice, extent);
        }
        else
        {
            zzzSwapChain = std::make_unique<ZzzSwapChain>(zzzDevice, extent, std::move(zzzSwapChain));

            if (zzzSwapChain->imageCount() != commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
    }

    void ZzzRenderer::createCommandBuffers()
    {
        commandBuffers.resize(zzzSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = zzzDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if
        (
            vkAllocateCommandBuffers
            (
                zzzDevice.device(),
                &allocInfo,
                commandBuffers.data()
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void ZzzRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers
        (
            zzzDevice.device(),
            zzzDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data()
        );
        commandBuffers.clear();
    }

    VkCommandBuffer ZzzRenderer::beginFrame()
    {
        assert(!isFrameStarted && "can't call beginFrame while already in progress");

        auto result = zzzSwapChain->acquireNextImage(&currentImageIndex);

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

        VkCommandBufferBeginInfo beginInfo{};
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

    void ZzzRenderer::endFrame()
    {
        assert(isFrameStarted && "can't call endFrame while frame is not in progress");
        
        auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = zzzSwapChain->submitCommandBuffers
        (
            &commandBuffer,
            &currentImageIndex
        );

        if
        (
            result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR ||
            zzzWindow.wasWindowResized()
        )
        {
            zzzWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }

        isFrameStarted = false;
    }

    void ZzzRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = zzzSwapChain->getRenderPass();
        renderPassInfo.framebuffer = zzzSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = zzzSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
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

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(zzzSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(zzzSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, zzzSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void ZzzRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }
}
