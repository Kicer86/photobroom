
import QtQuick
import quick_items


Item {
    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl
    }

    Component {
        id: staticImage

        PhotoSphere {
            id: sphere
            visible: true
            anchors.fill: parent
            image: ctrl.path

            scale: 1
            longitude: 0
            latitude: 0.5

            MouseArea {
                id: ma
                anchors.fill: parent
                property var clickedPos;
                property var clickedAzimuth;
                property var clickedElevation;
                property var clickedFoV;
                onPressed: {
                    clickedPos = Qt.point(mouseX, mouseY)
                    clickedAzimuth = sphere.azimuth
                    clickedElevation = sphere.elevation
                }
                onPositionChanged: {
                    var curpos = Qt.point(mouseX, mouseY)
                    var posDiff = Qt.point(curpos.x - ma.clickedPos.x, curpos.y - ma.clickedPos.y)

                    sphere.azimuth = clickedAzimuth + posDiff.x / 6.0
                    sphere.elevation = clickedElevation + posDiff.y / 6.0
                    console.log(sphere.azimuth, sphere.elevation)
                }
                onWheel: {
                    if (wheel.modifiers & Qt.ControlModifier) {
                        sphere.fieldOfView +=  wheel.angleDelta.y / 120;
                        console.log(sphere.fieldOfView)
                    }
                }
            }
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
