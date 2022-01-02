
import QtQuick
import photo_broom.qml 1.0
import "../../Components" as Components


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

    Component {
        id: categoryDelegate

        Rectangle {
            radius: 5
            color: variant.toColor(value);
            border.color: Qt.darker(color, 2.0)
            border.width: 1
        }
    }

    Component {
        id: ratingDelegate

        Components.StarRating {
            rating: variant.localize(tagType, value);
        }
    }

    Loader {
        active: value !== undefined
        sourceComponent: tagType === TagEnums.Place ||
                         tagType === TagEnums.Event ||
                         tagType === TagEnums.Date  ||
                         tagType === TagEnums.Time    ? textDelegate: undefined; anchors.fill: parent
    }

    Loader {
        active: value !== undefined
        sourceComponent: tagType === TagEnums.Category? categoryDelegate: undefined; anchors.fill: parent
    }

    Loader {
        active: value !== undefined
        sourceComponent: tagType === TagEnums.Rating? ratingDelegate: undefined; anchors.fill: parent
    }
}
