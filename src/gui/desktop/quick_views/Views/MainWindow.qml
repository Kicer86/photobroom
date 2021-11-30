
import QtQuick 2.15
import QtQuick.Controls 2.15
import "ViewsComponents" as Internals


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
            enabled: projectOpened

            width: parent.width
            height: parent.height - notifications.height

            onSelectedPhotosChanged: {
                mainWindow.selectedPhotos = selectedPhotos;
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
