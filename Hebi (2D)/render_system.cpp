// hebi

#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace hebi 
{
    struct HebiPushConstantData
    {
        glm::mat2 transform { 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    HebiRenderSystem::HebiRenderSystem
    (
        HebiDevice &device,
        VkRenderPass renderPass
    ): hebiDevice{device}
    {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    HebiRenderSystem::~HebiRenderSystem()
    {
        vkDestroyPipelineLayout
        (
            hebiDevice.device(),
            pipelineLayout,
            nullptr
        );
    }

    void HebiRenderSystem::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(HebiPushConstantData);

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
                hebiDevice.device(),
                &pipelineLayoutInfo,
                nullptr,
                &pipelineLayout
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void HebiRenderSystem::createPipeline(VkRenderPass renderPass)
    {
        PipelineConfigInfo pipelineConfig {};
        HebiPipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        hebiPipeline = std::make_unique<HebiPipeline>
        (
            hebiDevice,
            "./shaders/2D/simple_shader.vert.spv",
            "./shaders/2D/simple_shader.frag.spv",
            pipelineConfig
        );
    }

    void HebiRenderSystem::renderGameObjects
    (
        VkCommandBuffer commandBuffer,
        std::vector<HebiGameObject> &gameObjects
    )
    {
        hebiPipeline->bind(commandBuffer);

        for (auto &obj : gameObjects)
        {
            // i wanted to add a cringe joke here but noone would notice
            obj.transform2d.rotation = glm::mod
            (
                obj.transform2d.rotation + 0.01f,
                glm::two_pi<float>()
            );

            HebiPushConstantData push {};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants
            (
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(HebiPushConstantData),
                &push
            );
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}
