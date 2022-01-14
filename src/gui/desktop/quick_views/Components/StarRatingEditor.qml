
import QtQuick
import photo_broom.qml 1.0


Item {
    id: rootItem

    property int rating: 0

    implicitWidth: editor.implicitWidth
    implicitHeight: editor.implicitHeight

    StarRating {
        id: view
        opacity: 0.3
        anchors.fill: parent
    }

    StarRating {
        id: editor

        anchors.fill: parent

        MouseArea {
            anchors.fill: parent

            hoverEnabled: true

            onPositionChanged: function(mouse) {
                const v = (mouse.x / editor.implicitWidth + 0.15 / 5) * 10;
                editor.rating = clamp(v, 0, 10);
            }
        }
    }

    function clamp(number, min, max) {
        return Math.max(min, Math.min(number, max));
    }

    Component.onCompleted: {
        view.rating = rootItem.rating
    }
}
