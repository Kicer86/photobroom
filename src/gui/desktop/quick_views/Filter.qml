import QtQuick 2.0
import QtQuick.Controls 2.15

Item {
    id: column

    property alias searchExpression: searchExpression.text
    property alias from: timeRange.from
    property alias to: timeRange.to
    property alias viewFrom: timeRange.viewFrom
    property alias viewTo: timeRange.viewTo

    property var controller

    height: childrenRect.height
    width: childrenRect.width

    clip: true

    state: "simpleState"

    Row {
        id: row
        spacing: 2

        TimeRange {
            id: timeRange

            model: column.controller
            visible: column.controller.datesCount > 0

            Connections {
                target: timeRange.viewFrom
                function onPressedChanged() {
                    if (timeRange.viewFrom.pressed === false)
                        column.controller.timeViewFrom = timeRange.viewFrom.value
                }
            }

            Connections {
                target: timeRange.viewTo
                function onPressedChanged() {
                    if (timeRange.viewTo.pressed === false)
                        column.controller.timeViewTo = timeRange.viewTo.value
                }
            }
        }

        Text {
            id: search
            text: qsTr("Search:")
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            id: rectangle
            width: 200
            height: 24
            color: "white"
            border.color: "black"

            TextInput{
                id: searchExpression
                anchors.rightMargin: 2
                anchors.leftMargin: 2
                anchors.fill: parent
            }
        }

        Switch {
            id: moreOptionsButton
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: qsTr("More options")
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Row {
        id: row1
        spacing: 2
        anchors.top: row.bottom

        Text {
            id: categoriesText
            text: qsTr("Category:")
            anchors.verticalCenter: parent.verticalCenter
        }

        ComboBox {
            id: comboBox
        }
    }

    states: [
        State {
            name: "simpleState"
            when: moreOptionsButton.checked == false

            PropertyChanges {
                target: column
                height: row.height
            }
        },
        State {
            name: "advancedState"
            when: moreOptionsButton.checked

            PropertyChanges {
                target: column
                height: row.height + row1.height
            }
        }
    ]

    transitions: Transition {
        PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad; duration: 200 }
    }
}


