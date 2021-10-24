// ashi

#include "model.hpp"

#include <cassert>
#include <cstring>

namespace ashi
{
    AshiModel::AshiModel
    (
        AshiDevice &device,
        const std::vector<Vertex> &vertices
    ): ashiDevice{device}
    {
        createVertexBuffers(vertices);
    }

    AshiModel::~AshiModel()
    {
        vkDestroyBuffer(ashiDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(ashiDevice.device(), vertexBufferMemory, nullptr);
    }

    void AshiModel::createVertexBuffers(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        ashiDevice.createBuffer
        (
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory
        );

        void *data;
        vkMapMemory
        (
            ashiDevice.device(),
            vertexBufferMemory,
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
            vertexBufferMemory
        );
    }

    void AshiModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }
    
    void AshiModel::draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
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
            { 0, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex, position) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)    }
        };
    }
}
