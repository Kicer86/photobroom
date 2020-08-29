
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
        opacity: selected? 0.5: 1

        MouseArea {
            anchors.fill: parent
            onClicked: delegateId.GridView.view.currentIndex = index
        }
    }

    highlight: highlightId
    keyNavigationEnabled: true
    currentIndex: -1

    ScrollBar.vertical: ScrollBar { }

    Component {
        id: highlightId

        Rectangle {
            color: "lightsteelblue"
            opacity: 0.6
            z: 2
        }
    }

}
