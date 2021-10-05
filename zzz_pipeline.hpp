// zzz

#pragma once

#include "zzz_device.hpp"

#include <string>
#include <vector>

namespace zzz
{
    struct PipelineConfigInfo{};

    class zzzPipeline
    {
        public:
            zzzPipeline
            (
                zzzDevice& device,
                const std::string& vertFilepath,
                const std::string& fragFilepath,
                const PipelineConfigInfo& configInfo
            );
            ~zzzPipeline() {};

            zzzPipeline(const zzzPipeline&) = delete;
            void operator=(const zzzPipeline&) = delete;

            static PipelineConfigInfo defaultPipelineConfigInfo
            (
                uint32_t width,
                uint32_t height
            );

        private:
            static std::vector<char> readFile(const std::string& filepath);

            void createGraphicsPipeline
            (
                const std::string& vertFilepath,
                const std::string& fragFilepath,
                const PipelineConfigInfo& configInfo
            );

            void createShaderModule
            (
                const std::vector<char>& code,
                VkShaderModule* shaderModule
            );

            zzzDevice& zzzDevice; // could dangle? part03 7:30
            VkPipeline graphicsPipeLine;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}
