#include "IndexBuffer.hpp"
#include "Context.hpp"

auto LoopEngine::Graphics::create_index_buffer(vk::DeviceSize size) -> std::shared_ptr<IndexBuffer> {
    vk::BufferUsageFlags usage;
    usage |= vk::BufferUsageFlagBits::eIndexBuffer;

    vk::BufferCreateInfo buffer_info{};
    buffer_info.setSize(size);
    buffer_info.setUsage(usage);

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
    alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VkBuffer handle;
    VmaAllocation allocation;
    vmaCreateBuffer(context().allocator, reinterpret_cast<const VkBufferCreateInfo *>(&buffer_info), &alloc_info, &handle, &allocation, nullptr);

    auto buffer = std::make_shared<IndexBuffer>();
    buffer->handle = handle;
    buffer->allocation = allocation;
    return buffer;
}

void LoopEngine::Graphics::release_index_buffer(const IndexBuffer &buffer) {
    vmaDestroyBuffer(context().allocator, buffer.handle, buffer.allocation);
}

auto LoopEngine::Graphics::map_index_buffer(const IndexBuffer &buffer) -> void* {
    void *data;
    vmaMapMemory(context().allocator, buffer.allocation, &data);
    return data;
}

void LoopEngine::Graphics::unmap_index_buffer(const IndexBuffer &buffer) {
    vmaUnmapMemory(context().allocator, buffer.allocation);
}

void LoopEngine::Graphics::update_index_buffer(const IndexBuffer &buffer, const void *data, vk::DeviceSize size) {
    std::memcpy(map_index_buffer(buffer), data, size);
    unmap_index_buffer(buffer);
}
