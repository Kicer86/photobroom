
import QtQuick
import QtQuick.Controls

import photo_broom.models
import photo_broom.singletons
import quick_items
import "ViewsComponents" as Internals


Item {

    required property var selection

    implicitHeight: view.implicitHeight

    onSelectionChanged: tagsModel.setPhotos(selection)

    TableView {
        id: view

        anchors.fill: parent

        implicitHeight: contentHeight
        enabled: tagsModel.busy === false
        opacity: enabled? 1: 0.5

        Behavior on opacity { PropertyAnimation {} }

        SystemPalette { id: sysPalette; colorGroup: SystemPalette.Active }

        model: TagsModel {
            id: tagsModel

            database: PhotoBroomProject.database
        }

        columnWidthProvider: _columnWidthFun

        onWidthChanged: view.forceLayout();

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
                    text: display === undefined? "": display
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

        TextMetrics {
            id: metrics
        }

        function _columnWidthFun(column) {
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
                return view.width - _columnWidthFun(0);
            else
                return 0;
        }
    }

    BusyIndicator {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        running: tagsModel.busy
    }

}

