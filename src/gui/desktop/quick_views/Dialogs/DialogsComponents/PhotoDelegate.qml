
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

        source: photoData.path

        PhotoDataQml {
            id: photoDataProperies
            data: photoData
        }

        Text {
            x: 5
            y: 5

            property bool isNew: photoDataProperies.flags[PhotoEnums.StagingArea] === 1
            property bool isGroup: photoDataProperies.group

            visible: isNew || isGroup

            text: (isNew? qsTr("NEW"): "") + (isGroup? " " + qsTr("Group", "a noun"): "")
            font.pixelSize: 12
            font.family: "Nimbus Mono PS"

            color: "white"
            style: Text.Outline
            styleColor: "black"
        }
    }
}
