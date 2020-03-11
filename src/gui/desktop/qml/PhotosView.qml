
import QtQuick 2.14


Rectangle
{
    id: rootId

    ListModel {
        id: emptyModelId
    }

    GridView {
        id: photosViewId
        objectName: "photos_view"

        anchors.fill: parent
        model: emptyModelId

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
