
import QtQuick 2.15
import QtQuick.Controls 2.15

/*
 * Reusable dates range slider.
 */

Item {

    width: childrenRect.width
    height: childrenRect.height

    property alias from: timeSliderId.from
    property alias to: timeSliderId.to

    property alias viewFrom: timeSliderId.first
    property alias viewTo: timeSliderId.second

    property var model

    Row {
        id: row

        Text {
            text: qsTr("Time range:");
            anchors.verticalCenter: parent.verticalCenter
        }

        RangeSlider {
            id: timeSliderId

            from: 0
            to: 0

            stepSize: 1
            snapMode: RangeSlider.SnapAlways
        }

        Text {
            property var from: timeSliderId.first.value
            property var to: timeSliderId.second.value

            text: formatDate(model.dateFor(from)) + " - " + formatDate(model.dateFor(to))
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    function formatDate(date) {
        if (isNaN(date.getTime())) {
            return qsTr("unknown");
        } else {
            return Qt.formatDate(new Date(date), Qt.ISODate);
        }
    }

}
