import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle
{
    id: friendsListViewRoot
    color: "#313338"

    property string selectedFriendName: ""
    property string selectedFriendGUID: ""

    Menu
    {
        id: globalContextMenu
        background: Rectangle { implicitWidth: 180; color: "#111214"; border.color: "#1f2023"; border.width: 1; radius: 4 }

        delegate: MenuItem
        {
            id: menuItem
            leftPadding: 12; rightPadding: 12; topPadding: 8; bottomPadding: 8
            contentItem: Text { text: menuItem.text; color: menuItem.hovered ? "#ffffff" : "#b5bac1"; font.pixelSize: 13 }
            background: Rectangle { color: menuItem.hovered ? "#4752c4" : "transparent"; radius: 4; anchors.fill: parent; anchors.margins: 4 }
        }

        Action
        {
            text: "Написать сообщение";
            onTriggered: {
                ChatManager.openOrCreateChatFromUI(0, friendsListViewRoot.selectedFriendGUID, "", "");
            }
        }

        Menu
        {
            id: inviteSubMenu
            title: "Пригласить на сервер"
            visible: ServerManager.serverList.length > 0
            background: Rectangle { implicitWidth: 180; color: "#111214"; border.color: "#1f2023"; border.width: 1; radius: 4 }

            Instantiator
            {
                model: ServerManager.serverList
                onObjectAdded: (index, object) => inviteSubMenu.insertItem(index, object)
                onObjectRemoved: (index, object) => inviteSubMenu.removeItem(object)

                delegate: MenuItem
                {
                    text: (model && model.modelData && model.modelData.name) ? model.modelData.name : "Без названия"
                    leftPadding: 12; rightPadding: 12; topPadding: 8; bottomPadding: 8
                    contentItem: Text { text: parent.text; color: parent.hovered ? "#ffffff" : "#b5bac1"; font.pixelSize: 13 }
                    background: Rectangle { color: hovered ? "#4752c4" : "transparent"; radius: 4; anchors.fill: parent; anchors.margins: 4 }

                    onTriggered: {
                        console.log("UI | Инвайт на сервер:", model.modelData.name, "для друга GUID:", friendsListViewRoot.selectedFriendGUID);
                        ClientConnection.requestGenerateInvite(model.modelData.serverGUID, friendsListViewRoot.selectedFriendGUID);
                    }
                }
            }
        }

        MenuSeparator { contentItem: Rectangle { implicitHeight: 1; color: "#1f2023"; anchors.left: parent ? parent.left : undefined; anchors.right: parent ? parent.right : undefined; anchors.leftMargin: 8; anchors.rightMargin: 8 } }
        Action { text: "Удалить из друзей"; onTriggered: { console.log("Удаление: " + friendsListViewRoot.selectedFriendName); } }
        Action { text: "Заблокировать"; onTriggered: { console.log("Блокировка: " + friendsListViewRoot.selectedFriendName); } }
    }

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        // ==========================================
        // ХЕДЕР СПИСКА ДРУЗЕЙ (Синхронизирован с C++)
        // ==========================================
        Rectangle
        {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: "#313338"
            border.color: "#1f2023"
            border.width: 1

            RowLayout
            {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 16

                Text
                {
                    text: "🧑‍🤝‍🧑 Друзья";
                    color: "#ffffff";
                    font.bold: true;
                    font.pixelSize: 16
                }

                Rectangle { width: 1; height: 16; color: "#3f4147" }

                // Вкладка "Все контакты" (State === 0)
                Text
                {
                    text: "Все контакты (" + friendsMainView.count + ")";
                    color: FriendsManager.State === 0 ? "#ffffff" : "#b5bac1";
                    font.bold: true;
                    font.pixelSize: 14

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            FriendsManager.setState(0);
                            ClientConnection.requestDirectChats(); // Запрос актуализации списков
                        }
                    }
                }

                // Вкладка "Ожидание" (Переключает на State === 3)
                Text
                {
                    text: "Ожидание";
                    color: FriendsManager.State === 3 ? "#ffffff" : "#b5bac1";
                    font.bold: true;
                    font.pixelSize: 14

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            FriendsManager.setState(3);
                        }
                    }
                }

                // Кнопка "Добавить в друзья" (Переключает на State === 2)
                Button
                {
                    id: addFriendBtn
                    text: "Добавить в друзья"

                    contentItem: Text {
                        text: addFriendBtn.text
                        color: FriendsManager.State === 2 ? "#23a55a" : "#ffffff"
                        font.bold: true
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        implicitWidth: 140
                        implicitHeight: 28
                        color: FriendsManager.State === 2 ? "transparent" : "#248046"
                        radius: 4
                    }

                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                    onClicked: FriendsManager.setState(2);
                }

                Item { Layout.fillWidth: true }
            }
        }

        // ==========================================
        // КОНТЕНТ: Лента подтвержденных контактов
        // ==========================================
        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                text: "У вас пока нет друзей. Нажмите кнопку Добавить в друзья!"
                color: "#949ba4"
                font.pixelSize: 15
                anchors.centerIn: parent
                visible: friendsMainView.count === 0
            }

            ListView
            {
                id: friendsMainView
                anchors.fill: parent
                anchors.margins: 20
                model: FriendsManager.friendsList
                spacing: 12
                clip: true
                visible: count > 0

                delegate: FriendListItem
                {
                    width: friendsMainView.width
                    _avatarColor: modelData["avatarUrl"] ? modelData["avatarUrl"] : "#5865f2"
                    _statusColor: modelData["presenceStatus"] === 1 ? "#23a55a" : "#80848e"
                    _name: modelData["displayName"] ? modelData["displayName"] : ""
                    _status: modelData["presenceStatus"] === 1 ? "В сети" : "Не в сети"
                    _currentActivity: ""

                    onRightClicked: (friendName, friendGUID, sourceItem, mx, my) => {
                        friendsListViewRoot.selectedFriendName = friendName;
                        friendsListViewRoot.selectedFriendGUID = friendGUID;
                        var mappedPos = sourceItem.mapToItem(friendsListViewRoot, mx, my);
                        globalContextMenu.popup(mappedPos.x, mappedPos.y);
                    }
                }
            }
        }
    }
}
