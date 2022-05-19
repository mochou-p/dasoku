// dsk

#include "descriptors.hpp"

#include <cassert>
#include <stdexcept>

#include <iostream>

namespace dsk
{
    DskDescriptorSetLayout::Builder &DskDescriptorSetLayout::Builder::addBinding
    (
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count
    )
    {
        assert(bindings.count(binding) == 0 && "binding already in use");

        VkDescriptorSetLayoutBinding layoutBinding {};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;

        bindings[binding] = layoutBinding;

        return *this;
    }

    std::unique_ptr<DskDescriptorSetLayout> DskDescriptorSetLayout::Builder::build() const
    {
        return std::make_unique<DskDescriptorSetLayout>(dskDevice, bindings);
    }

    DskDescriptorSetLayout::DskDescriptorSetLayout
    (
        DskDevice &dskDevice,
        std::unordered_map<uint32_t,
        VkDescriptorSetLayoutBinding> bindings
    ): dskDevice{dskDevice}, bindings{bindings}
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings {};

        for (auto kv : bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo {};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if
        (
            vkCreateDescriptorSetLayout
            (
                dskDevice.device(),
                &descriptorSetLayoutInfo,
                nullptr,
                &descriptorSetLayout
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    DskDescriptorSetLayout::~DskDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(dskDevice.device(), descriptorSetLayout, nullptr);
    }

    DskDescriptorPool::Builder &DskDescriptorPool::Builder::addPoolSize
    (
        VkDescriptorType descriptorType,
        uint32_t count
    )
    {
        poolSizes.push_back({descriptorType, count});

        return *this;
    }

    DskDescriptorPool::Builder &DskDescriptorPool::Builder::setPoolFlags
    (
        VkDescriptorPoolCreateFlags flags
    )
    {
        poolFlags = flags;

        return *this;
    }

    DskDescriptorPool::Builder &DskDescriptorPool::Builder::setMaxSets(uint32_t count)
    {
        maxSets = count;

        return *this;
    }

    std::unique_ptr<DskDescriptorPool> DskDescriptorPool::Builder::build() const
    {
        return std::make_unique<DskDescriptorPool>(dskDevice, maxSets, poolFlags, poolSizes);
    }

    DskDescriptorPool::DskDescriptorPool
    (
        DskDevice &dskDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes
    ): dskDevice{dskDevice}
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo {};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if
        (
            vkCreateDescriptorPool
            (
                dskDevice.device(),
                &descriptorPoolInfo,
                nullptr,
                &descriptorPool
            ) != VK_SUCCESS
        )
        {
            throw std::runtime_error("failed to create descriptor pool");
        }
    }

    DskDescriptorPool::~DskDescriptorPool()
    {
        vkDestroyDescriptorPool(dskDevice.device(), descriptorPool, nullptr);
    }

    bool DskDescriptorPool::allocateDescriptor
    (
        const VkDescriptorSetLayout descriptorSetLayout,
        VkDescriptorSet &descriptor
    ) const
    {
        // simplify
        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // might want to create a DescriptorPoolManager class that handles this case, and builds
        // a new pool whenever an old pool fills up
        if
        (
            vkAllocateDescriptorSets
            (
                dskDevice.device(),
                &allocInfo,
                &descriptor
            ) != VK_SUCCESS
        )
        {
            std::cout << "HERE IT IS\n" << vkAllocateDescriptorSets(dskDevice.device(),&allocInfo,&descriptor) << "HERE IT WAS\n";
            return false;
        }

        return true;
    }

    void DskDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const
    {
        vkFreeDescriptorSets
        (
            dskDevice.device(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data()
        );
    }

    void DskDescriptorPool::resetPool()
    {
        vkResetDescriptorPool(dskDevice.device(), descriptorPool, 0);
    }

    DskDescriptorWriter::DskDescriptorWriter
    (
        DskDescriptorSetLayout &setLayout,
        DskDescriptorPool &pool
    ): setLayout{setLayout}, pool{pool} {}

    DskDescriptorWriter &DskDescriptorWriter::writeBuffer
    (
        uint32_t binding,
        VkDescriptorBufferInfo *bufferInfo
    )
    {
        assert(setLayout.bindings.count(binding) == 1 && "layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        assert
        (
            bindingDescription.descriptorCount == 1 &&
            "binding single descriptor info, but binding expects multiple"
        );

        // simplify
        VkWriteDescriptorSet write {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;
        writes.push_back(write);

        return *this;
    }

    DskDescriptorWriter &DskDescriptorWriter::writeImage
    (
        uint32_t binding,
        VkDescriptorImageInfo *imageInfo,
        int descriptorCount
    )
    {
        assert(setLayout.bindings.count(binding) == 1 && "layout does not contain specified binding");

        auto &bindingDescription = setLayout.bindings[binding];

        //assert
        //(
        //    bindingDescription.descriptorCount == 1 &&
        //    "binding single descriptor info, but binding expects multiple"
        //);

        // simplify
        VkWriteDescriptorSet write {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = descriptorCount;
        writes.push_back(write);

        return *this;
    }

    bool DskDescriptorWriter::build(VkDescriptorSet &set)
    {
        bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);

        if (!success)
        {
            return false;
        }

        overwrite(set);

        return true;
    }

    void DskDescriptorWriter::overwrite(VkDescriptorSet &set)
    {
        for (auto &write : writes) {
            write.dstSet = set;
        }

        vkUpdateDescriptorSets(pool.dskDevice.device(), writes.size(), writes.data(), 0, nullptr);
    }
}
