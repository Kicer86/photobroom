
import QtQuick
import QtQuick.Controls

import quick_items
import photo_broom.models
import photo_broom.singletons
import QmlItems

import "../Components" as Components
import "ViewsComponents" as Internals


Internals.ToolsBase
{
    idlePrompt:    qsTr("Click here to load duplicates.")
    loadingPrompt: qsTr("Looking for duplicates...")
    loadedPrompt:  qsTr("Click here to search for duplicates again.")
    emptyPrompt:   qsTr("No duplicates found")

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    model: DuplicatesModel {
        id: duplicatesModel
        database: PhotoBroomProject.database
    }

    view: Component { ExListView  {
        id: duplicatesList

        focusPolicy: Qt.StrongFocus
        model: duplicatesModel

        delegate: ExListView {
            required property var duplicates

            implicitHeight: contentItem.childrenRect.height
            implicitWidth:  duplicatesList.width

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

        highlight: Rectangle {
            color: currentPalette.highlight
            radius: 5
        }
    }}
}
