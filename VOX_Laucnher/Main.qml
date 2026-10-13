import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Window
{
    id: launcherWindow
    visible: true
    title: "VOX Launcher"

    flags: Qt.Window | Qt.FramelessWindowHint
    color: "transparent"

    width: LauncherCtrl.uiState === 0 ? 300 : 550
    height: LauncherCtrl.uiState === 0 ? 300 : 150

    Behavior on width { NumberAnimation { duration: 250; easing.type: Easing.InOutQuad } }
    Behavior on height { NumberAnimation { duration: 250; easing.type: Easing.InOutQuad } }

    onWidthChanged: {
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2
    }

    Rectangle
    {
        anchors.fill: parent
        color: "#1e1f22"
        radius: 8
        border.color: "#2b2d31"
        border.width: 1

        MouseArea {
            anchors.fill: parent
            property point clickPos: "0,0"
            onPressed: (mouse) => clickPos = Qt.point(mouse.x, mouse.y)
            onPositionChanged: (mouse) => {
                var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                launcherWindow.x += delta.x
                launcherWindow.y += delta.y
            }
        }

        // =====================================================================
        // КНОПКА ЗАКРЫТИЯ
        // =====================================================================
        Button {
            id: closeButton
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 8
            anchors.rightMargin: 8
            z: 999

            implicitWidth: 24
            implicitHeight: 24
            flat: true

            background: Rectangle
            {
                color: closeButton.hovered ? "#f23f43" : "transparent"
                radius: 4
                Behavior on color { ColorAnimation { duration: 100 } }
            }

            contentItem: Text {
                text: "✕"
                font.pixelSize: 12
                font.bold: true
                color: closeButton.hovered ? "#ffffff" : "#72767d"
                anchors.centerIn: parent
            }

            HoverHandler { cursorShape: Qt.PointingHandCursor }

            onClicked: Qt.quit()
        }

        // =====================================================================
        // Квадратный блок со спиннером
        // =====================================================================
        ColumnLayout {
            id: discordStyleLayout
            anchors.centerIn: parent
            spacing: 20
            visible: LauncherCtrl.uiState === 0

            BusyIndicator {
                id: startSpinner
                Layout.alignment: Qt.AlignHCenter
                implicitWidth: 54
                implicitHeight: 54
                running: LauncherCtrl.uiState === 0

                contentItem: Item {
                    implicitWidth: 64
                    implicitHeight: 64
                    Canvas {
                        id: canvas
                        anchors.fill: parent
                        Connections {
                            target: startSpinner
                            function onRunningChanged() { canvas.requestPaint() }
                        }
                        onPaint: {
                            var ctx = canvas.getContext("2d");
                            ctx.reset();
                            ctx.strokeStyle = "#5865f2";
                            ctx.lineWidth = 3;
                            ctx.beginPath();
                            ctx.arc(width / 2, height / 2, width / 2 - ctx.lineWidth, 0, Math.PI * 1.5);
                            ctx.stroke();
                        }
                        RotationAnimator {
                            target: canvas
                            running: startSpinner.running
                            from: 0; to: 360; loops: Animation.Infinite; duration: 1000
                        }
                    }
                }
            }

            Text {
                text: "VOX"
                color: "#ffffff"
                font.bold: true; font.pixelSize: 22; font.letterSpacing: 4
                Layout.alignment: Qt.AlignHCenter
            }

            Text
            {
                text: "Версия " + LauncherCtrl.versionName
                color: "#4e5058" // Приглушенный серый цвет
                font.pixelSize: 11
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: -12 // Слегка прижимаем к заголовку VOX
            }

            Text {
                text: LauncherCtrl.statusText
                color: "#949ba4"; font.pixelSize: 12
                Layout.alignment: Qt.AlignHCenter
            }
        }

        // =====================================================================
        // Панель скачивания
        // =====================================================================
        ColumnLayout {
            id: epicStyleLayout
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 24
            anchors.rightMargin: 24
            spacing: 14
            visible: LauncherCtrl.uiState === 1

            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: LauncherCtrl.statusText
                    color: "#ffffff"; font.bold: true; font.pixelSize: 13
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: Math.round(LauncherCtrl.downloadProgress * 100) + "%"
                    color: "#5865f2"; font.bold: true; font.pixelSize: 13
                }
            }

            Rectangle {
                id: progressBarBackground
                Layout.fillWidth: true; height: 6
                color: "#35373c"; radius: 3
                Rectangle {
                    width: progressBarBackground.width * LauncherCtrl.downloadProgress
                    height: parent.height; color: "#5865f2"; radius: 3
                    Behavior on width { NumberAnimation { duration: 100 } }
                }
            }

            RowLayout
            {
                Layout.fillWidth: true
                Text
                {
                    text: "Обновление ядра под версию " + LauncherCtrl.versionName
                    color: "#949ba4"; font.pixelSize: 11
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: "Платформа: WINDOWS"
                    color: "#4e5058"; font.pixelSize: 11
                }
            }
        }
    }
}
