
import QtQuick 2.15
import QtQuick.Controls 2.15
import "ViewsComponents" as Internals
import photo_broom.qml


SwipeView {
    id: mainWindow
    objectName: "MainWindow"

    property bool projectOpened: false
    property var selectedPhotos: []

    anchors.fill: parent

    interactive: false

    onCurrentIndexChanged: {
        selectedPhotos = [];            // reset selected photos when view changes
    }

    // main view
    Column {
        PhotosView {
            id: photosView

            enabled: projectOpened

            width: parent.width
            height: parent.height - notifications.height

            onSelectedPhotosChanged: {
                mainWindow.selectedPhotos = selectedPhotos;
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
        }

        Internals.NotificationsBar {
            id: notifications

            width: parent.width
        }
    }

    // photo data completion view
    Item {

        Button {
            id: backButton

            text: qsTr("Back to photos")

            onClicked: {
                mainWindow.currentIndex = 0
            }
        }

        PhotoDataCompletion {
            id: completer
            width: parent.width
            anchors.bottom: parent.bottom
            anchors.top: backButton.bottom

            onSelectedPhotoChanged: {
                mainWindow.selectedPhotos = [ selectedPhoto ];
            }
        }
    }
}
