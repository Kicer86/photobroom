
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

            anchors.fill: parent
            anchors.margins: 1

            color: "lightsteelblue"
            radius: 5

            opacity: 0.6
            visible: delegateId.selected
        }
    }

    keyNavigationEnabled: true
    currentIndex: -1

    ScrollBar.vertical: ScrollBar { }
}
