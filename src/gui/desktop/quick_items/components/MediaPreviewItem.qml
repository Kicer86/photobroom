
import QtQuick
import QtMultimedia
import QtQuick.Controls
import quick_items
import QmlItems

import photo_broom.singletons


Item {
    property alias photoID: ctrl.photoID        // set to make this Item work
    property alias mediaType: ctrl.mode         // get media type

    MediaViewCtrl {
        id: ctrl
        core: PhotoBroomProject.coreFactory
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
    // TODO: check https://doc-snapshots.qt.io/qt6-dev/qml-qtquick-frameanimation.html if it solves above problem
}

