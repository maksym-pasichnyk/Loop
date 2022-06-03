#include "Texture.hpp"
#include "Context.hpp"

auto LoopEngine::Graphics::get_texture_from_assets(const std::string& filename) -> std::shared_ptr<Texture> {
    throw std::runtime_error("LoopEngine::Graphics::get_texture_from_assets is not implemented");
}

void LoopEngine::Graphics::release_texture(const Texture &texture) {
    context().device.destroySampler(texture.sampler);
    context().device.destroyImageView(texture.image_view);
    vmaDestroyImage(context().allocator, texture.image, texture.allocation);
}
