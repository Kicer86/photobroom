
import QtQuick
import photo_broom.qml 1.0
import "../../Components" as Components


Item {
    id: editor

    required property var tagType
    required property var value

    signal accepted(var value)
    signal rejected()

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

    Component {
        id: timeEditor

        Components.TimeEdit {
            anchors.fill: parent
            value: editor.value === undefined? new Date(): editor.value

            onAccepted: editor.accepted(value)
        }
    }

    Component {
        id: textEditor

        TextInput {
            anchors.fill: parent
            height: parent.height

            selectByMouse: true
            verticalAlignment: TextInput.AlignVCenter
            text: editor.value === undefined? "": editor.value

            onEditingFinished: editor.accepted(text)

            Component.onCompleted: forceActiveFocus();
        }
    }

    Component {
        id: ratingsEditor

        Components.StarRatingEditor {
            rating: editor.value === undefined? 0: editor.value

            onAccepted: editor.accepted(rating)
            onRejected: editor.rejected()

            Component.onCompleted: forceActiveFocus();
        }
    }

    Loader {
        sourceComponent: {
            switch(tagType) {
                case TagEnums.Date: return dateEditor;
                case TagEnums.Time: return timeEditor;
                case TagEnums.Place: return textEditor;
                case TagEnums.Event: return textEditor;
                case TagEnums.Rating: return ratingsEditor;
                default: {
                    console.log("unknown tag type for TagValueEditor");
                    return undefined;
                }
            }
        }

        anchors.fill: parent
    }
}
