
import QtQuick


Item {
    id: ratingItem

    implicitWidth: container.width
    implicitHeight: container.height

    property int rating: 0

    property int cellSize: height

    Row {
        id: container

        width: childrenRect.width
        height: childrenRect.height

        Repeater {
            model: ~~(rating / 2)                   // handle full stars

            Image {
                x: modelData * cellSize
                height: ratingItem.cellSize
                width:  ratingItem.cellSize
                scale: 0.8

                source: "qrc:/gui/star.svg"
            }
        }

        Repeater {
            model: rating % 2                       // optional half star

            Image {
                height: ratingItem.cellSize
                width:  ratingItem.cellSize
                scale: 0.8

                source: "qrc:/gui/half_star.svg"
            }
        }

        Repeater {
            model: ~~((10 - rating) / 2)            // remaining stars - empty ones

            Image {
                x: modelData * cellSize
                height: ratingItem.cellSize
                width:  ratingItem.cellSize
                scale: 0.8

                source: "qrc:/gui/empty_star.svg"
            }
        }
    }
}
