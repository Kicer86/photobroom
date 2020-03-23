
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
                photoHeight: photoProperties.height
                photoWidth: photoProperties.width

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
        }
    }
}
