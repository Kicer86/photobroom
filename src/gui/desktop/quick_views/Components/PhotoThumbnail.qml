
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.qml 1.0

/*
 * photo thumbnail
 */

Item {
    id: root

    property var photoID

    BusyIndicator {
        id: busyId

        anchors.centerIn: parent

        Behavior on opacity { PropertyAnimation {} }
    }

    StaticImageThumbnail {
        id: image

        anchors.fill: parent
        opacity: 0

        photoID: root.photoID

        thumbnails: thumbnailsManager.get()
    }

    states: [
        State {
            name: "unknown"
            when: image.state === StaticImageThumbnail.NotFetched
        },
        State {
            name: "loading"
            when: image.state === StaticImageThumbnail.Fetching
            PropertyChanges {
                target: busyId
                running: true
            }
        },
        State {
            name: "done"
            when: image.state === StaticImageThumbnail.Fetched
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
                target: image
                properties: "opacity"
                from: 0
                to: 1
            }
        },
        Transition {
            from: "unknown"
            to: "done"
            PropertyAnimation {
                target: image
                properties: "opacity"
                from: 0
                to: 1
                duration: 0
            }
        }
    ]
}
