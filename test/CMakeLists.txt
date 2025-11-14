project(PopupUITest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets)
find_package(QLogCollector REQUIRED COMPONENTS Server)

add_compile_definitions(ROOT_PROJECT_PATH="${CMAKE_SOURCE_DIR}")

add_executable(${PROJECT_NAME} WIN32
    main.cpp

    overlay/overlaytest.h
    overlay/overlaytest.cpp
    overlay/overlaydlg1.h
    overlay/overlaydlg1.cpp
    overlay/overlaydlg2.h
    overlay/overlaydlg2.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets

    QLogCollector::Server
    DialogOverlay
)