
import QtQuick
import QtQuick.Controls


GroupBox {
    id: groupBox

    default property alias d: groupBox.contentItem
    property bool collapsed: false

    clip: true
    height: collapsed? implicitLabelHeight: implicitHeight

    Behavior on height { PropertyAnimation{} }

    label: Row {
        Button {
            width: titleText.height
            height: titleText.height
            flat: true
            text: groupBox.collapsed ? "+" : "-"
            onClicked: groupBox.collapsed = !groupBox.collapsed
        }

        Text {
            id: titleText
            text: groupBox.title
        }
    }
}
