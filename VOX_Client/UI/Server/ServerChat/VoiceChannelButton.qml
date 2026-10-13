import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Column {
    id: control
    width: parent.width
    spacing: 2

    property string channelGUID: ""
    property string channelName: ""

    readonly property bool isSelected: VoiceEngine.currentRoomGUID === channelGUID

    // Кнопка самого голосового канала
    ItemDelegate {
        id: channelItem
        width: parent.width
        height: 34

        background: Rectangle {
            color: control.isSelected ? "#404249" : (channelItem.hovered ? "#35373c" : "transparent")
            radius: 4
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
        }

        contentItem: RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 6

            Text {
                text: "🔊"
                color: control.isSelected ? "#ffffff" : (channelItem.hovered ? "#dbdee1" : "#949ba4")
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                text: control.channelName ? control.channelName : "без названия"
                color: control.isSelected ? "#ffffff" : (channelItem.hovered ? "#dbdee1" : "#949ba4")
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 13
                font.bold: control.isSelected
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        onClicked: {
            console.log("UI | Запрос на подключение к голосу:", control.channelName)
            ServerManager.joinVoiceChannelFromUI(ServerManager.activeServerGUID, control.channelGUID)
        }
    }

    // Вложенный список участников внутри этого голосового канала
    Column {
        id: voiceUsersContainer
        width: parent.width
        visible: voiceUsersRepeater.count > 0
        leftPadding: 32
        spacing: 4

        Binding {
            target: voiceUsersRepeater
            property: "model"
            value: ServerManager.getVoiceChannelUsers(control.channelGUID)
        }

        Connections {
            target: ServerManager
            function onVoiceUsersChanged(channelGUID) {
                if (control.channelGUID === channelGUID) {
                    voiceUsersRepeater.model = [];
                    voiceUsersRepeater.model = ServerManager.getVoiceChannelUsers(channelGUID);
                }
            }
        }

        Repeater {
            id: voiceUsersRepeater
            model: ServerManager.getVoiceChannelUsers(control.channelGUID)

            delegate: Item {
                width: parent.width - 32
                height: 24
                readonly property string userName: UserManager.getDisplayName(modelData)

                RowLayout {
                    anchors.fill: parent
                    spacing: 8

                    Rectangle {
                        width: 18; height: 18; radius: 9; color: "#5865f2"
                        Layout.alignment: Qt.AlignVCenter

                        Text {
                            text: userName ? userName.charAt(0).toUpperCase() : "?"
                            color: "white"; font.bold: true; font.pixelSize: 10
                            anchors.centerIn: parent
                        }
                    }

                    Text {
                        text: userName
                        color: "#b5bac1"; font.pixelSize: 12
                        verticalAlignment: Text.AlignVCenter
                        Layout.fillWidth: true; elide: Text.ElideRight
                    }
                }
            }
        }
    }
}
