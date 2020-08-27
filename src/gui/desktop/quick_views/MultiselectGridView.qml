
import QtQuick 2.15

GridView {
    id: grid

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton
        propagateComposedEvents: true

        onClicked: {
            if (mouse.modifiers & Qt.ControlModifier) {
                var index = grid.indexAt(mouse.x, mouse.y)

                console.log("Ctrl!")
            }

            mouse.accepted = false
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
