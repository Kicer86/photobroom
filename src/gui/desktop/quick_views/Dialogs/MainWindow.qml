
import QtQuick 2.15
import QtQuick.Controls 2.15
import "DialogsComponents" as Internals


SwipeView {
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

    PhotoDataCompletion {

    }
}
