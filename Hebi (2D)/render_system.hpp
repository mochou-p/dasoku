// hebi

#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"

#include <memory>
#include <vector>

namespace hebi
{
    class HebiRenderSystem
    {
        public:
            HebiRenderSystem(HebiDevice &device, VkRenderPass renderPass);
            ~HebiRenderSystem();

            HebiRenderSystem(const HebiRenderSystem &) = delete;
            HebiRenderSystem &operator=(const HebiRenderSystem &) = delete;

            void renderGameObjects
            (
                VkCommandBuffer commandBuffer,
                std::vector<HebiGameObject> &gameObjects
            );

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            HebiDevice &hebiDevice;

            std::unique_ptr<HebiPipeline> hebiPipeline;
            VkPipelineLayout pipelineLayout;
    };
}
