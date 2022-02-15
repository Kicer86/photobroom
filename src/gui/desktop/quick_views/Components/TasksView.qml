
import QtQuick
import photo_broom.qml

ListView {
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

        Text { text: '<b>' + name + ':</b> ' + _progress + "(" + _details + ")"  }
    }

    displaced: Transition {
        NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad; duration: 1000 }
    }

    remove: Transition {
        NumberAnimation { property: "opacity"; to: 0; easing.type: Easing.InOutQuad; duration: 1000 }
    }
}
