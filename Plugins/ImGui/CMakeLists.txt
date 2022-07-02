add_library(ImGui STATIC
    src/ImGuiPlugin.cpp
    src/ImGuiPlugin.hpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui.h
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imconfig.h
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_internal.h
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/misc/cpp/imgui_stdlib.h
    ${PROJECT_SOURCE_DIR}/deps/imgui/misc/cpp/imgui_stdlib.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends/imgui_impl_glfw.h
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends/imgui_impl_vulkan.h
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends/imgui_impl_vulkan.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_demo.cpp
)
set_target_properties(ImGui PROPERTIES CXX_EXTENSIONS OFF CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(ImGui PUBLIC src ${PROJECT_SOURCE_DIR}/deps/imgui)
target_compile_definitions(ImGui PUBLIC -DIMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(ImGui PUBLIC Loop)
