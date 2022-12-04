
import QtQuick 2.15
import QtQuick.Controls 2.15
import QmlItems
import quick_items
import photo_broom.models
import photo_broom.singletons
import photo_broom.utils
import "../Components" as Components

Item {
    id: main

    property alias photoID: ctrl.photoID

    state: "Detecting Faces"

// private:
    Timer {
        id: faceSelectionUpdater
        running: false
        repeat: false
        onTriggered: updateFaceSelection()
    }

    property var selectedFaces: new Set()

    function selectFace(index) {
        selectedFaces.add(index);
        postFaceSelectionUpdate();
    }

    function deselectFace(index) {
        selectedFaces.delete(index);
        postFaceSelectionUpdate();
    }

    function postFaceSelectionUpdate() {
        faceSelectionUpdater.interval = 100;
        faceSelectionUpdater.running = true;
    }

    function updateFaceSelection() {
        if (selectedFaces.size > 1)
            postFaceSelectionUpdate();
        else if (selectedFaces.size === 0)
            shadow.clear();
        else {
            const row = selectedFaces.values().next().value;
            var index = facesModel.index(row, 0);
            const face = facesModel.data(index, FacesModel.FaceRectRole);
            shadow.setFocus(face);
        }
    }

    MediaViewCtrl {
        id: ctrl
    }

    FacesModel {
        id: facesModel

        photoID: ctrl.photoID
        database: PhotoBroomProject.database
        core: PhotoBroomProject.coreFactory

        onStateChanged: (state) => {
            if (state === 0) {
                // nothing to do
            } else if (state === 1) {
                main.state = "Faces Detected";
                notificationTimer.running = true;
            } else if (state === 2) {
                main.state = "No Face Detected";
                notificationTimer.running = true;
            } else if (state === 10) {
                main.state = "Notification Hidden";
            }
        }
    }

    TableView {
        anchors.top: parent.top
        anchors.right: parent.right
        implicitWidth: contentWidth
        height: contentHeight
        z:1

        model: facesModel

        delegate: TextField {
            id: name

            required property var display       // DisplayRole
            required property var index

            readOnly: true
            hoverEnabled: true
            text: display
            placeholderText: qsTr("unknown")

            onPressed: name.readOnly = false

            onHoveredChanged: {
                if (hovered)
                    selectFace(index);
                else
                    deselectFace(index);
            }

            onEditingFinished: {
                var model_index = facesModel.index(index, 0);
                facesModel.setData(model_index, text);
                name.readOnly = true
            }
        }
    }

    Timer {
        id: notificationTimer
        interval: 4000
        //onTriggered: setDetectionState(10);
    }

    Components.ZoomableImage {

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: notificationArea.top

        boundsBehavior: Flickable.StopAtBounds

        source: ctrl.path

        ShadowFocus {
            id: shadow

            opacity: 0.7
            anchors.fill: parent

            fadeInOutEnabled: !facesSwitch.checked  // disable face selection shadow when found faces are marked
        }

        Item {
            anchors.fill: parent

            visible: shadow.hasFocus == false
            opacity: facesSwitch.checked? 1.0: 0.0

            Behavior on opacity { PropertyAnimation {} }  // disable animations when selecting face

            Repeater {
                id: facesMarker

                model: facesModel.facesMask

                delegate: Rectangle {
                    x: modelData.x
                    y: modelData.y
                    width: modelData.width
                    height: modelData.height

                    color: "black"
                    opacity: 0.7
                }
            }
        }
    }

    Rectangle {
        id: notificationArea

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: switchesArea.top
        clip: true

        height: row.height
        color: "dodgerblue"

        radius: 5

        Row {
            id: row

            height: busyIndicator.height
            leftPadding: 5

            Text {
                id: status
                text: qsTr("Detecting and analyzing faces")
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
            }

            BusyIndicator {
                id: busyIndicator
                running: false
            }
        }
    }

    Rectangle {
        id: switchesArea

        height: switches.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Row {
            id: switches

            Switch {
                id: facesSwitch

                text: qsTr("Mark found faces")
            }
        }
    }

    states: [
        State {
            name: "Detecting Faces"

            PropertyChanges {
                target: busyIndicator
                running: true
            }

            PropertyChanges {
                target: switchesArea
                height: 0
            }
        },
        State {
            name: "No Face Detected"

            PropertyChanges {
                target: status
                text: qsTr("Could not detect any face.")
            }
        },
        State {
            name: "Faces Detected"

            PropertyChanges {
                target: status
                text: qsTr("Faces detected a recognised.")
            }
        },
        State {
            name: "Notification Hidden"

            PropertyChanges {
                target: notificationArea
                height: 0
            }
        }
    ]

    transitions: [
        Transition {
            id: facesDetected

            to: "Notification Hidden"

            PropertyAnimation { target: notificationArea; properties: "height"; }
        },
        Transition {
            from: "Detecting Faces"

            PropertyAnimation { target: switchesArea; properties: "height"; }
        }
    ]
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
