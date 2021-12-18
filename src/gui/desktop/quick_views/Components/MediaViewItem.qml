
import QtQuick
import photo_broom.qml 1.0


Item {
    property alias photoID: ctrl.photoID

    // disable view
    function reset() {
        ctrl.reset();
    }

    MediaViewCtrl {
        id: ctrl
    }

    Component {
        id: staticImage

        Image {
            anchors.fill: parent

            source: ctrl.path
            asynchronous: true
            autoTransform: true
            fillMode: Image.PreserveAspectFit
        }
    }

    Component {
        id: video

        VideoPlayer {
            anchors.fill: parent

            source: ctrl.path
            fillMode: Image.PreserveAspectFit
        }
    }

    Component {
        id: animatedImage

        AnimatedImage {
            anchors.fill: parent

            source: ctrl.path
            asynchronous: true
            autoTransform: true
            fillMode: Image.PreserveAspectFit
        }
    }

    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.StaticImage?   staticImage:   undefined; anchors.fill: parent }
    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.Video?         video:         undefined; anchors.fill: parent }
    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.AnimatedImage? animatedImage: undefined; anchors.fill: parent }
}
