
import QtQuick 2.0

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
        text: value
        visible: value[0] != "#"
    }
}
