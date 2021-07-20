
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.qml 1.0
import "../Components" as Components
import "DialogsComponents" as Internals

/*
 * Top level view for diplaying photos from APhotoInfoModel.
 */

Item {
    id: photosViewId

    PhotosModelController {
        id: photosModelControllerId
        objectName: "photos_model_controller"      // used by c++ part to find this model and set it up

        newPhotosOnly: filterId.newPhotosOnly
        category: filterId.categoryComboBox.currentIndex
        ratingFrom: filterId.ratingSlider.first.value
        ratingTo: filterId.ratingSlider.second.value

        onDatesCountChanged: {
            filterId.timeRange.from = 0
            filterId.timeRange.to = photosModelControllerId.datesCount > 0? photosModelControllerId.datesCount - 1: 0

            filterId.timeRange.viewFrom.value = 0
            filterId.timeRange.viewTo.value = photosModelControllerId.datesCount > 0? photosModelControllerId.datesCount - 1 : 0
        }

        searchExpression: filterId.searchExpression
    }

    Internals.Filter {
        id: filterId

        controller: photosModelControllerId

        visible: photosModelControllerId.datesCount > 0
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: filterId.bottom
        anchors.bottom: infoItem.top

        Internals.PhotosGridView {
            // grid with photos

            id: gridView

            anchors.fill: parent

            clip: true
            focus: true
            keyNavigationEnabled: true

            model: photosModelControllerId.photos
            thumbnailSize: thumbnailSliderId.size

            populate: Transition {
                NumberAnimation { properties: "x,y"; duration: 1000 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 250 }
            }

            Components.ThumbnailSlider {
                id: thumbnailSliderId
                anchors.bottom: parent.bottom
                anchors.right: parent.ScrollBar.vertical.left
            }

            onItemDoubleClicked: {
                fullscreenImage.setPhoto(index);
            }
        }

        Image {
            // image in full screen mode

            id: fullscreenImage

            function setPhoto(index) {
                var path = gridView.model.getPhotoPath(index);
                fullscreenImage.source = path;
                fullscreenImage.opacity = 1.0;
                fullscreenImage.focus = true;
                fullscreenImage.currentIndex = index;

                console.log("Fullscreen mode for photo: " + fullscreenImage.source);
            }

            anchors.fill: parent
            visible: opacity != 0.0
            opacity: 0.0

            asynchronous: true
            autoTransform: true
            fillMode: Image.PreserveAspectFit

            property int currentIndex: 0

            Behavior on opacity { PropertyAnimation{} }

            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: false

                onClicked: {
                    fullscreenImage.opacity = 0.0
                }
            }

            onStatusChanged: {
                if (fullscreenImage.status == Image.Error)
                    fullscreenImage.source = "qrc:/gui/error.svg";
            }

            /*
            Keys.onPressed: {
                if (event.key == Qt.Key_Left) {
                    fullscreenImage.setPhoto(fullscreenImage.currentIndex - 1);
                    event.accepted = true;
                }
                else if (event.key == Qt.Key_Right) {
                    fullscreenImage.setPhoto(fullscreenImage.currentIndex + 1);
                    event.accepted = true;
                }
            }
            Text {
                id: leftArrow
                color: "#ffffff"
                text: "<"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                font.family: "Times New Roman"
                font.pointSize: parent.height/4

                transform: Scale { origin.x: 0; xScale: 0.5}
            }

            Text {
                id: rightArrow
                color: "#ffffff"
                text: ">"
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                font.family: "Times New Roman"
                font.pointSize: parent.height/4

                transform: Scale { origin.x: rightArrow.width; xScale: 0.5}
            }
            */
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

                    text: qsTr("Click 'Accept' button to mark <b>all</b> new photos as reviewed.")
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
