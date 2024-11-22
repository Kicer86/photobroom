
import QtQuick
import QtQuick.Controls

import QmlItems
import "internal/ViewLogic.js" as Logic


ListView {
    id: listView

    boundsMovement: Flickable.StopAtBounds
    flickDeceleration: 10000
    highlightMoveDuration: 100
    highlightMoveVelocity: -1
    clip: true

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    ScrollBar.vertical: ScrollBar {
        anchors.right: parent.right
        visible: listView.contentHeight > listView.height
    }

    ScrollBar.horizontal: ScrollBar {
        anchors.bottom: parent.bottom
        visible: listView.contentWidth > listView.width
    }


    highlight: Rectangle {
        color: currentPalette.highlight
        radius: 5

        visible: listView.orientation == ListView.Vertical
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

            var clickedIndex = indexAt(mouse.x + contentX, mouse.y + contentY);
            if (clickedIndex != -1)
                listView.currentIndex = clickedIndex;
        }
    }

    Keys.onPressed: Logic.handleKeys(event, listView)
}
