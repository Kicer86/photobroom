
import QtQuick
import QtQuick.Controls
import "internal" as Internal

/*
 * Reusable combobox with list of colors
 */

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
