
import QtQuick 2.15
import "../Components" as Components

Item {
    Components.ZoomableImage {
        id: flickablePhoto
        objectName: "flickablePhoto"

        anchors.fill: parent

        boundsBehavior: Flickable.StopAtBounds
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
