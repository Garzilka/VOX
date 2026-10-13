import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle
{
    id: requestsWidgetRoot
    color: "#313338"

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        // ==========================================
        // ДУБЛИРУЮЩИЙ ХЕДЕР ДЛЯ СОХРАНЕНИЯ НАВИГАЦИИ
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

                Text
                {
                    text: "Все контакты";
                    color: FriendsManager.State === 0 ? "#ffffff" : "#b5bac1";
                    font.bold: true;
                    font.pixelSize: 14

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            FriendsManager.setState(0);
                            ClientConnection.requestDirectChats();
                        }
                    }
                }

                Text
                {
                    text: "Ожидание (" + requestsListView.count + ")";
                    color: FriendsManager.State === 3 ? "#ffffff" : "#b5bac1";
                    font.bold: true;
                    font.pixelSize: 14

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: FriendsManager.setState(3);
                    }
                }

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
        // КОНТЕНТНЫЙ БЛОК ЗАЯВОК
        // ==========================================
        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout
            {
                anchors.fill: parent
                anchors.margins: 30
                spacing: 20

                Text {
                    text: "ЗАЯВКИ В ДРУЗЬЯ"
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 16
                }

                Item
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // Заглушка
                    Rectangle
                    {
                        anchors.fill: parent
                        color: "transparent"
                        border.color: "#2b2d31"
                        border.width: 1
                        radius: 8
                        visible: requestsListView.count === 0

                        ColumnLayout
                        {
                            anchors.centerIn: parent
                            spacing: 12

                            Text {
                                text: "⌛ У вас нет ожидающих заявок"
                                color: "#949ba4"
                                font.pixelSize: 15
                                Layout.alignment: Qt.AlignHCenter
                            }
                        }
                    }

                    // Лента заявок
                    ListView
                    {
                        id: requestsListView
                        anchors.fill: parent
                        model: FriendsManager.pendingRequests
                        spacing: 8
                        clip: true
                        visible: count > 0

                        delegate: Rectangle
                        {
                            width: requestsListView.width
                            height: 60
                            color: itemMouseArea.containsMouse ? "#35373c" : "#2b2d31"
                            radius: 8

                            MouseArea {
                                id: itemMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                acceptedButtons: Qt.NoButton
                            }

                            RowLayout
                            {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12
                                spacing: 12

                                // Аватар
                                Rectangle
                                {
                                    width: 38
                                    height: 38
                                    radius: 19
                                    color: "#4f545c"

                                    Text {
                                        anchors.centerIn: parent
                                        text: (modelData && typeof modelData["displayName"] === "string" && modelData["displayName"].length > 0)
                                              ? modelData["displayName"].substring(0, 1).toUpperCase()
                                              : "?"
                                        color: "#ffffff"
                                        font.bold: true
                                        font.pixelSize: 16
                                    }
                                }

                                // Текстовый блок профиля
                                ColumnLayout
                                {
                                    spacing: 2
                                    Layout.fillWidth: true

                                    Text {
                                        text: modelData["displayName"] ? modelData["displayName"] : ""
                                        color: "#ffffff"
                                        font.bold: true
                                        font.pixelSize: 14
                                    }

                                    Text {
                                        text: modelData["relationStatus"] === 1 ? "Входящая заявка в друзья" : "Исходящая заявка в друзья"
                                        color: "#b5bac1"
                                        font.pixelSize: 12
                                    }
                                }

                                // Ряды интерактивных кнопок
                                RowLayout
                                {
                                    spacing: 8

                                    // Кнопка "Принять"
                                    Button
                                    {
                                        id: acceptBtn
                                        visible: modelData["relationStatus"] === 1
                                        text: "✔"

                                        contentItem: Text {
                                            text: acceptBtn.text
                                            color: "#ffffff"
                                            font.bold: true
                                            font.pixelSize: 14
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        background: Rectangle {
                                            implicitWidth: 36
                                            implicitHeight: 36
                                            color: acceptBtn.hovered ? "#1f6638" : "#248046"
                                            radius: 18
                                        }

                                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                                        onClicked: FriendsManager.handleFriendAction(modelData["guid"], 0);
                                    }

                                    // Кнопка "Отклонить"
                                    Button
                                    {
                                        id: declineBtn
                                        text: "✖"

                                        contentItem: Text
                                        {
                                            text: declineBtn.text
                                            color: "#ffffff"
                                            font.bold: true
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        background: Rectangle
                                        {
                                            implicitWidth: 36
                                            implicitHeight: 36
                                            color: declineBtn.hovered ? "#952b2a" : "#da373c"
                                            radius: 18
                                        }
                                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                                        onClicked: FriendsManager.handleFriendAction(modelData["guid"], 1);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
