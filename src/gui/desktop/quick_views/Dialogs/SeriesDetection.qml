
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "../Components" as Components
import "DialogsComponents" as Internals

/*
 * Series detection dialog
 */

Item
{
    id: seriesDetectionMainId
    objectName: "seriesDetectionMain"

    state: "LoadingState"

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    RowLayout {
        id: groupsId
        anchors.fill: parent

        Behavior on opacity { PropertyAnimation{} }

        ColumnLayout {
            id: column
            width: 200
            height: 400

            Text {
                id: element
                text: qsTr("Group candidates")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                font.pixelSize: 12
            }

            Components.DelegateState {
                id: delegateState

                defaultValue: true
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
                                    property var photoData: members[index]

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
                text: qsTr("Group", "used as verb - group photos")

                Connections {
                    target: button
                    function onClicked() {
                        var unselected = delegateState.getItems((state) => {return state === false;});

                        groupsModelId.groupBut(unselected);
                        delegateState.clear();
                    }
                }
            }
        }
    }

    Item {
        id: loadingId
        anchors.fill: parent

        Behavior on opacity { PropertyAnimation{} }

        Item {
            id: containerId
            width: childrenRect.width
            height: childrenRect.height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            // TODO: For some reason BusyIndicator causes
            // 'QQmlEngine::setContextForObject(): Object already has a QQmlContext'
            // error in output
            BusyIndicator {
                id: busyIndicatorId
                anchors.top: infoId.bottom
                anchors.topMargin: 0
                anchors.horizontalCenter: infoId.horizontalCenter
            }

            Text {
                id: infoId
                text: qsTr("Looking for group candidates...")
                anchors.top: parent.top
                font.pixelSize: 12
            }
        }
    }

    Text {
        id: emptyListInfo

        Behavior on opacity { PropertyAnimation{} }

        text: qsTr("There are no group candidates.")
        anchors.verticalCenter: loadingId.verticalCenter
        anchors.horizontalCenter: loadingId.horizontalCenter
        font.pixelSize: 12
    }

    states: [
        State {
            name: "LoadingState"

            PropertyChanges {
                target: groupsId
                opacity: 0
            }

            PropertyChanges {
                target: emptyListInfo
                opacity: 0
            }
        },
        State {
            name: "LoadedState"
            when: groupsModelId.loaded && groupsModelId.isEmpty() === false

            PropertyChanges {
                target: emptyListInfo
                opacity: 0
            }

            PropertyChanges {
                target: loadingId
                opacity: 0
            }
        },
        State {
            name: "LoadedEmptyState"
            when: groupsModelId.loaded && groupsModelId.isEmpty()

            PropertyChanges {
                target: groupsId
                opacity: 0
            }

            PropertyChanges {
                target: loadingId
                opacity: 0
            }
        }
    ]
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
