// hebi

#pragma once

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace hebi
{
    class HebiModel
    {
        public:
            struct Vertex
            {
                glm::vec2 position;
                glm::vec3 color;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };

            HebiModel
            (
                HebiDevice &device,
                const std::vector<Vertex> &vertices
            );
            ~HebiModel();

            HebiModel(const HebiModel &) = delete;
            HebiModel &operator=(const HebiModel &) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex> &vertices);

            HebiDevice &hebiDevice;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;
    };
}
