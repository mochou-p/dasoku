// dsk

#include "model.hpp"
#include "utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std
{
    template <>
    struct hash<dsk::DskModel::Vertex>
    {
        size_t operator()(dsk::DskModel::Vertex const &vertex) const
        {
            size_t seed = 0;
            dsk::hashCombine
            (
                seed,
                vertex.position,
                vertex.color,
                vertex.normal,
                vertex.uv
            );

            return seed;
        }
    };
}

namespace dsk
{
    DskModel::DskModel
    (
        DskDevice &device,
        const DskModel::Data &data
    ): dskDevice{device}
    {
        createVertexBuffers(data.vertices);
        createIndexBuffer(data.indices);
    }

    DskModel::~DskModel() {}
    
    std::unique_ptr<DskModel> DskModel::createModelFromFile
    (
        DskDevice &device,
        const std::string &filepath
    )
    {
        // hereherehere
        Data data {};
        data.loadModel(filepath);

        return std::make_unique<DskModel>(device, data);
    }

    void DskModel::createVertexBuffers(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "vertex count must be at least 3");

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        DskBuffer stagingBuffer
        {
            dskDevice,
            vertexSize,
            vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)vertices.data());

        vertexBuffer = std::make_unique<DskBuffer>
        (
            dskDevice,
            vertexSize,
            vertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        dskDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }

    void DskModel::createIndexBuffer(const std::vector<uint32_t> &indices)
    {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer) { return; }

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        uint32_t indexSize = sizeof(indices[0]);
        
        DskBuffer stagingBuffer
        {
            dskDevice,
            indexSize,
            indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)indices.data());

        indexBuffer = std::make_unique<DskBuffer>
            (
                dskDevice,
                indexSize,
                indexCount,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            );
        
        dskDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
    }

    void DskModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {vertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void DskModel::draw(VkCommandBuffer commandBuffer)
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

    std::vector<VkVertexInputBindingDescription> DskModel::Vertex::getBindingDescriptions()
    {
        return
        {
            // hereherehere
            // BINDING, STRIDE,         INPUT RATE
            {  0,       sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX  }
        };
    }

    std::vector<VkVertexInputAttributeDescription> DskModel::Vertex::getAttributeDescriptions()
    {
        return
        {
            // LOCATION, BINDING, FORMAT,                     OFFSET
            {  0,        0,       VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)  },
            {  1,        0,       VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)     },
            {  2,        0,       VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)    },
            {  3,        0,       VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex, uv)        }
        };
    }

    void DskModel::Data::loadModel(const std::string &filepath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if
        (
            !tinyobj::LoadObj
            (
                &attrib, &shapes, &materials,
                &warn, &err, filepath.c_str()
            )
        )
        {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices {};
        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex {};

                if (index.vertex_index >= 0)
                {
                    vertex.position =
                        {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]
                        };
                    

                    vertex.color =
                        {
                            attrib.colors[3 * index.vertex_index + 0],
                            attrib.colors[3 * index.vertex_index + 1],
                            attrib.colors[3 * index.vertex_index + 2]
                        };
                }

                if (index.normal_index >= 0)
                {
                    vertex.normal =
                        {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]
                        };
                }

                if (index.texcoord_index >= 0)
                {
                    vertex.uv =
                        {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            1 - attrib.texcoords[2 * index.texcoord_index + 1] // c:
                        };
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}
