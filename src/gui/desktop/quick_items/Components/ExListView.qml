
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
        id: exListMouseArea
        anchors.fill: listView
        enabled: listView.focusPolicy & Qt.ClickFocus == Qt.ClickFocus
        propagateComposedEvents: true

        onPressed: function(mouse) {
            mouse.accepted = false;

            if (listView.activeFocus == false)
                listView.forceActiveFocus();

            var clickedIndex = indexAt(mouse.x, mouse.y);
            if (clickedIndex != -1)
                listView.currentIndex = clickedIndex;
        }
    }
}
