import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import VOX_Client

Rectangle
{
    id: friendsPannelRoot
    color: "#2b2d31"
    implicitWidth: 240
    implicitHeight: parent ? parent.height : 500

    Component.onCompleted:
    {
            console.log("UI | Панель личных чатов создана. Запрос актуализации комнат...");
            ClientConnection.requestDirectChats();
        }
    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        // Поиск беседы
        Rectangle
        {
            Layout.fillWidth: true; Layout.preferredHeight: 48; color: "#2b2d31"
            Rectangle
            {
                anchors.fill: parent; anchors.margins: 8; color: "#1e1f22"; radius: 4
                RowLayout
                {
                    anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8
                    TextField
                    {
                        id: searchField
                        Layout.fillWidth: true; placeholderText: "Найти беседу..."
                        placeholderTextColor: "#949ba4"; color: "#dbdee1"; font.pixelSize: 13
                        background: Item {}
                    }
                    Text { text: "🔍"; color: "#949ba4"; font.pixelSize: 12 }
                }
            }
        }

        // Кнопка "Друзья"
        ItemDelegate
        {
            id: friendsButton
            Layout.fillWidth: true; Layout.preferredHeight: 40
            highlighted: FriendsManager.State === 0
            background: Rectangle {
                color: friendsButton.highlighted ? "#35373c" : (friendsButton.hovered ? "#35373c" : "transparent")
                radius: 4; anchors.fill: parent; anchors.margins: 4
            }
            contentItem: RowLayout {
                spacing: 12
                Text { text: "🧑‍🤝‍🧑"; font.pixelSize: 16 }
                Text { text: "Друзья"; color: "#ffffff"; font.bold: true; font.pixelSize: 13 }
            }
            onClicked: {
                FriendsManager.State = 0;
                ChatManager.activeRoomGUID = "";
            }
        }

        // Кнопка "Заявки"
        ItemDelegate
        {
            id: requestsTabButton
            Layout.fillWidth: true; Layout.preferredHeight: 40
            highlighted: FriendsManager.State === 3
            background: Rectangle {
                color: requestsTabButton.highlighted ? "#35373c" : (requestsTabButton.hovered ? "#35373c" : "transparent")
                radius: 4; anchors.fill: parent; anchors.margins: 4
            }
            contentItem: RowLayout {
                spacing: 12
                Text { text: "🔔"; font.pixelSize: 16 }
                Text { text: "Заявки"; color: "#ffffff"; font.bold: true; font.pixelSize: 13 }
            }
            onClicked: {
                FriendsManager.State = 3;
                ChatManager.activeRoomGUID = "";
            }
        }

        Text
        {
            text: "ЛИЧНЫЕ СООБЩЕНИЯ"
            color: "#949ba4"; font.pixelSize: 11; font.bold: true
            Layout.leftMargin: 12; Layout.topMargin: 16; Layout.bottomMargin: 8
        }

        // ВСТАВКА ВЫДЕЛЕННОГО ВИДЖЕТА СПИСКА ЛС
        DirectChatsList
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
