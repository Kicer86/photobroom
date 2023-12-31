
import QtQuick
import QtQuick3D
import QtQuick3D.Helpers

View3D {
    id: viewport
    anchors.fill: parent

    property alias source: sourceTexture.source

    property real _scaleF: 1.0

    environment: SceneEnvironment {
        antialiasingMode: SceneEnvironment.MSAA
        antialiasingQuality: SceneEnvironment.VeryHigh
        lightProbe: Texture {
            id: sourceTexture
        }
        backgroundMode: SceneEnvironment.SkyBox
    }

    Node {
        id: originNode
        PerspectiveCamera {
            id: cameraNode
            fieldOfView: 60 * viewport._scaleF
        }
    }

    OrbitCameraController {
        origin: originNode
        camera: cameraNode
        xInvert: true
        yInvert: false
        xSpeed: 0.5 * (viewport.width / viewport.height) * ySpeed         // a bit troublesome equation made up to keep horizonal moves as natural as possible
        ySpeed: (1024 / viewport.height) / 10 * viewport._scaleF        // All parameters chosen empirically.
    }

    MouseArea {
        anchors.fill: parent

        onWheel: function(wheel) {
            var scaleF = viewport._scaleF - wheel.angleDelta.y / 120 * 0.05

            if (scaleF > 2)
                scaleF = 2
            if (scaleF < 0.5)
                scaleF = 0.5

            viewport._scaleF = scaleF
        }
    }
}
