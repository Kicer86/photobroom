
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.qml 1.0
import "../Components" as Components
import "ViewsComponents" as Internals

/*
 * Top level view for diplaying photos from APhotoInfoModel.
 */

Item {
    id: photosViewId

    property var selectedPhotos: []                // list of selected photo ids
    state: "gallery"

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
        id: photosArea

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: filterId.bottom
        anchors.bottom: infoItem.top

        Internals.PhotosGridView {
            // grid with photos
            id: gridView

            anchors.fill: parent

            clip: true
            activeFocusOnTab: true
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

            onItemDoubleClicked: (index) => turnOnFullscreenMode(index)

            onSelectedIndexesChanged: function() {
                var ids = [];

                selectedIndexes.forEach((index) => {
                    var id = gridView.model.getId(index);
                    ids.push(id);
                });

                selectedPhotos = ids;
            }

            Components.Focuser { }
        }

        Item {
            id: fullscreenView
            anchors.fill: parent

            visible: opacity != 0.0

            Behavior on opacity { PropertyAnimation{} }

            Rectangle {
                // gallery shadow for full screen mode
                id: shadow

                anchors.fill: parent
                color: "black"
                opacity: 0.7

                Behavior on opacity { PropertyAnimation{} }

                MouseArea {
                    anchors.fill: parent

                    propagateComposedEvents: false

                    onClicked: {}
                }
            }

            Components.MediaViewItem {
                // image in full screen mode

                id: fullscreenImage

                anchors.fill: parent

                Keys.onPressed: function(event) {
                    if (event.key == Qt.Key_Escape) {
                        turnOnGalleryMode();
                        event.accepted = true;
                    }
                }

                Components.Focuser { }
            }
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

    function turnOnFullscreenMode(index) {
        photosViewId.state = "fullscreen"

        var id = gridView.model.getId(index);
        fullscreenImage.photoID = id;
        fullscreenImage.forceActiveFocus(Qt.MouseFocusReason);

        console.log("Fullscreen mode for photo: " + gridView.model.getPhotoPath(index));
    }

    function turnOnGalleryMode() {
        photosViewId.state = "gallery"

        fullscreenImage.reset();
        gridView.forceActiveFocus(Qt.MouseFocusReason);
    }

    states: [
        State {
            name: "gallery"

            PropertyChanges {
                target: fullscreenView
                opacity: 0.0
            }
        },
        State {
            name: "fullscreen"

            PropertyChanges {
                target: fullscreenView
                opacity: 1.0
            }
        }
    ]

}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
