
import QtQuick

MouseArea {

    required property Item target

    anchors.fill: target
    propagateComposedEvents: true

    onClicked: function(mouse) {
        mouse.accepted = false;
        target.forceActiveFocus();
    }
}
