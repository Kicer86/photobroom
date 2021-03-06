
#ifndef QML_SETUP_HPP
#define QML_SETUP_HPP

#include "quick_views_export.h"
#include "qml_utils.hpp"

QUICK_VIEWS_EXPORT void register_qml_types();

struct IThumbnailsManager;
INVOKABLE_ACCESSOR_FOR_INTERFACE(IThumbnailsManager);

#endif
