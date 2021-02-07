
import QtQuick 2.15
import QtQml.Models 2.15


Item {
    id: rectangle

    property alias model: listView.model

    height: column.height
    state: "Hidden"

    Column {
        id: column

        width: parent.width
        height: listView.height + notifications.height

        ListView {
            id: listView
            clip: true
            height: 0
            width: parent.width

            delegate: Rectangle {
                color: "lightsteelblue"
                radius: 5
                border.width: 0

                Text {
                    text: notificationText
                }
            }
        }

        Item {
            id: notifications

            height: 0
            width: parent.width

            MouseArea {
                anchors.fill: parent

                cursorShape: Qt.PointingHandCursor

                onClicked: {

                }
            }

            Text {
                id: notificationsText
                text: qsTr("There are %n notification(s) awaiting.", "0", 0)
            }
        }
    }

    states: [
        State {
            name: "Hidden"
        },
        State {
            name: "OneLine"

            PropertyChanges {
                target: notifications
                height: notificationsText.height
            }
        },
        State {
            name: "Full"

            PropertyChanges {
                target: listView
                height: 60
            }

            PropertyChanges {
                target: notifications
                height: notificationsText.height
            }
        }
    ]
}
