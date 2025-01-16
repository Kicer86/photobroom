
import QtQuick 2.15
import QtQuick.Controls 2.15

CheckBox {

    required property int index
    required property DelegateState state

    checkState: state.state(index) === true? Qt.Checked: Qt.Unchecked

    onCheckStateChanged: {
        if (checkState)
            state.setState(index, true);
        else
            state.setState(index, false);
    }
}
