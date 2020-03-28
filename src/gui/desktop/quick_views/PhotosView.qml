
import QtQuick 2.14
import QtQuick.Controls 2.3
import photo_broom.qml 1.0


Item
{
    id: rootId

    GridView {
        id: photosViewId
        objectName: "photos_view"       // used by c++ part to find this view and set proper model

        property int thumbnailSize: 160
        property int thumbnailMargin: 2

        anchors.fill: parent

        clip: true
        cellWidth: thumbnailSize + thumbnailMargin
        cellHeight: thumbnailSize + thumbnailMargin
        delegate: delegateId
        highlight: highlightId
        keyNavigationEnabled: true

        ScrollBar.vertical: ScrollBar { }
    }

    Component {
        id: delegateId

        Rectangle {
            id: rectId

            width:  photosViewId.thumbnailSize
            height: photosViewId.thumbnailSize

            border.width: 1

            BusyIndicator {
                id: busyId

                anchors.centerIn: parent
            }

            Photo {
                id: imageId

                anchors.centerIn: parent
                height: parent.height
                width:  parent.width
                opacity: 0

                source: photoProperties.path
                photoSize: photoProperties.size

                thumbnails: thumbnailsManager.get()
            }

            states: [
                State {
                    name: "loading"
                    when: imageId.ready == false
                },
                State {
                    name: "done"
                    when: imageId.ready == true
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
                onClicked: rectId.GridView.view.currentIndex = index
            }
        }
    }

    Component {
        id: highlightId

        Rectangle {
            width: view.cellWidth;
            height: view.cellHeight
            color: "lightsteelblue";
            opacity: 0.7
            x: view.currentItem.x
            y: view.currentItem.y
            z: 1
            Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
            Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }
        }
    }
}
