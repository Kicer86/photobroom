
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
        if (index < dataSet.count) {
            let obj = dataSet.objectAt(index);

            if (obj)
                return obj.state;
        }

        return false;
    }

    function setState(index, state) {
        if (index < dataSet.count) {
            let obj = dataSet.objectAt(index);

            if (obj)
                obj.state = state;
        }
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
