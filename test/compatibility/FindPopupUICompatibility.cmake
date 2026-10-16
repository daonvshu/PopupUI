if(POPUPUI_COMPAT_PACKAGE STREQUAL "DialogOverlay")
    find_package(DialogOverlay REQUIRED CONFIG)
elseif(POPUPUI_COMPAT_PACKAGE STREQUAL "DialogShadow")
    find_package(DialogShadow REQUIRED CONFIG)
elseif(POPUPUI_COMPAT_PACKAGE STREQUAL "DialogAnimations")
    find_package(DialogAnimations REQUIRED CONFIG)
else()
    find_package(PopupUI 1.1.0 REQUIRED COMPONENTS DialogOverlay)
endif()

if(NOT TARGET PopupUI::DialogAnimations)
    message(FATAL_ERROR "The animation dependency was not imported automatically")
endif()
if(NOT TARGET Qt5::Widgets AND NOT TARGET Qt6::Widgets)
    message(FATAL_ERROR "The Qt dependency was not imported automatically")
endif()

if(TARGET PopupUI::DialogOverlay)
    get_target_property(overlay_links PopupUI::DialogOverlay INTERFACE_LINK_LIBRARIES)
    if(NOT "PopupUI::DialogAnimations" IN_LIST overlay_links)
        message(FATAL_ERROR "DialogOverlay does not expose its animation dependency")
    endif()

    get_target_property(overlay_includes PopupUI::DialogOverlay INTERFACE_INCLUDE_DIRECTORIES)
    list(GET overlay_includes 0 include_root)
    foreach(header IN ITEMS
        popupui/dialogoverlay/dialogoverlay.h
        popupui/dialogoverlay/dialogmask.h
        popupui/dialogoverlay/popupanimation.h
        popupui/dialogoverlay/popupproperty.h
        popupui/dialogoverlay/slidedirection.h
        popupui/dialogoverlay/animations/fadepopupanimation.h
        popupui/dialogoverlay/animations/parallelpopupanimation.h
        popupui/dialogoverlay/animations/scalepopupanimation.h
        popupui/dialogoverlay/animations/slidepopupanimation.h
        popupui/overlay/animations/fadepopupanimation.h
        popupui/overlay/animations/parallelpopupanimation.h
        popupui/overlay/animations/scalepopupanimation.h
        popupui/overlay/animations/slidepopupanimation.h
        popupui/overlay/dialogoverlay.h
        popupui/animations/fadepopupanimation.h
        popupui/animations/parallelpopupanimation.h
        popupui/animations/scalepopupanimation.h
        popupui/animations/slidepopupanimation.h
    )
        if(NOT EXISTS "${include_root}/${header}")
            message(FATAL_ERROR "Missing installed compatibility header: ${header}")
        endif()
    endforeach()
endif()

set(PopupUICompatibility_FOUND TRUE)
