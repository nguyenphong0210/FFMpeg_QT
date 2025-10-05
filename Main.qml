import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: win
    width: 800
    height: 600
    visible: true
    title: "FFmpeg + Qt QML Video Example"

    Image {
        id: videoFrame
        anchors.fill: parent
        source: "image://videoframe/frame"
        fillMode: Image.PreserveAspectFit

        Timer {
            interval: 33
            running: true
            repeat: true
            onTriggered: videoFrame.source = "image://videoframe/frame?" + Math.random()
        }
    }
}
