// dsk

#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace dsk
{
    void DskTexture::createCommandBuffer(DskDevice &dskDevice)
    {
        commandBuffer = dskDevice.beginSingleTimeCommands();
    }

    void DskTexture::freeCommandBuffer(DskDevice &dskDevice)
    {
        dskDevice.endSingleTimeCommands(commandBuffer);
    }

    void DskTexture::createBuffer(DskDevice &dskDevice)
    {
        VkDeviceSize texSize = texWidth * texHeight * 4;

        DskBuffer stagingBuffer
        {
            dskDevice,
            texSize,
            1,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer(pixels);

        VkImageCreateInfo imageInfo {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = texWidth;
        imageInfo.extent.height = texHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        dskDevice.createImageWithInfo
        (
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            image,
            imageMemory
        );

        dskDevice.copyBufferToImage(stagingBuffer.getBuffer(), image, texWidth, texHeight, 1);
    }

    void DskTexture::loadImage(const std::string &filepath, DskDevice &dskDevice)
    {
        pixels = stbi_load
            (
                filepath.c_str(),
                &texWidth,
                &texHeight,
                &texChannels,
                STBI_rgb_alpha
            );
        
        createBuffer(dskDevice);

        stbi_image_free(pixels);

        VkImageViewCreateInfo imageViewInfo {};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.pNext = nullptr;
        imageViewInfo.image = image;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageViewInfo.components = {};
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(dskDevice.device(), &imageViewInfo, nullptr, &imageView);
    }
}
