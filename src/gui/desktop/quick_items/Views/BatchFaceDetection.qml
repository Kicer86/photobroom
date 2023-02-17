
import QtQuick 2.15
import photo_broom.models
import photo_broom.singletons


Item {

    QMLFlatModel {
        id: data_model
        database: PhotoBroomProject.database
        text_filters: ["faces not analysed"]
    }
}
