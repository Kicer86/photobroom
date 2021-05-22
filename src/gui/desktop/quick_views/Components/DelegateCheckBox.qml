
import QtQuick 2.15
import QtQuick.Controls 2.15

CheckBox {

    required property int index
    required property DelegateState state

    checkState: state.state(index) === 1? Qt.Checked: Qt.Unchecked

    onCheckStateChanged: {
        if (checkState)
            state.setState(index, 1);
        else
            state.setState(index, 0);
    }
}
