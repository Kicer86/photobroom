
/**
 * @arg scaleDelta
 * @arg mouse point on view where zoom occurs
 * @arg view which part of image is shown.
 * @arg canvas image size
 */

function scaleOffsets(scaleDelta, mouse, view, canvas)
{
    var result = Qt.point(0, 0);

    var oldMouseXAbs = mouse.x + view.x;
    var newMouseXAbs = oldMouseXAbs * scaleDelta;
    var centerX = mouse.x;
    var xOffset = newMouseXAbs - centerX;
    result.x = xOffset;

    var oldMouseYAbs = mouse.y + view.y;
    var newMouseYAbs = oldMouseYAbs * scaleDelta;
    var centerY = mouse.y;
    var yOffset = newMouseYAbs - centerY;
    result.y = yOffset;

    return result;
}
