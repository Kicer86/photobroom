
import QtQuick 2.14
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.14


Item
{
    id: rootId

    ColumnLayout {

        anchors.fill: parent

        Row {

            height: childrenRect.height

            Text {
                text: qsTr("Time range:");
            }

            RangeSlider {
                id: timeSliderId
            }

            Text {
                id: rangeId

                property int from_time
                property int to_time

                text: Qt.formatDateTime(Date(from_time), Qt.ISODate) + " - " + Qt.formatDateTime(Date(to_time), Qt.ISODate)
            }
        }

        GridView {
            id: photosViewId
            objectName: "photos_view"       // used by c++ part to find this view and set proper model

            property int thumbnailSize: 160
            property int thumbnailMargin: 2

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            clip: true
            cellWidth: thumbnailSize + thumbnailMargin
            cellHeight: thumbnailSize + thumbnailMargin
            delegate: PhotoDelegate { }
            highlight: highlightId
            keyNavigationEnabled: true

            ScrollBar.vertical: ScrollBar { }

            Component {
                id: highlightId

                Rectangle {
                    width: view.cellWidth;
                    height: view.cellHeight
                    color: "lightsteelblue";
                    opacity: 0.7
                    x: view.currentItem.x
                    y: view.currentItem.y
                    z: 1
                    Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
                    Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }
                }
            }
        }
    }


}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
