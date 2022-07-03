
import QtQuick
import QtQuick.Controls

import "../../Components" as Components
import quick_items
import QmlItems


Components.ExListView  {

    delegate: Components.ExListView {

        required property var duplicates

        implicitHeight: contentItem.childrenRect.height
        implicitWidth: contentItem.childrenRect.width

        orientation: ListView.Horizontal
        model: duplicates

        delegate: Column {

            required property var modelData

            padding: Components.Constants.defaultThumbnailMargin
            spacing: Components.Constants.defaultThumbnailMargin * 2

            PhotoDataQml {
                id: dataQml
                data: modelData
            }

            Row {
                Components.MediaPreviewItem {
                    id: mediaPreview

                    height: Components.Constants.defaultThumbnailSize
                    width:  Components.Constants.defaultThumbnailSize

                    photoID: modelData.id
                }

                Column {
                    RoundButton {
                        action: Action {
                            icon.name: "folder"
                            onTriggered: {
                                var path = Utils.fileInfo().fullDirectoryPath(dataQml.path);
                                var url  = Utils.fileInfo().toUrl(path);
                                Qt.openUrlExternally(url);
                            }
                        }
                    }
                }
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
