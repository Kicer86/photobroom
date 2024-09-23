
import QtQuick
import QtQuick.Controls
import photo_broom.singletons
import quick_items
import QmlItems
import "../Components" as Components
import "ViewsComponents" as Internals

/*
 * Top level view for diplaying photos from APhotoInfoModel.
 */

Item {
    id: photosViewId

    state: "gallery"

    PhotosModelController {
        id: photosModelControllerId
        objectName: "photos_model_controller"      // used by c++ part to find this object and set it up

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

        // actions on database close/open
        Connections {
            target: PhotoBroomProject

            function onProjectOpenChanged(open) {
                if (open === false)
                    photosArea.pop();
            }
        }

        Keys.onPressed: function(event) {
            if (event.key == Qt.Key_Escape && photosArea.depth > 1) {
                event.accepted = true;
                photosArea.pop();
                console.log("Closing page");
            }
        }
    }

    Component {
        id: photosPage

        SplitView {

            StackView.visible: true

            // grid with photos
            Internals.PhotosGridView {
                id: gridView

                SplitView.preferredWidth: parent.width * 3/4

                clip: true

                model: photosModelControllerId.photos
                thumbnailSize: thumbnailSliderId.size

                property var selectedPhotos: []                // list of selected photo ids

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

                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    propagateComposedEvents: true

                    onClicked: function(mouse) {
                        forceActiveFocus();
                        mouse.accepted = false;                                 // reject event, so it will be passed down, to particular items

                        if (mouse.button == Qt.RightButton) {
                            contextMenu.selection = gridView.selectedPhotos;
                            contextMenu.popup(mouse.x, mouse.y);
                        }
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
                            if (gridView.selectedIndexes.length == 1)
                                gridView.turnOnFaceRecognitionMode(gridView.selectedIndexes[0])
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
                    console.log("Face recognition mode for photo: " + gridView.model.getPhotoPath(index));
                }
            }

            Column {
                SplitView.fillWidth: true

                spacing: 10

                CollapsibleGroupBox {
                    id: tagEditor

                    width: parent.width
                    title: qsTr("<b>Properties</b>")

                    TagEditor {
                        enabled: PhotoBroomProject.projectOpen

                        selection: gridView.selectedPhotos
                    }
                }

                CollapsibleGroupBox {
                    id: propertiesWindow

                    width: parent.width
                    title: qsTr("<b>Media information</b>")
                    clip: true

                    Components.MediaPropertiesView {
                        photos: gridView.selectedPhotos
                    }
                }

                CollapsibleGroupBox {
                    id: debugWindow

                    width: parent.width
                    visible: ObservablesRegistry.enabled
                    title: qsTr("<b>Debug window</b>")
                    clip: true

                    Internals.DebugWindow { }
                }
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

            ShadowBackground {
                anchors.fill: parent
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

        FacesDialog { }
    }
}
