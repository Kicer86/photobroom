

#include "qml_setup.hpp"

#include "photo_item.hpp"
#include "photos_model_controller_component.hpp"
#include "picture_item.hpp"
#include "selection_manager_component.hpp"
#include "models/flat_model.hpp"


void register_qml_types()
{
    qmlRegisterType<PhotoItem>("photo_broom.qml", 1, 0, "Photo");
    qmlRegisterType<PictureItem>("photo_broom.qml", 1, 0, "Picture");
    qmlRegisterType<PhotosModelControllerComponent>("photo_broom.qml", 1, 0, "PhotosModelController");
    qmlRegisterType<SelectionManagerComponent>("photo_broom.qml", 1, 0, "SelectionManager");
    qmlRegisterInterface<IThumbnailsManager>("IThumbnailsManager");
    qmlRegisterInterface<FlatModel>("FlatModel");
    qRegisterMetaType<QAbstractItemModel*>("QAbstractItemModel*");
    qmlRegisterUncreatableMetaObject(Photo::staticMetaObject, "photo_broom.qml", 1, 0, "PhotoEnums", "Error: only enums");
}
