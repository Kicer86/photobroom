
import QtQuick 2.15
import QtMultimedia
import photo_broom.qml 1.0


Item {

    property alias photoID: ctrl.photoID

    MediaViewCtrl {
        id: ctrl

        onModeChanged: function(mode) {

        }
    }

    Image {
        id: staticImage

        anchors.fill: parent

        enabled: ctrl.mode === MediaViewCtrl.StaticImage
        visible: enabled

        source: enabled? ctrl.path: ""
        asynchronous: true
        autoTransform: true
        fillMode: Image.PreserveAspectFit
    }

    VideoOutput {
        id: video

        anchors.fill: parent

        enabled: ctrl.mode === MediaViewCtrl.Video
        visible: enabled

        fillMode: Image.PreserveAspectFit

        MediaPlayer {
            id: player
            source: video.enabled? ctrl.path: ""
            audioOutput: AudioOutput {}
            videoOutput: video

            onErrorOccurred: function(error, errorString) {
                console.log("Error (" + error + ") when opening file " + ctrl.path + " : " + errorString);
            }

            onSourceChanged: {
                if (source === "")
                {
                    player.stop();
                    console.log("Stopping video");
                }
                else
                {
                    console.log("Playing video: " + player.source);
                    player.play();
                }
            }
        }
    }
}
