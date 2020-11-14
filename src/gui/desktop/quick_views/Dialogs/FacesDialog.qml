
import QtQuick 2.15
import QtQuick.Controls 2.15
import "../Components" as Components

Item {
    id: main

    state: "Detecting Faces"

    function setDetectionState(status) {
        if (status === 0) {
            // nothing to do
        } else if (status === 1) {
            main.state = "Faces Detected"
        }
    }

    Components.ZoomableImage {
        id: imageArea
        objectName: "flickablePhoto"

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: notificationArea.top

        boundsBehavior: Flickable.StopAtBounds
    }

    Rectangle {
        id: notificationArea

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        height: row.height

        radius: 5

        Row {
            id: row

            height: busyIndicator.height
            leftPadding: 5

            Text {
                text: qsTr("Detecting and analyzing faces")
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
            }

            BusyIndicator {
                id: busyIndicator
            }
        }
    }
    states: [
        State {
            name: "Detecting Faces"
        },
        State {
            name: "Faces Detected"

            PropertyChanges {
                target: notificationArea
                height: 0
            }

            PropertyChanges {
                target: busyIndicator
                running: false
            }
        }
    ]

    transitions: [
        Transition {
            id: facesDetected

            to: "Faces Detected"
            from: "Detecting Faces"

            PropertyAnimation { target: notificationArea; properties: "height"; }
        }
    ]
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
