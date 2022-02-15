
import QtQuick 2.15
import QtQuick.Controls 2.15
import "ViewsComponents" as Internals
import "../Components" as Components
import photo_broom.qml


SwipeView {
    id: mainWindow
    objectName: "MainWindow"

    property bool projectOpened: false
    property var selectedPhotos: null

    anchors.fill: parent

    interactive: false

    onCurrentIndexChanged: selectedPhotos = [];            // reset selected photos when view changes

    // main view
    SplitView {
        Column {
            SplitView.preferredWidth: parent.width * 3/4

            PhotosView {
                id: photosView

                enabled: projectOpened

                width: parent.width
                height: parent.height - notifications.height - tasksView.height

                onSelectedPhotosChanged: mainWindow.selectedPhotos = selectedPhotos

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

            Components.TasksView {
                id: tasksView
                objectName: "TasksView"
                height: 100
                width: 300
            }

            Internals.NotificationsBar {
                id: notifications

                width: parent.width
            }
        }

        Column {
            SplitView.fillWidth: true

            spacing: 10

            GroupBox {
                objectName: "TagEditor"

                width: parent.width
                title: qsTr("<b>Properties</b>")

                TagEditor {
                    enabled: projectOpened
                    width: parent.width

                    selection: mainWindow.selectedPhotos
                }
            }

            GroupBox {
                objectName: "PropertiesWindow"

                width: parent.width
                title: qsTr("<b>Media information</b>")

                TableView {
                    id: propertiesTable
                    implicitHeight: contentHeight
                    implicitWidth: contentWidth
                    columnSpacing: 5

                    model: PhotoPropertiesModel {
                        property var _photos: mainWindow.selectedPhotos
                        database: PhotoBroomProject.database

                        on_PhotosChanged: setPhotos(_photos)
                    }

                    delegate: Text {
                        text: display
                    }
                }
            }

            GroupBox {
                width: parent.width
                title: qsTr("<b>Debug window</b>")

                DebugWindow {
                    objectName: "DebugWindow"
                }
            }
        }
    }

    // photo data completion view
    Item {

        Button {
            id: backButton

            text: qsTr("Back to photos")

            onClicked: mainWindow.currentIndex = 0
        }

        PhotoDataCompletion {
            id: completer
            width: parent.width
            anchors.bottom: parent.bottom
            anchors.top: backButton.bottom

            onSelectedPhotoChanged: mainWindow.selectedPhotos = selectedPhoto
        }
    }
}
