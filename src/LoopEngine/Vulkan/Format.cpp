#include "Format.hpp"

#include <map>
#include <string_view>

using namespace std::string_view_literals;

inline constexpr auto fnv1a(std::string_view str) -> std::uint32_t {
    std::uint32_t hash = 2166136261UL;
    for (char i : str) {
        hash ^= i;
        hash *= 16777619;
    }
    return hash;
}

auto LoopEngine::Vulkan::get_format_from_string(const std::string &format) -> vk::Format {
    switch(fnv1a(format)) {
    case fnv1a("Undefined"sv): return vk::Format::eUndefined;
    case fnv1a("R4G4UnormPack8"sv): return vk::Format::eR4G4UnormPack8;
    case fnv1a("R4G4B4A4UnormPack16"sv): return vk::Format::eR4G4B4A4UnormPack16;
    case fnv1a("B4G4R4A4UnormPack16"sv): return vk::Format::eB4G4R4A4UnormPack16;
    case fnv1a("R5G6B5UnormPack16"sv): return vk::Format::eR5G6B5UnormPack16;
    case fnv1a("B5G6R5UnormPack16"sv): return vk::Format::eB5G6R5UnormPack16;
    case fnv1a("R5G5B5A1UnormPack16"sv): return vk::Format::eR5G5B5A1UnormPack16;
    case fnv1a("B5G5R5A1UnormPack16"sv): return vk::Format::eB5G5R5A1UnormPack16;
    case fnv1a("A1R5G5B5UnormPack16"sv): return vk::Format::eA1R5G5B5UnormPack16;
    case fnv1a("R8Unorm"sv): return vk::Format::eR8Unorm;
    case fnv1a("R8Snorm"sv): return vk::Format::eR8Snorm;
    case fnv1a("R8Uscaled"sv): return vk::Format::eR8Uscaled;
    case fnv1a("R8Sscaled"sv): return vk::Format::eR8Sscaled;
    case fnv1a("R8Uint"sv): return vk::Format::eR8Uint;
    case fnv1a("R8Sint"sv): return vk::Format::eR8Sint;
    case fnv1a("R8Srgb"sv): return vk::Format::eR8Srgb;
    case fnv1a("R8G8Unorm"sv): return vk::Format::eR8G8Unorm;
    case fnv1a("R8G8Snorm"sv): return vk::Format::eR8G8Snorm;
    case fnv1a("R8G8Uscaled"sv): return vk::Format::eR8G8Uscaled;
    case fnv1a("R8G8Sscaled"sv): return vk::Format::eR8G8Sscaled;
    case fnv1a("R8G8Uint"sv): return vk::Format::eR8G8Uint;
    case fnv1a("R8G8Sint"sv): return vk::Format::eR8G8Sint;
    case fnv1a("R8G8Srgb"sv): return vk::Format::eR8G8Srgb;
    case fnv1a("R8G8B8Unorm"sv): return vk::Format::eR8G8B8Unorm;
    case fnv1a("R8G8B8Snorm"sv): return vk::Format::eR8G8B8Snorm;
    case fnv1a("R8G8B8Uscaled"sv): return vk::Format::eR8G8B8Uscaled;
    case fnv1a("R8G8B8Sscaled"sv): return vk::Format::eR8G8B8Sscaled;
    case fnv1a("R8G8B8Uint"sv): return vk::Format::eR8G8B8Uint;
    case fnv1a("R8G8B8Sint"sv): return vk::Format::eR8G8B8Sint;
    case fnv1a("R8G8B8Srgb"sv): return vk::Format::eR8G8B8Srgb;
    case fnv1a("B8G8R8Unorm"sv): return vk::Format::eB8G8R8Unorm;
    case fnv1a("B8G8R8Snorm"sv): return vk::Format::eB8G8R8Snorm;
    case fnv1a("B8G8R8Uscaled"sv): return vk::Format::eB8G8R8Uscaled;
    case fnv1a("B8G8R8Sscaled"sv): return vk::Format::eB8G8R8Sscaled;
    case fnv1a("B8G8R8Uint"sv): return vk::Format::eB8G8R8Uint;
    case fnv1a("B8G8R8Sint"sv): return vk::Format::eB8G8R8Sint;
    case fnv1a("B8G8R8Srgb"sv): return vk::Format::eB8G8R8Srgb;
    case fnv1a("R8G8B8A8Unorm"sv): return vk::Format::eR8G8B8A8Unorm;
    case fnv1a("R8G8B8A8Snorm"sv): return vk::Format::eR8G8B8A8Snorm;
    case fnv1a("R8G8B8A8Uscaled"sv): return vk::Format::eR8G8B8A8Uscaled;
    case fnv1a("R8G8B8A8Sscaled"sv): return vk::Format::eR8G8B8A8Sscaled;
    case fnv1a("R8G8B8A8Uint"sv): return vk::Format::eR8G8B8A8Uint;
    case fnv1a("R8G8B8A8Sint"sv): return vk::Format::eR8G8B8A8Sint;
    case fnv1a("R8G8B8A8Srgb"sv): return vk::Format::eR8G8B8A8Srgb;
    case fnv1a("B8G8R8A8Unorm"sv): return vk::Format::eB8G8R8A8Unorm;
    case fnv1a("B8G8R8A8Snorm"sv): return vk::Format::eB8G8R8A8Snorm;
    case fnv1a("B8G8R8A8Uscaled"sv): return vk::Format::eB8G8R8A8Uscaled;
    case fnv1a("B8G8R8A8Sscaled"sv): return vk::Format::eB8G8R8A8Sscaled;
    case fnv1a("B8G8R8A8Uint"sv): return vk::Format::eB8G8R8A8Uint;
    case fnv1a("B8G8R8A8Sint"sv): return vk::Format::eB8G8R8A8Sint;
    case fnv1a("B8G8R8A8Srgb"sv): return vk::Format::eB8G8R8A8Srgb;
    case fnv1a("A8B8G8R8UnormPack32"sv): return vk::Format::eA8B8G8R8UnormPack32;
    case fnv1a("A8B8G8R8SnormPack32"sv): return vk::Format::eA8B8G8R8SnormPack32;
    case fnv1a("A8B8G8R8UscaledPack32"sv): return vk::Format::eA8B8G8R8UscaledPack32;
    case fnv1a("A8B8G8R8SscaledPack32"sv): return vk::Format::eA8B8G8R8SscaledPack32;
    case fnv1a("A8B8G8R8UintPack32"sv): return vk::Format::eA8B8G8R8UintPack32;
    case fnv1a("A8B8G8R8SintPack32"sv): return vk::Format::eA8B8G8R8SintPack32;
    case fnv1a("A8B8G8R8SrgbPack32"sv): return vk::Format::eA8B8G8R8SrgbPack32;
    case fnv1a("A2R10G10B10UnormPack32"sv): return vk::Format::eA2R10G10B10UnormPack32;
    case fnv1a("A2R10G10B10SnormPack32"sv): return vk::Format::eA2R10G10B10SnormPack32;
    case fnv1a("A2R10G10B10UscaledPack32"sv): return vk::Format::eA2R10G10B10UscaledPack32;
    case fnv1a("A2R10G10B10SscaledPack32"sv): return vk::Format::eA2R10G10B10SscaledPack32;
    case fnv1a("A2R10G10B10UintPack32"sv): return vk::Format::eA2R10G10B10UintPack32;
    case fnv1a("A2R10G10B10SintPack32"sv): return vk::Format::eA2R10G10B10SintPack32;
    case fnv1a("A2B10G10R10UnormPack32"sv): return vk::Format::eA2B10G10R10UnormPack32;
    case fnv1a("A2B10G10R10SnormPack32"sv): return vk::Format::eA2B10G10R10SnormPack32;
    case fnv1a("A2B10G10R10UscaledPack32"sv): return vk::Format::eA2B10G10R10UscaledPack32;
    case fnv1a("A2B10G10R10SscaledPack32"sv): return vk::Format::eA2B10G10R10SscaledPack32;
    case fnv1a("A2B10G10R10UintPack32"sv): return vk::Format::eA2B10G10R10UintPack32;
    case fnv1a("A2B10G10R10SintPack32"sv): return vk::Format::eA2B10G10R10SintPack32;
    case fnv1a("R16Unorm"sv): return vk::Format::eR16Unorm;
    case fnv1a("R16Snorm"sv): return vk::Format::eR16Snorm;
    case fnv1a("R16Uscaled"sv): return vk::Format::eR16Uscaled;
    case fnv1a("R16Sscaled"sv): return vk::Format::eR16Sscaled;
    case fnv1a("R16Uint"sv): return vk::Format::eR16Uint;
    case fnv1a("R16Sint"sv): return vk::Format::eR16Sint;
    case fnv1a("R16Sfloat"sv): return vk::Format::eR16Sfloat;
    case fnv1a("R16G16Unorm"sv): return vk::Format::eR16G16Unorm;
    case fnv1a("R16G16Snorm"sv): return vk::Format::eR16G16Snorm;
    case fnv1a("R16G16Uscaled"sv): return vk::Format::eR16G16Uscaled;
    case fnv1a("R16G16Sscaled"sv): return vk::Format::eR16G16Sscaled;
    case fnv1a("R16G16Uint"sv): return vk::Format::eR16G16Uint;
    case fnv1a("R16G16Sint"sv): return vk::Format::eR16G16Sint;
    case fnv1a("R16G16Sfloat"sv): return vk::Format::eR16G16Sfloat;
    case fnv1a("R16G16B16Unorm"sv): return vk::Format::eR16G16B16Unorm;
    case fnv1a("R16G16B16Snorm"sv): return vk::Format::eR16G16B16Snorm;
    case fnv1a("R16G16B16Uscaled"sv): return vk::Format::eR16G16B16Uscaled;
    case fnv1a("R16G16B16Sscaled"sv): return vk::Format::eR16G16B16Sscaled;
    case fnv1a("R16G16B16Uint"sv): return vk::Format::eR16G16B16Uint;
    case fnv1a("R16G16B16Sint"sv): return vk::Format::eR16G16B16Sint;
    case fnv1a("R16G16B16Sfloat"sv): return vk::Format::eR16G16B16Sfloat;
    case fnv1a("R16G16B16A16Unorm"sv): return vk::Format::eR16G16B16A16Unorm;
    case fnv1a("R16G16B16A16Snorm"sv): return vk::Format::eR16G16B16A16Snorm;
    case fnv1a("R16G16B16A16Uscaled"sv): return vk::Format::eR16G16B16A16Uscaled;
    case fnv1a("R16G16B16A16Sscaled"sv): return vk::Format::eR16G16B16A16Sscaled;
    case fnv1a("R16G16B16A16Uint"sv): return vk::Format::eR16G16B16A16Uint;
    case fnv1a("R16G16B16A16Sint"sv): return vk::Format::eR16G16B16A16Sint;
    case fnv1a("R16G16B16A16Sfloat"sv): return vk::Format::eR16G16B16A16Sfloat;
    case fnv1a("R32Uint"sv): return vk::Format::eR32Uint;
    case fnv1a("R32Sint"sv): return vk::Format::eR32Sint;
    case fnv1a("R32Sfloat"sv): return vk::Format::eR32Sfloat;
    case fnv1a("R32G32Uint"sv): return vk::Format::eR32G32Uint;
    case fnv1a("R32G32Sint"sv): return vk::Format::eR32G32Sint;
    case fnv1a("R32G32Sfloat"sv): return vk::Format::eR32G32Sfloat;
    case fnv1a("R32G32B32Uint"sv): return vk::Format::eR32G32B32Uint;
    case fnv1a("R32G32B32Sint"sv): return vk::Format::eR32G32B32Sint;
    case fnv1a("R32G32B32Sfloat"sv): return vk::Format::eR32G32B32Sfloat;
    case fnv1a("R32G32B32A32Uint"sv): return vk::Format::eR32G32B32A32Uint;
    case fnv1a("R32G32B32A32Sint"sv): return vk::Format::eR32G32B32A32Sint;
    case fnv1a("R32G32B32A32Sfloat"sv): return vk::Format::eR32G32B32A32Sfloat;
    case fnv1a("R64Uint"sv): return vk::Format::eR64Uint;
    case fnv1a("R64Sint"sv): return vk::Format::eR64Sint;
    case fnv1a("R64Sfloat"sv): return vk::Format::eR64Sfloat;
    case fnv1a("R64G64Uint"sv): return vk::Format::eR64G64Uint;
    case fnv1a("R64G64Sint"sv): return vk::Format::eR64G64Sint;
    case fnv1a("R64G64Sfloat"sv): return vk::Format::eR64G64Sfloat;
    case fnv1a("R64G64B64Uint"sv): return vk::Format::eR64G64B64Uint;
    case fnv1a("R64G64B64Sint"sv): return vk::Format::eR64G64B64Sint;
    case fnv1a("R64G64B64Sfloat"sv): return vk::Format::eR64G64B64Sfloat;
    case fnv1a("R64G64B64A64Uint"sv): return vk::Format::eR64G64B64A64Uint;
    case fnv1a("R64G64B64A64Sint"sv): return vk::Format::eR64G64B64A64Sint;
    case fnv1a("R64G64B64A64Sfloat"sv): return vk::Format::eR64G64B64A64Sfloat;
    case fnv1a("B10G11R11UfloatPack32"sv): return vk::Format::eB10G11R11UfloatPack32;
    case fnv1a("E5B9G9R9UfloatPack32"sv): return vk::Format::eE5B9G9R9UfloatPack32;
    case fnv1a("D16Unorm"sv): return vk::Format::eD16Unorm;
    case fnv1a("X8D24UnormPack32"sv): return vk::Format::eX8D24UnormPack32;
    case fnv1a("D32Sfloat"sv): return vk::Format::eD32Sfloat;
    case fnv1a("S8Uint"sv): return vk::Format::eS8Uint;
    case fnv1a("D16UnormS8Uint"sv): return vk::Format::eD16UnormS8Uint;
    case fnv1a("D24UnormS8Uint"sv): return vk::Format::eD24UnormS8Uint;
    case fnv1a("D32SfloatS8Uint"sv): return vk::Format::eD32SfloatS8Uint;
    case fnv1a("Bc1RgbUnormBlock"sv): return vk::Format::eBc1RgbUnormBlock;
    case fnv1a("Bc1RgbSrgbBlock"sv): return vk::Format::eBc1RgbSrgbBlock;
    case fnv1a("Bc1RgbaUnormBlock"sv): return vk::Format::eBc1RgbaUnormBlock;
    case fnv1a("Bc1RgbaSrgbBlock"sv): return vk::Format::eBc1RgbaSrgbBlock;
    case fnv1a("Bc2UnormBlock"sv): return vk::Format::eBc2UnormBlock;
    case fnv1a("Bc2SrgbBlock"sv): return vk::Format::eBc2SrgbBlock;
    case fnv1a("Bc3UnormBlock"sv): return vk::Format::eBc3UnormBlock;
    case fnv1a("Bc3SrgbBlock"sv): return vk::Format::eBc3SrgbBlock;
    case fnv1a("Bc4UnormBlock"sv): return vk::Format::eBc4UnormBlock;
    case fnv1a("Bc4SnormBlock"sv): return vk::Format::eBc4SnormBlock;
    case fnv1a("Bc5UnormBlock"sv): return vk::Format::eBc5UnormBlock;
    case fnv1a("Bc5SnormBlock"sv): return vk::Format::eBc5SnormBlock;
    case fnv1a("Bc6HUfloatBlock"sv): return vk::Format::eBc6HUfloatBlock;
    case fnv1a("Bc6HSfloatBlock"sv): return vk::Format::eBc6HSfloatBlock;
    case fnv1a("Bc7UnormBlock"sv): return vk::Format::eBc7UnormBlock;
    case fnv1a("Bc7SrgbBlock"sv): return vk::Format::eBc7SrgbBlock;
    case fnv1a("Etc2R8G8B8UnormBlock"sv): return vk::Format::eEtc2R8G8B8UnormBlock;
    case fnv1a("Etc2R8G8B8SrgbBlock"sv): return vk::Format::eEtc2R8G8B8SrgbBlock;
    case fnv1a("Etc2R8G8B8A1UnormBlock"sv): return vk::Format::eEtc2R8G8B8A1UnormBlock;
    case fnv1a("Etc2R8G8B8A1SrgbBlock"sv): return vk::Format::eEtc2R8G8B8A1SrgbBlock;
    case fnv1a("Etc2R8G8B8A8UnormBlock"sv): return vk::Format::eEtc2R8G8B8A8UnormBlock;
    case fnv1a("Etc2R8G8B8A8SrgbBlock"sv): return vk::Format::eEtc2R8G8B8A8SrgbBlock;
    case fnv1a("EacR11UnormBlock"sv): return vk::Format::eEacR11UnormBlock;
    case fnv1a("EacR11SnormBlock"sv): return vk::Format::eEacR11SnormBlock;
    case fnv1a("EacR11G11UnormBlock"sv): return vk::Format::eEacR11G11UnormBlock;
    case fnv1a("EacR11G11SnormBlock"sv): return vk::Format::eEacR11G11SnormBlock;
    case fnv1a("Astc4x4UnormBlock"sv): return vk::Format::eAstc4x4UnormBlock;
    case fnv1a("Astc4x4SrgbBlock"sv): return vk::Format::eAstc4x4SrgbBlock;
    case fnv1a("Astc5x4UnormBlock"sv): return vk::Format::eAstc5x4UnormBlock;
    case fnv1a("Astc5x4SrgbBlock"sv): return vk::Format::eAstc5x4SrgbBlock;
    case fnv1a("Astc5x5UnormBlock"sv): return vk::Format::eAstc5x5UnormBlock;
    case fnv1a("Astc5x5SrgbBlock"sv): return vk::Format::eAstc5x5SrgbBlock;
    case fnv1a("Astc6x5UnormBlock"sv): return vk::Format::eAstc6x5UnormBlock;
    case fnv1a("Astc6x5SrgbBlock"sv): return vk::Format::eAstc6x5SrgbBlock;
    case fnv1a("Astc6x6UnormBlock"sv): return vk::Format::eAstc6x6UnormBlock;
    case fnv1a("Astc6x6SrgbBlock"sv): return vk::Format::eAstc6x6SrgbBlock;
    case fnv1a("Astc8x5UnormBlock"sv): return vk::Format::eAstc8x5UnormBlock;
    case fnv1a("Astc8x5SrgbBlock"sv): return vk::Format::eAstc8x5SrgbBlock;
    case fnv1a("Astc8x6UnormBlock"sv): return vk::Format::eAstc8x6UnormBlock;
    case fnv1a("Astc8x6SrgbBlock"sv): return vk::Format::eAstc8x6SrgbBlock;
    case fnv1a("Astc8x8UnormBlock"sv): return vk::Format::eAstc8x8UnormBlock;
    case fnv1a("Astc8x8SrgbBlock"sv): return vk::Format::eAstc8x8SrgbBlock;
    case fnv1a("Astc10x5UnormBlock"sv): return vk::Format::eAstc10x5UnormBlock;
    case fnv1a("Astc10x5SrgbBlock"sv): return vk::Format::eAstc10x5SrgbBlock;
    case fnv1a("Astc10x6UnormBlock"sv): return vk::Format::eAstc10x6UnormBlock;
    case fnv1a("Astc10x6SrgbBlock"sv): return vk::Format::eAstc10x6SrgbBlock;
    case fnv1a("Astc10x8UnormBlock"sv): return vk::Format::eAstc10x8UnormBlock;
    case fnv1a("Astc10x8SrgbBlock"sv): return vk::Format::eAstc10x8SrgbBlock;
    case fnv1a("Astc10x10UnormBlock"sv): return vk::Format::eAstc10x10UnormBlock;
    case fnv1a("Astc10x10SrgbBlock"sv): return vk::Format::eAstc10x10SrgbBlock;
    case fnv1a("Astc12x10UnormBlock"sv): return vk::Format::eAstc12x10UnormBlock;
    case fnv1a("Astc12x10SrgbBlock"sv): return vk::Format::eAstc12x10SrgbBlock;
    case fnv1a("Astc12x12UnormBlock"sv): return vk::Format::eAstc12x12UnormBlock;
    case fnv1a("Astc12x12SrgbBlock"sv): return vk::Format::eAstc12x12SrgbBlock;
    case fnv1a("G8B8G8R8422Unorm"sv): return vk::Format::eG8B8G8R8422Unorm;
    case fnv1a("B8G8R8G8422Unorm"sv): return vk::Format::eB8G8R8G8422Unorm;
    case fnv1a("G8B8R83Plane420Unorm"sv): return vk::Format::eG8B8R83Plane420Unorm;
    case fnv1a("G8B8R82Plane420Unorm"sv): return vk::Format::eG8B8R82Plane420Unorm;
    case fnv1a("G8B8R83Plane422Unorm"sv): return vk::Format::eG8B8R83Plane422Unorm;
    case fnv1a("G8B8R82Plane422Unorm"sv): return vk::Format::eG8B8R82Plane422Unorm;
    case fnv1a("G8B8R83Plane444Unorm"sv): return vk::Format::eG8B8R83Plane444Unorm;
    case fnv1a("R10X6UnormPack16"sv): return vk::Format::eR10X6UnormPack16;
    case fnv1a("R10X6G10X6Unorm2Pack16"sv): return vk::Format::eR10X6G10X6Unorm2Pack16;
    case fnv1a("R10X6G10X6B10X6A10X6Unorm4Pack16"sv): return vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16;
    case fnv1a("G10X6B10X6G10X6R10X6422Unorm4Pack16"sv): return vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16;
    case fnv1a("B10X6G10X6R10X6G10X6422Unorm4Pack16"sv): return vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16;
    case fnv1a("G10X6B10X6R10X63Plane420Unorm3Pack16"sv): return vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16;
    case fnv1a("G10X6B10X6R10X62Plane420Unorm3Pack16"sv): return vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16;
    case fnv1a("G10X6B10X6R10X63Plane422Unorm3Pack16"sv): return vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16;
    case fnv1a("G10X6B10X6R10X62Plane422Unorm3Pack16"sv): return vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16;
    case fnv1a("G10X6B10X6R10X63Plane444Unorm3Pack16"sv): return vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16;
    case fnv1a("R12X4UnormPack16"sv): return vk::Format::eR12X4UnormPack16;
    case fnv1a("R12X4G12X4Unorm2Pack16"sv): return vk::Format::eR12X4G12X4Unorm2Pack16;
    case fnv1a("R12X4G12X4B12X4A12X4Unorm4Pack16"sv): return vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16;
    case fnv1a("G12X4B12X4G12X4R12X4422Unorm4Pack16"sv): return vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16;
    case fnv1a("B12X4G12X4R12X4G12X4422Unorm4Pack16"sv): return vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16;
    case fnv1a("G12X4B12X4R12X43Plane420Unorm3Pack16"sv): return vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16;
    case fnv1a("G12X4B12X4R12X42Plane420Unorm3Pack16"sv): return vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16;
    case fnv1a("G12X4B12X4R12X43Plane422Unorm3Pack16"sv): return vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16;
    case fnv1a("G12X4B12X4R12X42Plane422Unorm3Pack16"sv): return vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16;
    case fnv1a("G12X4B12X4R12X43Plane444Unorm3Pack16"sv): return vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16;
    case fnv1a("G16B16G16R16422Unorm"sv): return vk::Format::eG16B16G16R16422Unorm;
    case fnv1a("B16G16R16G16422Unorm"sv): return vk::Format::eB16G16R16G16422Unorm;
    case fnv1a("G16B16R163Plane420Unorm"sv): return vk::Format::eG16B16R163Plane420Unorm;
    case fnv1a("G16B16R162Plane420Unorm"sv): return vk::Format::eG16B16R162Plane420Unorm;
    case fnv1a("G16B16R163Plane422Unorm"sv): return vk::Format::eG16B16R163Plane422Unorm;
    case fnv1a("G16B16R162Plane422Unorm"sv): return vk::Format::eG16B16R162Plane422Unorm;
    case fnv1a("G16B16R163Plane444Unorm"sv): return vk::Format::eG16B16R163Plane444Unorm;
    case fnv1a("G8B8R82Plane444Unorm"sv): return vk::Format::eG8B8R82Plane444Unorm;
    case fnv1a("G10X6B10X6R10X62Plane444Unorm3Pack16"sv): return vk::Format::eG10X6B10X6R10X62Plane444Unorm3Pack16;
    case fnv1a("G12X4B12X4R12X42Plane444Unorm3Pack16"sv): return vk::Format::eG12X4B12X4R12X42Plane444Unorm3Pack16;
    case fnv1a("G16B16R162Plane444Unorm"sv): return vk::Format::eG16B16R162Plane444Unorm;
    case fnv1a("A4R4G4B4UnormPack16"sv): return vk::Format::eA4R4G4B4UnormPack16;
    case fnv1a("A4B4G4R4UnormPack16"sv): return vk::Format::eA4B4G4R4UnormPack16;
    case fnv1a("Astc4x4SfloatBlock"sv): return vk::Format::eAstc4x4SfloatBlock;
    case fnv1a("Astc5x4SfloatBlock"sv): return vk::Format::eAstc5x4SfloatBlock;
    case fnv1a("Astc5x5SfloatBlock"sv): return vk::Format::eAstc5x5SfloatBlock;
    case fnv1a("Astc6x5SfloatBlock"sv): return vk::Format::eAstc6x5SfloatBlock;
    case fnv1a("Astc6x6SfloatBlock"sv): return vk::Format::eAstc6x6SfloatBlock;
    case fnv1a("Astc8x5SfloatBlock"sv): return vk::Format::eAstc8x5SfloatBlock;
    case fnv1a("Astc8x6SfloatBlock"sv): return vk::Format::eAstc8x6SfloatBlock;
    case fnv1a("Astc8x8SfloatBlock"sv): return vk::Format::eAstc8x8SfloatBlock;
    case fnv1a("Astc10x5SfloatBlock"sv): return vk::Format::eAstc10x5SfloatBlock;
    case fnv1a("Astc10x6SfloatBlock"sv): return vk::Format::eAstc10x6SfloatBlock;
    case fnv1a("Astc10x8SfloatBlock"sv): return vk::Format::eAstc10x8SfloatBlock;
    case fnv1a("Astc10x10SfloatBlock"sv): return vk::Format::eAstc10x10SfloatBlock;
    case fnv1a("Astc12x10SfloatBlock"sv): return vk::Format::eAstc12x10SfloatBlock;
    case fnv1a("Astc12x12SfloatBlock"sv): return vk::Format::eAstc12x12SfloatBlock;
    case fnv1a("Pvrtc12BppUnormBlockIMG"sv): return vk::Format::ePvrtc12BppUnormBlockIMG;
    case fnv1a("Pvrtc14BppUnormBlockIMG"sv): return vk::Format::ePvrtc14BppUnormBlockIMG;
    case fnv1a("Pvrtc22BppUnormBlockIMG"sv): return vk::Format::ePvrtc22BppUnormBlockIMG;
    case fnv1a("Pvrtc24BppUnormBlockIMG"sv): return vk::Format::ePvrtc24BppUnormBlockIMG;
    case fnv1a("Pvrtc12BppSrgbBlockIMG"sv): return vk::Format::ePvrtc12BppSrgbBlockIMG;
    case fnv1a("Pvrtc14BppSrgbBlockIMG"sv): return vk::Format::ePvrtc14BppSrgbBlockIMG;
    case fnv1a("Pvrtc22BppSrgbBlockIMG"sv): return vk::Format::ePvrtc22BppSrgbBlockIMG;
    case fnv1a("Pvrtc24BppSrgbBlockIMG"sv): return vk::Format::ePvrtc24BppSrgbBlockIMG;
    }
    return vk::Format::eUndefined;
}
