import QtQuick 2.0
import QtQuick.Controls 2.15

Column {
    id: column

    state: "advancedState"

    Row {
        id: row

        Text {
            id: search
            text: qsTr("Search:")
        }

        TextInput{
            id: searchExpression
            width: 200
        }

        Switch {
            id: advancedOptionsButton
            text: qsTr("Advanced")
            checked: true
        }

    }

    Row {
        id: row1

        Text {
            id: categoriesText
            text: qsTr("Category:")
        }

        ComboBox {
            id: comboBox
        }
    }

    states: [
        State {
            name: "simpleState"
            when: advancedOptionsButton.checked == false

            PropertyChanges {
                target: row1
                visible: false
            }
        },
        State {
            name: "advancedState"
            when: advancedOptionsButton.checked
        }
    ]
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
