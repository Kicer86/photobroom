
import QtQuick
import QtQuick.Controls

import QmlItems


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

    Keys.onPressed: function (event) {
        let delegateHeight = listView.contentItem.children[0]?.height || 1;

        event.accepted = true;
        switch (event.key) {
            case Qt.Key_PageDown:
                listView.currentIndex = listView.currentIndex + Math.floor(listView.height / delegateHeight);
                break;
            case Qt.Key_PageUp:
                listView.currentIndex = listView.currentIndex - Math.floor(listView.height / delegateHeight)
                break;
            case Qt.Key_Home:
                listView.currentIndex = 0;
                break;
            case Qt.Key_End:
                listView.currentIndex = listView.count - 1;
                break;
            default:
                event.accepted = false;
        }
    }
}
