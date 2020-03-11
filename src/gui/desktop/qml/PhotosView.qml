
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

        delegate: Rectangle {
            width: 100; height: 30
            border.width: 1
            color: "lightsteelblue"
            Text {
                anchors.centerIn: parent
                text: name
            }
        }
    }
}
