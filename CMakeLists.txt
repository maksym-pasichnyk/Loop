cmake_minimum_required(VERSION 3.21)
project(Loop)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

find_package(Vulkan REQUIRED)

set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
set(VMA_DYNAMIC_VULKAN_FUNCTIONS ON)

add_subdirectory(deps/glm)
add_subdirectory(deps/glfw)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/yaml-cpp)
add_subdirectory(deps/range-v3)
add_subdirectory(deps/VulkanMemoryAllocator)

add_subdirectory(AssetBuilder)

add_library(Loop STATIC src/LoopEngine/Input/InputSystem.cpp src/LoopEngine/Core/DisableCopyAndMove.hpp src/LoopEngine/Graphics/Graphics.cpp src/LoopEngine/Camera/CameraSystem.cpp src/LoopEngine/Event/EventSystem.cpp src/LoopEngine/Platform/Display.cpp src/LoopEngine/Platform/Display.hpp src/LoopEngine/Camera/CameraSystem.hpp src/LoopEngine/Input/InputController.hpp src/LoopEngine/Input/InputSystem.hpp src/LoopEngine/Event/EventSystem.hpp src/LoopEngine/Graphics/Context.cpp src/LoopEngine/Graphics/Context.hpp src/LoopEngine/Graphics/Material.cpp src/LoopEngine/Graphics/Material.hpp src/LoopEngine/Graphics/Texture.hpp src/LoopEngine/Camera/Camera.hpp src/LoopEngine/Graphics/Graphics.hpp src/LoopEngine/Core/Singleton.hpp src/LoopEngine/Graphics/Texture.cpp src/LoopEngine/Event/Delegate.hpp src/LoopEngine/Graphics/UniformBuffer.cpp src/LoopEngine/Graphics/UniformBuffer.hpp src/LoopEngine/Graphics/IndexBuffer.cpp src/LoopEngine/Graphics/IndexBuffer.hpp src/LoopEngine/Graphics/VertexBuffer.cpp src/LoopEngine/Graphics/VertexBuffer.hpp src/LoopEngine/Event/EventHandler.hpp src/LoopEngine/Application.cpp src/LoopEngine/Application.hpp src/LoopEngine/Asset/AssetSystem.cpp src/LoopEngine/Asset/AssetSystem.hpp src/LoopEngine/Vulkan/Enums.cpp src/LoopEngine/Vulkan/Enums.hpp)
set_target_properties(Loop PROPERTIES CXX_EXTENSIONS OFF CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(Loop PUBLIC glm glfw range-v3 spdlog yaml-cpp Vulkan::Vulkan VulkanMemoryAllocator)
target_include_directories(Loop PUBLIC src)
target_compile_definitions(Loop PUBLIC
    -D_USE_MATH_DEFINES
    -DGLM_FORCE_XYZW_ONLY
    -DGLM_FORCE_DEFAULT_ALIGNED_GENTYPES
    -DVK_NO_PROTOTYPES
    -DVK_USE_PLATFORM_MACOS_MVK
    -DGLFW_INCLUDE_NONE
    -DGLFW_INCLUDE_VULKAN
)

add_subdirectory(Plugins/ImGui)
add_subdirectory(examples/ParticleSystem)
