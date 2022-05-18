// dsk

#pragma once

#include "camera.hpp"

#include <vulkan/vulkan.h>

namespace dsk
{
    struct DskFrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        DskCamera camera;
        VkDescriptorSet globalDescriptorSet;
        VkDescriptorSet textureDescriptorSet;
        VkDescriptorSet sets[2] = {globalDescriptorSet, textureDescriptorSet};

        DskFrameInfo() {};

        void init
        (
            int frameIndex,
            float frameTime,
            VkCommandBuffer commandBuffer,
            DskCamera &camera,
            VkDescriptorSet globalDescriptorSet,
            VkDescriptorSet textureDescriptorSet
        );
    };
}
