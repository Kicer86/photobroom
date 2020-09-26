
import QtQuick 2.0
import QtQuick.Controls 2.0
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
        id: imageId

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
            when: imageId.state === Photo.NotFetched
        },
        State {
            name: "loading"
            when: imageId.state === Photo.Fetching
        },
        State {
            name: "done"
            when: imageId.state === Photo.Fetched
        }
    ]

    transitions: [
        Transition {
            from: "loading"
            to: "done"
            ParallelAnimation {
                PropertyAnimation {
                    target: busyId
                    properties: "opacity"
                    from: 1
                    to: 0
                }
                PropertyAnimation {
                    target: imageId
                    properties: "opacity"
                    from: 0
                    to: 1
                }
            }
        },
        Transition {
            from: "unknown"
            to: "done"
            PropertyAnimation {
                target: busyId
                properties: "opacity"
                from: 1
                to: 0
                duration: 0
            }
            PropertyAnimation {
                target: imageId
                properties: "opacity"
                from: 0
                to: 1
                duration: 0
            }
        }
    ]
}
