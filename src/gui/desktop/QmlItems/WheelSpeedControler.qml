
import QtQuick

// an item providing nice speed for wheel scrolling
// based on https://forum.qt.io/topic/157817/flick-listview-with-mouse-wheel

WheelHandler
{
    id: wheeleHandler

    property int speed: 10
    required property Flickable flickable

    onWheel: (event) =>
    {
        let scroll_flick = event.angleDelta.y * speed;

        if (flickable.verticalOvershoot != 0.0 || (scroll_flick > 0 && flickable.verticalVelocity <= 0) || (scroll_flick < 0 && flickable.verticalVelocity >= 0))
            flickable.flick(0, scroll_flick - flickable.verticalVelocity);
        else
            flickable.cancelFlick();
    }
}
