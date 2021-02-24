
import QtQuick 2.15
import QtQuick.Controls 2.15
import "DialogsComponents" as Internals


SwipeView {
    id: mainWindow
    objectName: "MainWindow"

    property bool projectOpened: false

    anchors.fill: parent

    Column {
        PhotosView {
            enabled: projectOpened

            width: parent.width
            height: parent.height - notifications.height
        }

        Internals.NotificationsBar {
            id: notifications

            width: parent.width
        }
    }

    Item {

        Button {
            id: backButton

            text: qsTr("Back to photos")

            onClicked: {
                mainWindow.currentIndex = 0
            }
        }

        PhotoDataCompletion {
            width: parent.width
            anchors.bottom: parent.bottom
            anchors.top: backButton.bottom
        }
    }
}
