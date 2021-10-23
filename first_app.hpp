// zzz

#pragma once

#include "zzz_pipeline.hpp"
#include "zzz_window.hpp"
#include "zzz_device.hpp"
#include "zzz_model.hpp"
#include "zzz_game_object.hpp"
#include "zzz_renderer.hpp"

#include <memory>
#include <vector>

namespace zzz
{
    class FirstApp
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            FirstApp();
            ~FirstApp();

            FirstApp(const FirstApp &) = delete;
            FirstApp &operator=(const FirstApp &) = delete;

            void run();

        private:
            void loadGameObjects();
            void createPipelineLayout();
            void createPipeline();
            void renderGameObjects(VkCommandBuffer commandBuffer);

            ZzzWindow zzzWindow{ WIDTH, HEIGHT, "zzz" };
            ZzzDevice zzzDevice{ zzzWindow };
            ZzzRenderer zzzRenderer{ zzzWindow, zzzDevice};
            std::unique_ptr<ZzzPipeline> zzzPipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<ZzzGameObject> gameObjects;
    };
}
