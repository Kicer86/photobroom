

import QtQuick 2.15
import photo_broom.qml 1.0

/*
 * Item displays a provided busy indicator until `embeddedItem` (needs to be `Image` based) is ready to be shown
 */

Item {

    default required property var embeddedItem
    required property var busyIndicator

    Component.onCompleted: {
        embeddedItem.opacity = 0;
        children.push(embeddedItem);
    }

    states: [
        State {
            name: "unknown"
            when: embeddedItem.status === Image.Null
        },
        State {
            name: "loading"
            when: embeddedItem.status === Image.Loading
            PropertyChanges {
                target: busyId
                running: true
            }
        },
        State {
            name: "done"
            when: embeddedItem.status === Image.Ready
            PropertyChanges {
                target: busyId
                running: false
            }
        }
    ]

    transitions: [
        Transition {
            from: "loading"
            to: "done"
            PropertyAnimation {
                target: embeddedItem
                properties: "opacity"
                from: 0
                to: 1
            }
        },
        Transition {
            from: "unknown"
            to: "done"
            PropertyAnimation {
                target: embeddedItem
                properties: "opacity"
                from: 0
                to: 1
                duration: 0
            }
        }
    ]
}
