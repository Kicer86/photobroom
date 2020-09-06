
import QtQuick 2.14
import QtQuick.Layouts 1.14
import photo_broom.qml 1.0

/*
 * Top level view for diplaying photos from APhotoInfoModel.
 */

Item {
    id: photosViewId

    PhotosModelController {
        id: photosModelControllerId
        objectName: "photos_model_controller"      // used by c++ part to find this model and set it up

        newPhotosOnly: filterId.newPhotosOnly

        onDatesCountChanged: {
            filterId.timeRange.from = 0
            filterId.timeRange.to = photosModelControllerId.datesCount > 0? photosModelControllerId.datesCount - 1: 0

            filterId.timeRange.viewFrom.value = 0
            filterId.timeRange.viewTo.value = photosModelControllerId.datesCount > 0? photosModelControllerId.datesCount - 1 : 0
        }

        searchExpression: filterId.searchExpression
    }

    ColumnLayout {

        anchors.fill: parent

        Flow {
            Filter {
                id: filterId

                controller: photosModelControllerId

                visible: photosModelControllerId.datesCount > 0
            }
        }

        PhotosGridView {
            id: photosGridViewId

            clip: true
            model: photosModelControllerId.photos
            thumbnailSize: thumbnailSliderId.size

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            populate: Transition {
                NumberAnimation { properties: "x,y"; duration: 1000 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 250 }
            }

            ThumbnailSlider {
                id: thumbnailSliderId
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }
        }

    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
