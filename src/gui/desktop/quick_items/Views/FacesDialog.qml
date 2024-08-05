
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

// private:
    SystemPalette {
        id: systemPalette
        colorGroup: SystemPalette.Active
    }

    Timer {
        id: faceSelectionUpdater
        running: false
        repeat: false
        onTriggered: updateFaceSelection()
    }

    property var hoveredItems: new Set()
    property var editedItems: new Set()
    property int _facesModelState: 0            // an indirect property for items depending facesModel.state. Direct usage won't work for some reason

    function hoveredItem(index) {
        hoveredItems.add(index);
        postFaceSelectionUpdate();
    }

    function unhoveredItem(index) {
        hoveredItems.delete(index);
        postFaceSelectionUpdate();
    }

    function editedItem(index) {
        editedItems.add(index);
        postFaceSelectionUpdate();
    }

    function finishedItem(index) {
        editedItems.delete(index);
        postFaceSelectionUpdate();
    }

    function postFaceSelectionUpdate() {
        faceSelectionUpdater.interval = 200;
        faceSelectionUpdater.running = true;
    }

    function updateFaceSelection() {
        var toSelect = -1;

        if (editedItems.size > 1)
            postFaceSelectionUpdate();
        else if (editedItems.size === 1)
            toSelect = editedItems.values().next().value;
        else if (hoveredItems.size > 1)
            postFaceSelectionUpdate();
        else if (hoveredItems.size === 1)
            toSelect = hoveredItems.values().next().value;

        if (toSelect === -1)
            shadow.clear();
        else {
            var index = facesModel.index(toSelect, 0);
            const face = facesModel.data(index, FacesModel.FaceRectRole);
            shadow.setFocus(face);
        }
    }

    MediaViewCtrl {
        id: ctrl
        core: PhotoBroomProject.coreFactory
    }

    FacesModel {
        id: facesModel

        photoID: ctrl.photoID
        database: PhotoBroomProject.database
        core: PhotoBroomProject.coreFactory

        onStateChanged: (state) => {
            main._facesModelState = state;
        }
    }

    ShadowBackground {
        anchors.fill: parent
    }

    Components.ZoomableImage {
        anchors.left: parent.left
        anchors.right: peopleList.left
        anchors.top: parent.top
        anchors.bottom: toolsArea.top

        clip: true
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

    TableView {
        id: peopleList

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: toolsArea.top
        implicitWidth: contentWidth

        model: facesModel

        Behavior on implicitWidth { PropertyAnimation{} }

        delegate: TextField {
            id: name

            required property var index
            required property var display       // DisplayRole
            required property bool uncertain    // UncertainRole

            readOnly: true
            hoverEnabled: true
            text: display
            placeholderText: qsTr("unknown")

            palette {
                base: uncertain? "yellow" : systemPalette.base
            }

            onPressed: {
                name.readOnly = false;
                editedItem(index);
            }

            onHoveredChanged: {
                if (hovered)
                    hoveredItem(index);
                else
                    unhoveredItem(index);
            }

            onEditingFinished: {
                var model_index = facesModel.index(index, 0);
                facesModel.setData(model_index, text);
                name.readOnly = true
                finishedItem(index);
            }
        }
    }

    Rectangle {
        id: toolsArea

        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        color: systemPalette.window

        Row {
            height: childrenRect.height
            visible: _facesModelState == 1

            Switch {
                id: facesSwitch

                text: qsTr("Mark found faces")
            }
        }

        Row {
            height: childrenRect.height
            visible: _facesModelState !== 1

            Text {
                id: stateLabel

                anchors.verticalCenter: busyIndicator.verticalCenter

                text: _facesModelState === 0? qsTr("Detecting and analyzing faces") : qsTr("Could not detect any face.")
            }

            BusyIndicator {
                id: busyIndicator
                running: _facesModelState === 0
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
