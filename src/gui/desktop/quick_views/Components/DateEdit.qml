
import QtQuick
import QtQuick.Controls


Row {
    id: root

    required property date value
    signal accepted()

    TextInput {
        id: input

        height: parent.height
        verticalAlignment: TextInput.AlignVCenter
        selectByMouse: true
        inputMask: "00.00.0000"
        color: valid? "black": "red"

        property bool valid: true

        onTextChanged: {
            // translate input into date
            const userDate = Date.fromLocaleDateString(Qt.locale(), input.text, "d.M.yyyy");

            // if date is valid, update public property
            input.valid = isNaN(userDate.getTime()) == false
            if (input.valid)
                root.value = userDate
        }

        onEditingFinished: root.accepted()
    }

    Component.onCompleted: input.text = root.value.toLocaleDateString(Qt.locale(), "dd.MM.yyyy")
}
