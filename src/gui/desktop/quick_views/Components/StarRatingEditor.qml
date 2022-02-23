
import QtQuick
import photo_broom.items


Item {
    id: rootItem

    property int rating: 0
    signal accepted()
    signal rejected()

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
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPositionChanged: function(mouse) {
                const v = (mouse.x / editor.implicitWidth + 0.15 / 5) * 10;
                editor.rating = clamp(v, 0, 10);
            }

            onClicked: function(mouse) {
                if (mouse.button === Qt.RightButton)
                    rootItem.rejected();
                else if (mouse.button === Qt.LeftButton)
                {
                    rootItem.rating =  editor.rating
                    rootItem.accepted();
                }
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
