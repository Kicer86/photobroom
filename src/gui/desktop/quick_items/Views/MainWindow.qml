
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.models
import photo_broom.singletons
import photo_broom.items
import "ViewsComponents" as Internals
import "../Components" as Components


ApplicationWindow {
    id: mainWindow
    objectName: "MainWindow"

    property bool projectOpened: false
    property var selectedPhotos: null

    // TODO: these signals should be removed.
    //       Some cpp singletons could manage it.
    signal openProject(string path)
    signal openProjectDialog()
    signal newProject()
    signal closeProject()
    signal scanCollection()
    signal seriesDetector()
    signal configuration()

    visible: true
    width: 1024
    height: 768

    menuBar: MenuBar {
        Menu {
            title: qsTr("&Photo collection")
            Action { text: qsTr("&New...");   icon.name: "document-new";  onTriggered: newProject(); }
            Action { text: qsTr("&Open...");  icon.name: "document-open"; onTriggered: openProjectDialog(); }
            Menu {
                id: recentsMenu
                title: qsTr("Open &recent")

                width: {
                    var result = 0;
                    var padding = 0;

                    for (var i = 0; i < count; i++) {
                        var item = itemAt(i);
                        result = Math.max(item.contentItem.implicitWidth, result);
                        padding = Math.max(item.padding, padding);
                    }

                    return result + padding * 2;
                }

                Instantiator {
                    model: PhotoBroomProject.recentProjects

                    delegate: MenuItem {
                        required property var modelData

                        text: modelData

                        onTriggered: mainWindow.openProject(modelData)
                    }

                    onObjectAdded: (index, object) => recentsMenu.insertItem(index, object)
                    onObjectRemoved: (object) => recentsMenu.removeItem(object)
                }
            }
            Action { text: qsTr("&Close"); enabled: projectOpened; icon.name: "window-close"; onTriggered: closeProject(); }
            MenuSeparator { }
            Action { text: qsTr("&Quit"); icon.name: "application-exit"; onTriggered: Qt.quit(); }
        }
        Menu {
            title: qsTr("P&hotos")
            enabled: projectOpened
            Action { text: qsTr("S&can collection..."); onTriggered: scanCollection(); }
        }
        Menu {
            title: qsTr("&Windows")
            Action { text: qsTr("P&roperties");        checkable: true; checked: tagEditor.visible;        onTriggered: tagEditor.visible = !tagEditor.visible }
            Action { text: qsTr("&Media information"); checkable: true; checked: propertiesWindow.visible; onTriggered: propertiesWindow.visible = !propertiesWindow.visible }
            Action { text: qsTr("T&asks");             checkable: true; checked: tasksViewDock.visible;    onTriggered: tasksViewDock.visible = !tasksViewDock.visible; }
        }
        Menu {
            title: qsTr("&Tools")
            enabled: projectOpened
            Action { text: qsTr("S&eries detector...");       onTriggered: seriesDetector(); }
            Action { text: qsTr("Ph&oto data completion..."); onTriggered: mainView.currentIndex = 1; }
        }
        Menu {
            title: qsTr("&Settings")
            Action { text: qsTr("&Configuration"); onTriggered: configuration();  }
        }
    }

    SwipeView {
        id: mainView
        anchors.fill: parent

        interactive: false

        onCurrentIndexChanged: selectedPhotos = [];            // reset selected photos when view changes

        // main view
        SplitView {
            Column {
                SplitView.preferredWidth: parent.width * 3/4

                PhotosView {
                    id: photosView

                    enabled: mainWindow.projectOpened

                    width: parent.width
                    height: parent.height - notifications.height - tasksViewDock.height

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

                GroupBox {
                    id: tasksViewDock

                    width: parent.width
                    title: "<b>" + qsTr("Tasks") +"</b"

                    Components.TasksView {
                        id: tasksView
                        objectName: "TasksView"
                        width: parent.width
                        implicitHeight: contentHeight

                        Behavior on implicitHeight {
                            NumberAnimation { duration: 1000 }
                        }
                    }
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
                    id: tagEditor

                    width: parent.width
                    title: qsTr("<b>Properties</b>")

                    TagEditor {
                        enabled: mainWindow.projectOpened
                        width: parent.width

                        selection: mainWindow.selectedPhotos
                    }
                }

                GroupBox {
                    id: propertiesWindow

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
                    id: debugWindow
                    width: parent.width
                    visible: ObservablesRegistry.enabled
                    title: qsTr("<b>Debug window</b>")

                    DebugWindow { }
                }
            }
        }

        // photo data completion view
        Item {

            Button {
                id: backButton

                text: qsTr("Back to photos")

                onClicked: mainView.currentIndex = 0
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

}
