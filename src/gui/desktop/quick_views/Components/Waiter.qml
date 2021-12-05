

import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.qml 1.0

/*
 * photo thumbnail
 */

Item {

    default required property var embeddedItem

    Component.onCompleted: {
        embeddedItem.opacity = 0;
        children.push(embeddedItem);
    }

    BusyIndicator {
        id: busyId

        anchors.centerIn: parent

        Behavior on opacity { PropertyAnimation {} }
    }

    states: [
        State {
            name: "unknown"
            when: embeddedItem.status === AMediaItem.NotFetched
        },
        State {
            name: "loading"
            when: embeddedItem.status === AMediaItem.Fetching
            PropertyChanges {
                target: busyId
                running: true
            }
        },
        State {
            name: "done"
            when: embeddedItem.status === AMediaItem.Fetched
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
