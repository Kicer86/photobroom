
import QtQuick
import QtQuick.Controls


Row {
    required property date value

    TextInput {
        id: input
        inputMask: "99.99.9999"

        color: valid? "black": "red"

        property bool valid: true

        onTextChanged: {
            const userDate = Date.fromLocaleDateString(Qt.locale(), input.text, "d.M.yyyy");

            input.valid = isNaN(userDate.getTime()) == false
        }
    }

    Component.onCompleted: {
        input.text = value.toLocaleDateString(null, "dd.MM.yyyy");
    }
}
