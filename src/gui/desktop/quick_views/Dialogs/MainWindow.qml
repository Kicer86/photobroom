
import QtQuick 2.15
import "DialogsComponents" as Internals

Column {
    id: column
    objectName: "MainWindow"

    property bool projectOpened: false

    anchors.fill: parent

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
