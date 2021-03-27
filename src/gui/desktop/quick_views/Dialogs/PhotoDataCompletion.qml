
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15
import photo_broom.qml 1.0
import "../Components" as Components

Item {

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    // Model working directly on database
    PhotosDataGuesser {
        id: dataSource
        database: PhotoBroomProject.database

        onFetchInProgressChanged: {
            if (fetchInProgress == false)
                status.state = "summary";
        }
    }

    Column {
        id: statusArea

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Components.InfoItem {
            id: status
            state: "information"

            width: parent.width
            height: desiredHeight

            MouseArea {
                anchors.fill: parent

                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    dataSource.performAnalysis();
                    status.state = "fetching"
                }
            }

            states: [
                State {
                    name: "information"
                    PropertyChanges {
                        target: status
                        text: qsTr("Click here to scan for additional information about photos from file names and paths.")
                    }
                },
                State {
                    name: "fetching"
                    PropertyChanges {
                        target: status
                        text: qsTr("Processing photos...")
                    }
                },
                State {
                    name: "summary"
                    PropertyChanges {
                        target: status
                        text: qsTr("%n photo(s) were analysed. Review collected data and approve it.", "", listView.count)
                    }
                }
            ]
        }

        Button {
            text: qsTr("Apply changes on selected photos")

            visible: status.state == "summary"
        }
    }

    ListView {
        id: listView

        clip: true

        anchors.top:statusArea.bottom
        anchors.topMargin: 5
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        spacing: 2
        highlightMoveDuration: 100
        highlightMoveVelocity: -1
        model: dataSource

        property var notSelected: new Set()

        delegate: Item {
            required property var photoPath
            required property var suggestedDate
            required property var suggestedTime
            required property int index

            width: listView.width
            height: 60

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    listView.currentIndex = index
                }
            }

            Row {
                CheckBox {
                    checkState: listView.notSelected.has(index)? Qt.Unchecked: Qt.Checked

                    onCheckStateChanged: {
                        if (checkState)
                            listView.notSelected.delete(index);
                        else
                            listView.notSelected.add(index);
                    }

                }

                Components.PhotoThumbnail {

                    width: 60
                    height: 60

                    source: photoPath
                }

                Column {
                    Text {
                        text: photoPath
                    }

                    Text {
                        text: suggestedDate
                    }

                    Text {
                        text: suggestedTime
                    }
                }
            }
        }

        highlight: Rectangle {
            color: currentPalette.highlight
            radius: 5
        }

        ScrollBar.vertical: ScrollBar {}
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
