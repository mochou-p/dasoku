// dsk

#include "buffer.hpp"

#include <cassert>
#include <cstring>

namespace dsk
{
    VkDeviceSize DskBuffer::getAlignment
    (
        VkDeviceSize instanceSize,
        VkDeviceSize minOffsetAlignment
    )
    {
        if (minOffsetAlignment > 0)
        {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }

        return instanceSize;
    }

    DskBuffer::DskBuffer
    (
        DskDevice &device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment
    ):
        dskDevice{device},
        instanceSize{instanceSize},
        instanceCount{instanceCount},
        usageFlags{usageFlags},
        memoryPropertyFlags{memoryPropertyFlags}
    {
        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
    }

    DskBuffer::~DskBuffer()
    {
        unmap();
        vkDestroyBuffer(dskDevice.device(), buffer, nullptr);
        vkFreeMemory(dskDevice.device(), memory, nullptr);
    }

    VkResult DskBuffer::map(VkDeviceSize size, VkDeviceSize offset)
    {
        assert(buffer && memory && "called map on buffer before create");

        return vkMapMemory(dskDevice.device(), memory, offset, size, 0, &mapped);
    }

    void DskBuffer::unmap()
    {
        if (mapped)
        {
            vkUnmapMemory(dskDevice.device(), memory);
            mapped = nullptr;
        }
    }

    void DskBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset)
    {
        assert(mapped && "cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(mapped, data, bufferSize);
        }
        else
        {
            char *memOffset = (char *)mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult DskBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
    {
        // simplify
        VkMappedMemoryRange mappedRange {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;

        return vkFlushMappedMemoryRanges(dskDevice.device(), 1, &mappedRange);
    }

    VkResult DskBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        // simplify
        VkMappedMemoryRange mappedRange {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;

        return vkInvalidateMappedMemoryRanges(dskDevice.device(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo DskBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
    {
        return VkDescriptorBufferInfo
        {
            buffer,
            offset,
            size,
        };
    }

    void DskBuffer::writeToIndex(void *data, int index)
    {
        writeToBuffer(data, instanceSize, index * alignmentSize);
    }

    VkResult DskBuffer::flushIndex(int index)
    {
        return flush(alignmentSize, index * alignmentSize);
    }

    VkDescriptorBufferInfo DskBuffer::descriptorInfoForIndex(int index)
    {
        return descriptorInfo(alignmentSize, index * alignmentSize);
    }

    VkResult DskBuffer::invalidateIndex(int index)
    {
        return invalidate(alignmentSize, index * alignmentSize);
    }
}
