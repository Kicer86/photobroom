
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: shadow
    anchors.fill: parent
    opacity: 0.5

    property rect focusRect: Qt.rect(0, 0, parent.width, parent.height)

    property real leftEdge: focusRect.left
    property real rightEdge: focusRect.right
    property real topEdge: focusRect.top
    property real bottomEdge: focusRect.bottom

    Behavior on leftEdge { PropertyAnimation {} }
    Behavior on rightEdge { PropertyAnimation {} }
    Behavior on topEdge { PropertyAnimation {} }
    Behavior on bottomEdge { PropertyAnimation {} }

    Rectangle {
        id: leftRect
        x: 0
        y: 0

        width: shadow.leftEdge
        height: shadow.height

        color: "black"
    }

    Rectangle {
        id: rightRect
        x: shadow.rightEdge
        y: 0

        width: shadow.width - shadow.rightEdge
        height: shadow.height

        color: "black"
    }

    Rectangle {
        id: topRect
        x: shadow.leftEdge
        y: 0

        width: shadow.rightEdge - shadow.leftEdge
        height: shadow.topEdge

        color: "black"
    }

    Rectangle {
        id: bottomRect
        x: shadow.leftEdge
        y: shadow.bottomEdge

        width: shadow.rightEdge - shadow.leftEdge
        height: shadow.height - shadow.bottomEdge

        color: "black"
    }
}
