
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

            width: Math.max(flickable.width, photo.width * photo.scale)
            height: Math.max(flickable.height, photo.height * photo.scale)

            Picture {
                id: photo
                objectName: "photo"

                anchors.centerIn: parent

                width: implicitWidth
                height: implicitHeight

                MouseArea {
                    anchors.fill: parent

                    onWheel: {
                        var pictureScale = photo.scale

                        if (wheel.angleDelta.y > 0 && pictureScale < 8) {
                            pictureScale *= 1.4;
                        }
                        else if (wheel.angleDelta.y < 0 && pictureScale > 1/8) {
                            pictureScale /= 1.4;
                        }

                        photo.scale = pictureScale;
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
