
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

    delegate: PhotoDelegate {
        id: delegateId

        required property var index
        property bool selected

        width:  cellWidth
        height: cellHeight
        margin: thumbnailMargin

        HighlightDelegate {
            id: highlightId
            anchors.fill: parent
            anchors.margins: 0

            opacity: delegateId.selected? 1.0: delegateId.GridView.view.currentIndex == index? 0.5 : 0
            z: -1

            Behavior on opacity { PropertyAnimation{} }
        }

        MouseArea {
            anchors.fill: parent

            onDoubleClicked: {
                grid.itemDoubleClicked(index)
            }
        }

        // load selection information from selection manager
        Component.onCompleted: selected = grid.isIndexSelected(index);
    }

    currentIndex: -1

    ScrollBar.vertical: ScrollBar { }

    // TODO: without it parent (PhotosView) doesn't get focus and Esc key does not work.
    // Figure out how to drop this.
    Components.Focuser { }
}
