
import QtQuick 2.15
import photo_broom.qml 1.0

Item {

    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl

        onModeChanged: function(path) {

        }
    }
}
