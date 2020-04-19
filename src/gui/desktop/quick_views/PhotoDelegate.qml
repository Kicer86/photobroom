
import QtQuick 2.14
import QtQuick.Controls 2.3
import photo_broom.qml 1.0


Component {
    id: delegateId

    Item {
        id: itemId

        width:  GridView.view.cellWidth
        height: GridView.view.cellHeight

        BusyIndicator {
            id: busyId

            anchors.centerIn: parent
        }

        Photo {
            id: imageId

            anchors.centerIn: parent
            height: itemId.GridView.view.thumbnailSize
            width:  itemId.GridView.view.thumbnailSize
            opacity: 0

            source: photoProperties.path
            photoSize: photoProperties.size

            thumbnails: thumbnailsManager.get()
        }

        states: [
            State {
                name: "loading"
                when: imageId.ready === false
            },
            State {
                name: "done"
                when: imageId.ready === true
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
            }
        ]

        MouseArea {
            anchors.fill: parent
            onClicked: itemId.GridView.view.currentIndex = index
        }
    }
}
