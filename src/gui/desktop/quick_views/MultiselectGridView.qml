
import QtQuick 2.15
import photo_broom.qml 1.0


GridView {
    id: grid

    function isSelected(index) {
        return selectionManager.isIndexSelected(index)
    }

    signal selectionChanged()

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

            var index = grid.indexAt(mouse.x, mouse.y)
            selectionManager.toggleIndexSelection(index)

            mouse.accepted = false

            selectionChanged()
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
