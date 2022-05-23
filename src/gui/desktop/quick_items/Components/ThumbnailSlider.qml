
import QtQuick 2.15
import QtQuick.Controls 2.15

/*
 * Reusable thumbnail size slider
 */

Item {
    id: rootId

    width: dummyContainer.width
    height: dummyContainer.height

    anchors.margins: 10
    property alias minimumSize: sliderId.from
    property alias maximumSize: sliderId.to

    property alias size: sliderId.value
    property real transparency: 0.3

    opacity: transparency
    state: "fade_out"

    Item {
        id: dummyContainer
        width: childrenRect.width
        height: childrenRect.height

        Text {
            id: labelId
            text: qsTr("Thumbnail size:")
            anchors.verticalCenter: sliderId.verticalCenter
            style: Text.Outline
            styleColor: "#ffffff"
            font.pixelSize: 13
        }

        Slider {
            id: sliderId
            width: 120

            anchors.left: labelId.right

            stepSize: 10
            from: 40
            to: 400
            value: Constants.defaultThumbnailSize
        }

        Text {
            id: element
            text: sliderId.value
            anchors.verticalCenter: sliderId.verticalCenter
            anchors.left: sliderId.right

            style: Text.Outline
            styleColor: "#ffffff"
            font.pixelSize: 13
        }
    }

    MouseArea {
       id: mouseAreaId

       anchors.fill: parent
       anchors.margins: -10
       hoverEnabled: true
       acceptedButtons: Qt.NoButton

       onExited: rootId.state = "fade_out"
       onEntered: rootId.state = "fade_in"
    }

    states: [
        State {
            name: "fade_out"
        },
        State {
           name: "fade_in"
        }
    ]

    transitions: [
        Transition {
            from: "fade_out"
            to: "fade_in"
            PropertyAnimation {
                target: rootId
                properties: "opacity"
                from: transparency
                to: 1.0
            }
        },
        Transition {
            from: "fade_in"
            to: "fade_out"
            PropertyAnimation {
                target: rootId
                properties: "opacity"
                from: 1.0
                to: transparency
            }
        }
    ]
}

/*##^##
Designer {
    D{i:0;height:22;width:207}
}
##^##*/
