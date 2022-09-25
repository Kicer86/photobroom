
import QtQuick 2.15
import quick_items

// Generic component extending GridView by multiselection.
// Requires c++ SelectionManager class

GridView {
    id: grid

    property alias selectedIndexes: selectionManager.selected

    function isIndexSelected(idx) {
        return selectionManager.isIndexSelected(idx);
    }

    SelectionManager {
        id: selectionManager
    }

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: true

        onClicked: function(mouse) {
            var ctrl = mouse.modifiers & Qt.ControlModifier;
            var shift = mouse.modifiers & Qt.ShiftModifier;

            var index = grid.indexAt(mouse.x, mouse.y + contentY);

            if (shift)
                selectionManager.selectTo(index);
            else {
                if (ctrl)
                    selectionManager.toggleIndexSelection(index);
                else
                    selectionManager.clearAndToggleIndexSelection(index);
            }

            grid.currentIndex = index;

            mouse.accepted = false;
        }

        onWheel: function(wheel) {
            wheel.accepted = true;

            if (!grid.contentItem) {
                return;
            }

            // Inverted for some reason
            var newX = grid.contentX - wheel.angleDelta.x;

            const maxX = grid.contentItem.width - grid.width;

            if (newX < 0) {
                newX = 0;
            } else if (newX > maxX) {
                newX = maxX;
            }

            grid.contentX = newX;

            var newY = grid.contentY - wheel.angleDelta.y;
            const maxY = grid.contentItem.height - grid.height;

            if (newY < 0) {
                newY = 0;
            } else if (newY > maxY) {
                newY = maxY;
            }

            grid.contentY = newY;
        }
    }

    Connections {
        target: selectionManager

        function onSelectionChanged(unselectedItems, selectedItems) {
            for (var i = 0; i < unselectedItems.length; i++) {
                var item = itemAtIndex(unselectedItems[i])

                if (item)
                    item.selected = false
            }

            for (var i = 0; i < selectedItems.length; i++) {
                var item = itemAtIndex(selectedItems[i])

                if (item)
                    item.selected = true
            }
        }
    }

    // any change in model should invalid selection as all stored indexes become invalid
    Connections {
        target: model

        function onRowsAboutToBeInserted(parent, first, last) {
            selectionManager.clearSelection();
        }

        function onRowsAboutToBeMoved(parent, first, last, destination, row) {
            selectionManager.clearSelection();
        }

        function onRowsAboutToBeRemoved(parent, first, last) {
            selectionManager.clearSelection();
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
