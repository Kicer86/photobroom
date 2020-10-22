
import QtQuick 2.15
import photo_broom.qml 1.0

Item {
    Flickable {
        id: flickable
        anchors.fill: parent

        contentWidth: photo.width
        contentHeight: photo.height

        Picture {
            id: photo
            objectName: "photo"
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
