import QtQuick 2.0
import QtQuick.Controls 2.15

Item {
    id: root

    property alias searchExpression: searchExpression.text
    property alias timeRange: timeRange
    property bool newPhotosOnly: newPhotosSwitch.checked

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
            anchors.verticalCenter: parent.verticalCenter

            model: root.controller

            Connections {
                target: timeRange.viewFrom
                function onPressedChanged() {
                    if (timeRange.viewFrom.pressed === false)
                        root.controller.timeViewFrom = timeRange.viewFrom.value
                }
            }

            Connections {
                target: timeRange.viewTo
                function onPressedChanged() {
                    if (timeRange.viewTo.pressed === false)
                        root.controller.timeViewTo = timeRange.viewTo.value
                }
            }
        }

        ToolSeparator {}


        Text {
            id: search
            text: qsTr("Search:")
            anchors.verticalCenter: parent.verticalCenter
        }


        TextField {
            id: searchExpression
        }


        ToolSeparator {}


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

        Switch {
            id: newPhotosSwitch
            text: qsTr("")
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: qsTr("New photos only")
            anchors.verticalCenter: parent.verticalCenter
        }

        ToolSeparator {
            id: toolSeparator
        }

        Text {
            id: categoriesText
            text: qsTr("Category:")
            anchors.verticalCenter: parent.verticalCenter
        }

        ComboBox {
            id: comboBox

            model: controller.categories

            delegate: ItemDelegate {
                width: comboBox.width
                height: 25
                contentItem: Rectangle {
                    color: modelData

                    anchors.fill: parent
                    anchors.margins: 1.5
                }
                highlighted: comboBox.highlightedIndex === index
            }
        }
    }

    states: [
        State {
            name: "simpleState"
            when: moreOptionsButton.checked == false

            PropertyChanges {
                target: root
                height: row.height
            }
        },
        State {
            name: "advancedState"
            when: moreOptionsButton.checked

            PropertyChanges {
                target: root
                height: row.height + row1.height
            }
        }
    ]

    transitions: Transition {
        PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad; duration: 200 }
    }
}
