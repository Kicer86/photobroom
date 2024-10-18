
import QtQuick

MouseArea {
    required property Item target

    anchors.fill: target
    enabled: target.focusPolicy & Qt.ClickFocus == Qt.ClickFocus
    propagateComposedEvents: true

    onPressed: function(mouse) {
        target.forceActiveFocus();
        mouse.accepted = false;
    }
}
