
import QtQuick

import "internal/ZoomLogic.js" as Logic
import QmlItems


Flickable {
    id: flickableArea

    property alias source: image.source
    default property alias imageLayer: image.data

    contentWidth: area.width
    contentHeight: area.height

    function zoomToFit() {
        area.zoomToFit();
    }

    onHeightChanged: area.availableAreaChanged()
    onWidthChanged: area.availableAreaChanged()

    Item {
        id: area

        width: Math.max(flickableArea.width, image.width * image.scale)
        height: Math.max(flickableArea.height, image.height * image.scale)

        readonly property int freeZoomMode: 0
        readonly property int zoomToFitMode: 1
        readonly property int fullZoomMode: 2

        property int zoomType: freeZoomMode
        property double zoomToFitScale: 1.0

        function calculateZoomToFitScale() {
            if (image.width == 0 || image.height == 0)
                return;

            if (image.width > image.height)
                zoomToFitScale = flickableArea.width / image.width;
            else
                zoomToFitScale = flickableArea.height / image.height;
        }

        function availableAreaChanged() {
            area.calculateZoomToFitScale()

            if (zoomType === area.zoomToFitMode)
                zoomToFit()
        }

        function zoomToFit() {
            image.scale = area.zoomToFitScale
            area.zoomType = area.zoomToFitMode
        }

        function pointInView(mouse) {
            var p = Qt.point(mouse.x, mouse.y)

            p.x -= flickableArea.contentX
            p.y -= flickableArea.contentY

            return p
        }

        function imageView() {
            var v = Qt.rect(flickableArea.contentX, flickableArea.contentY,
                            flickableArea.width, flickableArea.height)

            if (image.width * image.scale < flickableArea.width)
                v.x -= (area.width - image.width * image.scale) / 2

            if (image.height * image.scale < flickableArea.height)
                v.y -= (area.height - image.height * image.scale) / 2

            return v
        }

        function imageSize() {
            var s = Qt.size(image.width * image.scale, image.height * image.scale)

            return s
        }

        function followMouse(scaleDetla, mouse, view, image)
        {
            var offset = Logic.scaleOffsets(scaleDetla, mouse, view, image)

            offset.x = Math.max(offset.x, 0);                                               // eliminate negative numbers
            offset.x = Math.min(offset.x, image.width * scaleDetla - flickableArea.width)   // eliminate values above edge

            offset.y = Math.max(offset.y, 0);                                               // eliminate negative numbers
            offset.y = Math.min(offset.y, image.height * scaleDetla - flickableArea.height) // eliminate values above edge

            // if image is smaller than area then do not use any offsets as image is autoaligned in center in such cases
            if (image.height * scaleDetla <= view.height)
                offset.y = 0;

            if (image.width * scaleDetla <= view.width)
                offset.x = 0;

            flickableArea.contentX = offset.x
            flickableArea.contentY = offset.y
        }

        Image {
            id: image
            anchors.centerIn: parent

            width: sourceSize.width
            height: sourceSize.height
            asynchronous: true
            autoTransform: true

            onStatusChanged: {
                if (image.status == Image.Ready) {
                    image.width = sourceSize.width;
                    image.height = sourceSize.height;
                    area.calculateZoomToFitScale();
                    area.zoomToFit();
                }
            }
        }

        MouseArea {
            anchors.fill: parent

            onWheel: {
                var pictureScale = image.scale

                if (wheel.angleDelta.y > 0 && pictureScale < 8)
                    pictureScale *= 1.4;
                else if (wheel.angleDelta.y < 0)
                    pictureScale /= 1.4;

                if (pictureScale < area.zoomToFitScale)
                    pictureScale = area.zoomToFitScale

                var currentScale = image.scale
                var point = area.pointInView(wheel)
                var v = area.imageView()
                var is = area.imageSize()

                image.scale = pictureScale;
                area.zoomType = area.freeZoomMode

                area.followMouse(image.scale/currentScale, point, v, is);
            }

            onDoubleClicked: {
                if (area.zoomType !== area.zoomToFitMode) {
                    flickableArea.contentX = 0;
                    flickableArea.contentY = 0;

                    area.zoomToFit();
                    area.zoomType = area.zoomToFitMode
                } else {
                    var currentScale = image.scale;
                    var point = area.pointInView(mouse)
                    var v = area.imageView()
                    var is = area.imageSize()

                    image.scale = 1.0
                    area.zoomType = area.fullZoomMode

                    area.followMouse(image.scale/currentScale, point, v, is);
                }
            }
        }
    }
}
