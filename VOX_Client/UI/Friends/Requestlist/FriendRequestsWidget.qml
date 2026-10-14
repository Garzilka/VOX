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
        // НАВИГАЦИОННЫЙ ХЕДЕР ЭКРАНА
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

                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            FriendsManager.setState(0);
                            ClientConnection.requestDirectChats();
                        }
                    }
                }

                Text
                {
                    text: "Ожидание (" + FriendsManager.pendingRequests.length + ")";
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

                    contentItem: Text
                    {
                        text: addFriendBtn.text
                        color: FriendsManager.State === 2 ? "#23a55a" : "#ffffff"
                        font.bold: true
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle
                    {
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
        // ОСНОВНОЙ КОНТЕНТНЫЙ БЛОК
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

                Text
                {
                    text: "ЗАЯВКИ В ДРУЗЬЯ"
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 16
                }

                RequestsListWidget
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
