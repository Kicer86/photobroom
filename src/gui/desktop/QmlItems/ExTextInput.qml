
import QtQuick
import QtQuick.Controls

TextInput {
    signal focusLost()

    onCursorVisibleChanged: {
        if (cursorVisible === false)
            focusLost();
    }
}
