// zzz

#include "first_app.hpp"

#include <stdexcept>

namespace zzz 
{
    FirstApp::FirstApp()
    {
        loadModels();
        createPipelineLayout();
        createPipeline();
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
            {{  0.0f, -0.5f }},
            {{  0.5f,  0.5f }},
            {{ -0.5f,  0.5f }}
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
        auto pipelineConfig = ZzzPipeline::defaultPipelineConfigInfo
        (
            zzzSwapChain.width(),
            zzzSwapChain.height()
        );
        pipelineConfig.renderPass = zzzSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        zzzPipeline = std::make_unique<ZzzPipeline>
        (
            zzzDevice,
            "shaders/color.vert.spv",
            "shaders/color.frag.spv",
            pipelineConfig
        );
    }

    void FirstApp::createCommandBuffers()
    {
        commandBuffers.resize(zzzSwapChain.imageCount());

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

        for (int i = 0; i < commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if
            (
                vkBeginCommandBuffer
                (
                    commandBuffers[i],
                    &beginInfo
                ) != VK_SUCCESS
            )
            {
                throw std::runtime_error("failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = zzzSwapChain.getRenderPass();
            renderPassInfo.framebuffer = zzzSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = zzzSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f }; // background
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass
            (
                commandBuffers[i],
                &renderPassInfo,
                VK_SUBPASS_CONTENTS_INLINE
            );

            zzzPipeline->bind(commandBuffers[i]);
            zzzModel->bind(commandBuffers[i]);
            zzzModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer");
            }
        }
    }

    void FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        auto result = zzzSwapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        result = zzzSwapChain.submitCommandBuffers
        (
            &commandBuffers[imageIndex],
            &imageIndex
        );

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}
