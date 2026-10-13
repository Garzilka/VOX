import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import VOX_Client

ListView
{
    id: directMessagesList
    clip: true
    spacing: 2

    ListModel { id: realDirectChatsModel }
    model: realDirectChatsModel

    function refreshChats() {
        var chatsList = ChatManager.getDirectChatsList();

        if (realDirectChatsModel.count === chatsList.length) {
            for (var i = 0; i < chatsList.length; i++) {
                realDirectChatsModel.setProperty(i, "description", chatsList[i].description);
                // Больше не пишем participantGUIDs в модель, предотвращая ошибку типов!
            }
        } else {
            realDirectChatsModel.clear();
            for (var j = 0; j < chatsList.length; j++) {
                realDirectChatsModel.append({
                    "chatGUID": chatsList[j].chatGUID,
                    "chatType": chatsList[j].chatType,
                    "name": chatsList[j].name,
                    "description": chatsList[j].description
                });
            }
        }
    }

    Connections
    {
        target: ChatManager
        function onDirectChatsChanged() { directMessagesList.refreshChats(); }
    }

    Connections
    {
        target: ClientConnection
        function onDirectChatsLoaded(chatsList) { directMessagesList.refreshChats(); }
    }

    delegate: DirectChatRow
    {
        width: directMessagesList.width
        chatGUID: model.chatGUID
        chatType: model.chatType
        chatName: model.name ? model.name : ""
        lastMessageDescription: model.description ? model.description : ""
        // Свойство participantGUIDs внутри делегата теперь само заберет данные из C++
    }
}
