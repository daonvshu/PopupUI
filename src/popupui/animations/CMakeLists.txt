project(DialogAnimations)

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
    fadepopupanimation.h
    fadepopupanimation.cpp
    scalepopupanimation.h
    scalepopupanimation.cpp
    slidepopupanimation.h
    slidepopupanimation.cpp
    parallelpopupanimation.h
    parallelpopupanimation.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
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
    EXPORT DialogAnimationsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/popupui/animations
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/popupui/comm/
    DESTINATION include/popupui/comm
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/DialogAnimationsConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/DialogAnimationsConfig.cmake"
    INSTALL_DESTINATION lib/cmake/DialogAnimations
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DialogAnimationsConfig.cmake"
    DESTINATION lib/cmake/DialogAnimations
)

install(EXPORT DialogAnimationsTargets
    FILE DialogAnimationsTargets.cmake
    NAMESPACE PopupUI::
    DESTINATION lib/cmake/DialogAnimations
)