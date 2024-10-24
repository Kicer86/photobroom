
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import photo_broom.models
import photo_broom.singletons

import "../Components" as Components
import "ViewsComponents" as Internals


/*
 * Series detection dialog
 */

Internals.ToolsBase
{
    id: seriesDetectionMainId

    Components.DelegateState {
        id: delegateState

        defaultValue: true
        model: groupsModelId
    }

    idlePrompt:    qsTr("Click here to scan for photo series.")
    loadingPrompt: qsTr("Looking for group candidates...")
    loadedPrompt:  qsTr("Click here to rescan for photo series.")
    emptyPrompt:   qsTr("There are no group candidates.")
    storingPrompt: qsTr("Saving groups...")

    model: SeriesModel {
        id: groupsModelId

        coreFactory: PhotoBroomProject.coreFactory
        project: PhotoBroomProject.project
    }

    view: Component { Components.ExListView {
        id: groupsListId

        property alias thumbnailSize: thumbnailSliderId.size

        model: groupsModelId
        spacing: 10
        focusPolicy: Qt.StrongFocus

        delegate: Item {
            id: delegateId

            width: delegateId.ListView.view.width       // using 'parent' causes erros in output after thumbnail being resized
            height: groupDetails.height

            // from view
            required property int index

            // from model - roles
            required property var photoData
            required property var groupType
            required property var members

            Row {
                anchors.fill: parent

                Components.DelegateCheckBox {
                    id: checkBox

                    state: delegateState
                    index: delegateId.index
                }

                Item {
                    id: groupDetails

                    width: parent.width - checkBox.width
                    height: groupTypeId.height + membersList.height

                    Text {
                        id: groupTypeId
                        text: groupType

                        anchors.bottom: membersList.top
                    }

                    ListView {
                        id: membersList

                        clip: true

                        width: parent.width
                        height: groupsListId.thumbnailSize
                        anchors.bottom: parent.bottom

                        orientation: ListView.Horizontal
                        model: members

                        delegate: Internals.PhotoDelegate
                        {
                            // from view
                            required property int index

                            photoData: members[index]

                            width: membersList.height
                            height: membersList.height
                        }
                    }
                }
            }
        }

        Components.ThumbnailSlider {
            id: thumbnailSliderId
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            minimumSize: 100
        }
    }}

    apply: Component { Button {
        text: qsTr("Group", "used as verb - group photos")

        onClicked: {
            var selected = delegateState.getItems((state) => {return state;});

            groupsModelId.apply(selected);
            delegateState.clear();
        }
    }}
}
