
import QtQuick 2.14
import photo_broom.qml 1.0


Item
{
    id: rootId

    FlowView {
        id: photosViewId
        objectName: "photos_view"       // used by c++ part to find this view and set proper model

        anchors.fill: parent

        delegate: delegateId
    }

    ScrollBar {
        id: verticalScrollBarId
        width: 12; height: photosViewId.height-12
        anchors.right: photosViewId.right
        opacity: 1
        orientation: Qt.Vertical
        position: photosViewId.visibleArea.yPosition
        pageSize: photosViewId.visibleArea.heightRatio
    }

    Component {
        id: delegateId

        Rectangle {
            id: rectId

            width:  if (imageId.width + 5 < 60) 60; else imageId.width + 5
            height: 125
            border.width: 1

            Photo {
                id: imageId
                anchors.centerIn: parent
                height: 120
                width: 100

                thumbnails: thumbnailsManager.get()

                source: photoPath
                //sourceSize.height: 120
                //asynchronous: true
            }
        }
    }
}
