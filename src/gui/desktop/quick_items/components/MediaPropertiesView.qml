
import QtQuick
import QtQuick.Controls

import photo_broom.models
import photo_broom.singletons


Item {

    property alias photos: view.photos

    implicitHeight: view.contentHeight
    implicitWidth: view.contentWidth

    TableView {
        id: view

        property var photos: []

        anchors.fill: parent

        implicitHeight: contentHeight
        implicitWidth: contentWidth
        columnSpacing: 5

        enabled: propertiesModel.busy === false
        opacity: enabled? 1: 0.5

        Behavior on opacity { PropertyAnimation {} }

        model: PhotoPropertiesModel {
            id: propertiesModel

            property var _photos: view.photos
            database: PhotoBroomProject.database

            on_PhotosChanged: setPhotos(_photos)
        }

        delegate: Text {
            text: display
        }
    }

    BusyIndicator {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        running: propertiesModel.busy
    }
}
