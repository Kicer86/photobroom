
import QtQuick
import QtQuick.Controls

ListView {
    implicitHeight: contentHeight
    implicitWidth: contentWidth

    delegate: Item {
        required property int minValue
        required property int maxValue
        required property int value
        required property string name

        implicitWidth: childrenRect.width
        implicitHeight: childrenRect.height

        property int _steps: maxValue - minValue
        property string _progress: _steps == 0? "": (Math.round(value / _steps * 100) + "% ")
        property string _details: value + "/" + _steps

        Row {
            height: busy.height

            Text { id: title; anchors.verticalCenter: parent.verticalCenter;  text: '<b>' + name + ':</b>&nbsp;' }
            Text { visible: _steps > 0; anchors.verticalCenter: parent.verticalCenter; text: _progress + "(" + _details + ")"  }
            BusyIndicator { id: busy; running: _steps === 0; height: title.height * 1.5; anchors.verticalCenter: parent.verticalCenter }
        }
    }

    displaced: Transition {
        NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad; duration: 1000 }
    }

    remove: Transition {
        NumberAnimation { property: "opacity"; to: 0; easing.type: Easing.InOutQuad; duration: 1000 }
    }
}
