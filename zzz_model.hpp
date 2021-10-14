// zzz

#pragma once

#include "zzz_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace zzz
{
    class ZzzModel
    {
        public:
            struct Vertex
            {
                glm::vec2 position;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };

            ZzzModel
            (
                ZzzDevice &device,
                const std::vector<Vertex> &vertices
            );
            ~ZzzModel();

            ZzzModel(const ZzzModel &) = delete;
            ZzzModel &operator=(const ZzzModel &) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex> &vertices);

            ZzzDevice& zzzDevice;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;
    };
}
