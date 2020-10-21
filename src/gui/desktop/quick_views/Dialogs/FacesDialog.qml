
import QtQuick 2.15
import photo_broom.qml 1.0

Item {
    Flickable {
        id: flickable
        anchors.fill: parent

        Picture {
            id: photo
            objectName: "photo"

            anchors.fill: parent
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
