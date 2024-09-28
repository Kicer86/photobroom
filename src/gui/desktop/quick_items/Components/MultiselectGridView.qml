
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

        propagateComposedEvents: true

        onPressed: function(mouse) {
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
