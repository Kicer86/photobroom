

import QtQml.Models 2.15
import QtQuick 2.14
import QtQuick.Layouts 1.15

Item
{
    id: seriesDetectionMainId
    state: "LoadingState"

    RowLayout {
        id: rowId
        visible: true
        anchors.fill: parent

        ListView {
            id: groupsId
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
    }
    states: [
        State {
            name: "LoadingState"

            PropertyChanges {
                target: rowId
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
