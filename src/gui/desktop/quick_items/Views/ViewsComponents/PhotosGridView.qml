
import QtQuick 2.15
import QtQuick.Controls 2.15

import QmlItems
import "../../Components" as Components


/*
 * Photos grid with multiselection component
 */

Components.MultiselectGridView {

    id: grid

    property int thumbnailSize: Components.Constants.defaultThumbnailSize
    property int thumbnailMargin: Components.Constants.defaultThumbnailMargin

    signal itemDoubleClicked(int index)

    boundsMovement: Flickable.StopAtBounds
    flickDeceleration: 10000
    cellWidth: thumbnailSize + thumbnailMargin * 2
    cellHeight: thumbnailSize + thumbnailMargin * 2
    currentIndex: -1

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

    ScrollBar.vertical: ScrollBar { }

    WheelSpeedControler {
        flickable: grid
    }

    FocusOnClick { target: grid }

    Keys.onPressed: function (event) {
        let delegateWidth = gridView.contentItem.children[0]?.width || 1;
        let delegateHeight = gridView.contentItem.children[0]?.height || 1;
        let visibleColumns = Math.floor(gridView.width / delegateWidth);
        let visibleRows = Math.floor(gridView.height / delegateHeight);
        let itemsPerPage = visibleRows * visibleColumns;

        if (grid.keyNavigationEnabled == false)
            return

        event.accepted = true;
        switch (event.key) {
            case Qt.Key_PageDown:
                grid.currentIndex = gridView.currentIndex + itemsPerPage;
                break;
            case Qt.Key_PageUp:
                grid.currentIndex = gridView.currentIndex - itemsPerPage;
                break;
            case Qt.Key_Home:
                grid.currentIndex = 0;
                break;
            case Qt.Key_End:
                grid.currentIndex = grid.count - 1;
                break;
            default:
                event.accepted = false;
        }
    }
}
