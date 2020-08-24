
import QtQuick 2.15
import QtQuick.Controls 2.15


Item {
    id: rootId

    width: childrenRect.width
    height: childrenRect.height

    property alias size: sliderId.value

    Text {
        id: labelId
        text: qsTr("Thumbnail size:")
        font.pixelSize: 12
    }

    Slider {
        id: sliderId
        width: 120

        anchors.left: labelId.right

        stepSize: 10
        from: 30
        to: 300
        value: 160
    }
}
