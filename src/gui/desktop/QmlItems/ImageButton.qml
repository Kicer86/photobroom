
import QtQuick 2.15

Item {
    id: root

    enum Style {
        BackLight,
        Scale
    }

    required property string source
    property int style: Style.BackLight

    signal clicked()


    Rectangle {
        anchors.fill: parent

        id: background
        color: "black"

        visible: root.style === root.BackLight
        opacity: 0.0 + mouseArea.containsMouse * 0.2 + mouseArea.pressed * 0.2
    }

    Image {
        anchors.fill: parent

        source: root.source
        scale: root.style === root.Scale? (mouseArea.containsMouse? 1.0: 0.7): 1.0

        Behavior on scale { PropertyAnimation{ duration: 100 } }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true

        onClicked: root.clicked()
    }
}
