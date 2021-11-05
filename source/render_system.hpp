// dsk

#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "frame_info.hpp"

#include <memory>
#include <vector>

namespace dsk
{
    class DskRenderSystem
    {
        public:
            DskRenderSystem
            (
                DskDevice &device,
                VkRenderPass renderPass,
                VkDescriptorSetLayout globalSetLayout,
                VkDescriptorSetLayout textureSetLayout
            );
            ~DskRenderSystem();

            DskRenderSystem(const DskRenderSystem &) = delete;
            DskRenderSystem &operator=(const DskRenderSystem &) = delete;

            void renderGameObjects
            (
                DskFrameInfo &frameInfo,
                std::vector<DskGameObject> &gameObjects
            );

        private:
            void createPipelineLayout
            (
                VkDescriptorSetLayout globalSetLayout,
                VkDescriptorSetLayout textureSetLayout
            );
            void createPipeline(VkRenderPass renderPass);

            DskDevice &dskDevice;

            std::unique_ptr<DskPipeline> dskPipeline;
            VkPipelineLayout pipelineLayout;
    };
}
