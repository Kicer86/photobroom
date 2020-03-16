
#ifndef QML_SETUP_HPP
#define QML_SETUP_HPP

#include "quick_views_export.h"
#include "../ui_utils/qml_utils.hpp"

QUICK_VIEWS_EXPORT void register_qml_types();

struct IThumbnailsManager;
INVOKABLE_WRAPPER_FOR_INTERFACE(IThumbnailsManager);

#endif
