
import QtQuick

/*
 * Item displays a provided busy indicator until `embeddedItem` (needs to be `Image` based) is ready to be shown
 */


Item {
    id: rootId
    default required property Image embeddedItem

    // Use our interface for the busy‐indicator so it has a `running` property:
    required property Item busyIndicator

    Component.onCompleted: {
        // hide until ready
        embeddedItem.opacity = 0

        children.push(embeddedItem)

        embeddedItem.statusChanged.connect(onImageStatusChanged)

        onImageStatusChanged()
    }

    function onImageStatusChanged() {
        busyIndicator.running = (embeddedItem.status === Image.Loading)

        if (embeddedItem.status === Image.Ready) {
            embeddedItem.opacity = 1
        }
    }
}
