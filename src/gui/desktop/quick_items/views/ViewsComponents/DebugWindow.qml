
import QtQuick
import QtQuick.Controls
import QmlItems

import photo_broom.singletons
import quick_items.components as Components


Item {
    id: root

    implicitWidth: 300
    implicitHeight: Math.min(contentColumn.implicitHeight, maxHeight)

    property int maxHeight: 280
    property int maxTasksViewHeight: 220

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true

        Column {
            id: contentColumn
            width: scrollView.availableWidth
            spacing: 8

            Repeater {
                model: ObservablesRegistry.executors

                Column {
                    width: parent.width
                    spacing: 4

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

                    Components.CollapsibleGroupBox {
                        width: parent.width
                        title: qsTr("<b>Tasks</b>")
                        background: Item {}
                        clip: true
                        collapsed: true

                        ListView {
                            width: parent.width
                            clip: true

                            model: modelData.tasks
                            implicitHeight: Math.min(contentHeight, root.maxTasksViewHeight)
                            height: implicitHeight
                            interactive: contentHeight > height

                            delegate: Row {
                                required property string name
                                required property int count

                                width: ListView.view.width
                                spacing: 10

                                Text { text: name }
                                Text { text: count }
                            }
                        }
                    }
                }
            }
        }
    }
}
