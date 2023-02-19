
import QtQuick 2.15
import photo_broom.models
import photo_broom.singletons
import "ViewsComponents" as Internals


Item {

    QMLFlatModel {
        id: data_model
        database: PhotoBroomProject.database
        text_filters: [data_model.facesNotAnalysed, data_model.validMedia]
    }

    Internals.PhotosGridView {
        anchors.fill: parent
        model: data_model
    }
}
