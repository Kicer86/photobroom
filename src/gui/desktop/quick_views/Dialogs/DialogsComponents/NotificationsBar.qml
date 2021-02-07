
import QtQuick 2.15
import QtQml.Models 2.15


Item {
    property alias model: listView.model

    height: column.height
    state: "Hidden"

    clip: true

    function notificationsCount() {
        return listView.model.rowCount();
    }

    property bool fullMode: false

    Column {
        id: column

        width: parent.width
        height: listView.height + notifications.height

        ListView {
            id: listView
            objectName: "NotificationsList"

            clip: true

            height: 0
            width: parent.width

            model: ListModel {}

            delegate: Rectangle {
                required property string display

                color: "deepskyblue"
                radius: 5
                border.width: 1

                height: text.height + text.y * 2
                width: parent.width

                Text {
                    id: text

                    y: 3

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5

                    text: parent.display
                }
            }
        }

        Item {
            id: notifications

            height: 0
            width: parent.width

            Text {
                id: notificationsText
                text: qsTr("There are %n notification(s) awaiting.", "0", notificationsCount())

                MouseArea {
                    anchors.fill: parent

                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        fullMode = !fullMode
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "Hidden"

            when: notificationsCount() == 0
        },
        State {
            name: "OneLine"

            when: notificationsCount() > 0 && !fullMode

            PropertyChanges {
                target: notifications
                height: notificationsText.height
            }
        },
        State {
            name: "Full"

            when: notificationsCount() > 0 && fullMode

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
