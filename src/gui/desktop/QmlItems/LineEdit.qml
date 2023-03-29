
import QtQuick 2.15

Rectangle {

    property alias text: input.text

    implicitHeight: input.implicitHeight + 8
    implicitWidth: input.implicitWidth

    border.color: "gray"
    border.width: 1
    radius: 2

    TextInput {
        id: input
        anchors.fill: parent
        anchors.margins: 4

        selectByMouse: true
    }
}
