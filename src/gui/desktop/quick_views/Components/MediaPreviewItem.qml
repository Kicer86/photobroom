
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

    Component {
        id: errorImage

         Waiter {
            busyIndicator: busyId

            Image {
                anchors.fill: parent

                source: "qrc:/gui/error.svg"
                sourceSize.width: width
                sourceSize.height: height
            }
         }
    }


    Loader { sourceComponent: ctrl.mode !== MediaViewCtrl.Unknown &&
                              ctrl.mode !== MediaViewCtrl.Error? staticImage: undefined; anchors.fill: parent }
    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.Error? staticImage: undefined; anchors.fill: parent }

    // TODO: use AnimatedImage for all kinds of non-static media files when https://bugreports.qt.io/browse/QTBUG-30524 is fixed
}

