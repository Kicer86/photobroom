
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import photo_broom.utils
import photo_broom.enums
import QmlItems
import quick_items
import quick_items.components as Components

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

        DateEdit {
            anchors.fill: parent
            value: editor.value === undefined? new Date(): editor.value

            onAccepted: editor.accepted(value)

            Component.onCompleted: forceActiveFocus();
        }
    }

    Component {
        id: timeEditor

        TimeEdit {
            anchors.fill: parent
            value: editor.value === undefined? new Date(): editor.value

            onAccepted: editor.accepted(value)

            Component.onCompleted: forceActiveFocus();
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

        StarRatingEditor {
            rating: editor.value === undefined? 0: editor.value

            onAccepted: editor.accepted(rating)
            onRejected: editor.rejected()

            Component.onCompleted: forceActiveFocus();
        }
    }

    Component {
        id: categoryEditor

        ColorDialog {

            visible: true
            modality: Qt.ApplicationModal

            onAccepted: editor.accepted(selectedColor)
            onRejected: editor.rejected()
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
                case TagEnums.Category: return categoryEditor;
                default: {
                    console.log("unknown tag type for TagValueEditor");
                    return undefined;
                }
            }
        }

        anchors.fill: parent
    }
}
