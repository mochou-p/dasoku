// zzz

#include "first_app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace zzz 
{
    struct SimplePushConstantData
    {
        glm::mat2 transform{ 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp()
    {
        loadGameObjects();
        createPipelineLayout();
        createPipeline();
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
            
            if (auto commandBuffer = zzzRenderer.beginFrame())
            {
                zzzRenderer.beginSwapChainRenderPass(commandBuffer);
                renderGameObjects(commandBuffer);
                zzzRenderer.endSwapChainRenderPass(commandBuffer);
                zzzRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(zzzDevice.device());
    }

    void FirstApp::loadGameObjects()
    {
        std::vector<ZzzModel::Vertex> vertices
        {
            {{  0.0f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
            {{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f }}
        };

        auto zzzModel = std::make_shared<ZzzModel>(zzzDevice, vertices);

        auto triangle = ZzzGameObject::createGameObject();
        triangle.model = zzzModel;
        triangle.color = { 0.8f, 0.3f, 0.0f };
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = { 2.0f, 0.5f };
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void FirstApp::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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

        pipelineConfig.renderPass = zzzRenderer.getSwapChainRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        zzzPipeline = std::make_unique<ZzzPipeline>
        (
            zzzDevice,
            "shaders/test.vert.spv",
            "shaders/test.frag.spv",
            pipelineConfig
        );
    }

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer)
    {
        zzzPipeline->bind(commandBuffer);

        for (auto& obj : gameObjects)
        {
            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants
            (
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}
