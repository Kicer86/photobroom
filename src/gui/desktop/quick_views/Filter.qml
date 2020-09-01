import QtQuick 2.0
import QtQuick.Controls 2.15

Item {
    id: column

    property alias searchText: search.text

    height: childrenRect.height
    width: childrenRect.width

    clip: true

    state: "simpleState"

    Row {
        id: row

        Text {
            id: search
            text: qsTr("Search:")
            anchors.verticalCenter: parent.verticalCenter
        }

        TextInput{
            id: searchExpression
            width: 200
            font.pointSize: 15
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


