
import QtQuick 2.0

Item {
    width: grid.width
    height: grid.height

    Grid {
        id: grid
        width: childrenRect.width
        height: childrenRect.height
        columns: 2

        Text {
            text: qsTr("Executor tasks:")
            font.pixelSize: 12
        }

        Text {
            id: executorTasks
            text: "0"
            font.pixelSize: 12
        }

        Text {
            text: qsTr("Database tasks:")
            font.pixelSize: 12
        }

        Text {
            id: databaseTasks
            text: "0"
            font.pixelSize: 12
        }
    }
}
