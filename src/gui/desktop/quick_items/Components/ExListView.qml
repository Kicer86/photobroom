
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

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true

        onWheel: function(wheel) {
            wheel.accepted = true;

            if (!listView.contentItem) {
                return;
            }

            // Inverted for some reason
            var newX = listView.contentX - wheel.angleDelta.x;

            const maxX = listView.contentItem.width - listView.width;

            if (newX < 0) {
                newX = 0;
            } else if (newX > maxX) {
                newX = maxX;
            }

            listView.contentX = newX;

            var newY = listView.contentY - wheel.angleDelta.y;
            const maxY = listView.contentItem.height - listView.height;

            if (newY < 0) {
                newY = 0;
            } else if (newY > maxY) {
                newY = maxY;
            }

            listView.contentY = newY;
        }
    }
}
