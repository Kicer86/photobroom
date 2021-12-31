
import QtQuick
import photo_broom.qml 1.0


Item {
    required property var tagType
    required property var value

    Variant {
        id: variant
    }

    Component {
        id: textDelegate

        Text {
            verticalAlignment: Text.AlignVCenter
            text: variant.localize(tagType, value);
        }
    }

    Loader {
        sourceComponent: tagType === TagEnums.Place ||
                         tagType === TagEnums.Event ||
                         tagType === TagEnums.Date  ||
                         tagType === TagEnums.Time   ? textDelegate: undefined; anchors.fill: parent
    }
}
