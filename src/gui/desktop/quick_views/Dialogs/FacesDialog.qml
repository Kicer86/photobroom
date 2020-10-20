
import QtQuick 2.15

Item {
    Flickable {
        id: flickable
        anchors.fill: parent

        Image {
            id: photo
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2}D{i:1}
}
##^##*/
