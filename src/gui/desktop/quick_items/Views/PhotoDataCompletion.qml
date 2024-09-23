
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15
import photo_broom.models
import photo_broom.singletons
import quick_items

import "../Components" as Components
import "ViewsComponents" as Internals


Internals.ToolsBase {
    id: root

    property var selectedPhoto: []      // selected photo

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    Components.DelegateState {
        id: delegateState

        defaultValue: true
        model: dataSource
    }

    idlePrompt:    qsTr("Click here to scan for additional information about photos from file names and paths.")
    loadingPrompt: qsTr("Processing photos...")
    loadedPrompt:  qsTr("Processing done. Click here for rerun.")
    emptyPrompt:   qsTr("No results.")
    storingPrompt: qsTr("Saving results...")

    // Model working directly on database
    model: PhotosDataGuesser {
        id: dataSource
        database: PhotoBroomProject.database
    }

    view: Component { Components.ExListView {
        id: listView
        objectName: "Data Completion List"

        spacing: 5
        highlightMoveDuration: 100
        highlightMoveVelocity: -1

        model: dataSource

        delegate: Item {
            id: delegateRoot

            required property var photoID
            required property var photoPath
            required property var suggestedDate
            required property var suggestedTime
            required property int index

            width: listView.width - listView.ScrollBar.vertical.width
            height: 60

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    listView.forceActiveFocus();
                    listView.currentIndex = index
                }
            }

            Row {
                Components.DelegateCheckBox {
                    state: delegateState
                    index: delegateRoot.index
                }

                Components.MediaPreviewItem {

                    width: 60
                    height: 60

                    photoID: delegateRoot.photoID
                }

                Column {
                    Text {
                        text: delegateRoot.photoPath
                    }

                    Text {
                        text: qsTr("Guessed date:") + "\n" + suggestedDate + " " + suggestedTime
                    }
                }
            }
        }

        highlight: Rectangle {
            color: currentPalette.highlight
            radius: 5
        }

        onCurrentIndexChanged: {
            root.selectedPhoto = dataSource.getId(currentIndex);
        }
    } }

    apply: Component { Button {
        id: applyButton

        anchors.bottom: parent.bottom
        anchors.left: parent.left

        text: qsTr("Save selected photos")

        onClicked: {
            var toBeExcluded = delegateState.getItems((state) => {return state;});

            dataSource.apply(toBeExcluded);
        }
    } }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
