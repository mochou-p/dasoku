// zzz

#pragma once

#include "zzz_pipeline.hpp"
#include "zzz_window.hpp"
#include "zzz_device.hpp"

namespace zzz
{
    class FirstApp
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            void run();

        private:
            zzzWindow zzzWindow{ WIDTH, HEIGHT, "zzz" };
            zzzDevice zzzDevice{ zzzWindow };
            zzzPipeline zzzPipeline
            {
                zzzDevice,
                "./shaders/simple_shader.vert.spv",
                "./shaders/simple_shader.frag.spv",
                zzzPipeline::defaultPipelineConfigInfo
                (
                    WIDTH, HEIGHT
                )
            };
    };
}
