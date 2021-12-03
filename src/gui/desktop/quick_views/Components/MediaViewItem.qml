
import QtQuick 2.15
import photo_broom.qml 1.0

Item {

    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl

        onModeChanged: function(mode) {

        }
    }

    Image {
        id: staticImage

        anchors.fill: parent

        enabled:  ctrl.mode === MediaViewCtrl.StaticImage
        visible: enabled

        source: enabled? ctrl.path: ""
        asynchronous: true
        autoTransform: true
        fillMode: Image.PreserveAspectFit
    }
}
