// ashi

#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace ashi 
{
    struct AshiPushConstantData
    {
        glm::mat2 transform { 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    AshiRenderSystem::AshiRenderSystem
    (
        AshiDevice &device,
        VkRenderPass renderPass
    ): ashiDevice{device}
    {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    AshiRenderSystem::~AshiRenderSystem()
    {
        vkDestroyPipelineLayout
        (
            ashiDevice.device(),
            pipelineLayout,
            nullptr
        );
    }

    void AshiRenderSystem::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(AshiPushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if
        (
            vkCreatePipelineLayout
            (
                ashiDevice.device(),
                &pipelineLayoutInfo,
                nullptr,
                &pipelineLayout
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void AshiRenderSystem::createPipeline(VkRenderPass renderPass)
    {
        PipelineConfigInfo pipelineConfig {};
        AshiPipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        ashiPipeline = std::make_unique<AshiPipeline>
        (
            ashiDevice,
            "./shaders/simple_shader.vert.spv",
            "./shaders/simple_shader.frag.spv",
            pipelineConfig
        );
    }

    void AshiRenderSystem::renderGameObjects
    (
        VkCommandBuffer commandBuffer,
        std::vector<AshiGameObject> &gameObjects
    )
    {
        ashiPipeline->bind(commandBuffer);

        for (auto &obj : gameObjects)
        {
            // i wanted to add a cringe joke here but noone would notice
            obj.transform2d.rotation = glm::mod
            (
                obj.transform2d.rotation + 0.01f,
                glm::two_pi<float>()
            );

            AshiPushConstantData push {};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants
            (
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(AshiPushConstantData),
                &push
            );
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}
