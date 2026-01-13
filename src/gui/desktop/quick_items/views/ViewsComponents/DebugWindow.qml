
import QtQuick
import QtQuick.Controls

import quick_items.components as Components

import photo_broom.models
import photo_broom.singletons


Item {
    id: root

    implicitWidth: 300
    implicitHeight: contentColumn.implicitHeight

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

                Components.CollapsibleGroupBox {
                    id: executorGroup

                    width: parent.width
                    clip: true

                    readonly property ObservableExecutorModel executorModel: ObservableExecutorModel {
                        executor: modelData
                    }

                    title: "<b>" + executorModel.title + "</b>"

                    TreeView {
                        width: parent.width
                        clip: true
                        model: executorModel
                        delegate: TreeViewDelegate {
                            id: treeDelegate

                            contentItem: Item {
                                implicitHeight: Math.max(labelText.implicitHeight, valueText.implicitHeight)
                                implicitWidth: labelText.implicitWidth + (valueText.visible ? valueText.implicitWidth + 12 : 0)

                                Text {
                                    id: labelText
                                    anchors.left: parent.left
                                    anchors.right: valueText.left
                                    anchors.rightMargin: valueText.visible ? 12 : 0
                                    anchors.verticalCenter: parent.verticalCenter
                                    elide: Text.ElideRight
                                    color: treeDelegate.highlighted ? treeDelegate.palette.highlightedText : treeDelegate.palette.buttonText
                                    text: model.label !== undefined ? model.label : model.display
                                }

                                Text {
                                    id: valueText
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: visible ? implicitWidth : 0
                                    color: treeDelegate.highlighted ? treeDelegate.palette.highlightedText : treeDelegate.palette.buttonText
                                    text: model.value !== undefined ? model.value : ""
                                    visible: model.value !== undefined
                                }
                            }
                        }

                        implicitHeight: contentHeight
                        height: implicitHeight
                    }
                }
            }
        }
    }
}
