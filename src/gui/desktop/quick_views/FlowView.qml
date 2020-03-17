
import QtQuick 2.14

// inspiration / copy of https://github.com/ruedigergad/FlowListView
// https://ruedigergad.com/2012/11/24/qml-flowlistview/

Flickable {
    id: flowListViewId
    anchors.fill: parent

    contentWidth: parent.width;
    contentHeight: flowId.childrenRect.height

    property alias count: repeaterId.count
    property alias delegate: repeaterId.delegate
    property alias flow: flowId.flow
    property alias model: repeaterId.model

    Flow {
        id: flowId
        width: parent.width

        Repeater {
            id: repeaterId
        }
    }
}
