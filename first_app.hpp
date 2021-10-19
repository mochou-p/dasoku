// zzz

#pragma once

#include "zzz_pipeline.hpp"
#include "zzz_window.hpp"
#include "zzz_device.hpp"
#include "zzz_swap_chain.hpp"
#include "zzz_model.hpp"

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
            void loadModels();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void freeCommandBuffers();
            void drawFrame();
            void recreateSwapChain();
            void recordCommandBuffer(int imageIndex);

            ZzzWindow zzzWindow{ WIDTH, HEIGHT, "zzz" };
            ZzzDevice zzzDevice{ zzzWindow };
            std::unique_ptr<ZzzSwapChain> zzzSwapChain;
            std::unique_ptr<ZzzPipeline> zzzPipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<ZzzModel> zzzModel;
    };
}
