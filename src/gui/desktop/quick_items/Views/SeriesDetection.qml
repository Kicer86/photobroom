
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import photo_broom.models
import photo_broom.singletons

import "../Components" as Components
import "ViewsComponents" as Internals

/*
 * Series detection dialog
 */

Item
{
    id: seriesDetectionMainId

    state: "LoadingState"

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    SeriesModel {
        id: groupsModelId

        coreFactory: PhotoBroomProject.coreFactory
        project: PhotoBroomProject.project
    }

    Item {
        anchors.fill: parent

        Behavior on opacity { PropertyAnimation{} }

        Components.InfoItem {
            id: status

            width: parent.width
            anchors.top: parent.top

            MouseArea {
                anchors.fill: parent

                enabled: seriesDetectionMainId.state == "Idle" || seriesDetectionMainId.state == "LoadedState"
                cursorShape: enabled? Qt.PointingHandCursor: Qt.ArrowCursor

                onClicked: groupsModelId.reload()
            }
        }

        ColumnLayout {
            id: groupsId

            width: parent.width
            anchors.top: status.bottom
            anchors.bottom: parent.bottom

            Components.DelegateState {
                id: delegateState

                defaultValue: true
                model: groupsModelId
            }

            ListView {
                id: groupsListId
                Layout.fillWidth: true
                Layout.fillHeight: true

                property alias thumbnailSize: thumbnailSliderId.size

                clip: true
                model: groupsModelId
                spacing: 10

                ScrollBar.vertical: ScrollBar { }
                flickDeceleration: 10000

                delegate: Item {
                    id: delegateId

                    width: delegateId.ListView.view.width       // using 'parent' causes erros in output after thumbnail being resized
                    height: groupDetails.height

                    // from view
                    required property int index

                    // from model - roles
                    required property var photoData
                    required property var groupType
                    required property var members

                    Row {
                        anchors.fill: parent

                        Components.DelegateCheckBox {
                            id: checkBox

                            state: delegateState
                            index: delegateId.index
                        }

                        Item {
                            id: groupDetails

                            width: parent.width - checkBox.width
                            height: groupTypeId.height + membersList.height

                            Text {
                                id: groupTypeId
                                text: groupType

                                anchors.bottom: membersList.top
                            }

                            ListView {
                                id: membersList

                                clip: true

                                width: parent.width
                                height: groupsListId.thumbnailSize
                                anchors.bottom: parent.bottom

                                orientation: ListView.Horizontal
                                model: members

                                delegate: Internals.PhotoDelegate
                                {
                                    // from view
                                    required property int index

                                    photoData: members[index]

                                    width: membersList.height
                                    height: membersList.height
                                }
                            }
                        }
                    }
                }

                Components.ThumbnailSlider {
                    id: thumbnailSliderId
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right

                    minimumSize: 100
                }
            }

            Button {
                id: button

                visible: groupsModelId.state === SeriesModel.Loaded && groupsModelId.isEmpty() === false

                text: qsTr("Group", "used as verb - group photos")

                onClicked: {
                    var selected = delegateState.getItems((state) => {return state;});

                    groupsModelId.group(selected);
                    delegateState.clear();
                }
            }
        }

        BusyIndicator {
            id: progressId

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            running: progressId.opacity > 0.0

            Behavior on opacity { PropertyAnimation{} }
        }

        Text {
            id: emptyListInfo

            opacity: groupsModelId.state == SeriesModel.Loaded && groupsModelId.isEmpty()? 1.0 : 0.0

            Behavior on opacity { PropertyAnimation{} }

            text: qsTr("There are no group candidates.")
            anchors.verticalCenter: progressId.verticalCenter
            anchors.horizontalCenter: progressId.horizontalCenter
            font.pixelSize: 12
        }
    }

    states: [
        State {
            name: "Idle"
            when: groupsModelId.state == SeriesModel.Idle

            PropertyChanges {
                target: groupsId
                opacity: 0.0
            }

            PropertyChanges {
                target: progressId
                opacity: 0.0
            }

            PropertyChanges {
                target: status
                text: qsTr("Click here to scan for photo series.")
            }
        },
        State {
            name: "StoringState"
            when: groupsModelId.state == SeriesModel.Storing

            PropertyChanges {
                target: groupsId
                opacity: 0.5
            }

            PropertyChanges {
                target: status
                text: qsTr("Saving groups...")
            }
        },
        State {
            name: "LoadingState"
            when: groupsModelId.state == SeriesModel.Fetching

            PropertyChanges {
                target: groupsId
                opacity: 0.0
            }

            PropertyChanges {
                target: status
                text: qsTr("Looking for group candidates...")
            }
        },
        State {
            name: "LoadedState"
            when: groupsModelId.state == SeriesModel.Loaded

            PropertyChanges {
                target: progressId
                opacity: 0.0
            }

            PropertyChanges {
                target: status
                text: qsTr("Click here to rescan for photo series.")
            }
        }
    ]
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
