
import QtQuick 2.15
import photo_broom.qml 1.0


GridView {
    id: grid

    function isSelected(index) {
        return selectionManager.isIndexSelected(index)
    }

    SelectionManager {
        id: selectionManager
    }

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: true

        onClicked: {
            var ctrl = mouse.modifiers & Qt.ControlModifier;

            if (ctrl == false)
                selectionManager.clearSelection()

            var index = grid.indexAt(mouse.x, mouse.y + contentY)

            selectionManager.toggleIndexSelection(index)

            mouse.accepted = false
        }
    }

    Connections {
        target: selectionManager

        function onSelectionChanged(unselected, selectedItems) {
            for (var i = 0; i < unselected.length; i++) {
                var item = itemAtIndex(unselected[i])

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
