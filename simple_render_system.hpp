// zzz

#pragma once

#include "zzz_pipeline.hpp"
#include "zzz_device.hpp"
#include "zzz_game_object.hpp"

#include <memory>
#include <vector>

namespace zzz
{
    class SimpleRenderSystem
    {
        public:
            SimpleRenderSystem(ZzzDevice &device, VkRenderPass renderPass);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem &) = delete;
            SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

            void renderGameObjects
            (
                VkCommandBuffer commandBuffer,
                std::vector<ZzzGameObject> &gameObjects
            );

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            ZzzDevice &zzzDevice;

            std::unique_ptr<ZzzPipeline> zzzPipeline;
            VkPipelineLayout pipelineLayout;
    };
}
