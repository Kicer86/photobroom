
import QtQuick 2.15

Item {

    property var properties: new Map()
    property var defaultValue

    function clear() {
        properties.clear();
    }

    function state(index) {
        if (properties.has(index))
            return properties.get(index);
        else
            return defaultValue;
    }

    function setState(index, state) {
        return properties.set(index, state);
    }

    function getItems(condition) {
        var result = []

        properties.forEach( (state, index, map) => {
            if (condition(state))
                result.push(index);
        });

        return result;
    }
}
