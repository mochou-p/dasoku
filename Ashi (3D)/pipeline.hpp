// ashi

#pragma once

#include "device.hpp"

#include <string>
#include <vector>

namespace ashi
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

    class AshiPipeline
    {
        public:
            AshiPipeline
            (
                AshiDevice &device,
                const std::string &vertFilepath,
                const std::string &fragFilepath,
                const PipelineConfigInfo &configInfo
            );
            ~AshiPipeline();

            AshiPipeline(const AshiPipeline &) = delete;
            AshiPipeline &operator=(const AshiPipeline &) = delete;

            AshiPipeline() = default; // ??? p10

            void bind(VkCommandBuffer commandBuffer);

            static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

        private:
            static std::vector<char> readFile(const std::string &filepath);

            void createGraphicsPipeline
            (
                const std::string &vertFilepath,
                const std::string &fragFilepath,
                const PipelineConfigInfo &configInfo
            );

            void createShaderModule
            (
                const std::vector<char> &code,
                VkShaderModule *shaderModule
            );

            AshiDevice &ashiDevice;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}
