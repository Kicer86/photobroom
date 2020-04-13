
import QtQuick 2.14
import QtQuick.Controls 2.3


Item {

    width: childrenRect.width
    height: childrenRect.height

    property alias from: timeSliderId.from
    property alias to: timeSliderId.to

    property alias viewFrom: timeSliderId.first
    property alias viewTo: timeSliderId.second

    property var model

    Row {

        Text {
            text: qsTr("Time range:");
        }

        RangeSlider {
            id: timeSliderId

            from: 0
            to: new Date().getTime()

            stepSize: 1
            snapMode: RangeSlider.SnapAlways
        }

        Text {
            property var from: timeSliderId.first.value
            property var to: timeSliderId.second.value

            text: Qt.formatDate(new Date(model.dateFor(from)), Qt.ISODate) + " - " + Qt.formatDate(new Date(model.dateFor(to)), Qt.ISODate)
        }
    }

}
