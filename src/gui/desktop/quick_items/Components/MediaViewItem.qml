
import QtQuick
import quick_items


Item {
    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl
    }

    Component {
        id: staticImage

        ZoomableImage {
            anchors.fill: parent

            source: ctrl.path
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

    Loader {
        anchors.fill: parent

        sourceComponent: {
            switch (ctrl.mode) {
                case MediaViewCtrl.StaticImage:   return staticImage;
                case MediaViewCtrl.Video:         return video;
                case MediaViewCtrl.AnimatedImage: return animatedImage;
                default:                          return undefined;
            }
        }
    }
}
