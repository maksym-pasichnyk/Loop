add_executable(AssetBuilder src/main.cpp)
set_target_properties(AssetBuilder PROPERTIES CXX_EXTENSIONS OFF CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(AssetBuilder spdlog yaml-cpp)

function(target_compile_assets TARGET)
    get_target_property(TARGET_SOURCE_DIR ${TARGET} SOURCE_DIR)

    set(ASSETS_DIR ${TARGET_SOURCE_DIR}/assets)
    file(GLOB_RECURSE ASSETS_FILES ${TARGET_SOURCE_DIR}/assets/*)

    set(ASSETS_YAML_FILE "${CMAKE_CURRENT_BINARY_DIR}/assets.yaml")
    set(ASSETS_BINARY_FILE "${CMAKE_CURRENT_BINARY_DIR}/assets.bin")

    add_custom_command(OUTPUT "${ASSETS_BINARY_FILE}" "${ASSETS_YAML_FILE}"
        COMMAND AssetBuilder "${ASSETS_BINARY_FILE}" "${ASSETS_YAML_FILE}" "${ASSETS_DIR}" ${ASSETS_FILES}
        DEPENDS AssetBuilder ${ASSETS_FILES}
        COMMENT "Compile assets"
    )

    add_custom_target(${TARGET}_assets ALL DEPENDS "${ASSETS_BINARY_FILE}" "${ASSETS_YAML_FILE}")
    add_dependencies(${TARGET} ${TARGET}_assets)
endfunction()