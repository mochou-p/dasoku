// dsk

#pragma once

#include "device.hpp"
#include "buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace dsk
{
    class DskModel
    {
        public:
            struct Vertex
            {
                glm::vec3 position {};
                glm::vec3 color {};
                glm::vec3 normal {};
                glm::vec2 uv {};

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

                bool operator==(const Vertex &other) const
                {
                    return position == other.position &&
                        color == other.color &&
                        normal == other.normal &&
                        uv == other.uv;
                }
            };

            struct Data
            {
                std::vector<Vertex> vertices {};
                std::vector<uint32_t> indices {};

                void loadModel(const std::string &filepath);
            };

            DskModel
            (
                DskDevice &device,
                const DskModel::Data &data
            );
            ~DskModel();

            DskModel(const DskModel &) = delete;
            DskModel &operator=(const DskModel &) = delete;

            static std::unique_ptr<DskModel> createModelFromFile
            (
                DskDevice &device,
                const std::string &filepath
            );

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex> &vertices);
            void createIndexBuffer(const std::vector<uint32_t> &indices);

            DskDevice &dskDevice;

            std::unique_ptr<DskBuffer> vertexBuffer;
            uint32_t vertexCount;

            bool hasIndexBuffer = false;
            std::unique_ptr<DskBuffer> indexBuffer;
            uint32_t indexCount;
    };
}
