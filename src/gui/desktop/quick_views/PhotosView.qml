
import QtQuick 2.14
import QtQuick.Controls 2.15
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

    Filter {
        id: filterId

        controller: photosModelControllerId

        visible: photosModelControllerId.datesCount > 0
    }

    PhotosGridView {
        id: photosGridViewId
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: filterId.bottom
        anchors.bottom: infoItem.top

        clip: true
        model: photosModelControllerId.photos
        thumbnailSize: thumbnailSliderId.size

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

    Item {
        id: infoItem
        height: 0

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true

        state: "hidden"

        Rectangle {
            id: infoBalloon

            width: childrenRect.width
            height: childrenRect.height

            color: "CornflowerBlue"
            radius: 5

            Row {
                spacing: 25

                Text {
                    id: infoItemText

                    text: qsTr("Click 'Accept' button to mark new photos as reviewed.")
                }

                Button {
                    text: qsTr("Accept")
                    anchors.verticalCenter: parent.verticalCenter

                    onClicked: photosModelControllerId.markNewAsReviewed()
                }
            }
        }

        states: [
            State {
                name: "hidden"
                when: filterId.newPhotosOnly == false
            },
            State {
                name: "visible"
                when: filterId.newPhotosOnly

                PropertyChanges {
                    target: infoItem
                    height: infoBalloon.height
                }
            }
        ]

        transitions: Transition {
            PropertyAnimation { properties: "height"; easing.type: Easing.InOutQuad; duration: 200 }
        }
    }
}



/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
