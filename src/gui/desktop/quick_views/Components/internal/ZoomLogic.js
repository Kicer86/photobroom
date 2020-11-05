
/**
 * @arg scaleDelta
 * @arg mouse point on view where zoom occurs
 * @arg view which part of image is shown.
 * @arg canvas image size
 */

function scaleOffsets(scaleDelta, mouse, view, canvas)
{
    var result = Qt.point(0, 0);
    var offset = 0.0;

    if (canvas.width >= view.width)
    {
        var oldMouseXAbs = mouse.x;
        var newMouseXAbs = oldMouseXAbs * scaleDelta;
        var centerX = oldMouseXAbs - view.x
        offset = newMouseXAbs - centerX;
        offset = Math.max(offset, 0);                               // eliminate negative numbers
        offset = Math.min(offset, canvas.width - view.width) // eliminate values above edge
        result.x = offset;
    }

    if (canvas.height >= view.height)
    {
        var oldMouseYAbs = mouse.y;
        var newMouseYAbs = oldMouseYAbs * scaleDelta;
        var centerY = oldMouseYAbs - view.y
        offset = newMouseYAbs - centerY;
        offset = Math.max(offset, 0);                                 // eliminate negative numbers
        offset = Math.min(offset, canvas.height - view.height) // eliminate values above edge
        result.y = offset;
    }

    return result;
}
