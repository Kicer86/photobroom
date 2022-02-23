
import QtQuick 2.15

Rectangle {
    id: root

    property alias text: textItem.text
    readonly property alias desiredHeight: textItem.height

    color: "deepskyblue"
    radius: 5
    border.width: 1

    Text {
        id: textItem

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 5
        anchors.rightMargin: 5
    }
}
