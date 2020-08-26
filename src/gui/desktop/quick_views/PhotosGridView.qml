
import QtQuick 2.14
import QtQuick.Controls 2.3

GridView {

    property int thumbnailSize: 160
    property int thumbnailMargin: 5

    cellWidth: thumbnailSize + thumbnailMargin * 2
    cellHeight: thumbnailSize + thumbnailMargin * 2

    delegate: PhotoDelegate {
        id: delegateId

        width:  cellWidth
        height: cellHeight
        margin: thumbnailMargin

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
