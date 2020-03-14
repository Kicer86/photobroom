
import QtQuick 2.14

Rectangle
{
    id: rootId

    ListModel {
        id: emptyModelId
    }

    FlowView {
        id: photosViewId
        objectName: "photos_view"

        anchors.fill: parent
        model: emptyModelId

        delegate: delegateId
        /*
        highlight: Rectangle {
            width: parent.width
            height: parent.height
            color: "red"
            border.width: 3
            z: 1
        }
        */
    }

    Component {
        id: delegateId

        Rectangle {
            id: rectId

            width:  if (imageId.width + 5 < 60) 60; else imageId.width + 5
            height: 125
            border.width: 1

            Image {
                id: imageId
                anchors.centerIn: parent
                sourceSize.height: 120
                height: 120

                source: photoPath
            }

            /*
            MouseArea {
                anchors.fill: parent
                onClicked: rectId.GridView.view.currentIndex = index
            }
            */
        }
    }
}
