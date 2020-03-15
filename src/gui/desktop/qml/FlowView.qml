
import QtQuick 2.14

// inspiration / copy of https://github.com/ruedigergad/FlowListView
// https://ruedigergad.com/2012/11/24/qml-flowlistview/

Flickable {
    id: flowListViewId
    anchors.fill: parent

    contentWidth: parent.width;
    contentHeight: flowId.childrenRect.height

    property alias count: repeaterId.count
    property int currentIndex: -1
    property variant currentItem;
    property alias delegate: repeaterId.delegate
    property alias flow: flowId.flow
    property alias model: repeaterId.model

    onCurrentIndexChanged: {
        currentItem = model.get(currentIndex)
    }

    Flow {
        id: flowId
        width: parent.width

        Repeater {
            id: repeaterId

            onCountChanged: {
                if (flowListViewId.currentIndex === -1 && count > 0) {
                    flowListViewId.currentIndex = 0
                }
                if (flowListViewId.currentIndex >= count) {
                    flowListViewId.currentIndex = count - 1
                }
            }
        }
    }
}
