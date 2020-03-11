
import QtQuick 2.14


Rectangle
{
    id: rootId

    PhotosModel {
        id: itemModel
    }

    GridView {
        anchors.fill: parent
        model: itemModel
    }
}
