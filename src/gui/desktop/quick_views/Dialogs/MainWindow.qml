
import QtQuick 2.15
import "DialogsComponents" as Internals

Column {
    id: column

    anchors.fill: parent

    PhotosView {
        width: parent.width
        height: parent.height - notifications.height
    }

    Internals.NotificationsBar {
        id: notifications

        width: parent.width
    }

}
