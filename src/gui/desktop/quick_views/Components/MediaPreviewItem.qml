
import QtQuick 2.15
import QtMultimedia
import QtQuick.Controls
import photo_broom.qml 1.0


Item {
    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl
    }

    BusyIndicator {
        id: busyId

        anchors.centerIn: parent
    }

    Waiter {
        anchors.fill: parent

        busyIndicator: busyId

        StaticImageThumbnail {
            anchors.fill: parent

            photoID: ctrl.photoID
            thumbnails: thumbnailsManager.get()
        }
    }
}

