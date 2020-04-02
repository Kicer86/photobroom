

#include "qml_setup.hpp"

#include "photo_item.hpp"
#include "photos_model_component.hpp"
#include "models/flat_model.hpp"


void register_qml_types()
{
    qmlRegisterType<PhotoItem>("photo_broom.qml", 1, 0, "Photo");
    qmlRegisterType<PhotosModelComponent>("photo_broom.qml", 1, 0, "PhotosModel");
    qmlRegisterInterface<IThumbnailsManager>("IThumbnailsManager");
    qmlRegisterInterface<FlatModel>("FlatModel");
}
