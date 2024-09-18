
import QtQuick
import QtQuick.Controls

import QmlItems


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

    WheelSpeedControler
    {
        enabled: listView.orientation == ListView.Vertical
        flickable: listView
    }

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
    }
}
