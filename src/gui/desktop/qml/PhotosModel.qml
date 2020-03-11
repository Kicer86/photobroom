
import QtQuick 2.0
import QtQml.Models 2.1


ListModel {
    id: photosModelId

    ListElement {
        name: "Apple"
        cost: 2.45
        attributes: [
            ListElement { description: "Core" },
            ListElement { description: "Deciduous" }
        ]
    }
}
