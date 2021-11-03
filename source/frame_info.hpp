// dsk

#pragma once

#include "camera.hpp"

#include <vulkan/vulkan.h>

namespace dsk
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        DskCamera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}
