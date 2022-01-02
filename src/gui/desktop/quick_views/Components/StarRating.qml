
import QtQuick


Item {
    id: ratingItem

    property int rating: 0

    property int cellSize: height
    Repeater {
        id: stars
        model: 5

        Item {
            Image {
                id: star

                x: modelData * cellSize
                height: ratingItem.cellSize
                width:  ratingItem.cellSize
                scale: 0.8
                visible: modelData * 2 < rating                      // Desaturate will paint star

                source: "qrc:/gui/star.svg"
            }

            Desaturate {
                anchors.fill: star
                source: star
                scale: 0.8
                visible: !star.visible

                desaturation: 1.0
            }
        }
    }
}
