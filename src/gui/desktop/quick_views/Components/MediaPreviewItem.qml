
import QtQuick 2.15
import QtMultimedia
import photo_broom.qml 1.0


Item {
    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl
    }

    Component {
        id: staticImage

        StaticImageThumbnail {

            anchors.fill: parent
            opacity: 0

            photoID: ctrl.photoID

            thumbnails: thumbnailsManager.get()
        }
    }

    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.StaticImage? staticImage: undefined; anchors.fill: parent }
}

