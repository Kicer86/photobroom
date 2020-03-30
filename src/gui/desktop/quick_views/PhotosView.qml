
import QtQuick 2.14
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.14


Item {
    id: rootId
    objectName: "photos_view"       // used by c++ part to find this view and set proper model

    property alias model: photosViewId.model

    ColumnLayout {

        anchors.fill: parent

        TimeRange {
            id: timeRangeId
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
