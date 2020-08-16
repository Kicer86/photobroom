
import QtQuick 2.14
import QtQuick.Controls 2.3
import photo_broom.qml 1.0


Item {
    id: delegateId

    property int margin

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

        source: photoProperties.path
        photoSize: photoProperties.size

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

    MouseArea {
        anchors.fill: parent
        onClicked: delegateId.GridView.view.currentIndex = index
    }
}
