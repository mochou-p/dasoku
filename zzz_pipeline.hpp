// zzz

#pragma once

#include "zzz_device.hpp"

#include <string>
#include <vector>

namespace zzz
{
    struct PipelineConfigInfo
    {
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
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

            ZzzPipeline(const ZzzPipeline &) = delete;
            ZzzPipeline &operator=(const ZzzPipeline &) = delete;

            void bind(VkCommandBuffer commandBuffer);

            static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

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
