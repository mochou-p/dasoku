// zzz

#pragma once

#include "zzz_device.hpp"

#include <string>
#include <vector>

namespace zzz
{
    struct PipelineConfigInfo
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class ZzzPipeline
    {
        public:
            ZzzPipeline
            (
                ZzzDevice& device,
                const std::string& vertFilepath,
                const std::string& fragFilepath,
                const PipelineConfigInfo& configInfo
            );
            ~ZzzPipeline();

            ZzzPipeline(const ZzzPipeline&) = delete;
            void operator=(const ZzzPipeline&) = delete;

            void bind(VkCommandBuffer commandBuffer);

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

            ZzzDevice& zzzDevice; // could dangle? part03 7:30
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}
