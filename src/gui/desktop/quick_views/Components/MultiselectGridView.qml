
import QtQuick 2.15
import photo_broom.qml 1.0

// Generic component extending GridView by multiselection.
// Requires c++ SelectionManager class

GridView {
    id: grid

    SelectionManager {
        id: selectionManager

        objectName: "selectionManager"
    }

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: true

        onClicked: {
            var ctrl = mouse.modifiers & Qt.ControlModifier;
            var shift = mouse.modifiers & Qt.ShiftModifier;

            if (ctrl == false && shift == false)
                selectionManager.clearSelection();

            var index = grid.indexAt(mouse.x, mouse.y + contentY);

            if (shift)
                selectionManager.selectTo(index);
            else
                selectionManager.toggleIndexSelection(index);

            grid.currentIndex = index

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
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
