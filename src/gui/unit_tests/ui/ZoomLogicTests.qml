
import QtQuick 2.15
import QtTest 1.0
import "../../desktop/quick_views/Components/internal/ZoomLogic.js" as ZoomLogic

TestCase {
    name: "ZoomLogic tests"

    function test_top_left_corner_zoom_in() {
        var newOffsets = ZoomLogic.scaleOffsets(2.0, Qt.point(0, 0), Qt.rect(0, 0, 100, 100), Qt.size(100, 100));

        compare(newOffsets.x, 0, "x offset");
        compare(newOffsets.y, 0, "y offset");
    }
}
