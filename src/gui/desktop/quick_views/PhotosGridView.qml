
import QtQuick 2.14
import QtQuick.Controls 2.3

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
            anchors.margins: 1
            state: "unselected"

            color: "lightsteelblue"
            radius: 5

            states: [
                State {
                    name: "selected"
                    when: delegateId.selected
                    PropertyChanges { target: highlightId; opacity: 0.6 }
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

    keyNavigationEnabled: true
    currentIndex: -1

    ScrollBar.vertical: ScrollBar { }
}
