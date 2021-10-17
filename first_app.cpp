// zzz

#include "first_app.hpp"

#include <stdexcept>

namespace zzz 
{
    FirstApp::FirstApp()
    {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout
        (
            zzzDevice.device(),
            pipelineLayout,
            nullptr
        );
    }
    void FirstApp::run()
    {
        while (!zzzWindow.shouldClose())
        {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(zzzDevice.device());
    }

    void FirstApp::loadModels()
    {
        std::vector<ZzzModel::Vertex> vertices
        {
            {{  0.0f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
            {{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f }}
        };

        zzzModel = std::make_unique<ZzzModel>(zzzDevice, vertices);
    }

    void FirstApp::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if
        (
            vkCreatePipelineLayout
            (
                zzzDevice.device(),
                &pipelineLayoutInfo,
                nullptr,
                &pipelineLayout
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void FirstApp::createPipeline()
    {
        PipelineConfigInfo pipelineConfig{};
        ZzzPipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = zzzSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        zzzPipeline = std::make_unique<ZzzPipeline>
        (
            zzzDevice,
            "shaders/test.vert.spv",
            "shaders/test.frag.spv",
            pipelineConfig
        );
    }

    void FirstApp::recreateSwapChain()
    {
        auto extent = zzzWindow.getExtent();

        while (extent.width == 0 || extent.height == 0)
        {
            extent = zzzWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(zzzDevice.device());
        zzzSwapChain = std::make_unique<ZzzSwapChain>(zzzDevice, extent);
        createPipeline();
    }

    void FirstApp::createCommandBuffers()
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
        //
    }

    void FirstApp::recordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if
            (
                vkBeginCommandBuffer
                (
                    commandBuffers[imageIndex],
                    &beginInfo
                ) != VK_SUCCESS
            )
            {
                throw std::runtime_error("failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = zzzSwapChain->getRenderPass();
            renderPassInfo.framebuffer = zzzSwapChain->getFrameBuffer(imageIndex);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = zzzSwapChain->getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f }; // background
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass
            (
                commandBuffers[imageIndex],
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
            vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

            zzzPipeline->bind(commandBuffers[imageIndex]);
            zzzModel->bind(commandBuffers[imageIndex]);
            zzzModel->draw(commandBuffers[imageIndex]);

            vkCmdEndRenderPass(commandBuffers[imageIndex]);
            if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer");
            }
    }

    void FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        auto result = zzzSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        recordCommandBuffer(imageIndex);

        result = zzzSwapChain->submitCommandBuffers
        (
            &commandBuffers[imageIndex],
            &imageIndex
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
            return;
        }

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}
