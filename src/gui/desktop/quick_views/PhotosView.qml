
import QtQuick 2.14
import QtQuick.Layouts 1.14


Item {
    id: photosViewId
    objectName: "photos_view"       // used by c++ part to find this view and set proper model

    property alias model: photosGridViewId.model

    ColumnLayout {

        anchors.fill: parent

        TimeRange {
            id: timeRangeId

            function updateRange() {
                photosViewId.model.timeViewFrom = new Date(timeRangeId.viewFrom.value)
                photosViewId.model.timeViewTo = new Date(timeRangeId.viewTo.value)
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
            id: photosGridViewId

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
