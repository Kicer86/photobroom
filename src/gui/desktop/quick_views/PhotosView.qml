
import QtQuick 2.14
import QtQuick.Layouts 1.14
import photo_broom.qml 1.0


Item {
    id: photosViewId

    PhotosModel {
        id: photosModelId
        objectName: "photos_model"      // used by c++ part to find this model and set it up

        onTimeRangeFromChanged: {
            timeRangeId.from = photosModelId.timeRangeFrom.getTime()
            timeRangeId.viewFrom.value = photosModelId.timeViewFrom.getTime()
        }

        onTimeRangeToChanged: {
            timeRangeId.to = photosModelId.timeRangeTo.getTime()
            timeRangeId.viewTo.value = photosModelId.timeViewTo.getTime()
        }
    }

    ColumnLayout {

        anchors.fill: parent

        TimeRange {
            id: timeRangeId

            Connections {
                target: timeRangeId.viewFrom
                onPressedChanged: {
                    if (timeRangeId.viewFrom.pressed === false)
                        photosModelId.timeViewFrom = new Date(timeRangeId.viewFrom.value)
                }
            }

            Connections {
                target: timeRangeId.viewTo
                onPressedChanged: {
                    if (timeRangeId.viewTo.pressed === false)
                        photosModelId.timeViewTo = new Date(timeRangeId.viewTo.value)
                }
            }
        }

        PhotosGridView {
            id: photosGridViewId

            clip: true

            model: photosModelId.model

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            populate: Transition {
                NumberAnimation { properties: "x,y"; duration: 1000 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 250 }
            }
        }

    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
