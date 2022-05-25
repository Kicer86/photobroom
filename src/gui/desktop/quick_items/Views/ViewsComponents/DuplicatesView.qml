
import QtQuick

import "../../Components" as Components
import quick_items


ListView {

    implicitHeight: contentHeight
    implicitWidth: contentWidth

    boundsBehavior: Flickable.StopAtBounds

    delegate: ListView {

        required property var duplicates

        implicitHeight: contentItem.childrenRect.height
        implicitWidth: contentItem.childrenRect.width

        orientation: ListView.Horizontal
        model: duplicates
        boundsBehavior: Flickable.StopAtBounds

        delegate: Column {

            padding: Components.Constants.defaultThumbnailMargin
            spacing: Components.Constants.defaultThumbnailMargin * 2

            required property var modelData

            PhotoDataQml {
                id: dataQml
                data: modelData
            }

            Components.MediaPreviewItem {

                height: Components.Constants.defaultThumbnailSize
                width:  Components.Constants.defaultThumbnailSize

                photoID: modelData.id
            }

            Text {
                text: dataQml.path
            }
        }
    }
}
