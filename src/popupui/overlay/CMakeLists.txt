project(DialogOverlay)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(CMAKE_DEBUG_POSTFIX "d")

add_library(${PROJECT_NAME}
    dialogoverlay.h
    dialogoverlay.cpp
    dialogmask.h
    dialogmask.cpp

    animations/fadepopupanimation.h
    animations/fadepopupanimation.cpp
    animations/scalepopupanimation.h
    animations/scalepopupanimation.cpp
    animations/slidepopupanimation.h
    animations/slidepopupanimation.cpp
    animations/parallelpopupanimation.h
    animations/parallelpopupanimation.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT DialogOverlayTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/popupui/dialogoverlay
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/popupui/comm/
    DESTINATION include/popupui/comm
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/DialogOverlayConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/DialogOverlayConfig.cmake"
    INSTALL_DESTINATION lib/cmake/DialogOverlay
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DialogOverlayConfig.cmake"
    DESTINATION lib/cmake/DialogOverlay
)

install(EXPORT DialogOverlayTargets
    FILE DialogOverlayTargets.cmake
    NAMESPACE PopupUI::
    DESTINATION lib/cmake/DialogOverlay
)