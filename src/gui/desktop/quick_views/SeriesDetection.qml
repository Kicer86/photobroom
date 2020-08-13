

import QtQuick 2.14
import QtQuick.Dialogs.qml 1.0
Item
{
    id: seriesDetectionMainId

    Column {
        id: columnId
        anchors.fill: parent

        ListView {
            id: groupsId
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
                        color: colorCode
                    }

                    Text {
                        text: name
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
            model: ListModel {
                ListElement {
                    name: "Grey"
                    colorCode: "grey"
                }

                ListElement {
                    name: "Red"
                    colorCode: "red"
                }

                ListElement {
                    name: "Blue"
                    colorCode: "blue"
                }

                ListElement {
                    name: "Green"
                    colorCode: "green"
                }
            }
        }

        Item {
            id: groupMembersId
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2;anchors_height:160;anchors_width:110}
D{i:12;anchors_height:200;anchors_width:200}D{i:1;anchors_height:400;anchors_width:200}
}
##^##*/
