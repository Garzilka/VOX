import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import VOX_Client

Rectangle
{
    id: membersWidgetRoot
    color: "#2b2d31"

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Text {
            text: "УЧАСТНИКИ — " + participantsListView.count
            color: "#949ba4"; font.pixelSize: 11; font.bold: true
            Layout.bottomMargin: 8
        }

        ListView
        {
            id: participantsListView
            Layout.fillWidth: true; Layout.fillHeight: true
            model: ServerManager.currentParticipants // Массив UUID-строк от сервера
            spacing: 6
            clip: true

            delegate: ItemDelegate
            {
                id: participantDelegateItem
                width: participantsListView.width;
                height: 34

                property string uName: UserManager.getDisplayName(modelData)
                property color uStatusColor: UserManager.getStatusColor(modelData)

                readonly property bool isLoading: participantDelegateItem.uName === "Загрузка..." || participantDelegateItem.uName === ""

                background: Rectangle
                {
                    color: participantDelegateItem.hovered ? "#35373c" : "transparent"; radius: 4
                }

                contentItem: RowLayout
                {
                    anchors.fill: parent; anchors.leftMargin: 8; spacing: 10

                    // Аватарка со статусом
                    Rectangle
                    {
                        id: avatarCircle
                        width: 24; height: 24; radius: 12; color: "#5865f2"

                        // Анимация мягкой пульсации аватара во время ожидания батча сети
                        SequentialAnimation on opacity
                        {
                            running: participantDelegateItem.isLoading
                            loops: Animation.Infinite
                            NumberAnimation { to: 0.4; duration: 600; easing.type: Easing.InOutQuad }
                            NumberAnimation { to: 1.0; duration: 600; easing.type: Easing.InOutQuad }
                        }

                        Rectangle
                        {
                            width: 8; height: 8; radius: 4
                            color: participantDelegateItem.uStatusColor
                            border.color: "#2b2d31"; border.width: 1
                            anchors.right: parent.right; anchors.bottom: parent.bottom
                            visible: !participantDelegateItem.isLoading // Прячем точку, пока статус не загружен
                        }
                    }

                    // Никнейм / Скелетон-текст
                    Item
                    {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 16

                        Text {
                            text: participantDelegateItem.isLoading ? "Загрузка..." : participantDelegateItem.uName
                            color: participantDelegateItem.isLoading ? "#4e5058" : "#dbdee1"
                            font.pixelSize: 13; font.bold: true
                            anchors.verticalCenter: parent.verticalCenter
                            elide: Text.ElideRight
                            width: parent.width
                        }
                    }
                }

                // =============================================================
                // Триггер обновления профилей участников
                // =============================================================
                Connections
                {
                    target: UserManager // Слушаем твой глобальный синглтон менеджера пользователей

                    function onDataChanged()
                    {
                        // Как только батч прилетел по сети — принудительно пересчитываем C++ методы для этого UUID
                        participantDelegateItem.uName = UserManager.getDisplayName(modelData)
                        participantDelegateItem.uStatusColor = UserManager.getStatusColor(modelData)

                        // Сбрасываем прозрачность аватарки в исходное монолитное состояние 1.0
                        if (!participantDelegateItem.isLoading)
                        {
                            avatarCircle.opacity = 1.0
                        }
                    }
                }
            }
        }
    }
}
