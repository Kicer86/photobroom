
import QtQuick

Item {
    id: root

    property alias model: dataSet.model
    property var defaultValue

    function clear() {
        for (let i = 0; i < dataSet.count; i++)
            dataSet.objectAt(i).state = root.defaultValue
    }

    function state(index) {
        return dataSet.objectAt(index).state;
    }

    function setState(index, state) {
        dataSet.objectAt(index).state = state;
    }

    function getItems(condition) {
        var result = []

        for (let i = 0; i < dataSet.count; i++) {
            const itemState = state(i);
            if (condition(itemState))
                result.push(i);
        }

        return result;
    }

    Instantiator {
        id: dataSet

        delegate: Item {
            property bool state: root.defaultValue
        }
    }
}
