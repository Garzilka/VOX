import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import VOX_Client

ItemDelegate
{
    id: dmRowRoot

    property string chatGUID: ""
    property int chatType: 0
    property string chatName: ""
    property string lastMessageDescription: "" // Сюда прилетает Description из SUnifiedChatData

    // Получаем список участников напрямую из C++ хэш-таблицы
    readonly property var participantGUIDs: chatGUID !== "" ? ChatManager.getParticipantGUIDs(chatGUID) : []

    width: parent ? parent.width : 240
    height: 44 // Немного увеличили высоту, чтобы двухстрочный текст (Имя + Сообщение) сидел свободно
    highlighted: ChatManager.activeRoomGUID === chatGUID

    // Безопасный поиск GUID собеседника (исключая свой собственный UUID)
    readonly property string targetUserGUID: {
        if (!participantGUIDs || participantGUIDs.length === 0) return "";

        var myUUID = ClientConnection.getUUID();
        for (var i = 0; i < participantGUIDs.length; i++) {
            var currentGuid = participantGUIDs[i];
            if (currentGuid && currentGuid !== myUUID) {
                return currentGuid;
            }
        }
        return participantGUIDs[0] ? participantGUIDs[0] : "";
    }

    property string dmName: {
        if (chatType === 1) return chatName ? chatName : "Групповой чат";
        if (targetUserGUID === "") return "Загрузка...";
        return UserManager.getDisplayName(targetUserGUID);
    }

    property color dmStatusColor: targetUserGUID !== "" ? UserManager.getStatusColor(targetUserGUID) : "#80848e"
    readonly property bool isLoading: targetUserGUID === "" || dmName === "Загрузка..." || dmName === ""

    background: Rectangle {
        color: dmRowRoot.highlighted ? "#404249" : (dmRowRoot.hovered ? "#35373c" : "transparent")
        radius: 4
        anchors.fill: parent
        anchors.margins: 4
    }

    contentItem: RowLayout
    {
        spacing: 10
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8

        Rectangle
        {
            id: avatarCircle
            width: 32; height: 32; radius: 16; color: "#5865f2"
            Layout.alignment: Qt.AlignVCenter // Центрируем аватарку по вертикали

            SequentialAnimation on opacity {
                running: dmRowRoot.isLoading
                loops: Animation.Infinite
                NumberAnimation { to: 0.4; duration: 600; easing.type: Easing.InOutQuad }
                NumberAnimation { to: 1.0; duration: 600; easing.type: Easing.InOutQuad }
            }

            Rectangle {
                width: 10; height: 10; radius: 5
                color: dmRowRoot.dmStatusColor
                border.color: "#2b2d31"; border.width: 1.5
                anchors.right: parent.right; anchors.bottom: parent.bottom
                visible: dmRowRoot.chatType === 0 && !dmRowRoot.isLoading
            }
            Text {
                text: (!dmRowRoot.isLoading && dmRowRoot.dmName.length > 0) ? dmRowRoot.dmName.charAt(0).toUpperCase() : "?"
                color: "white"; font.bold: true; font.pixelSize: 12; anchors.centerIn: parent
            }
        }

        ColumnLayout {
            spacing: 2 // Отступ между Именем и Текстом сообщения
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            // Отображение имени пользователя или названия группы
            Text {
                text: dmRowRoot.dmName
                color: dmRowRoot.isLoading ? "#4e5058" : (dmRowRoot.hovered || dmRowRoot.highlighted ? "#dbdee1" : "#949ba4")
                font.pixelSize: 13; font.bold: dmRowRoot.highlighted
                Layout.fillWidth: true; elide: Text.ElideRight
            }

            // ИСПРАВЛЕНО: Отображение превью последнего сообщения (Description)
            Text {
                text: dmRowRoot.lastMessageDescription
                color: "#949ba4" // Приглушенный серый цвет текста для превью
                font.pixelSize: 11
                Layout.fillWidth: true; elide: Text.ElideRight
                // Показываем блок только если сообщение не пустое и профиль загружен
                visible: dmRowRoot.lastMessageDescription !== "" && !dmRowRoot.isLoading
            }
        }
    }

    Connections
    {
        target: UserManager
        function onDataChanged() {
            if (dmRowRoot.targetUserGUID !== "") {
                dmRowRoot.dmName = dmRowRoot.chatType === 1 ? (chatName ? chatName : "Групповой чат") : UserManager.getDisplayName(dmRowRoot.targetUserGUID)
                dmRowRoot.dmStatusColor = UserManager.getStatusColor(dmRowRoot.targetUserGUID)
                if (!dmRowRoot.isLoading) {
                    avatarCircle.opacity = 1.0
                }
            }
        }
    }

    onClicked: {
        ChatManager.activeRoomGUID = dmRowRoot.chatGUID;
        FriendsManager.setState(1);
    }
}
