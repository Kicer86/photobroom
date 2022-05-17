
import QtQuick


Row {
    id: root

    required property date value
    signal accepted()

    TextInput {
        id: input

        height: parent.height
        verticalAlignment: TextInput.AlignVCenter
        selectByMouse: true
        inputMask: "00:00:00"
        color: valid? "black": "red"

        property bool valid: true

        onTextChanged: {
            // translate input into time
            const userDate = Date.fromLocaleTimeString(Qt.locale(), input.text, "h:m:s");

            // if time is valid, update public property
            input.valid = isNaN(userDate.getTime()) == false
            if (input.valid)
                root.value = userDate
        }

        onEditingFinished: root.accepted()
    }

    Component.onCompleted: input.text = root.value.toLocaleTimeString(Qt.locale(), "hh:mm:ss")
}

