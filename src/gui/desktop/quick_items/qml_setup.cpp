
#include <QQuickWindow>

#include <database/database_tools/photos_data_guesser.hpp>

#include "objects_accessor.hpp"
#include "qml_setup.hpp"
#include "models/flat_model.hpp"
#include "models/tags_model.hpp"
#include "models/photo_properties_model.hpp"
#include "utils/variant_display.hpp"


void register_qml_types()
{
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

    qmlRegisterType<PhotosDataGuesser>("photo_broom.database", 1, 0, "PhotosDataGuesser");
    qmlRegisterType<PhotoPropertiesModel>("photo_broom.models", 1, 0, "PhotoPropertiesModel");
    qmlRegisterType<TagsModel>("photo_broom.models", 1, 0, "TagsModel");
    qmlRegisterType<Variant>("photo_broom.utils", 1, 0, "Variant");
    qRegisterMetaType<QAbstractItemModel*>("QAbstractItemModel*");
    qmlRegisterInterface<Database::IDatabase>("Database::IDatabase", 1);
    qmlRegisterInterface<FlatModel>("FlatModel", 1);
    qmlRegisterUncreatableMetaObject(Photo::staticMetaObject, "photo_broom.enums", 1, 0, "PhotoEnums", "Error: only enums");
    qmlRegisterUncreatableMetaObject(Tag::staticMetaObject, "photo_broom.enums", 1, 0, "TagEnums", "Error: only enums");
}
