// dsk

#pragma once

#include "stb_image.h"
#include "buffer.hpp"
#include "device.hpp"

#include <string>

namespace dsk
{
    class DskTexture
    {
        public:
            void createCommandBuffer(DskDevice &dskDevice);
            void freeCommandBuffer(DskDevice &dskDevice);

            void createBuffer(DskDevice &dskDevice);

            void loadImage(const std::string &filepath, DskDevice &DskDevice);

            VkImageView imageView;

        private:
            int texWidth;
            int texHeight;
            int texChannels;

            stbi_uc *pixels;

            VkCommandBuffer commandBuffer;
            VkBuffer buffer;

            VkImage image;
            VkDeviceMemory imageMemory;
    };        
}
