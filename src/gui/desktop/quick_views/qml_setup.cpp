
#include <database/database_tools/photos_data_guesser.hpp>

#include "context_menu_manager.hpp"
#include "media_view_ctrl.hpp"
#include "objects_accessor.hpp"
#include "photo_data_qml.hpp"
#include "photos_model_controller_component.hpp"
#include "picture_item.hpp"
#include "selection_manager_component.hpp"
#include "qml_setup.hpp"
#include "models/flat_model.hpp"


void register_qml_types()
{
    qmlRegisterType<ContextMenuManager>("photo_broom.qml", 1, 0, "ContextMenuManager");
    qmlRegisterType<ObjectsAccessor>("photo_broom.qml", 1, 0, "PhotoBroomProject");
    qmlRegisterType<MediaViewCtrl>("photo_broom.qml", 1, 0, "MediaViewCtrl");
    qmlRegisterType<PictureItem>("photo_broom.qml", 1, 0, "Picture");
    qmlRegisterType<PhotosModelControllerComponent>("photo_broom.qml", 1, 0, "PhotosModelController");
    qmlRegisterType<SelectionManagerComponent>("photo_broom.qml", 1, 0, "SelectionManager");
    qmlRegisterType<PhotosDataGuesser>("photo_broom.qml", 1, 0, "PhotosDataGuesser");
    qmlRegisterType<PhotoDataQml>("photo_broom.qml", 1, 0, "PhotoDataQml");
    qRegisterMetaType<QAbstractItemModel*>("QAbstractItemModel*");
    qmlRegisterInterface<Database::IDatabase>("Database::IDatabase", 1);
    qmlRegisterInterface<FlatModel>("FlatModel", 1);
    qmlRegisterUncreatableMetaObject(Photo::staticMetaObject, "photo_broom.qml", 1, 0, "PhotoEnums", "Error: only enums");
}
