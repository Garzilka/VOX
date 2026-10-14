import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import VOX_Client

Rectangle
{
    id: searchItemRoot
    width: ListView.view ? ListView.view.width : 300
    height: 60
    color: searchItemMouseArea.containsMouse ? "#35373c" : "#2b2d31"
    radius: 8

    // Входное свойство — только UUID пользователя
    property string userGuid: ""

    // Внутренние реактивные свойства (загружаются из UserManager)
    property string uDisplayName: UserManager.getDisplayName(userGuid)
    property string uStatusColor: UserManager.getStatusColor(userGuid)

    // Признак асинхронной загрузки данных из сети
    readonly property bool isLoading: searchItemRoot.uDisplayName === "Загрузка..." || searchItemRoot.uDisplayName === ""

    MouseArea {
        id: searchItemMouseArea
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

        // Аватар пользователя с анимацией пульсации во время загрузки батча
        Rectangle
        {
            id: avatarCircle
            width: 38
            height: 38
            radius: 19
            color: "#5865f2"

            SequentialAnimation on opacity
            {
                running: searchItemRoot.isLoading
                loops: Animation.Infinite
                NumberAnimation { to: 0.4; duration: 600; easing.type: Easing.InOutQuad }
                NumberAnimation { to: 1.0; duration: 600; easing.type: Easing.InOutQuad }
            }

            Text
            {
                anchors.centerIn: parent
                text: (!searchItemRoot.isLoading && searchItemRoot.uDisplayName.length > 0)
                      ? searchItemRoot.uDisplayName.substring(0, 1).toUpperCase()
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
                color: searchItemRoot.uStatusColor
                border.color: searchItemRoot.color
                border.width: 1.5
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                visible: !searchItemRoot.isLoading
            }
        }

        // Текстовый блок (Имя и юзернейм)
        ColumnLayout
        {
            spacing: 2
            Layout.fillWidth: true

            Text
            {
                text: searchItemRoot.isLoading ? "Загрузка..." : searchItemRoot.uDisplayName
                color: searchItemRoot.isLoading ? "#4e5058" : "#ffffff"
                font.bold: true
                font.pixelSize: 14
            }
            Text
            {
                // Для юзернейма выводим заглушку загрузки или пустой ник, пока батч не прилетел
                text: searchItemRoot.isLoading ? "" : "@id_" + searchItemRoot.userGuid.substring(0,6) // Временный фоллбек, если юзернейм не прокинут отдельно в UserManager
                color: "#b5bac1"
                font.pixelSize: 12
            }
        }

        // Кнопка действия
        Button
        {
            id: addBtn
            text: "Добавить"
            enabled: !searchItemRoot.isLoading

            contentItem: Text
            {
                text: addBtn.text
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle
            {
                implicitWidth: 85
                implicitHeight: 30
                color: addBtn.enabled ? (addBtn.hovered ? "#1f6638" : "#248046") : "#1f2023"
                radius: 4
            }

            HoverHandler
            {
                cursorShape: addBtn.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            }

            onClicked:
            {
                ClientConnection.requestFriend(searchItemRoot.userGuid, "");
                addBtn.enabled = false;
            }
        }
    }

    Connections
    {
        target: UserManager

        function onDataChanged()
        {
            searchItemRoot.uDisplayName = UserManager.getDisplayName(searchItemRoot.userGuid)
            searchItemRoot.uStatusColor = UserManager.getStatusColor(searchItemRoot.userGuid)

            if (!searchItemRoot.isLoading)
            {
                avatarCircle.opacity = 1.0
            }
        }
    }
}
