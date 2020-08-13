

import QtQuick 2.14
import QtQuick.Layouts 1.15

Item
{
    id: seriesDetectionMainId

    RowLayout {
        id: rowId
        anchors.fill: parent

        ListView {
            id: groupsId
            Layout.minimumWidth: 240
            Layout.fillHeight: true
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
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
