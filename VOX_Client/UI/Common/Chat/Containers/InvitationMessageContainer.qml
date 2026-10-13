import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import VOX_Client

Rectangle
{
    id: root

    // Входной параметр: теперь сюда прокидывается чистый UUID сервера из Attachments
    property string targetServerGUID: ""
    property string inviteToken: "" // Токен для вступления (передаем через metaData вложения)

    // Лениво вычисляем имя сервера через C++ менеджер
    property string serverName: ServerManager.getServerName(root.targetServerGUID)

    // Проверяем состояние загрузки имени
    readonly property bool isLoading: root.serverName === "Загрузка..." || root.serverName === ""

    // Проверяем, состоит ли пользователь уже на этом сервере
    readonly property bool isAlreadyMember: {
        if (!root.targetServerGUID) return false;
        return ServerManager.isUserMemberOfServer(root.targetServerGUID);
    }

    // Внутренний флаг ожидания сети
    property bool isWaitingResponse: false

    Layout.maximumWidth: 450
    height: 72
    color: "#2b2d31"
    radius: 8
    border.color: "#1f2023"
    border.width: 1

    // Функция обновления текстовых полей
    function refreshInviteData() {
        if (!root.targetServerGUID) return;
        root.serverName = ServerManager.getServerName(root.targetServerGUID)
    }

    // Слушаем сигналы обновлений из C++
    Connections
    {
        target: ServerManager
        function onServerRegistryUpdated()
        {
            root.refreshInviteData()
        }
        function onServerListChanged()
        {
            root.refreshInviteData()
        }
    }

    Connections
    {
        target: ClientConnection
        // Когда бэкенд подтвердил успешный коннект по инвайту
        function onInviteAccepted(serverGUID, serverName, generalChatGUID) {
            if (root.targetServerGUID === serverGUID) {
                root.isWaitingResponse = false;
            }
        }
    }

    RowLayout
    {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // Аватарка инвайта (с анимацией пульсации во время ленивой загрузки имени)
        Rectangle
        {
            id: serverAvatar
            width: 48; height: 48; radius: 12; color: "#35363c"
            Text { text: root.isLoading ? "" : "🌐"; font.pixelSize: 22; anchors.centerIn: parent }

            SequentialAnimation on opacity {
                running: root.isLoading
                loops: Animation.Infinite
                NumberAnimation { to: 0.4; duration: 600; easing.type: Easing.InOutQuad }
                NumberAnimation { to: 1.0; duration: 600; easing.type: Easing.InOutQuad }
            }
        }

        ColumnLayout
        {
            Layout.fillWidth: true; spacing: 2
            Text
            {
                text: root.isAlreadyMember ? "ВЫ СОСТОИТЕ В ЭТОМ СООБЩЕСТВЕ" : "ВАС ПРИГЛАШАЮТ ПРИСОЕДИНИТЬСЯ"
                color: root.isAlreadyMember ? "#23a55a" : "#949ba4"
                font.pixelSize: 10; font.bold: true
            }
            Text
            {
                // Показывает "Загрузка..." красивым серым цветом, либо реальное имя сервера
                text: root.serverName
                color: root.isLoading ? "#4e5058" : "#ffffff"
                font.bold: true
                font.pixelSize: 14
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }

        Button
        {
            id: joinButton

            // Активируем кнопку, только если имя загружено, мы не там и токен валиден
            enabled: !root.isLoading && root.inviteToken !== "" && !root.isAlreadyMember && !root.isWaitingResponse

            contentItem: Text {
                text: root.isAlreadyMember ? "Вступил" : (root.isWaitingResponse ? "Ждите..." : "Принять")
                color: root.isAlreadyMember ? "#b5bac1" : "white"
                font.bold: true; font.pixelSize: 13
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                implicitWidth: 80; implicitHeight: 32
                color: root.isAlreadyMember ? "#35373c" :
                       (root.isWaitingResponse ? "#5865f2" :
                       (joinButton.hovered ? "#248046" : "#23a55a"))
                radius: 4
            }

            HoverHandler { cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor }

            onClicked:
            {
                root.isWaitingResponse = true
                ClientConnection.requestAcceptInvite(root.inviteToken)
            }
        }
    }
}
