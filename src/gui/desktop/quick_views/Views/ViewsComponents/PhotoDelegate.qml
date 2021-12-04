
import QtQuick 2.15
import QtQuick.Controls 2.15
import photo_broom.qml 1.0
import "../../Components" as Components


/*
 * Delegate for views.
 * Compatible with APhotoInfoModel.
 * Loads photo in lazy manner.
 */

Item {
    id: delegateId

    width: 50
    height: 50
    property int margin: 2

    Components.PhotoThumbnail {

        anchors.centerIn: parent
        height: delegateId.height - delegateId.margin * 2
        width:  delegateId.width - delegateId.margin * 2

        photoID: photoData.id

        PhotoDataQml {
            id: photoDataProperies
            data: photoData
        }

        Item {
            x: 5
            y: 5

            width: 24
            height: 24

            visible: photoDataProperies.flags[PhotoEnums.StagingArea] === 1

            Rectangle {
                anchors.fill: parent

                opacity: 0.7

                radius: 12
                color: "white"
            }

            Image {
                anchors.fill: parent

                sourceSize.width: 24
                sourceSize.height: 24

                source: "qrc:/gui/new.svg"
            }
        }

        Item {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.bottomMargin: 5
            anchors.leftMargin: 5

            width: 24
            height: 24

            visible: photoDataProperies.group

            Rectangle {
                anchors.fill: parent

                opacity: 0.7

                radius: 3
                color: "white"
            }

            Image {
                anchors.fill: parent

                sourceSize.width: 24
                sourceSize.height: 24

                source: "qrc:/gui/paper.svg"
            }
        }

    }
}
