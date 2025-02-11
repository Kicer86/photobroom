
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import photo_broom.singletons
import quick_items
import quick_items.components as Components
import quick_items.views

import "ViewsComponents" as Internals


ApplicationWindow {
    id: mainWindow
    objectName: "MainWindow"

    property string projectName: ""                 // TODO: read title from PhotoBroomProject

    // TODO: these signals should be removed.
    //       cpp singletons could manage it.
    signal openProject(string path)
    signal openProjectDialog()
    signal newProject()
    signal closeProject()
    signal scanCollection()
    signal configuration()

    title: PhotoBroomProject.projectOpen? "Photo broom: " + projectName : qsTr("No collection opened")

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

                    onObjectAdded: (index, object) => recentsMenu.insertItem(index, object);
                }
            }
            Action { text: qsTr("&Close"); enabled: PhotoBroomProject.projectOpen; icon.name: "window-close"; onTriggered: closeProject(); }
            MenuSeparator { }
            Action { text: qsTr("&Quit"); icon.name: "application-exit"; onTriggered: Qt.quit(); }
        }
        Menu {
            id: photosMenu
            title: qsTr("P&hotos")
            enabled: PhotoBroomProject.projectOpen
            Action { text: qsTr("&Refresh collection..."); onTriggered: { photosMenu.dismiss(); scanCollection(); } }
        }
        Menu {
            title: qsTr("&Windows")
            Action { text: qsTr("T&asks");             checkable: true; checked: tasksViewDock.visible;    onTriggered: tasksViewDock.visible = !tasksViewDock.visible; }
        }
        Menu {
            id: toolsMenu
            title: qsTr("&Tools")
            enabled: PhotoBroomProject.projectOpen
            Action { text: qsTr("S&eries detector...");       onTriggered: { toolsStackView.currentIndex = 1; mainView.currentIndex = 1; } }
            Action { text: qsTr("Ph&oto data completion..."); onTriggered: { toolsStackView.currentIndex = 2; mainView.currentIndex = 1; } }
            Action { text: qsTr("Look for &duplicates");      onTriggered: { toolsStackView.currentIndex = 3; mainView.currentIndex = 1; } }
            //Action { text: qsTr("&Face detection...");        onTriggered: { toolsStackView.currentIndex = 4; mainView.currentIndex = 1; } }
        }
        Menu {
            id: settingsMenu
            title: qsTr("&Settings")
            Action { text: qsTr("&Configuration"); icon.name: "applications-system"; onTriggered: { settingsMenu.dismiss(); configuration(); } }
        }
    }

    // actions on database close/open
    Connections {
        target: PhotoBroomProject

        function onProjectOpenChanged(open) {
            mainView.currentIndex = 0;
        }
    }

    SwipeView {
        id: mainView
        anchors.fill: parent

        interactive: false

        // main view
        Column {
            SplitView.preferredWidth: parent.width * 3/4

            PhotosView {
                id: photosView

                enabled: PhotoBroomProject.projectOpen

                width: parent.width
                height: parent.height - notifications.height - tasksViewDock.height
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

        ColumnLayout {

            Button {
                id: backButton

                text: qsTr("Back to photos")

                onClicked: mainView.currentIndex = 0
            }

            StackLayout {
                id: toolsStackView

                // default, empty item used when no tool is meant to be active
                Item {

                }

                Loader {
                    active: PhotoBroomProject.projectOpen && toolsStackView.currentIndex == StackLayout.index

                    Component {
                        id: series_detection
                        SeriesDetection { }
                    }

                    sourceComponent: PhotoBroomProject.projectOpen? series_detection : undefined
                }

                Loader {
                    active: PhotoBroomProject.projectOpen && toolsStackView.currentIndex == StackLayout.index

                    Component {
                        id: data_completion
                        PhotoDataCompletion { }
                    }

                    sourceComponent: PhotoBroomProject.projectOpen? data_completion : undefined
                }

                Loader {
                    active: PhotoBroomProject.projectOpen && toolsStackView.currentIndex == StackLayout.index

                    Component {
                        id: duplicates_view
                        DuplicatesView { }
                    }

                    sourceComponent: PhotoBroomProject.projectOpen? duplicates_view : undefined
                }

                Loader {
                    active: PhotoBroomProject.projectOpen && toolsStackView.currentIndex == StackLayout.index

                    Component {
                        id: batch_face_detection
                        BatchFaceDetection { }
                    }

                    sourceComponent: PhotoBroomProject.projectOpen? batch_face_detection : undefined
                }
            }
        }
    }
}
