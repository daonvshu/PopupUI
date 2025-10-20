project(DialogOverlay)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets)

include_directories(${CMAKE_SOURCE_DIR}/src)

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