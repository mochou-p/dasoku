// ashi

#pragma once

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"

#include <memory>
#include <vector>

namespace ashi
{
    class AshiRenderSystem
    {
        public:
            AshiRenderSystem(AshiDevice &device, VkRenderPass renderPass);
            ~AshiRenderSystem();

            AshiRenderSystem(const AshiRenderSystem &) = delete;
            AshiRenderSystem &operator=(const AshiRenderSystem &) = delete;

            void renderGameObjects
            (
                VkCommandBuffer commandBuffer,
                std::vector<AshiGameObject> &gameObjects
            );

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            AshiDevice &ashiDevice;

            std::unique_ptr<AshiPipeline> ashiPipeline;
            VkPipelineLayout pipelineLayout;
    };
}
