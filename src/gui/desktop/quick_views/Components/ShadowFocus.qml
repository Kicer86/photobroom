
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: shadow

    // public properties and functions
    property bool hasFocus: false
    property bool fadeInOutEnabled: true

    function clear() {
        if (fadeInOutEnabled == false)
            animationEnabled = false;           // disable on fade out

        focusRect = Qt.rect(0, 0, parent.width, parent.height)
        hasFocus = false;
    }

    function setFocus(focus)
    {
        if (hasFocus == false && fadeInOutEnabled == false)
            animationEnabled = false;           // disable animations when focusing on something from 'clear' state (fade in)

        focusRect = focus;
        hasFocus = true;
        animationEnabled = true;
    }

    // private properties
    property rect focusRect: Qt.rect(0, 0, parent.width, parent.height)
    property bool animationEnabled: false

    property real leftEdge: focusRect.left
    property real rightEdge: focusRect.right
    property real topEdge: focusRect.top
    property real bottomEdge: focusRect.bottom

    Behavior on leftEdge   { enabled: animationEnabled; PropertyAnimation {}}
    Behavior on rightEdge  { enabled: animationEnabled; PropertyAnimation {} }
    Behavior on topEdge    { enabled: animationEnabled; PropertyAnimation {} }
    Behavior on bottomEdge { enabled: animationEnabled; PropertyAnimation {} }

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
