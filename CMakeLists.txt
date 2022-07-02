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

add_subdirectory(Tools)

add_library(Loop STATIC LoopEngine/Input/InputSystem.cpp LoopEngine/Core/DisableCopyAndMove.hpp LoopEngine/Graphics/Graphics.cpp LoopEngine/Camera/CameraSystem.cpp LoopEngine/Event/EventSystem.cpp LoopEngine/Platform/Window.cpp LoopEngine/Platform/Window.hpp LoopEngine/Camera/CameraSystem.hpp LoopEngine/Input/InputSystem.hpp LoopEngine/Event/EventSystem.hpp LoopEngine/Graphics/Context.cpp LoopEngine/Graphics/Context.hpp LoopEngine/Graphics/Material.cpp LoopEngine/Graphics/Material.hpp LoopEngine/Graphics/Texture.hpp LoopEngine/Camera/Camera.hpp LoopEngine/Graphics/Graphics.hpp LoopEngine/Core/Singleton.hpp LoopEngine/Graphics/Texture.cpp LoopEngine/Event/Delegate.hpp LoopEngine/Graphics/UniformBuffer.cpp LoopEngine/Graphics/UniformBuffer.hpp LoopEngine/Graphics/IndexBuffer.cpp LoopEngine/Graphics/IndexBuffer.hpp LoopEngine/Graphics/VertexBuffer.cpp LoopEngine/Graphics/VertexBuffer.hpp LoopEngine/Event/EventHandler.hpp LoopEngine/Application.cpp LoopEngine/Application.hpp LoopEngine/Asset/AssetSystem.cpp LoopEngine/Asset/AssetSystem.hpp LoopEngine/Lifecycle.hpp LoopEngine/VulkanEnums.cpp LoopEngine/VulkanEnums.hpp)
set_target_properties(Loop PROPERTIES CXX_EXTENSIONS OFF CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(Loop PUBLIC glm glfw range-v3 spdlog yaml-cpp Vulkan::Vulkan VulkanMemoryAllocator)
target_include_directories(Loop PUBLIC .)
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
