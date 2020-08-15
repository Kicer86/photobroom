

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.14

Item
{
    id: seriesDetectionMainId
    state: "LoadingState"

    RowLayout {
        id: groupsId
        anchors.fill: parent

        ListView {
            id: groupsListId
            Layout.minimumWidth: 240
            Layout.fillHeight: true

            model: groupsModelId

            delegate: Item {
                x: 5
                width: 80
                height: 40
                Row {
                    id: row1
                    spacing: 10
                    Rectangle {
                        width: 40
                        height: 40
                        color: "red"
                    }

                    Text {
                        text: "rerer"
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }

        Item {
            id: groupMembersId
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    Item {
        id: loadingId
        anchors.fill: parent

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
                text: qsTr("Looking for groups...")
                anchors.top: parent.top
                font.pixelSize: 12
            }
        }
    }

    states: [
        State {
            name: "LoadingState"

            PropertyChanges {
                target: groupsId
                visible: false
            }
        },
        State {
            name: "LoadedState"

            PropertyChanges {
                target: loadingId
                visible: false
            }
        }
    ]

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
