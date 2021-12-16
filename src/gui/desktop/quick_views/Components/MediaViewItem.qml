
import QtQuick
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
                id: mediaPlayer
                source: ctrl.path
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

                height: 10

                // based on https://titanwolf.org/Network/Articles/Article?AID=7a7ba3ff-e8f4-4662-832a-b55a8f2e84d6
                Rectangle {
                    id: progressBar
                    anchors.fill: parent
                    anchors.margins: 0
                    height: 10
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
