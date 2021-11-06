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
            void createBuffer(DskDevice &dskDevice);

            void loadImage(const std::string &filepath, DskDevice &DskDevice);

            VkImageView imageView;

            void cleanup(DskDevice &dskDevice);

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
