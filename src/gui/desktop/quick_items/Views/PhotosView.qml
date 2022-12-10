
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.singletons
import quick_items
import "../Components" as Components
import "ViewsComponents" as Internals

/*
 * Top level view for diplaying photos from APhotoInfoModel.
 */

FocusScope {
    id: photosViewId

    property var selectedPhotos: []                // list of selected photo ids
    state: "gallery"

    PhotosModelController {
        id: photosModelControllerId
        objectName: "photos_model_controller"      // used by c++ part to find this model and set it up

        database: PhotoBroomProject.database

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
        anchors.bottom: parent.bottom

        initialItem: photosPage

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

        // actions on database close/open
        Connections {
            target: PhotoBroomProject

            function onProjectOpenChanged(open) {
                if (open === false)
                    photosArea.pop();
            }
        }
    }

    Component {
        id: photosPage

        Internals.PhotosGridView {
            // grid with photos
            id: gridView

            StackView.visible: true

            clip: true
            activeFocusOnTab: true
            keyNavigationEnabled: true

            flickDeceleration: 10000
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

            MouseArea {
                anchors.fill: parent

                acceptedButtons: Qt.RightButton
                propagateComposedEvents: true

                onClicked: function(mouse) {
                    contextMenu.selection = photosView.selectedPhotos;
                    contextMenu.popup(mouse.x, mouse.y);
                }
            }

            Menu {
                id: contextMenu

                property alias selection: contextMenuManager.selection

                ContextMenuManager {
                    id: contextMenuManager

                    project: PhotoBroomProject.project
                    coreFactory: PhotoBroomProject.coreFactory

                    onFaceRecognitionAction: {
                        if (selectedIndexes.length == 1)
                            turnOnFaceRecognitionMode(selectedIndexes[0])
                        else
                            console.error("No photo selected, weird")
                    }
                }

                Instantiator {
                    id: instantiator

                    model: contextMenuManager.model

                    delegate: MenuItem {
                        required property var actionName
                        required property var actionEnabled
                        required property var actionIndex

                        enabled: actionEnabled
                        text: actionName

                        onTriggered: {
                            contextMenu.close();
                            contextMenuManager.model.trigger(actionIndex);
                        }
                    }

                    onObjectAdded: (index, object) => contextMenu.insertItem(index, object)
                    onObjectRemoved: (object) => contextMenu.removeItem(object)
                }
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

            function turnOnFullscreenMode(index) {
                var id = gridView.model.getId(index);

                photosArea.push(fullscreenPage, {"photoID": id});
                console.log("Fullscreen mode for photo: " + gridView.model.getPhotoPath(index));
            }

            function turnOnFaceRecognitionMode(index) {
                var id = gridView.model.getId(index);

                photosArea.push(faceRecognitionPage, {"photoID": id});
            }
        }
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
                clip: true
            }
        }
    }

    Component {
        id: faceRecognitionPage

        FacesDialog {
            clip: true
        }
    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
