
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
                id: gridView

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

                        Picture {
                            anchors.horizontalCenter: parent.horizontalCenter
                            height: 150
                            width: 150
                            source: decoration
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: display
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            gridView.currentIndex = index
                        }
                    }
                }

                highlight: Item {
                    height: 150
                    width: 150
                    z: 2

                    Rectangle {
                        anchors.top: parent.top
                        anchors.right: parent.right

                        width: 20
                        height: 20
                        radius: 10
                        color: "green"
                    }

                    Rectangle {
                        anchors.top: parent.top
                        anchors.left: parent.left

                        width: 20
                        height: 20
                        radius: 10
                        color: "red"
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
