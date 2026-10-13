import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import VOX_Client

Item
{
    ListModel
    {
        id: chatMessageData
    }

    Connections
    {
        target: ChatManager

        // Сигнал срабатывает, когда QConnection распарсил историю из сети
        function onHistoryLoaded(messages)
        {
            chatMessageData.clear(); // Очищаем старый чат

            for (var i = 0; i < messages.length; i++)
            {
                chatMessageData.append(
                {
                    "sender": messages[i].sender,
                    "messageText": messages[i].messageText,
                    "time": messages[i].time,
                    "avatarColor": messages[i].avatarColor,
                    "msgType": messages[i].msgType ? messages[i].msgType : 0
                });
            }
            console.log("QML | Модель чата успешно наполнена. Сообщений: " + chatMessageData.count);
        }
    }
    StackLayout
    {
        anchors.fill: parent
        currentIndex: FriendsManager.State

        FriendList
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle
        {
            color: "#313338"

            ChatSpace
            {
                anchors.fill: parent
            }
        }

        FriendSearchWidget
        {
            color: "#313338"
            id: searchWidget
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        FriendRequestsWidget
        {
            color: "#313338"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
