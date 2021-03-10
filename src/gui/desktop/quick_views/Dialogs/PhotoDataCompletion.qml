
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15
import photo_broom.qml 1.0
import "../Components" as Components

Item {

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    // Model working directly on database
    PropertiesControlledModel {
        id: dataSource
        database: PhotoBroomProject.database

        tags: { "Date": "" }
    }


    Timer {
        id: refreshTimer
        interval: 500; running: false; repeat: false
        onTriggered: visualModel.validate()
    }

    // proxy - for sorting/grouping reasons
    DelegateModel {
        id: visualModel

        function validate() {
            var allItems = unknownItems.count;
            var processed = 0;
            console.log("Processing " + allItems + " unknown items");

            for(var i = 0; i < unknownItems.count; i++) {
                var item = unknownItems.get(i);

                if (item.model.photoData.path !== null && item.model.photoData.path !== "") {
                    processed++;
                    var guessedInformation = photoDataComplete(item.model.photoData);

                    if (guessedInformation !== null && guessedInformation !== "") {
                        item.groups = "items";
                    }
                }
            }

            console.log("Resolved " + processed + " items");

            if (processed < allItems) {
                refreshTimer.start();
            }
        }

        items.includeByDefault: false

        groups: DelegateModelGroup {
            id: unknownItems
            name: "unknown"

            includeByDefault: true

            onChanged: {
                visualModel.validate();
            }
        }

        delegate: Item {
            required property var photoData
            required property int index

            readonly property string photoPath: photoData.path
            readonly property string guessedInformation: photoDataComplete(photoData)

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
                    checkState: guessedInformation == ""? Qt.Unchecked: Qt.Checked
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
                        text: guessedInformation
                    }
                }
            }
        }

        model: dataSource.model
    }

    ListView {
        id: listView

        clip: true
        anchors.fill: parent

        spacing: 2
        highlightMoveDuration: 100
        highlightMoveVelocity: -1
        model: visualModel

        highlight: Rectangle {
            color: currentPalette.highlight
            radius: 5
        }

        ScrollBar.vertical: ScrollBar {}
    }

    function photoDataComplete(photoData) {
        const pattern = /^[^0-9]*([0-9]{4})-?([0-9]{2})-?([0-9]{2})[^0-9]*$/
        const dateMatch = photoData.path.match(pattern);

        return dateMatch == null? null: dateMatch[1] + "-" + dateMatch[2] + "-" + dateMatch[3];
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
