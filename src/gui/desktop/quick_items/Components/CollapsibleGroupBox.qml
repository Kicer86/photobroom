
import QtQuick
import QtQuick.Controls


GroupBox {
    id: groupBox

    default property alias d: groupBox.contentItem
    property bool _collapsed: false

    clip: true
    height: _collapsed? implicitLabelHeight: implicitHeight

    Behavior on height { PropertyAnimation{} }

    label: Row {
        Button {
            width: titleText.height
            height: titleText.height
            flat: true
            text: groupBox._collapsed ? "+" : "-"
            onClicked: groupBox._collapsed = !groupBox._collapsed
        }

        Text {
            id: titleText
            text: groupBox.title
        }
    }
}
