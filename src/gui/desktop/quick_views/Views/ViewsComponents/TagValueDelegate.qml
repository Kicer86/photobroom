
import QtQuick
import photo_broom.qml 1.0


Item {
    required property var tagType
    required property var value

    Variant {
        id: variant
    }

    Text {
        anchors.fill: parent

        verticalAlignment: Text.AlignVCenter
        text: variant.localize(tagType, value);
    }
}
