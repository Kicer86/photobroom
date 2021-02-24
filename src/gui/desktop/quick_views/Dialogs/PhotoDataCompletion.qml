
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.qml 1.0


Item {

    Column {
        id: column
        anchors.fill: parent

        Text {
            text: qsTr("Text")
        }

        PropertiesControlledModel {
            id: dataSource
            database: PhotoBroomProject.database
        }

        ListView {
            id: listView

            clip: true
            height: 160
            anchors.left: parent.left
            anchors.right: parent.right

            model: dataSource.model

            delegate: Item {
                required property var photoData

                readonly property string photoPath: photoData.path
                readonly property string guessedInformation: photoDataComplete(photoData)

                width: listView.width
                height: 40

                Row {
                    CheckBox {
                        text: photoPath
                    }

                    Text {
                        text: guessedInformation
                    }
                }
            }
        }

        Text {
            text: qsTr("Text")
        }
    }

    function photoDataComplete(photoData) {
        return photoData.path + "ddd";
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:3}D{i:1}
}
##^##*/
