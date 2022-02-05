
import QtQuick
import photo_broom.qml 1.0
import "ViewsComponents" as Internals


TableView {

    id: view

    required property var selection

    topMargin: columnsHeader.implicitHeight + 10
    implicitHeight: contentHeight + columnsHeader.implicitHeight

    SystemPalette { id: sysPalette; colorGroup: SystemPalette.Active }

    model: TagsModel {
        id: tagsModel

        database: PhotoBroomProject.database
    }

    columnWidthProvider: columnWidthFun

    onWidthChanged: view.forceLayout();
    onSelectionChanged: tagsModel.setPhotos(selection)

    selectionModel: ItemSelectionModel {
        model: view.model
    }

    delegate: Rectangle {
        id: delegate

        implicitWidth: 30
        implicitHeight: 30

        color: sysPalette.base
        border.color: selected ? sysPalette.highlight : sysPalette.button

        required property bool selected
        required property int column
        required property int row
        required property var display
        required property var tagType

        property bool editState: false

        Component {
            id: labelDelegate

            Text {
                verticalAlignment: Text.AlignVCenter
                text: display
            }
        }

        Component {
            id: tagViewer

            Internals.TagValueViewer {
                id: tagViewerItem

                tagType: delegate.tagType
                value: delegate.display

                MouseArea {
                    anchors.fill: parent

                    onDoubleClicked: {
                        delegate.editState = true;
                    }

                    onClicked: {
                        const index = view.model.index(row, column);
                        view.selectionModel.select(index, ItemSelectionModel.ClearAndSelect);
                        tagViewerItem.forceActiveFocus();
                    }
                }

                Keys.onPressed: function(event) {
                    if (event.key == Qt.Key_Delete) {
                        const index = view.model.index(row, column);
                        view.model.setData(index, undefined);
                    }
                }
            }
        }

        Component {
            id: tagEditor

            Internals.TagValueEditor {
                tagType: delegate.tagType
                value: delegate.display

                onAccepted: function(value) {
                    model.edit = value;
                    delegate.editState = false;
                }

                onRejected: delegate.editState = false;

                Keys.onPressed: function(event) {
                    if (event.key == Qt.Key_Escape)
                        delegate.editState = false;
                }
            }
        }

        Loader {
            sourceComponent: column === 0? labelDelegate: (delegate.editState? tagEditor: tagViewer)

            anchors.fill: parent
            anchors.margins: 3
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
