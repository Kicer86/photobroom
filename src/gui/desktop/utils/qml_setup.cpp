
#include <QQmlEngine>
#include <QQuickWindow>

#include "qml_setup.hpp"
#include "models/tags_model.hpp"
#include "models/photo_properties_model.hpp"
#include "models/duplicates_model.hpp"
#include "models/faces_model.hpp"
#include "models/photos_data_guesser.hpp"
#include "models/series_model.hpp"
#include "utils/variant_display.hpp"
#include "inotifications.hpp"


void register_qml_types()
{
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

    qmlRegisterType<PhotosDataGuesser>("photo_broom.models", 1, 0, "PhotosDataGuesser");
    qmlRegisterType<PhotoPropertiesModel>("photo_broom.models", 1, 0, "PhotoPropertiesModel");
    qmlRegisterType<DuplicatesModel>("photo_broom.models", 1, 0, "DuplicatesModel");
    qmlRegisterType<FacesModel>("photo_broom.models", 1, 0, "FacesModel");
    qmlRegisterType<SeriesModel>("photo_broom.models", 1, 0, "SeriesModel");
    qmlRegisterType<TagsModel>("photo_broom.models", 1, 0, "TagsModel");
    qmlRegisterType<Variant>("photo_broom.utils", 1, 0, "Variant");
    qRegisterMetaType<QAbstractItemModel*>("QAbstractItemModel*");
    qmlRegisterUncreatableMetaObject(Photo::staticMetaObject, "photo_broom.enums", 1, 0, "PhotoEnums", "Error: only enums");
    qmlRegisterUncreatableMetaObject(Tag::staticMetaObject, "photo_broom.enums", 1, 0, "TagEnums", "Error: only enums");
    qmlRegisterUncreatableMetaObject(INotifications::staticMetaObject, "photo_broom.enums", 1, 0, "NotificationEnums", "Error: only enums");
}
