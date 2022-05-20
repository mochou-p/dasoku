// dsk

#include "renderer.hpp"

#include "imgui_impl_vulkan.h"

#include <stdexcept>

namespace dsk 
{
    DskRenderer::DskRenderer
    (
        DskWindow &window,
        DskDevice &device
    ): dskWindow{window}, dskDevice{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    DskRenderer::~DskRenderer()
    {
        freeCommandBuffers();
    }

    void DskRenderer::recreateSwapChain()
    {
        auto extent = dskWindow.getExtent();

        while (extent.width == 0 || extent.height == 0)
        {
            extent = dskWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(dskDevice.device());

        if (dskSwapChain == nullptr)
        {
            dskSwapChain = std::make_unique<DskSwapChain>(dskDevice, extent);
        }
        else
        {
            std::shared_ptr<DskSwapChain> oldSwapChain = std::move(dskSwapChain);
            dskSwapChain = std::make_unique<DskSwapChain>(dskDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*dskSwapChain.get()))
            {
                throw std::runtime_error("swap chain image (or depth) format has changed");
            }
        }
    }

    void DskRenderer::createCommandBuffers()
    {
        commandBuffers.resize(DskSwapChain::MAX_FRAMES_IN_FLIGHT);

        // simplify
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = dskDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if
        (
            vkAllocateCommandBuffers
            (
                dskDevice.device(),
                &allocInfo,
                commandBuffers.data()
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void DskRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers
        (
            dskDevice.device(),
            dskDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data()
        );
        commandBuffers.clear();
    }

    VkCommandBuffer DskRenderer::beginFrame()
    {
        assert(!isFrameStarted && "can't call beginFrame while already in progress");

        auto result = dskSwapChain->acquireNextImage(&currentImageIndex);

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

        // simplify
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

    void DskRenderer::endFrame()
    {
        assert(isFrameStarted && "can't call endFrame while frame is not in progress");
        
        auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = dskSwapChain->submitCommandBuffers
        (
            &commandBuffer,
            &currentImageIndex
        );

        if
        (
            result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR ||
            dskWindow.wasWindowResized()
        )
        {
            dskWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % DskSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void DskRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't begin render pass on command buffer from a different frame");

        // simplify
        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = dskSwapChain->getRenderPass();
        renderPassInfo.framebuffer = dskSwapChain->getFrameBuffer(currentImageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = dskSwapChain->getSwapChainExtent();

        // simplify
        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color = {0.003f, 0.003f, 0.003f};  // background
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass
        (
            commandBuffer,
            &renderPassInfo,
            VK_SUBPASS_CONTENTS_INLINE
        );

        // simplify
        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(dskSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(dskSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{{0, 0}, dskSwapChain->getSwapChainExtent()};

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void DskRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }
}
