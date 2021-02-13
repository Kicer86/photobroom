
import QtQuick 2.15

Item {
    id: root

    required property string source
    signal clicked()

    Rectangle {
        anchors.fill: parent

        id: background
        color: "black"

        opacity: 0.0 + mouseArea.containsMouse * 0.2 + mouseArea.pressed * 0.2
    }

    Image {
        anchors.fill: parent

        source: root.source
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true

        onClicked: root.clicked()
    }
}
