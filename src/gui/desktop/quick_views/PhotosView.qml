
import QtQuick 2.14
import QtQuick.Layouts 1.14
import photo_broom.qml 1.0


Item {
    id: photosViewId

    PhotosModelController {
        id: photosModelControllerId
        objectName: "photos_model_controller"      // used by c++ part to find this model and set it up

        onDatesCountChanged: {
            timeRangeId.from = 0
            timeRangeId.to = photosModelControllerId.datesCount > 0? photosModelControllerId.datesCount - 1: 0

            timeRangeId.viewFrom.value = 0
            timeRangeId.viewTo.value = photosModelControllerId.datesCount > 0? photosModelControllerId.datesCount - 1 : 0
        }

    }

    ColumnLayout {

        anchors.fill: parent

        TimeRange {
            id: timeRangeId

            model: photosModelControllerId
            visible: photosModelControllerId.datesCount > 0

            Connections {
                target: timeRangeId.viewFrom
                onPressedChanged: {
                    if (timeRangeId.viewFrom.pressed === false)
                        photosModelControllerId.timeViewFrom = timeRangeId.viewFrom.value
                }
            }

            Connections {
                target: timeRangeId.viewTo
                onPressedChanged: {
                    if (timeRangeId.viewTo.pressed === false)
                        photosModelControllerId.timeViewTo = timeRangeId.viewTo.value
                }
            }
        }

        PhotosGridView {
            id: photosGridViewId

            clip: true

            model: photosModelControllerId.photos

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            populate: Transition {
                NumberAnimation { properties: "x,y"; duration: 1000 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 250 }
            }

            onCurrentIndexChanged: photosModelControllerId.selectedPhoto = photosGridViewId.currentIndex
        }

    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
