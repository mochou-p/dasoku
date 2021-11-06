// dsk

#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace dsk 
{
    struct DskPushConstantData
    {
        glm::mat4 modelMatrix {1.0f};
        glm::mat4 normalMatrix {1.0f};
        int textureIndex;
    };

    DskRenderSystem::DskRenderSystem
    (
        DskDevice &device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout,
        VkDescriptorSetLayout textureSetLayout
    ): dskDevice{device}
    {
        createPipelineLayout(globalSetLayout, textureSetLayout);
        createPipeline(renderPass);
    }

    DskRenderSystem::~DskRenderSystem()
    {
        vkDestroyPipelineLayout
        (
            dskDevice.device(),
            pipelineLayout,
            nullptr
        );
    }

    void DskRenderSystem::createPipelineLayout
    (
        VkDescriptorSetLayout globalSetLayout,
        VkDescriptorSetLayout textureSetLayout
    )
    {
        VkPushConstantRange pushConstantRange {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(DskPushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayout {globalSetLayout, textureSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if
        (
            vkCreatePipelineLayout
            (
                dskDevice.device(),
                &pipelineLayoutInfo,
                nullptr,
                &pipelineLayout
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void DskRenderSystem::createPipeline(VkRenderPass renderPass)
    {
        PipelineConfigInfo pipelineConfig {};
        DskPipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        dskPipeline = std::make_unique<DskPipeline>
        (
            dskDevice,
            ".\\resources\\shaders\\diffuse_color.vert.spv",
            ".\\resources\\shaders\\diffuse_color.frag.spv",
            pipelineConfig
        );
    }

    void DskRenderSystem::renderGameObjects
    (
        DskFrameInfo &frameInfo,
        std::vector<DskGameObject> &gameObjects
    )
    {
        dskPipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets
        (
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            2,
            frameInfo.sets,
            0,
            nullptr
        );

        for (auto &obj : gameObjects)
        {
            DskPushConstantData push {};
            push.modelMatrix = obj.transform3d.mat4();
            push.normalMatrix = obj.transform3d.normalMatrix();
            push.textureIndex = obj.textureIndex;

            vkCmdPushConstants
            (
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(DskPushConstantData),
                &push
            );
            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }
}
