
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts

import photo_broom.models
import photo_broom.singletons
import quick_items

import "ViewsComponents" as Internals
import "../Components" as Components


ApplicationWindow {
    id: mainWindow
    objectName: "MainWindow"

    property bool projectOpened: false
    property string projectName: ""

    // TODO: these signals should be removed.
    //       cpp singletons could manage it.
    signal openProject(string path)
    signal openProjectDialog()
    signal newProject()
    signal closeProject()
    signal scanCollection()
    signal seriesDetector()
    signal configuration()

    title: projectOpened? "Photo broom: " + projectName : qsTr("No collection opened")

    visible: true
    width: 1024
    height: 768

    menuBar: MenuBar {
        Menu {
            id: collectionMenu
            title: qsTr("&Photo collection")
            Action { text: qsTr("&New...");   icon.name: "document-new";  onTriggered: { collectionMenu.dismiss(); newProject(); } }
            Action { text: qsTr("&Open...");  icon.name: "document-open"; onTriggered: { collectionMenu.dismiss(); openProjectDialog(); } }
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
            id: photosMenu
            title: qsTr("P&hotos")
            enabled: projectOpened
            Action { text: qsTr("S&can collection..."); onTriggered: { photosMenu.dismiss(); scanCollection(); } }
        }
        Menu {
            title: qsTr("&Windows")
            Action { text: qsTr("P&roperties");        checkable: true; checked: tagEditor.visible;        onTriggered: tagEditor.visible = !tagEditor.visible }
            Action { text: qsTr("&Media information"); checkable: true; checked: propertiesWindow.visible; onTriggered: propertiesWindow.visible = !propertiesWindow.visible }
            Action { text: qsTr("T&asks");             checkable: true; checked: tasksViewDock.visible;    onTriggered: tasksViewDock.visible = !tasksViewDock.visible; }
        }
        Menu {
            id: toolsMenu
            title: qsTr("&Tools")
            enabled: projectOpened
            Action { text: qsTr("S&eries detector...");       onTriggered: { toolsMenu.dismiss(); seriesDetector(); } }
            Action { text: qsTr("Ph&oto data completion..."); onTriggered: { toolsStackView.currentIndex = 0; mainView.currentIndex = 1; } }
            Action { text: qsTr("Look for &duplicates");      onTriggered: { toolsStackView.currentIndex = 1; mainView.currentIndex = 1; } }
        }
        Menu {
            id: settingsMenu
            title: qsTr("&Settings")
            Action { text: qsTr("&Configuration"); icon.name: "applications-system"; onTriggered: { settingsMenu.dismiss(); configuration(); } }
        }
    }

    SwipeView {
        id: mainView
        anchors.fill: parent

        interactive: false

        // main view
        SplitView {
            Column {
                SplitView.preferredWidth: parent.width * 3/4

                PhotosView {
                    id: photosView

                    enabled: mainWindow.projectOpened

                    width: parent.width
                    height: parent.height - notifications.height - tasksViewDock.height

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

                        selection: photosView.selectedPhotos
                    }
                }

                GroupBox {
                    id: propertiesWindow

                    width: parent.width
                    title: qsTr("<b>Media information</b>")
                    clip: true

                    Components.MediaPropertiesView {
                        photos: photosView.selectedPhotos
                    }
                }

                GroupBox {
                    id: debugWindow

                    width: parent.width
                    visible: ObservablesRegistry.enabled
                    title: qsTr("<b>Debug window</b>")
                    clip: true

                    Internals.DebugWindow { }
                }
            }
        }

        ColumnLayout {

            Button {
                id: backButton

                text: qsTr("Back to photos")

                onClicked: mainView.currentIndex = 0
            }

            StackLayout {
                id: toolsStackView

                // photo data completion view
                PhotoDataCompletion {
                    id: completer
                }

                // duplicates view
                Item {

                    anchors.fill: parent

                    Row {

                        id: duplicatesControls

                        Button {
                            text: qsTr("(re)load duplicates")
                            enabled: duplicatesModel.workInProgress == false

                            onPressed: duplicatesModel.reloadDuplicates()
                        }

                        BusyIndicator {
                            running: duplicatesModel.workInProgress
                        }
                    }

                    Internals.DuplicatesView {

                        anchors.top:    duplicatesControls.bottom
                        anchors.bottom: parent.bottom
                        width: parent.width

                        DuplicatesModel {
                            id: duplicatesModel
                            database: PhotoBroomProject.database
                        }

                        model: duplicatesModel
                    }
                }
            }
        }
    }
}
