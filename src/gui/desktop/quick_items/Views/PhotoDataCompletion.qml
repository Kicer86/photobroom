
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15
import photo_broom.models
import photo_broom.singletons
import quick_items
import "../Components" as Components

Item {
    id: root
    state: "information"

    property var selectedPhoto: []      // selected photo

    function reloadModel() {
        delegateState.clear();
        dataSource.reload();
    }

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    // Model working directly on database
    PhotosDataGuesser {
        id: dataSource
        database: PhotoBroomProject.database
    }

    Column {
        id: statusArea

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Components.InfoItem {
            id: status

            width: parent.width

            MouseArea {
                anchors.fill: parent

                enabled: root.state == "information" || root.state == "summary"
                cursorShape: enabled? Qt.PointingHandCursor: Qt.ArrowCursor

                onClicked: reloadModel()
            }
        }
    }

    Components.DelegateState {
        id: delegateState

        defaultValue: true
        model: dataSource
    }

    ListView {
        id: listView

        clip: true

        anchors.top:statusArea.bottom
        anchors.topMargin: 5
        anchors.bottom: applyButton.top
        anchors.left: parent.left
        anchors.right: parent.right

        spacing: 5
        highlightMoveDuration: 100
        highlightMoveVelocity: -1
        flickDeceleration: 10000
        model: dataSource

        delegate: Item {
            id: delegateRoot

            required property var photoID
            required property var photoPath
            required property var suggestedDate
            required property var suggestedTime
            required property int index

            width: listView.width - listView.ScrollBar.vertical.width
            height: 60

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    listView.currentIndex = index
                }
            }

            Row {
                Components.DelegateCheckBox {
                    state: delegateState
                    index: delegateRoot.index
                }

                Components.MediaPreviewItem {

                    width: 60
                    height: 60

                    photoID: delegateRoot.photoID
                }

                Column {
                    Text {
                        text: delegateRoot.photoPath
                    }

                    Text {
                        text: qsTr("Guessed date:") + "\n" + suggestedDate + " " + suggestedTime
                    }
                }
            }
        }

        highlight: Rectangle {
            color: currentPalette.highlight
            radius: 5
        }

        ScrollBar.vertical: ScrollBar {}

        onCurrentIndexChanged: {
            root.selectedPhoto = dataSource.getId(currentIndex);
        }
    }

    Button {
        id: applyButton

        anchors.bottom: parent.bottom
        anchors.left: parent.left

        text: qsTr("Save selected photos")

        visible: root.state == "summary" && listView.count > 0

        onClicked: {
            var toBeExcluded = delegateState.getItems((state) => {return state;});

            dataSource.apply(toBeExcluded);
        }
    }

    states: [
        State {
            name: "information"
            when: dataSource.state == SeriesModel.Idle

            PropertyChanges {
                target: status
                text: qsTr("Click here to scan for additional information about photos from file names and paths.")
            }
        },
        State {
            name: "fetching"
            when: dataSource.state == SeriesModel.Fetching

            PropertyChanges {
                target: status
                text: qsTr("Processing photos...")
            }
        },
        State {
            name: "summary"
            when: dataSource.state == SeriesModel.Loaded

            PropertyChanges {
                target: status
                text: qsTr("For %n photo(s) dates were detected in file names. Review results and save them if valid.", "", listView.count)
            }
        },
        State {
            name: "updating"
            when: dataSource.state == SeriesModel.Storing

            PropertyChanges {
                target: status
                text: qsTr("Saving results")
            }
        }
    ]
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
