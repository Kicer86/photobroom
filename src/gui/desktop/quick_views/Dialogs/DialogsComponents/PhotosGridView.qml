
import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../Components" as Components

/*
 * Photos grid with multiselection component
 */

Components.MultiselectGridView {

    property int thumbnailSize: 160
    property int thumbnailMargin: 5

    cellWidth: thumbnailSize + thumbnailMargin * 2
    cellHeight: thumbnailSize + thumbnailMargin * 2

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    delegate: PhotoDelegate {
        id: delegateId

        property bool selected

        width:  cellWidth
        height: cellHeight
        margin: thumbnailMargin

        Rectangle {
            id: highlightId
            anchors.fill: parent
            anchors.margins: 0
            state: "unselected"

            color: currentPalette.highlight
            radius: 5
            z: -1

            states: [
                State {
                    name: "selected"
                    when: delegateId.selected
                    PropertyChanges { target: highlightId; opacity: 1.0 }
                },
                State {
                    name: "unselected"
                    when: !delegateId.selected
                    PropertyChanges { target: highlightId; opacity: 0.0 }
                }
            ]

            transitions: Transition {
                PropertyAnimation { properties: "opacity"; easing.type: Easing.InOutQuad }
            }
        }
    }

    currentIndex: -1

    ScrollBar.vertical: ScrollBar { }
}
