
import QtQuick 2.14
import photo_broom.qml 1.0


Item
{
    id: rootId

    GridView {
        id: photosViewId
        objectName: "photos_view"       // used by c++ part to find this view and set proper model

        property int thumbnailSize: 120
        property int thumbnailMargin: 2

        anchors.fill: parent

        delegate: delegateId
        cellWidth: thumbnailSize + thumbnailMargin
        cellHeight: thumbnailSize + thumbnailMargin
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

            width:  photosViewId.thumbnailSize
            height: photosViewId.thumbnailSize

            border.width: 1

            Photo {
                id: imageId
                anchors.centerIn: parent
                height: (photoProperties.width < photoProperties.height)?  photosViewId.thumbnailSize : photoProperties.height * photosViewId.thumbnailSize / nullProtection(photoProperties.width)
                width:  (photoProperties.width >= photoProperties.height)? photosViewId.thumbnailSize : photoProperties.width * photosViewId.thumbnailSize / nullProtection(photoProperties.height)

                thumbnails: thumbnailsManager.get()

                source: photoProperties.path

                function nullProtection(value) {
                    return value == 0? 1: value;
                }
            }
        }
    }
}
