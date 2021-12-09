
import QtQuick 2.15
import QtMultimedia
import photo_broom.qml 1.0


Item {
    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl
    }

    Component {
        id: staticImage

        Image {
            anchors.fill: parent

            source: ctrl.path
            asynchronous: true
            autoTransform: true
            fillMode: Image.PreserveAspectFit
        }
    }

    Component {
        id: video

        VideoOutput {
            id: videoOutputItem
            anchors.fill: parent

            fillMode: Image.PreserveAspectFit

            MediaPlayer {
                id: player
                source: ctrl.path
                audioOutput: AudioOutput {}
                videoOutput: videoOutputItem

                onErrorOccurred: function(error, errorString) {
                    console.log("Error (" + error + ") when opening file " + ctrl.path + " : " + errorString);
                }

                onSourceChanged: {
                    if (source != "")
                    {
                        console.log("Playing video: " + player.source);
                        player.play();
                    }
                    else
                        console.log("Source changed to empty");
                }
            }
        }
    }

    Component {
        id: animatedImage

        AnimatedImage {
            anchors.fill: parent

            source: ctrl.path
            asynchronous: true
            autoTransform: true
            fillMode: Image.PreserveAspectFit
        }
    }

    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.StaticImage?   staticImage:   undefined; anchors.fill: parent }
    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.Video?         video:         undefined; anchors.fill: parent }
    Loader { sourceComponent: ctrl.mode === MediaViewCtrl.AnimatedImage? animatedImage: undefined; anchors.fill: parent }
}
