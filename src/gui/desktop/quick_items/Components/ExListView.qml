
import QtQuick
import QtQuick.Controls


ListView {
    id: listView

    boundsMovement: Flickable.StopAtBounds
    clip: true

    ScrollBar.vertical: ScrollBar {
        anchors.right: parent.right
        visible: listView.contentHeight > listView.height
    }

    ScrollBar.horizontal: ScrollBar {
        anchors.bottom: parent.bottom
        visible: listView.contentWidth > listView.width
    }
}
