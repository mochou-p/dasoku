// dsk

#pragma once

#include "device.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace dsk
{
    class DskDescriptorSetLayout
    {
        public:
            class Builder
            {
                public:
                    Builder(DskDevice &dskDevice): dskDevice{dskDevice} {}
                    Builder &addBinding
                    (
                        uint32_t binding,
                        VkDescriptorType descriptorType,
                        VkShaderStageFlags stageFlags,
                        uint32_t count = 1
                    );
                    std::unique_ptr<DskDescriptorSetLayout> build() const;

                private:
                    DskDevice &dskDevice;
                    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings {};
            };

            DskDescriptorSetLayout
            (
                DskDevice &dskDevice,
                std::unordered_map<uint32_t,
                VkDescriptorSetLayoutBinding> bindings
            );
            ~DskDescriptorSetLayout();

            DskDescriptorSetLayout(const DskDescriptorSetLayout &) = delete;
            DskDescriptorSetLayout &operator=(const DskDescriptorSetLayout &) = delete;

            VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

        private:
            DskDevice &dskDevice;
            VkDescriptorSetLayout descriptorSetLayout;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

            friend class DskDescriptorWriter;
    };

    class DskDescriptorPool
    {
        public:
            class Builder
            {
                public:
                    Builder(DskDevice &dskDevice): dskDevice{dskDevice} {}
                    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
                    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
                    Builder &setMaxSets(uint32_t count);
                    std::unique_ptr<DskDescriptorPool> build() const;

                private:
                    DskDevice &dskDevice;
                    std::vector<VkDescriptorPoolSize> poolSizes {};
                    uint32_t maxSets = 1000;
                    VkDescriptorPoolCreateFlags poolFlags = 0;
            };

            DskDescriptorPool
            (
                DskDevice &dskDevice,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes
            );
            ~DskDescriptorPool();

            DskDescriptorPool(const DskDescriptorPool &) = delete;
            DskDescriptorPool &operator=(const DskDescriptorPool &) = delete;

            bool allocateDescriptor
            (
                const VkDescriptorSetLayout descriptorSetLayout,
                VkDescriptorSet &descriptor
            ) const;
            void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
            void resetPool();

        private:
            DskDevice &dskDevice;
            VkDescriptorPool descriptorPool;

            friend class DskDescriptorWriter;
    };

    class DskDescriptorWriter
    {
        public:
            DskDescriptorWriter(DskDescriptorSetLayout &setLayout, DskDescriptorPool &pool);
            DskDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
            DskDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo, int descriptorCount);

            bool build(VkDescriptorSet &set);
            void overwrite(VkDescriptorSet &set);

        private:
            DskDescriptorSetLayout &setLayout;
            DskDescriptorPool &pool;
            std::vector<VkWriteDescriptorSet> writes;
    };
}
