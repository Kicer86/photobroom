
import QtQuick
import QtQuick.Controls
import photo_broom.utils
import photo_broom.enums
import QmlItems
import quick_items
import "../../Components" as Components
import "../../external/qml-colorpicker/colorpicker" as Colorpicker

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

        Item {
            id: editorRect

            Popup {
                id: popup

                parent: Overlay.overlay
                x: width > editorRect.width? _pos.x - (width - editorRect.width) : _pos.x
                y: _pos.y + editorRect.height
                modal: true
                focus: true
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                property point _pos: mapToItem(Overlay.overlay, editorRect.x, editorRect.y)
                property bool _accepted: false

                Column {
                    anchors.fill: parent

                    Colorpicker.Colorpicker {
                        id: picker

                        enableAlphaChannel: false
                        enableDetails: false
                        paletteMode: true
                    }

                    RoundButton {
                        height: 17
                        text: qsTr("Apply")
                        radius: 3

                        onClicked: {
                            popup._accepted = true;
                            editor.accepted(picker.colorValue);
                        }
                    }
                }

                Component.onCompleted: popup.open()
                onClosed: {
                    if (!_accepted)
                        editor.rejected();
                }
            }
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
