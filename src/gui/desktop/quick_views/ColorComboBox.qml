
import QtQuick 2.12
import QtQuick.Controls 2.12
import "internal" as Internal

ComboBox {
    id: root

    contentItem: Internal.ComboBoxDelegate {
        value: root.displayText
    }

    delegate: ItemDelegate {
        width: root.width
        height: 30

        contentItem: Internal.ComboBoxDelegate {
            value: modelData
        }

        highlighted: root.highlightedIndex === index
    }
}
