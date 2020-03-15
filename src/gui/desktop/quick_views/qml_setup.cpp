

#include "qml_setup.hpp"

#include "photo_item.hpp"
#include "config.hpp"

#define QML_NAMESPACE PHOTO_BROOM_NAMESPACE ".qml"


void register_qml_types()
{
    qmlRegisterType<PhotoItem>(QML_NAMESPACE, 1, 0, "PhotoItem");
}
