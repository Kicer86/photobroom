
import QtQuick 2.15
import QtQuick.Controls 2.15

/*
 * Photos grid with multiselection component
 */

MultiselectGridView {

    property int thumbnailSize: 160
    property int thumbnailMargin: 5

    cellWidth: thumbnailSize + thumbnailMargin * 2
    cellHeight: thumbnailSize + thumbnailMargin * 2

    delegate: PhotoDelegate {
        id: delegateId

        property bool selected

        width:  cellWidth
        height: cellHeight
        margin: thumbnailMargin

        MouseArea {
            anchors.fill: parent
            onClicked: delegateId.GridView.view.currentIndex = index
        }

        Rectangle {
            id: highlightId
            anchors.fill: parent
            anchors.margins: 0
            state: "unselected"

            color: "blue"
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

    //keyNavigationEnabled: true
    currentIndex: -1

    ScrollBar.vertical: ScrollBar { }
}
