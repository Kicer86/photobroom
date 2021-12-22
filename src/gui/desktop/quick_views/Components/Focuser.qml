
import QtQuick

MouseArea {
    anchors.fill: parent

    propagateComposedEvents: true

    onClicked: function(event) {
        if (parent.activeFocus === false)
            parent.forceActiveFocus();

        event.accepted = false;
    }
}
