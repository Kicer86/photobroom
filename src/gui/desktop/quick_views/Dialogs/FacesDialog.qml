
import QtQuick 2.15
import photo_broom.qml 1.0

Item {
    Flickable {
        id: flickable
        anchors.fill: parent

        boundsBehavior: Flickable.StopAtBounds
        contentWidth: wrapper.width
        contentHeight: wrapper.height

        Item {
            id: wrapper

            width: Math.max(flickable.width, photo.width)
            height: Math.max(flickable.height, photo.height)

            Picture {
                id: photo
                objectName: "photo"

                anchors.centerIn: parent

                MouseArea {
                    anchors.fill: parent

                    onWheel: {
                        var pictureScale = photo.pictureScale

                        if (wheel.angleDelta.y > 0) {
                            pictureScale *= 1.4;
                        }
                        else if (wheel.angleDelta.y < 0) {
                            pictureScale /= 1.4;
                        }

                        photo.pictureScale = pictureScale;
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
