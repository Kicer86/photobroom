
import QtQuick 2.0
import quick_items
import photo_broom.singletons


Item {
    implicitWidth: childrenRect.width
    implicitHeight: childrenRect.height

    Column {
        Repeater {
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
                    text: qsTr("Tasks executed") + ": " + modelData.tasksExecuted
                }

                Text {
                    text: qsTr("Execution speed") + ": " + modelData.executionSpeed + " " + qsTr("tps", "tasks per second");
                }

                Text {
                    text: qsTr("Tasks:");
                    font.italic: true
                }

                Repeater {
                    model: modelData.tasks

                    delegate: Text {
                        required property string name
                        required property int count

                        text: " " + name + " " + count
                    }
                }
            }
        }
    }
}
