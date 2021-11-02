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
        glm::mat4 transform {1.0f};
        glm::mat4 normalMatrix {1.0f};
    };

    DskRenderSystem::DskRenderSystem
    (
        DskDevice &device,
        VkRenderPass renderPass
    ): dskDevice{device}
    {
        createPipelineLayout();
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

    void DskRenderSystem::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(DskPushConstantData);

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
        FrameInfo &frameInfo,
        std::vector<DskGameObject> &gameObjects
    )
    {
        dskPipeline->bind(frameInfo.commandBuffer);

        auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

        for (auto &obj : gameObjects)
        {
            DskPushConstantData push {};
            auto modelMatrix = obj.transform3d.mat4();
            push.transform = projectionView * modelMatrix;
            push.normalMatrix = obj.transform3d.normalMatrix();

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
