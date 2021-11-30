
import QtQuick 2.0
import photo_broom.qml 1.0


Item {
    width: childrenRect.width
    height: childrenRect.height

    Column {
        Repeater {
            id: repeater
            model: ObservablesRegistry.executors

            Column {
                Text {
                    text: modelData.name
                    font.bold: true
                }

                Text {
                    text: qsTr("Tasks in queue") + ": " + modelData.awaitingTasks
                }

                Text {
                    text: qsTr("Tasks executed")+ ": " + modelData.tasksExecuted
                }

                Text {
                    text: qsTr("Execution speed")+ ": " + modelData.executionSpeed + " " + qsTr("tps", "tasks per second");
                }
            }
        }
    }
}
