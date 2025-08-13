import QtQuick
import QtQuick.Controls

Dialog {
    id: configurationDialog
    objectName: "ConfigurationDialog"
    title: qsTr("Configuration")
    modal: true
    standardButtons: Dialog.Ok

    contentItem: Column {
        spacing: 10
        padding: 20
        Label {
            text: qsTr("Configuration options are not implemented yet.")
            wrapMode: Text.WordWrap
        }
    }
}
