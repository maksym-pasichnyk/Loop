#include "VertexBuffer.hpp"
#include "Context.hpp"

auto LoopEngine::Graphics::create_vertex_buffer(vk::DeviceSize size) -> std::shared_ptr<VertexBuffer> {
    vk::BufferUsageFlags usage;
    usage |= vk::BufferUsageFlagBits::eVertexBuffer;

    vk::BufferCreateInfo buffer_info{};
    buffer_info.setSize(size);
    buffer_info.setUsage(usage);

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
    alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VkBuffer handle;
    VmaAllocation allocation;
    vmaCreateBuffer(context().allocator, reinterpret_cast<const VkBufferCreateInfo *>(&buffer_info), &alloc_info, &handle, &allocation, nullptr);

    auto buffer = std::make_shared<VertexBuffer>();
    buffer->handle = handle;
    buffer->allocation = allocation;
    return buffer;
}

void LoopEngine::Graphics::release_vertex_buffer(const VertexBuffer &buffer) {
    vmaDestroyBuffer(context().allocator, buffer.handle, buffer.allocation);
}

auto LoopEngine::Graphics::map_vertex_buffer(const VertexBuffer &buffer) -> void* {
    void *data;
    vmaMapMemory(context().allocator, buffer.allocation, &data);
    return data;
}

void LoopEngine::Graphics::unmap_vertex_buffer(const VertexBuffer &buffer) {
    vmaUnmapMemory(context().allocator, buffer.allocation);
}

void LoopEngine::Graphics::update_vertex_buffer(const VertexBuffer &buffer, const void *data, vk::DeviceSize size) {
    std::memcpy(map_vertex_buffer(buffer), data, size);
    unmap_vertex_buffer(buffer);
}

