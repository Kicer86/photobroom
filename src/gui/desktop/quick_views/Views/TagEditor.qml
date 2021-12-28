
import QtQuick
import photo_broom.qml 1.0
import "../Components/VariantConverter.js" as Logic


TableView {

    id: view

    property alias selection: tagsModel.selection

    topMargin: columnsHeader.implicitHeight + 10

    SystemPalette { id: sysPalette; colorGroup: SystemPalette.Active }

    model: TagsModel {
        id: tagsModel

        database: PhotoBroomProject.database
    }

    columnWidthProvider: columnWidthFun

    onWidthChanged: view.forceLayout();

    delegate: Rectangle {

        implicitWidth: 35
        implicitHeight: 35

        color: selected ? sysPalette.highlight : sysPalette.base
        border.color: sysPalette.button

        required property bool selected
        required property var display
        required property var tagType

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter

            text: Logic.variantToString(tagType, display);
        }
    }

    Text {
        id: columnsHeader

        y: view.contentY
        z: 2

        text: qsTr("Photo information")
    }

    /*
    Row {
        id: columnsHeader
        y: view.contentY
        z: 2

        Repeater {
            model: view.columns > 0 ? view.columns : 1

            Text {
                width: view.columnWidthProvider(modelData)
                height: 35
                padding: 10

                text: view.model.headerData(modelData, Qt.Horizontal)
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
    */

    TextMetrics {
        id: metrics
    }

    function columnWidthFun(column) {
        if (column === 0) {
            var rows = view.model.rowCount();
            var maxWidth = 0;

            for(var i = 0; i < rows; i++) {
                var index = view.model.index(i, 0);
                var data = view.model.data(index);

                metrics.text = data;
                var textWidth = metrics.boundingRect.width;

                if (textWidth > maxWidth)
                    maxWidth = textWidth;
            }

            return maxWidth + 7;   // some margin
        } else if (column === 1)
            return view.width - columnWidthFun(0);
        else
            return 0;
    }
}
