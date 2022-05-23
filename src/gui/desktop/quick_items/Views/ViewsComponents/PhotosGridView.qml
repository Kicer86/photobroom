
import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../Components" as Components

/*
 * Photos grid with multiselection component
 */

Components.MultiselectGridView {

    id: grid

    property int thumbnailSize: Components.Constants.defaultThumbnailSize
    property int thumbnailMargin: Components.Constants.defaultThumbnailMargin

    signal itemDoubleClicked(int index)

    cellWidth: thumbnailSize + thumbnailMargin * 2
    cellHeight: thumbnailSize + thumbnailMargin * 2

    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    delegate: PhotoDelegate {
        id: delegateId

        required property var index
        property bool selected

        width:  cellWidth
        height: cellHeight
        margin: thumbnailMargin

        Rectangle {
            id: highlightId
            anchors.fill: parent
            anchors.margins: 0

            opacity: delegateId.selected? 1.0: delegateId.GridView.view.currentIndex == index? 0.5 : 0
            color: currentPalette.highlight
            radius: 5
            z: -1

            Behavior on opacity { PropertyAnimation{} }
        }

        MouseArea {
            anchors.fill: parent

            onDoubleClicked: {
                grid.itemDoubleClicked(index)
            }
        }
    }

    currentIndex: -1

    ScrollBar.vertical: ScrollBar { }

    Components.Focuser { }
}
