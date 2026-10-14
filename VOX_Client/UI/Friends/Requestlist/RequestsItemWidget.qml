import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle
{
    id: requestsItemRoot
    width: ListView.view ? ListView.view.width : 300
    height: 60
    color: requestsItemMouseArea.containsMouse ? "#35373c" : "#2b2d31"
    radius: 8

    property string userGuid: ""
    property bool isIncoming: true

    property string uDisplayName: UserManager.getDisplayName(userGuid)
    property string uStatusColor: UserManager.getStatusColor(userGuid)

    readonly property bool isLoading: requestsItemRoot.uDisplayName === "Загрузка..." || requestsItemRoot.uDisplayName === ""

    MouseArea {
        id: requestsItemMouseArea
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

        // Аватар пользователя с анимацией пульсации во время ожидания батча
        Rectangle
        {
            id: avatarCircle
            width: 38
            height: 38
            radius: 19
            color: "#4f545c"

            SequentialAnimation on opacity
            {
                running: requestsItemRoot.isLoading
                loops: Animation.Infinite
                NumberAnimation { to: 0.4; duration: 600; easing.type: Easing.InOutQuad }
                NumberAnimation { to: 1.0; duration: 600; easing.type: Easing.InOutQuad }
            }

            Text
            {
                anchors.centerIn: parent
                text: (!requestsItemRoot.isLoading && requestsItemRoot.uDisplayName.length > 0)
                      ? requestsItemRoot.uDisplayName.substring(0, 1).toUpperCase()
                      : "?"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 16
            }

            // Точка статуса присутствия
            Rectangle
            {
                width: 10
                height: 10
                radius: 5
                color: requestsItemRoot.uStatusColor
                border.color: requestsItemRoot.color
                border.width: 1.5
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                visible: !requestsItemRoot.isLoading
            }
        }

        // Текстовый блок (Имя и описание типа заявки)
        ColumnLayout
        {
            spacing: 2
            Layout.fillWidth: true

            Text
            {
                text: requestsItemRoot.isLoading ? "Загрузка..." : requestsItemRoot.uDisplayName
                color: requestsItemRoot.isLoading ? "#4e5058" : "#ffffff"
                font.bold: true
                font.pixelSize: 14
            }
            Text
            {
                text: requestsItemRoot.isLoading ? "" : (requestsItemRoot.isIncoming ? "Входящая заявка в друзья" : "Исходящая заявка в друзья")
                color: "#b5bac1"
                font.pixelSize: 12
            }
        }

        // Ряды интерактивных кнопок действий
        RowLayout
        {
            spacing: 8
            visible: !requestsItemRoot.isLoading // Скрываем кнопки, пока профиль не подгружен

            // Кнопка "Принять" (Видна только для входящих)
            Button
            {
                id: acceptBtn
                visible: requestsItemRoot.isIncoming
                text: "✔"

                contentItem: Text
                {
                    text: acceptBtn.text
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle
                {
                    implicitWidth: 36
                    implicitHeight: 36
                    color: acceptBtn.enabled ? (acceptBtn.hovered ? "#1f6638" : "#248046") : "#1f2023"
                    radius: 18
                }

                HoverHandler { cursorShape: acceptBtn.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor }

                onClicked: {
                    FriendsManager.handleFriendAction(requestsItemRoot.userGuid, 0);
                    acceptBtn.enabled = false;
                    declineBtn.enabled = false;
                }
            }

            // Кнопка "Отклонить / Отменить"
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
                    color: declineBtn.enabled ? (declineBtn.hovered ? "#952b2a" : "#da373c") : "#1f2023"
                    radius: 18
                }

                HoverHandler { cursorShape: declineBtn.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor }

                onClicked:
                {
                    FriendsManager.handleFriendAction(requestsItemRoot.userGuid, 1);
                    acceptBtn.enabled = false;
                    declineBtn.enabled = false;
                }
            }
        }
    }

    Connections
    {
        target: UserManager

        function onDataChanged()
        {
            requestsItemRoot.uDisplayName = UserManager.getDisplayName(requestsItemRoot.userGuid)
            requestsItemRoot.uStatusColor = UserManager.getStatusColor(requestsItemRoot.userGuid)

            if (!requestsItemRoot.isLoading)
            {
                avatarCircle.opacity = 1.0
            }
        }
    }
}
