
import QtQuick 2.14
import QtQuick.Layouts 1.14


Item {
    id: rootId
    objectName: "photos_view"       // used by c++ part to find this view and set proper model

    property alias model: photosViewId.model

    ColumnLayout {

        anchors.fill: parent

        TimeRange {
            id: timeRangeId

            function updateRange() {
                rootId.model.timeView = new Date(timeRangeId.viewFrom.value), new Date(timeRangeId.viewTo.value)
            }

            Connections {
                target: timeRangeId.viewFrom
                onPressedChanged: {
                    if (timeRangeId.viewFrom.pressed === false)
                        timeRangeId.updateRange();
                }
            }

            Connections {
                target: timeRangeId.viewTo
                onPressedChanged: {
                    if (timeRangeId.viewTo.pressed === false)
                        timeRangeId.updateRange();
                }
            }
        }

        PhotosGridView {
            id: photosViewId

            clip: true

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
