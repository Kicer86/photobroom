
import QtQuick
import quick_items
import photo_broom.singletons


Item {
    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl
        core: PhotoBroomProject.coreFactory
    }

    Component {
        id: staticImage

         ZoomableImage {
            anchors.fill: parent

            boundsBehavior: Flickable.StopAtBounds
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

    Component {
        id: equirectangularProjection

        EquirectangularView {
            anchors.fill: parent

            source: ctrl.path
        }
    }

    Loader {
        anchors.fill: parent

        sourceComponent: {
            switch (ctrl.mode) {
                case MediaViewCtrl.StaticImage:                     return staticImage;
                case MediaViewCtrl.Video:                           return video;
                case MediaViewCtrl.AnimatedImage:                   return animatedImage;
                case MediaViewCtrl.EquirectangularProjectionImage:  return equirectangularProjection;
                default:                                            return undefined;
            }
        }
    }
}
