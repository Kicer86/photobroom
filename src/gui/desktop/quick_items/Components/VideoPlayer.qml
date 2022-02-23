
import QtQuick
import QtMultimedia
import photo_broom.items


VideoOutput {
    id: videoOutputItem

    property alias source: mediaPlayer.source

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (mediaPlayer.playbackState === MediaPlayer.PlayingState)
                mediaPlayer.pause();
            else
                mediaPlayer.play();
        }
    }

    MediaPlayer {
        id: mediaPlayer
        audioOutput: AudioOutput {}
        videoOutput: videoOutputItem

        onErrorOccurred: function(error, errorString) {
            console.log("Error (" + error + ") when opening file " + ctrl.path + " : " + errorString);
        }

        onSourceChanged: {
            if (source != "")
            {
                console.log("Playing video: " + mediaPlayer.source);
                mediaPlayer.play();
            }
            else
                console.log("Source changed to empty");
        }
    }

    // controls
    Item {
        id: playerControls

        anchors.bottom: parent.bottom
        anchors.left:   parent.left
        anchors.right:  parent.right

        height: 20

        // based on https://titanwolf.org/Network/Articles/Article?AID=7a7ba3ff-e8f4-4662-832a-b55a8f2e84d6
        Rectangle {
            id: progressBar
            anchors.fill: parent
            anchors.margins: 0

            color: "lightGray"

            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: mediaPlayer.duration > 0? parent.width * mediaPlayer.position / mediaPlayer.duration : 0
                color: "darkGreen"
            }

            MouseArea {
                property int pos
                anchors.fill: parent

                onClicked: function(mouse) {
                if (mediaPlayer.seekable)
                    pos = mediaPlayer.duration * mouse.x / width
                    mediaPlayer.position = pos
                }
            }
        }

        Text {
            anchors.centerIn: parent

            text: (mediaPlayer.playbackState === MediaPlayer.PlayingState? qsTr("Playing"): qsTr("Paused")) + " " +
                  msToTime(mediaPlayer.position) + " / " + msToTime(mediaPlayer.duration)

            function msToTime(s) {

                function pad(n, z) {
                    z = z || 2;
                    return ('00' + n).slice(-z);
                }

                var ms = s % 1000;
                s = (s - ms) / 1000;
                var secs = s % 60;
                s = (s - secs) / 60;
                var mins = s % 60;
                var hrs = (s - mins) / 60;

                return pad(hrs) + ':' + pad(mins) + ':' + pad(secs);
            }
        }
    }
}
