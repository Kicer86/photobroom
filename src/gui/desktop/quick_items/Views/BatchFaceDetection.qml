
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

            GridView {
                anchors.fill: parent
                model: detector

                cellWidth: 170
                cellHeight: 170

                delegate: Column {
                    required property var decoration
                    required property var display

                    Picture {
                        height: 150
                        width: 150
                        source: decoration
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: display
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("Photos to be analyzed")
            clip: true

            Internals.PhotosGridView {
                anchors.fill: parent
                model: data_model
            }
        }
    }
}
