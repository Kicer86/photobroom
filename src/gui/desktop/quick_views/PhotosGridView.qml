
import QtQuick 2.14
import QtQuick.Controls 2.3

GridView {

    property int thumbnailSize: 160
    property int thumbnailMargin: 2

    cellWidth: thumbnailSize + thumbnailMargin
    cellHeight: thumbnailSize + thumbnailMargin
    delegate: PhotoDelegate { }
    highlight: highlightId
    keyNavigationEnabled: true

    ScrollBar.vertical: ScrollBar { }

    Component {
        id: highlightId

        Rectangle {
            width: view.cellWidth;
            height: view.cellHeight
            color: "lightsteelblue";
            opacity: 0.7
            x: view.currentItem.x
            y: view.currentItem.y
            z: 1
            Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
            Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }
        }
    }

}
