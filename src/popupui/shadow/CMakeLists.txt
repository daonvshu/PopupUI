project(DialogShadow)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(CMAKE_DEBUG_POSTFIX "d")

if (POPUPUI_BUILD_STATIC)
    set(_build_type STATIC)
else ()
    set(_build_type SHARED)
endif ()

add_library(${PROJECT_NAME} ${_build_type}
    shadowwidget.h
    shadowwidget.cpp
    basepopupdlg.h
    basepopupdlg.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE
        DialogAnimations
)

if (POPUPUI_BUILD_STATIC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC POPUPUI_STATIC)
else ()
    target_compile_definitions(${PROJECT_NAME} PRIVATE POPUPUI_LIBRARY)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT DialogShadowTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/popupui/shadow
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/popupui/comm/
    DESTINATION include/popupui/comm
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/DialogShadowConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/DialogShadowConfig.cmake"
    INSTALL_DESTINATION lib/cmake/DialogShadow
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DialogShadowConfig.cmake"
    DESTINATION lib/cmake/DialogShadow
)

install(EXPORT DialogShadowTargets
    FILE DialogShadowTargets.cmake
    NAMESPACE PopupUI::
    DESTINATION lib/cmake/DialogShadow
)