
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.qml 1.0

/*
 * Delegate for views.
 * Compatible with APhotoInfoModel.
 * Loads photo in lazy manner.
 */

Item {
    id: delegateId

    width: 50
    height: 50
    property int margin: 2

    BusyIndicator {
        id: busyId

        anchors.centerIn: parent
    }

    Photo {
        id: image

        anchors.centerIn: parent
        height: delegateId.height - delegateId.margin * 2
        width:  delegateId.width - delegateId.margin * 2
        opacity: 0

        source: photoData.path
        photoSize: photoData.size

        thumbnails: thumbnailsManager.get()
    }

    states: [
        State {
            name: "unknown"
            when: image.state === Photo.NotFetched
        },
        State {
            name: "loading"
            when: image.state === Photo.Fetching
            PropertyChanges {
                target: busyId
                running: true
            }
        },
        State {
            name: "done"
            when: image.state === Photo.Fetched
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
