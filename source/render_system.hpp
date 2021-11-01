// dsk

#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "camera.hpp"

#include <memory>
#include <vector>

namespace dsk
{
    class DskRenderSystem
    {
        public:
            DskRenderSystem(DskDevice &device, VkRenderPass renderPass);
            ~DskRenderSystem();

            DskRenderSystem(const DskRenderSystem &) = delete;
            DskRenderSystem &operator=(const DskRenderSystem &) = delete;

            void renderGameObjects
            (
                VkCommandBuffer commandBuffer,
                std::vector<DskGameObject> &gameObjects,
                const DskCamera &camera
            );

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            DskDevice &dskDevice;

            std::unique_ptr<DskPipeline> dskPipeline;
            VkPipelineLayout pipelineLayout;
    };
}
