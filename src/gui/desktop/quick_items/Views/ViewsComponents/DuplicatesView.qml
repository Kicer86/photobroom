
import QtQuick
import QtQuick.Controls

import "../../Components" as Components
import quick_items
import photo_broom.models
import photo_broom.singletons
import QmlItems


Item
{
    Components.InfoItem {
        id: status

        width: parent.width
        height: desiredHeight

        text: duplicatesModel.workInProgress? qsTr("Looking for duplicates."): qsTr("Click here to load duplicates.")

        MouseArea {
            anchors.fill: parent

            enabled: duplicatesModel.workInProgress == false
            cursorShape: enabled? Qt.PointingHandCursor: Qt.ArrowCursor

            onClicked: duplicatesModel.reloadDuplicates()
        }
    }

    Components.ExListView  {

        id: duplicatesList

        width: parent.width
        anchors.top: status.bottom
        anchors.bottom: parent.bottom

        model: DuplicatesModel {
            id: duplicatesModel
            database: PhotoBroomProject.database
        }

        delegate: Components.ExListView {

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
    }
}
