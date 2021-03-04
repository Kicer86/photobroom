
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.qml 1.0
import "../Components" as Components

Item {

    PropertiesControlledModel {
        id: dataSource
        database: PhotoBroomProject.database

        tags: { "Date": "" }
    }

    ListView {
        id: listView

        clip: true
        height: 160
        anchors.fill: parent

        model: dataSource.model

        delegate: Item {
            required property var photoData

            readonly property string photoPath: photoData.path
            readonly property string guessedInformation: photoDataComplete(photoData)

            width: listView.width
            height: 60

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
