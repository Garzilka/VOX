import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import VOX_Client

Dialog {
    id: dialogRoot
    anchors.centerIn: parent
    width: 380
    height: 220
    modal: true
    title: "Создание сервера"

    // Синхронизируем состояние видимости с C++ UIManager
    visible: UIStateManager.isCreateServerDialogOpen

    onClosed: {
        UIStateManager.isCreateServerDialogOpen = false
        serverNameInput.text = ""
    }

    background: Rectangle {
        color: "#313338"
        radius: 8
        border.color: "#1f2023"
        border.width: 1
    }

    header: Rectangle {
        color: "#2b2d31"
        height: 40
        radius: 8

        Text {
            text: "Создать свой сервер"
            color: "white"
            font.bold: true
            font.pixelSize: 15
            anchors.centerIn: parent
        }
    }

    contentItem: ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: "Имя сервера определит его тематику. Вы всегда сможете изменить его позже."
            color: "#b5bac1"
            font.pixelSize: 12
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        TextField {
            id: serverNameInput
            Layout.fillWidth: true
            placeholderText: "Введите название сервера"
            color: "white"
            placeholderTextColor: "#949ba4"
            maximumLength: 32

            background: Rectangle {
                color: "#1e1f22"
                radius: 4
                border.color: serverNameInput.activeFocus ? "#5865f2" : "transparent"
                border.width: 1
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: 12

            Button {
                text: "Отмена"
                onClicked: dialogRoot.close()

                contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 13 }
                background: Rectangle { implicitWidth: 80; implicitHeight: 32; color: "#4e5058"; radius: 4 }
                HoverHandler { cursorShape: Qt.PointingHandCursor }
            }

            Button {
                text: "Создать"
                enabled: serverNameInput.text.trim().length > 0

                onClicked: {
                    console.log("UI | Запрос на создание сервера:", serverNameInput.text)

                    // Вызываем метод из QChatManager (который дернет сетевую команду 14 CreateServer)
                    ClientConnection.requestCreateServer(serverNameInput.text)

                    dialogRoot.close()
                }

                contentItem: Text { text: parent.text; color: "white"; font.bold: true; font.pixelSize: 13 }
                background: Rectangle {
                    implicitWidth: 80;
                    implicitHeight: 32;
                    color: parent.enabled ? "#5865f2" : "#3c4270"
                    radius: 4
                }
                HoverHandler { cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor }
            }
        }
    }
}
