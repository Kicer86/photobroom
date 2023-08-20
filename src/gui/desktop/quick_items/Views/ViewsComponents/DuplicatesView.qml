
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

        MouseArea {
            anchors.fill: parent

            enabled: duplicatesModel.state == SeriesModel.Idle || duplicatesModel.state == SeriesModel.Loaded
            cursorShape: enabled? Qt.PointingHandCursor: Qt.ArrowCursor

            onClicked: duplicatesModel.reload()
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

            onStateChanged: {
                switch (duplicatesModel.state) {
                    case SeriesModel.Fetching: status.text = qsTr("Looking for duplicates."); break;
                    case SeriesModel.Loaded: status.text = qsTr("Click here to search for duplicates again."); break;
                    case SeriesModel.Idle:  status.text = qsTr("Click here to load duplicates."); break;
                }

                duplicatesListStatus.visible = true;
            }
        }

        Item {
            id: duplicatesListStatus

            anchors.fill: parent

            visible: false

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter:   parent.verticalCenter

                text: qsTr("No duplicates found")
                visible: duplicatesList.count == 0 && duplicatesModel.state == SeriesModel.Loaded
            }

            BusyIndicator {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter:   parent.verticalCenter

                running: duplicatesModel.state == SeriesModel.Fetching
            }
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
