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

function(target_compile_shaders TARGET)
    set(SPIRV_BINARY_FILES)

    foreach(SOURCE_FILE ${ARGN})
        set(SPIRV "${SOURCE_FILE}.spv")
        add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND glslc ${SOURCE_FILE} -o ${SPIRV}
            DEPENDS ${SOURCE_FILE}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
    endforeach()

    add_custom_target(${TARGET}_shaders DEPENDS ${SPIRV_BINARY_FILES})
    add_dependencies(${TARGET} ${TARGET}_shaders)
endfunction()

add_library(imgui STATIC
    deps/imgui/imgui.h
    deps/imgui/imgui.cpp
    deps/imgui/imconfig.h
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_internal.h
    deps/imgui/imgui_widgets.cpp
    deps/imgui/misc/cpp/imgui_stdlib.h
    deps/imgui/misc/cpp/imgui_stdlib.cpp

    deps/imgui/backends/imgui_impl_glfw.h
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_vulkan.h
    deps/imgui/backends/imgui_impl_vulkan.cpp

    deps/imgui/imgui_demo.cpp
)
set_target_properties(imgui PROPERTIES CXX_EXTENSIONS OFF CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(imgui PUBLIC deps/imgui)
target_compile_definitions(imgui PUBLIC -DIMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)

add_library(Loop STATIC src/LoopEngine/Input/InputSystem.cpp src/LoopEngine/Core/DisableCopyAndMove.hpp src/LoopEngine/Graphics/Graphics.cpp src/LoopEngine/Camera/CameraSystem.cpp src/LoopEngine/Event/EventSystem.cpp src/LoopEngine/Platform/Display.cpp src/LoopEngine/Platform/Display.hpp src/LoopEngine/Camera/CameraSystem.hpp src/LoopEngine/Input/InputController.hpp src/LoopEngine/Input/InputSystem.hpp src/LoopEngine/Event/EventSystem.hpp src/LoopEngine/Graphics/Context.cpp src/LoopEngine/Graphics/Context.hpp src/LoopEngine/Graphics/Material.cpp src/LoopEngine/Graphics/Material.hpp src/LoopEngine/Graphics/Texture.hpp src/LoopEngine/Camera/Camera.hpp src/LoopEngine/Graphics/Graphics.hpp src/LoopEngine/Core/Singleton.hpp src/LoopEngine/Graphics/Texture.cpp src/LoopEngine/Event/Delegate.hpp src/LoopEngine/Graphics/UniformBuffer.cpp src/LoopEngine/Graphics/UniformBuffer.hpp src/LoopEngine/Graphics/IndexBuffer.cpp src/LoopEngine/Graphics/IndexBuffer.hpp src/LoopEngine/Graphics/VertexBuffer.cpp src/LoopEngine/Graphics/VertexBuffer.hpp src/LoopEngine/Event/EventHandler.hpp src/LoopEngine/Application.cpp src/LoopEngine/Application.hpp)
set_target_properties(Loop PROPERTIES CXX_EXTENSIONS OFF CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(Loop PUBLIC glm glfw imgui range-v3 spdlog yaml-cpp Vulkan::Vulkan VulkanMemoryAllocator)
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
file(GLOB SHADER_FILES assets/shaders/*.vert assets/shaders/*.frag)
target_compile_shaders(Loop ${SHADER_FILES})

add_subdirectory(examples/ParticleSystem)