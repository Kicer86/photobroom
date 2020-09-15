
import QtQuick 2.0
import QtQuick.Controls 2.15

ComboBox {
    id: root

    contentItem: Rectangle {

        color: "white"

        Rectangle {
            color: root.displayText

            anchors.fill: parent
            anchors.margins: 1.5

            visible: root.displayText[0] == "#"
        }
        Text {
            text: root.displayText
            visible: root.displayText[0] != "#"
        }
    }

    delegate: ItemDelegate {
        width: root.width
        height: 25

        contentItem: Rectangle {

            color: "white"

            Rectangle {
                color: modelData

                anchors.fill: parent
                anchors.margins: 1.5

                visible: modelData[0] == "#"
            }
            Text {
                text: modelData
                visible: modelData[0] != "#"
            }
        }
        highlighted: root.highlightedIndex === index
    }
}
