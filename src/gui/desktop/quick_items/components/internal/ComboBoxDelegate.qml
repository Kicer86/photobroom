
import QtQuick 2.15

Rectangle {

    property string value

    color: "white"

    Rectangle {
        color: value

        anchors.fill: parent
        anchors.margins: 1.5

        visible: value[0] == "#"
    }

    Text {
        anchors.fill: parent
        anchors.margins: 2

        visible: value[0] != "#"
        text: value
    }
}
