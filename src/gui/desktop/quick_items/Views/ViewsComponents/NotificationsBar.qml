
import QtQuick 2.15
import QtQml.Models 2.15
import QtQuick.Controls 2.15
import QmlItems
import "../../Components" as Components


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

            spacing: 2
            model: ListModel {}

            Behavior on height { PropertyAnimation{} }

            delegate: Rectangle {
                required property string display
                required property string index

                color: "goldenrod"
                radius: 5
                border.width: 1

                height: text.height + text.y * 2
                width: listView.width - listView.ScrollBar.vertical.width

                Text {
                    id: text

                    y: 3

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5

                    text: parent.display
                }

                ImageButton {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.rightMargin: 5
                    anchors.topMargin: 5

                    width: 16
                    height: 16

                    source: "qrc:/gui/close.svg"

                    onClicked: {
                        listView.model.removeRow(index);
                    }
                }
            }

            ScrollBar.vertical: ScrollBar { }
        }

        Components.InfoItem {
            id: notifications

            height: 0
            width: parent.width

            text: fullMode? qsTr("Click here to collapse notifications."):
                            qsTr("There are %n notification(s). Click here to read.", "", listView.model.count)

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
                height: notifications.desiredHeight
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
                height: notifications.desiredHeight
            }
        }
    ]
}
