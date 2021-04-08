
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15
import photo_broom.qml 1.0
import "../Components" as Components

Item {
    id: root
    state: "information"

    function reloadModel() {
        listView.notSelected = new Set();
        dataSource.performAnalysis();
    }

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    // Model working directly on database
    PhotosDataGuesser {
        id: dataSource
        database: PhotoBroomProject.database

        onFetchInProgressChanged: {
            if (fetchInProgress)
                root.state = "fetching";
            else
                root.state = "summary";
        }

        onUpdateInProgressChanged: {
            if (updateInProgress)
                root.state = "updating"
            else
                reloadModel();
        }
    }

    Column {
        id: statusArea

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Components.InfoItem {
            id: status

            width: parent.width
            height: desiredHeight

            MouseArea {
                anchors.fill: parent

                enabled: root.state == "information" || root.state == "summary"
                cursorShape: enabled? Qt.PointingHandCursor: Qt.ArrowCursor

                onClicked: reloadModel()
            }
        }
    }

    ListView {
        id: listView

        clip: true

        anchors.top:statusArea.bottom
        anchors.topMargin: 5
        anchors.bottom: applyButton.top
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

    Button {
        id: applyButton

        anchors.bottom: parent.bottom
        anchors.left: parent.left

        text: qsTr("Apply changes on selected photos")

        visible: root.state == "summary" && listView.count > 0

        onClicked: {
            var toBeExcluded = []

            for (let item of listView.notSelected)
                toBeExcluded.push(item);

            dataSource.applyBut(toBeExcluded);
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
        },
        State {
            name: "updating"
            PropertyChanges {
                target: status
                text: qsTr("Photos are being updated")
            }
        }
    ]
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
