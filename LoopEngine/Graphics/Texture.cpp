#include "Texture.hpp"
#include "Context.hpp"

auto LoopEngine::Graphics::get_texture_from_assets(const std::string& filename) -> std::shared_ptr<Texture> {
    throw std::runtime_error("LoopEngine::Graphics::get_texture_from_assets is not implemented");
}

void LoopEngine::Graphics::release_texture(const Texture &texture) {
    Context::get_instance()->device.destroySampler(texture.sampler);
    Context::get_instance()->device.destroyImageView(texture.image_view);
    vmaDestroyImage(Context::get_instance()->allocator, texture.image, texture.allocation);
}
