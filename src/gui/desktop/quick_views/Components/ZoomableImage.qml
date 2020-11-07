

import QtQuick 2.15
import "internal/ZoomLogic.js" as Logic
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

        function pointInView(mouse) {
            var p = Qt.point(mouse.x, mouse.y)

            p.x -= flickableArea.contentX
            p.y -= flickableArea.contentY

            if (photo.width * photo.scale < area.width)
                p.x -= (area.width - photo.width * photo.scale) / 2

            if (photo.height * photo.scale < area.heigh)
                p.y -= (area.height - photo.height * photo.scale) / 2

            return p
        }

        function imageView() {
            var v = Qt.rect(flickableArea.contentX, flickableArea.contentY,
                            flickableArea.width, flickableArea.height)

            return v
        }

        function imageSize() {
            var s = Qt.size(photo.width * photo.scale, photo.height * photo.scale)

            // when image is smaller than view then pretend it to be equal to view
            //s.width  = Math.max(s.width, area.width)
            //s.height = Math.max(s.height, area.height)

            return s
        }

        function followMouse(scaleDetla, mouse, view, image)
        {
            var offset = Logic.scaleOffsets(scaleDetla, mouse, view, image)

            offset.x = Math.max(offset.x, 0);                                               // eliminate negative numbers
            offset.x = Math.min(offset.x, image.width * scaleDetla - flickableArea.width)   // eliminate values above edge

            offset.y = Math.max(offset.y, 0);                                               // eliminate negative numbers
            offset.y = Math.min(offset.y, image.height * scaleDetla - flickableArea.height) // eliminate values above edge

            // if photo is smaller than area then do not use any offsets as photo is autoaligned in center in such cases
            if (image.height * scaleDetla <= view.height)
                offset.y = 0;

            if (image.width * scaleDetla <= view.width)
                offset.x = 0;

            flickableArea.contentX = offset.x
            flickableArea.contentY = offset.y
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
                } else if (wheel.angleDelta.y < 0 && pictureScale > 1/8) {
                    pictureScale /= 1.4;
                }

                var currentScale = photo.scale
                var point = area.pointInView(wheel)
                var v = area.imageView()
                var is = area.imageSize()

                photo.scale = pictureScale;
                area.zoomType = area.freeZoomMode

                area.followMouse(photo.scale/currentScale, point, v, is);
            }

            onDoubleClicked: {
                if (area.zoomType !== area.zoomToFitMode) {
                    flickableArea.contentX = 0;
                    flickableArea.contentY = 0;

                    area.zoomToFit();
                    area.zoomType = area.zoomToFitMode
                } else {
                    var currentScale = photo.scale;
                    var currentXoffset = flickableArea.contentX;
                    var currentYoffset = flickableArea.contentY;

                    if (photo.height * photo.scale < area.height)
                        currentYoffset = -(area.height - photo.height * photo.scale)/2;

                    if (photo.width * photo.scale < area.width)
                        currentXoffset = -photo.x;

                    photo.scale = 1.0
                    area.zoomType = area.fullZoomMode

                    area.followMouse(currentScale, photo.scale, mouse.x, mouse.y, currentXoffset, currentYoffset);
                }
            }
        }
    }
}
