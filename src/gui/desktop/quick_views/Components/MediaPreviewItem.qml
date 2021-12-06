
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

    Component {
        id: staticImage

        Waiter {
            busyIndicator: busyId
            StaticImageThumbnail {
                anchors.fill: parent

                photoID: ctrl.photoID
                thumbnails: thumbnailsManager.get()
            }
        }
    }

    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.StaticImage? staticImage: undefined; anchors.fill: parent }
}

