
import QtQuick
import QtQuick.Controls


ListView {
    id: listView

    boundsMovement: Flickable.StopAtBounds
    flickDeceleration: 10000
    clip: true

    ScrollBar.vertical: ScrollBar {
        anchors.right: parent.right
        visible: listView.contentHeight > listView.height
    }

    ScrollBar.horizontal: ScrollBar {
        anchors.bottom: parent.bottom
        visible: listView.contentWidth > listView.width
    }

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
    }
}
