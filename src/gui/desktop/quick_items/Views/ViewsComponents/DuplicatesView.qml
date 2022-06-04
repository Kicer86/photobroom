
import QtQuick
import QtQuick.Controls

import "../../Components" as Components
import quick_items


Components.ExListView  {

    implicitHeight: contentHeight
    implicitWidth: contentWidth

    boundsBehavior: Flickable.StopAtBounds

    delegate: Components.ExListView {

        required property var duplicates

        implicitHeight: contentItem.childrenRect.height
        implicitWidth: contentItem.childrenRect.width

        orientation: ListView.Horizontal
        model: duplicates
        boundsBehavior: Flickable.StopAtBounds

        delegate: Column {

            required property var modelData

            padding: Components.Constants.defaultThumbnailMargin
            spacing: Components.Constants.defaultThumbnailMargin * 2

            PhotoDataQml {
                id: dataQml
                data: modelData
            }

            Components.MediaPreviewItem {
                id: mediaPreview

                height: Components.Constants.defaultThumbnailSize
                width:  Components.Constants.defaultThumbnailSize

                photoID: modelData.id
            }

            Text {
                function removePrefix(value, prefix) {
                    return value.startsWith(prefix)? value.slice(prefix.length): value;
                }

                width: mediaPreview.width

                text: removePrefix(dataQml.path, Components.Constants.projectPrefix)
                wrapMode: Text.Wrap
            }
        }
    }
}
