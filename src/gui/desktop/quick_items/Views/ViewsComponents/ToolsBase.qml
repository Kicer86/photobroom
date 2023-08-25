

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import photo_broom.models
import photo_broom.singletons

import "../../Components" as Components

/**
 * @brief base for tool views
 **/

Item
{
    id: toolView
    state: "LoadingState"

    required property var model
    required property var view
    property var apply: undefined

    required property string idlePrompt
    required property string loadingPrompt
    required property string loadedPrompt
    required property string emptyPrompt
    property string storingPrompt: undefined


    SystemPalette { id: currentPalette; colorGroup: SystemPalette.Active }

    Item {
        anchors.fill: parent

        Behavior on opacity { PropertyAnimation{} }

        Components.InfoItem {
            id: status

            width: parent.width
            anchors.top: parent.top

            MouseArea {
                anchors.fill: parent

                enabled: toolView.state == "Idle" || toolView.state == "LoadedState"
                cursorShape: enabled? Qt.PointingHandCursor: Qt.ArrowCursor

                onClicked: model.reload()
            }
        }

        ColumnLayout {
            id: viewId

            width: parent.width
            anchors.top: status.bottom
            anchors.bottom: parent.bottom

            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true

                sourceComponent: toolView.view
            }

            Loader {
                sourceComponent: toolView.apply

                visible: toolView.state === "LoadedState" && toolView.model.isEmpty() === false
            }
        }

        BusyIndicator {
            id: progressId

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            running: progressId.opacity > 0.0

            Behavior on opacity { PropertyAnimation{} }
        }

        Text {
            id: emptyListInfo

            opacity: toolView.state == "LoadedState" && toolView.model.isEmpty()? 1.0 : 0.0

            Behavior on opacity { PropertyAnimation{} }

            text: toolView.emptyPrompt
            anchors.verticalCenter: progressId.verticalCenter
            anchors.horizontalCenter: progressId.horizontalCenter
            font.pixelSize: 12
        }
    }

    states: [
        State {
            name: "Idle"
            when: toolView.model.state == SeriesModel.Idle

            PropertyChanges {
                target: viewId
                opacity: 0.0
            }

            PropertyChanges {
                target: progressId
                opacity: 0.0
            }

            PropertyChanges {
                target: status
                text: toolView.idlePrompt
            }
        },
        State {
            name: "StoringState"
            when: toolView.model.state == SeriesModel.Storing

            PropertyChanges {
                target: viewId
                opacity: 0.5
            }

            PropertyChanges {
                target: status
                text: toolView.storingPrompt
            }
        },
        State {
            name: "LoadingState"
            when: toolView.model.state == SeriesModel.Fetching

            PropertyChanges {
                target: viewId
                opacity: 0.0
            }

            PropertyChanges {
                target: status
                text: toolView.loadingPrompt
            }
        },
        State {
            name: "LoadedState"
            when: toolView.model.state == SeriesModel.Loaded

            PropertyChanges {
                target: progressId
                opacity: 0.0
            }

            PropertyChanges {
                target: status
                text: toolView.loadedPrompt
            }
        }
    ]
}
