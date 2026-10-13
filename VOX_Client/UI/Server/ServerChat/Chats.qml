import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import VOX_Client

Item {
    id: channelsLayoutRoot
    anchors.fill: parent

    // Свойства для хранения состояния свернутости категорий
    property bool textChannelsCollapsed: false
    property bool voiceChannelsCollapsed: false

    // Внутренние свойства для управления созданием канала
    property int targetChatType: 2 // 2 - Текст, 5 - Голос

    ScrollView {
        anchors.fill: parent
        anchors.topMargin: 8
        clip: true
        contentWidth: availableWidth

        Column {
            width: parent.width
            spacing: 12

            // ==========================================
            // КАТЕГОРИЯ: ТЕКСТОВЫЕ КАНАЛЫ
            // ==========================================
            Column {
                width: parent.width
                spacing: 2

                ChannelCategoryHeader
                {
                    title: "Текстовые каналы"
                    isCollapsed: channelsLayoutRoot.textChannelsCollapsed

                    onToggleCollapsed: {
                        channelsLayoutRoot.textChannelsCollapsed = !channelsLayoutRoot.textChannelsCollapsed
                    }

                    onAddChannelClicked: {
                        // Вызываем С++ метод: открыть диалог с дефолтным типом Текст (2)
                        UIStateManager.openCreateChannelDialog(2)
                    }
                }

                Column {
                    width: parent.width
                    spacing: 2
                    visible: !channelsLayoutRoot.textChannelsCollapsed

                    Repeater {
                        model: ServerManager.currentChannels

                        delegate: Loader {
                            width: parent.width
                            active: modelData.chatType !== 5
                            visible: active
                            sourceComponent: TextChannelButton {
                                channelGUID: modelData.chatGUID
                                channelName: modelData.name
                            }
                        }
                    }
                }
            }

            // ==========================================
            // КАТЕГОРИЯ: ГОЛОСОВЫЕ КАНАЛЫ
            // ==========================================
            Column {
                width: parent.width
                spacing: 2

                ChannelCategoryHeader {
                    title: "Голосовые каналы"
                    isCollapsed: channelsLayoutRoot.voiceChannelsCollapsed

                    onToggleCollapsed: {
                        channelsLayoutRoot.voiceChannelsCollapsed = !channelsLayoutRoot.voiceChannelsCollapsed
                    }

                    onAddChannelClicked: {
                        // Вызываем С++ метод: открыть диалог с дефолтным типом Голос (5)
                        UIStateManager.openCreateChannelDialog(5)
                    }
                }

                Column {
                    width: parent.width
                    spacing: 2
                    visible: !channelsLayoutRoot.voiceChannelsCollapsed

                    Repeater {
                        model: ServerManager.currentChannels

                        delegate: Loader {
                            width: parent.width
                            active: modelData.chatType === 5
                            visible: active
                            sourceComponent: VoiceChannelButton {
                                channelGUID: modelData.chatGUID
                                channelName: modelData.name
                            }
                        }
                    }
                }
            }
        }
    }

    // Заглушка, если каналов на сервере совсем нет
    Text {
        text: "На этом сервере нет каналов"
        color: "#949ba4"
        font.pixelSize: 13
        anchors.centerIn: parent
        visible: ServerManager.currentChannels.length === 0
    }
}
