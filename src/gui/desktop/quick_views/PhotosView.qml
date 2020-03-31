
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

            Connections {
                target: timeRangeId.viewFrom
                onPressedChanged: {
                    if (timeRangeId.viewFrom.pressed === false)
                        photosViewId.model.timeViewFrom = new Date(timeRangeId.viewFrom.value)
                }
            }

            Connections {
                target: timeRangeId.viewTo
                onPressedChanged: {
                    if (timeRangeId.viewTo.pressed === false)
                        photosViewId.model.timeViewTo = new Date(timeRangeId.viewTo.value)
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

    Connections {
        target: photosViewId.model
        ignoreUnknownSignals: true                          // on startup model won't be set so expect errors here

        onTimeRangeFromChanged: {
            timeRangeId.from = model.timeRangeFrom.getTime()
            timeRangeId.viewFrom.value = model.timeRangeFrom.getTime()
        }

        onTimeRangeToChanged: {
            timeRangeId.to = model.timeRangeTo.getTime()
            timeRangeId.viewTo.value = model.timeRangeTo.getTime()
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
