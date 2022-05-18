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
            DskRenderSystem() {};
            ~DskRenderSystem();

            DskRenderSystem(const DskRenderSystem &) = delete;
            DskRenderSystem &operator=(const DskRenderSystem &) = delete;

            void init
            (
                DskDevice *device,
                VkRenderPass renderPass,
                VkDescriptorSetLayout globalSetLayout,
                VkDescriptorSetLayout textureSetLayout
            );

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

            DskDevice *dskDevice = nullptr;

            std::unique_ptr<DskPipeline> dskPipeline;
            VkPipelineLayout pipelineLayout;
    };
}
