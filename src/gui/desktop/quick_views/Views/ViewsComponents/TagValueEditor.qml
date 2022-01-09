
import QtQuick
import photo_broom.qml 1.0
import "../../Components" as Components


Item {
    id: editor

    required property var tagType
    required property var value

    signal accepted(var value)

    Variant {
        id: variant
    }

    Component {
        id: dateEditor

        Components.DateEdit {
            anchors.fill: parent
            value: editor.value === undefined? new Date(): editor.value

            onAccepted: editor.accepted(value)
        }
    }

    Loader {
        sourceComponent: tagType === TagEnums.Date? dateEditor: undefined

        anchors.fill: parent
    }
}
