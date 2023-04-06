
import QtQuick
import QtQuick.Controls

import photo_broom.models
import photo_broom.singletons
import photo_broom.utils
import QmlItems
import "ViewsComponents" as Internals


Item {

    QMLFlatModel {
        id: data_model
        text_filters: [data_model.facesNotAnalysed, data_model.validMedia]
        database: PhotoBroomProject.database
    }

    BatchFaceDetector {
        id: detector
        core: PhotoBroomProject.coreFactory
        db: PhotoBroomProject.database
        photos_model: data_model
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        GroupBox {
            title: qsTr("Discovered faces")
            clip: true

            SplitView.minimumHeight: 150

            GridView {
                anchors.fill: parent
                model: detector

                cellWidth: 170
                cellHeight: 200

                delegate: Item {
                    required property var decoration
                    required property var display
                    required property var index

                    width: 170
                    height: 200

                    Column {
                        anchors.fill: parent

                        Item {
                            anchors.horizontalCenter: parent.horizontalCenter
                            height: 150
                            width: 150

                            Picture {
                                anchors.fill: parent
                                source: decoration
                            }

                            MouseArea {
                                id: mouseArea
                                anchors.fill: parent
                                hoverEnabled: true

                                Item {
                                    anchors.fill: parent

                                    opacity: mouseArea.containsMouse? 1.0: 0.0
                                    Behavior on opacity { PropertyAnimation{} }

                                    ImageButton {
                                        anchors.top: parent.top
                                        anchors.right: parent.right

                                        width: 20
                                        height: 20
                                        source: "qrc:/gui/ok.svg"

                                        style: ImageButton.Scale
                                    }

                                    ImageButton {
                                        anchors.top: parent.top
                                        anchors.left: parent.left

                                        width: 20
                                        height: 20
                                        source: "qrc:/gui/trash.svg"

                                        style: ImageButton.Scale
                                    }
                                }
                            }
                        }

                        LineEdit {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: display
                            width: parent.width - 40
                        }
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("Photos to be analyzed")
            clip: true

            SplitView.minimumHeight: 150

            Internals.PhotosGridView {
                anchors.fill: parent
                model: data_model
            }
        }
    }
}
