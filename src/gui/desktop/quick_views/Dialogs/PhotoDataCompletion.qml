
import QtQuick 2.15
import QtQuick.Controls 2.15


Item {
    Column {
        id: column
        anchors.fill: parent

        Text {
            text: qsTr("Text")
        }

        ListView {
            id: listView
            height: 160
            anchors.left: parent.left
            anchors.right: parent.right
            model: ListModel {
                ListElement {
                    photoPath: "/home/user/photo1.jpeg"
                    guessedInformation: "Date: 1, Time 2"
                }

                ListElement {
                    photoPath: "/home/user/photo2.jpeg"
                    guessedInformation: "Date: 1, Time 2"
                }

                ListElement {
                    photoPath: "/home/user/photo3.jpeg"
                    guessedInformation: "Date: 1, Time 2"
                }

                ListElement {
                    photoPath: "/home/user/photo4.jpeg"
                    guessedInformation: "Date: 1, Time 2"
                }
            }
            delegate: Item {
                required property string photoPath
                required property string guessedInformation

                width: parent.width
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
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:3}D{i:1}
}
##^##*/
