// ashi

#include "model.hpp"

#include <cassert>
#include <cstring>

namespace ashi
{
    AshiModel::AshiModel
    (
        AshiDevice &device,
        const AshiModel::Data &data
    ): ashiDevice{device}
    {
        createVertexBuffers(data.vertices);
        createIndexBuffer(data.indices);
    }

    AshiModel::~AshiModel()
    {
        vkDestroyBuffer(ashiDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(ashiDevice.device(), vertexBufferMemory, nullptr);

        if (hasIndexBuffer)
        {
            vkDestroyBuffer(ashiDevice.device(), indexBuffer, nullptr);
            vkFreeMemory(ashiDevice.device(), indexBufferMemory, nullptr);
        }
    }

    void AshiModel::createVertexBuffers(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        ashiDevice.createBuffer
        (
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        void *data;
        vkMapMemory
        (
            ashiDevice.device(),
            stagingBufferMemory,
            0,
            bufferSize,
            0,
            &data
        );
        memcpy
        (
            data,
            vertices.data(),
            static_cast<size_t>(bufferSize)
        );
        vkUnmapMemory
        (
            ashiDevice.device(),
            stagingBufferMemory
        );

        ashiDevice.createBuffer
        (
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory
        );

        ashiDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(ashiDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(ashiDevice.device(), stagingBufferMemory, nullptr);

    }

    void AshiModel::createIndexBuffer(const std::vector<uint32_t> &indices)
    {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer) { return; }

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        ashiDevice.createBuffer
        (
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        void *data;
        vkMapMemory
        (
            ashiDevice.device(),
            stagingBufferMemory,
            0,
            bufferSize,
            0,
            &data
        );
        memcpy
        (
            data,
            indices.data(),
            static_cast<size_t>(bufferSize)
        );
        vkUnmapMemory
        (
            ashiDevice.device(),
            stagingBufferMemory
        );

        ashiDevice.createBuffer
        (
            bufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer,
            indexBufferMemory
        );

        ashiDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(ashiDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(ashiDevice.device(), stagingBufferMemory, nullptr);
    }

    void AshiModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void AshiModel::draw(VkCommandBuffer commandBuffer)
    {
        if (hasIndexBuffer)
        {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> AshiModel::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        // alternative to line 89
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> AshiModel::Vertex::getAttributeDescriptions()
    {
        // alternative to line 83
        return
        {
            // LOCATION, BINDING, FORMAT, OFFSET
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)    }
        };
    }
}
