
import QtQuick 2.15
import QtMultimedia
import QtQuick.Controls
import photo_broom.qml 1.0


Item {
    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl
    }

    BusyIndicator {
        id: busyId

        anchors.centerIn: parent
    }

    Component {
        id: staticImage

        Waiter {
            busyIndicator: busyId

            Image {
                anchors.fill: parent

                source: "image://thumbnail/" + ctrl.photoIDString
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectCrop
                cache: false
            }
        }
    }


    Loader { sourceComponent: ctrl.mode !== MediaViewCtrl.Unknown? staticImage: undefined; anchors.fill: parent }
}

