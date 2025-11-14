#pragma once

#include <QtCore/QtGlobal>

#define POPUPUI_BEGIN_NAMESPACE namespace PopupUI {
#define POPUPUI_END_NAMESPACE }
#define POPUPUI_USE_NAMESPACE using namespace PopupUI;

#ifndef POPUPUI_EXPORT
#  ifdef POPUPUI_STATIC
#    define POPUPUI_EXPORT
#  else
#    ifdef POPUPUI_LIBRARY
#      define POPUPUI_EXPORT Q_DECL_EXPORT
#    else
#      define POPUPUI_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif