
import QtQuick

import photo_broom.models
import photo_broom.singletons


TableView {
    id: root

    property var photos: []

    implicitHeight: contentHeight
    implicitWidth: contentWidth
    columnSpacing: 5

    model: PhotoPropertiesModel {
        property var _photos: root.photos
        database: PhotoBroomProject.database

        on_PhotosChanged: setPhotos(_photos)
    }

    delegate: Text {
        text: display
    }
}
