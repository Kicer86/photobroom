
import QtQuick 2.15
import photo_broom.qml 1.0

Item {
    Flickable {
        id: flickablePhoto
        objectName: "flickablePhoto"

        property alias source: photo.source

        anchors.fill: parent

        boundsBehavior: Flickable.StopAtBounds
        contentWidth: wrapper.width
        contentHeight: wrapper.height

        readonly property int freeZoomMode: 0
        readonly property int zoomToFitMode: 1
        readonly property int fullZoomMode: 2

        property int zoomType: freeZoomMode

        function zoomToFit() {
            photo.zoomToFit();
        }

        Item {
            id: wrapper

            width: Math.max(flickablePhoto.width, photo.width * photo.scale)
            height: Math.max(flickablePhoto.height, photo.height * photo.scale)

            Picture {
                id: photo

                anchors.centerIn: parent

                width: implicitWidth
                height: implicitHeight

                function zoomToFit() {
                    photo.scale = flickablePhoto.width / photo.width;
                }
            }

            MouseArea {
                anchors.fill: parent

                onWheel: {
                    var pictureScale = photo.scale

                    if (wheel.angleDelta.y > 0 && pictureScale < 8) {
                        pictureScale *= 1.4;
                    }
                    else if (wheel.angleDelta.y < 0 && pictureScale > 1/8) {
                        pictureScale /= 1.4;
                    }

                    photo.scale = pictureScale;
                    flickablePhoto.zoomType = flickablePhoto.freeZoomMode
                }

                onDoubleClicked: {
                    if (flickablePhoto.zoomType !== flickablePhoto.zoomToFitMode)
                    {
                        photo.zoomToFit();
                        flickablePhoto.zoomType = flickablePhoto.zoomToFitMode
                    }
                    else
                    {
                        photo.scale = 1.0
                        flickablePhoto.zoomType = flickablePhoto.fullZoomMode
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
