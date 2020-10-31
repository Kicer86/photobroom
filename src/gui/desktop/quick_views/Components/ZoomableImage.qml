

import QtQuick 2.15
import photo_broom.qml 1.0

Flickable {
    id: flickableArea

    property alias source: photo.source

    contentWidth: area.width
    contentHeight: area.height

    function zoomToFit() {
        area.zoomToFit();
    }

    Item {
        id: area

        readonly property int freeZoomMode: 0
        readonly property int zoomToFitMode: 1
        readonly property int fullZoomMode: 2

        property int zoomType: freeZoomMode

        width: Math.max(flickableArea.width, photo.width * photo.scale)
        height: Math.max(flickableArea.height, photo.height * photo.scale)

        function zoomToFit() {
            photo.scale = flickableArea.width / photo.width;
        }

        Picture {
            id: photo

            anchors.centerIn: parent

            width: implicitWidth
            height: implicitHeight
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
                area.zoomType = area.freeZoomMode
            }

            onDoubleClicked: {
                if (area.zoomType !== area.zoomToFitMode)
                {
                    area.zoomToFit();
                    area.zoomType = area.zoomToFitMode
                }
                else
                {
                    photo.scale = 1.0
                    area.zoomType = area.fullZoomMode
                }
            }
        }
    }
}
