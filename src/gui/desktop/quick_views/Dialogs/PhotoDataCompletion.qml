
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
    }

    Item {
        id: statusArea

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: status.height

        Components.InfoItem {
            id: status

            width: parent.width
            text: qsTr("Click here to scan for additional information about photos from file names and paths.")

            MouseArea {
                anchors.fill: parent

                cursorShape: Qt.PointingHandCursor

                onClicked: dataSource.performAnalysis();
            }
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
                    checkState: suggestedDate == ""? Qt.Unchecked: Qt.Checked
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
