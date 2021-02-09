
import QtQuick 2.15
import QtQml.Models 2.15


Item {
    property alias model: listView.model

    height: column.height
    state: "Hidden"

    clip: true

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

            Behavior on height { PropertyAnimation{} }

            delegate: Rectangle {
                required property string display

                color: "goldenrod"
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

        Rectangle {
            id: notifications

            height: 0
            width: parent.width

            color: "deepskyblue"
            radius: 5
            border.width: 1

            Text {
                id: notificationsText

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 5
                anchors.rightMargin: 5

                text: fullMode? qsTr("Click here to collapse notifications"):
                                qsTr("There are %n notification(s). Click here to read", "0", listView.model.count)
            }

            MouseArea {
                anchors.fill: parent

                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    fullMode = !fullMode
                }
            }
        }
    }

    states: [
        State {
            name: "Hidden"

            when: listView.model.count == 0
        },
        State {
            name: "OneLine"

            when: listView.model.count > 0 && !fullMode

            PropertyChanges {
                target: notifications
                height: notificationsText.height
            }
        },
        State {
            name: "Full"

            when: listView.model.count > 0 && fullMode

            PropertyChanges {
                target: listView
                height: 100
            }

            PropertyChanges {
                target: notifications
                height: notificationsText.height
            }
        }
    ]
}
