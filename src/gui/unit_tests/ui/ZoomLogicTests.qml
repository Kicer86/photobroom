
import QtQuick 2.15
import QtTest 1.0
import "../../desktop/quick_items/components/internal/ZoomLogic.js" as ZoomLogic

TestCase {
    name: "ZoomLogic tests"

    function test_top_left_corner_zoom_in() {

        // image takes 100% of screen and is being scaled 2 times at point 0,0
        var newOffsets = ZoomLogic.scaleOffsets(2.0, Qt.point(0, 0), Qt.rect(0, 0, 100, 100), Qt.size(100, 100));

        compare(newOffsets.x, 0, "x offset");
        compare(newOffsets.y, 0, "y offset");

        // image takes 200% (or something) of screen and is being scaled 2 times at point 0,0 (top left visible point)
        newOffsets = ZoomLogic.scaleOffsets(2.0, Qt.point(0, 0), Qt.rect(50, 50, 100, 100), Qt.size(200, 200));

        compare(newOffsets.x, 100, "x offset");
        compare(newOffsets.y, 100, "y offset");

        // image takes 50% (or something) of screen and is being scaled 2 times at point 0,0 (top left visible point)
        newOffsets = ZoomLogic.scaleOffsets(2.0, Qt.point(0, 0), Qt.rect(0, 0, 100, 100), Qt.size(50, 50));

        compare(newOffsets.x, 0, "x offset");
        compare(newOffsets.y, 0, "y offset");
    }

    function test_bottom_right_corner_zoom_in() {

        // image takes 100% of screen and is being scaled 2 times at point 99,99
        var newOffsets = ZoomLogic.scaleOffsets(2.0, Qt.point(99, 99), Qt.rect(0, 0, 100, 100), Qt.size(100, 100));

        compare(newOffsets.x, 99, "x offset");
        compare(newOffsets.y, 99, "y offset");

        // image takes 200% (or something) of screen and is being scaled 2 times at point 0,0 (top left visible point)
        newOffsets = ZoomLogic.scaleOffsets(2.0, Qt.point(149, 149), Qt.rect(50, 50, 100, 100), Qt.size(200, 200));

        compare(newOffsets.x, 249, "x offset");
        compare(newOffsets.y, 249, "y offset");

        // image takes 50% (or something) of screen and is being scaled 2 times at point 0,0 (top left visible point)
        newOffsets = ZoomLogic.scaleOffsets(2.0, Qt.point(49, 49), Qt.rect(0, 0, 100, 100), Qt.size(50, 50));

        compare(newOffsets.x, 49, "x offset");
        compare(newOffsets.y, 49, "y offset");
    }

    function middle_point_zoom_in() {

        // image takes 200% (or something) of screen and is being scaled 2 times at point 25,25
        newOffsets = ZoomLogic.scaleOffsets(2.0, Qt.point(25, 25), Qt.rect(50, 50, 100, 100), Qt.size(200, 200));

        compare(newOffsets.x, 125, "x offset");
        compare(newOffsets.y, 125, "y offset");
    }

    function test_top_left_corner_zoom_out() {

        // image takes 100% of screen and is being scaled down 2 times at point 0,0
        var newOffsets = ZoomLogic.scaleOffsets(0.5, Qt.point(0, 0), Qt.rect(0, 0, 100, 100), Qt.size(100, 100));

        compare(newOffsets.x, 0, "x offset");
        compare(newOffsets.y, 0, "y offset");

        // image takes 200% (or something) of screen and is being scaled down 2 times at point 0,0 (top left visible point)
        newOffsets = ZoomLogic.scaleOffsets(0.5, Qt.point(0, 0), Qt.rect(50, 50, 100, 100), Qt.size(200, 200));

        compare(newOffsets.x, 25, "x offset");
        compare(newOffsets.y, 25, "y offset");

        // image takes 50% (or something) of screen and is being scaled down 2 times at point 0,0 (top left visible point)
        newOffsets = ZoomLogic.scaleOffsets(0.5, Qt.point(0, 0), Qt.rect(0, 0, 100, 100), Qt.size(50, 50));

        compare(newOffsets.x, 0, "x offset");
        compare(newOffsets.y, 0, "y offset");
    }
}
