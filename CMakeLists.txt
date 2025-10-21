cmake_minimum_required(VERSION 3.15)

project(PopupUI VERSION 1.0.0)

add_subdirectory(src/popupui/overlay)
add_subdirectory(test)

include(CMakePackageConfigHelpers)

add_custom_target(PopupUI ALL)
add_dependencies(PopupUI
    DialogOverlay
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PopupUIConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/PopupUIConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PopupUIConfig.cmake"
    INSTALL_DESTINATION lib/cmake/PopupUI
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PopupUIConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PopupUIConfigVersion.cmake"
    DESTINATION lib/cmake/PopupUI
)
