
import QtQuick
import QtQuick.Controls

import photo_broom.models
import photo_broom.singletons
import photo_broom.utils
import "ViewsComponents" as Internals


Item {

    QMLFlatModel {
        id: data_model
        database: PhotoBroomProject.database
        text_filters: [data_model.facesNotAnalysed, data_model.validMedia]
    }

    BatchFaceDetector {
        id: detector
        core: PhotoBroomProject.coreFactory
        photos_model: data_model
    }

    GroupBox {
        anchors.fill: parent
        title: qsTr("Photos to be analyzed")

        Internals.PhotosGridView {
            anchors.fill: parent
            model: data_model
        }
    }
}
