
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.singletons
import photo_broom.items
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

        database: PhotoBroomProject.database

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

    StackView {
        id: photosArea

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: filterId.bottom
        anchors.bottom: infoItem.top

        focus: true
        focusPolicy: Qt.StrongFocus

        initialItem: Internals.PhotosGridView {
            // grid with photos
            id: gridView

            StackView.visible: true

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
        }

        pushEnter: Transition { PropertyAnimation { property: "opacity"; from: 0.0; to: 1.0} }
        pushExit:  Transition { PropertyAction    { property: "enabled"; value: false} }

        popEnter:  Transition { PropertyAction    { property: "enabled"; value: true} }
        popExit:   Transition { PropertyAnimation { property: "opacity"; to: 0.0} }

        Keys.onPressed: function(event) {
            if (event.key == Qt.Key_Escape && photosArea.depth > 1) {
                photosArea.pop();
                event.accepted = true;
            }
        }

        // TODO: troublesome item. Without it StackView does not get focus when clicked
        // so Keys.onPressed won't work. Adding this item helps, but requires z:1,
        // otherwise any new page pushed to StackView overlaps it and it does not work.
        // z:1 from the other side causes that scrollbars and other elements of
        // Internals.PhotosGridView (initial page) do not get focus for some reason...
        // Current solution is not perfect either - after poping page, Internals.PhotosGridView
        // requires double click to get focus. But that's the least broken solution.
        Components.Focuser { enabled: photosArea.depth > 1; z: 1}
    }

    Component {
        id: fullscreenPage

        Item {
            id: fullscreenView

            property alias photoID: fullscreenImage.photoID

            visible: opacity != 0.0

            Behavior on opacity { PropertyAnimation{} }

            Rectangle {
                // gallery shadow for full screen mode
                id: shadow

                anchors.fill: parent
                color: "black"
                opacity: 0.7
            }

            Components.MediaViewItem {
                id: fullscreenImage

                anchors.fill: parent
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
        var id = gridView.model.getId(index);

        photosArea.push(fullscreenPage, {"photoID": id});
        console.log("Fullscreen mode for photo: " + gridView.model.getPhotoPath(index));
    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
