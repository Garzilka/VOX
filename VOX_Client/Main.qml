import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import VOX_Client

ApplicationWindow
{
    id: splashWindow
    width: 300
    height: 350
    visible: true
    color: "#1e1f22"

    flags: Qt.Window | Qt.FramelessWindowHint

    x: (Screen.width - width) / 2
    y: (Screen.height - height) / 2

    property bool minTimeElapsed: false
    property bool isClosing: false

    Component.onCompleted:
    {
        openAnimation.start();
    }

    ParallelAnimation
    {
        id: openAnimation
        NumberAnimation { target: splashContent; property: "scale"; from: 0.85; to: 1.0; duration: 300; easing.type: Easing.OutCubic }
        NumberAnimation { target: splashContent; property: "opacity"; from: 0.0; to: 1.0; duration: 250; easing.type: Easing.OutCubic }
    }

    function checkLoadingStatus()
    {
        if (UIStateManager.securitySuccessfully && minTimeElapsed && !isClosing)
        {
            isClosing = true;
            closeAnimation.start();
        }
    }
    property var rootMainWindow: null

    ParallelAnimation
    {
        id: closeAnimation

        NumberAnimation { target: splashContent; property: "scale"; to: 0.6; duration: 250; easing.type: Easing.InBack }
        NumberAnimation { target: splashContent; property: "opacity"; to: 0.0; duration: 220; easing.type: Easing.OutCubic }

        onFinished:
        {
            var component = Qt.createComponent("MainWindow.qml");
            if (component.status === Component.Ready)
            {
                splashWindow.rootMainWindow = component.createObject(null);

                if (splashWindow.rootMainWindow)
                {
                    splashWindow.rootMainWindow.show();
                    splashWindow.close();
                }
            }
            else
            {
                console.error(component.errorString())
            }
        }
    }

    Connections
    {
        target: UIStateManager
        function onSecuritySuccessfullyChanged()
        {
            splashWindow.checkLoadingStatus();
        }
    }

    Timer
    {
        id: startupTimer
        interval: 750
        running: true
        repeat: false
        onTriggered:
        {
            splashWindow.minTimeElapsed = true;
            splashWindow.checkLoadingStatus();
        }
    }

    Item {
        id: splashContent
        anchors.fill: parent
        transformOrigin: Item.Center
        opacity: 0.0
        scale: 0.85

        ColumnLayout
        {
            anchors.centerIn: parent
            spacing: 24

            Rectangle
            {
                Layout.alignment: Qt.AlignHCenter
                width: 60
                height: 60
                color: "transparent"
                border.color: "#35373c"
                border.width: 4
                radius: 30

                Rectangle
                {
                    width: 60
                    height: 60
                    color: "transparent"
                    border.color: "#5865f2"
                    border.width: 4
                    radius: 30
                    clip: true

                    Rectangle
                    {
                        width: 30
                        height: 30
                        color: "#1e1f22"
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                    }
                    Rectangle
                    {
                        width: 30
                        height: 30
                        color: "#1e1f22"
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                    }
                }

                RotationAnimator on rotation
                {
                    from: 0
                    to: 360
                    duration: 1000
                    loops: Animation.Infinite
                    running: splashWindow.visible
                }
            }

            Text
            {
                text: "VOX"
                font.pixelSize: 22
                font.bold: true
                color: "#ffffff"
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Text
            {
                text: "Запуск протоколов безопасности..."
                font.pixelSize: 12
                color: "#949ba4"
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }
    }
}
