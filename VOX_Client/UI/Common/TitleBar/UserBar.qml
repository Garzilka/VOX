import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import VOX_Client

Rectangle
{
    id: userStatusBlock
    Layout.fillWidth: true
    // Если подключен голос — плашка выше, чтобы уместить статус созвона
    Layout.preferredHeight: VoiceEngine.currentRoomGUID !== "" ? 70 : 52
    color: "#232428"

    Behavior on Layout.preferredHeight { NumberAnimation { duration: 150 } }

    // Делаем свойства динамическими (убираем readonly)
    property string myGUID: ""
    property string myName: "Загрузка..."
    property color myStatusColor: "#80848e"

    // Функция, которая принудительно обновляет данные текущего пользователя из C++ кэша
    function refreshUserData() {
        // 1. Пытаемся получить актуальный UUID
        var currentUUID = ClientConnection.getUUID()
        if (!currentUUID) return; // Если еще не авторизовались, выходим

        userStatusBlock.myGUID = currentUUID

        // 2. Вытягиваем имя и цвет статуса из менеджера пользователей
        userStatusBlock.myName = UserManager.getDisplayName(currentUUID)
        userStatusBlock.myStatusColor = UserManager.getStatusColor(currentUUID)
    }

    // =====================================================================
    // ТРИГГЕРЫ ОБНОВЛЕНИЯ: Слушаем изменения состояния приложения и кэша
    // =====================================================================

    // 1. Как только компонент загрузился в память — пробуем вычитать данные
    Component.onCompleted: userStatusBlock.refreshUserData()

    // 2. Слушаем транспортное соединение: когда State переключается на 1 (Рабочая область)
    Connections
    {
        target: UIStateManager
        function onWorkStateChange()
        {
            if (UIStateManager.WorkState === 1)
            {
                userStatusBlock.refreshUserData()
            }
        }
    }

    // 3. Слушаем кэш пользователей (точно так же, как это сделано в Members.qml)
    Connections
    {
        target: UserManager
        function onDataChanged()
        {
            userStatusBlock.refreshUserData()
        }
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        // ВЕРХНЯЯ СТРОКА СТАТУСА: Показывается только если мы в голосовом канале
        RowLayout
        {
            Layout.fillWidth: true
            visible: VoiceEngine.currentRoomGUID !== ""
            spacing: 6

            Text { text: "🟢"; font.pixelSize: 10 }
            Text {
                text: "Голосовая связь подключена"
                color: "#23a55a"
                font.pixelSize: 11
                font.bold: true
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Button {
                implicitWidth: 16; implicitHeight: 16
                flat: true
                contentItem: Text { text: "❌"; font.pixelSize: 10; color: "#f23f43"; anchors.centerIn: parent }
                background: Item {}
                HoverHandler { cursorShape: Qt.PointingHandCursor }
                onClicked: ServerManager.leaveVoiceChannelFromUI(ServerManager.activeServerGUID, VoiceEngine.currentRoomGUID)
            }
        }

        // ОСНОВНАЯ СТРОКА: Профиль, микрофон, настройки
        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            // Круглая аватарка со статус-цветом
            Rectangle
            {
                width: 32; height: 32; radius: 16
                color: "#5865f2"

                Text {
                    // Используем первую букву имени, если оно загружено
                    text: (userStatusBlock.myName && userStatusBlock.myName !== "Загрузка...") ? userStatusBlock.myName.charAt(0).toUpperCase() : "?"
                    color: "white"; font.bold: true; font.pixelSize: 13; anchors.centerIn: parent
                }

                // Точка твоего личного онлайна
                Rectangle {
                    width: 10; height: 10; radius: 5
                    color: userStatusBlock.myStatusColor // Привязываемся к динамическому свойству цвета
                    border.color: "#232428"; border.width: 1.5
                    anchors.right: parent.right; anchors.bottom: parent.bottom
                    visible: userStatusBlock.myGUID !== "" // Показываем только когда GUID определен
                }
            }

            // Никнейм
            ColumnLayout {
                spacing: 0
                Layout.fillWidth: true
                Text {
                    text: userStatusBlock.myName
                    color: userStatusBlock.myName === "Загрузка..." ? "#4e5058" : "white"
                    font.bold: true; font.pixelSize: 13
                    Layout.fillWidth: true; elide: Text.ElideRight
                }
                Text {
                    // Показываем хэш-тег, только если GUID уже пришел
                    text: userStatusBlock.myGUID ? ("#" + userStatusBlock.myGUID.substring(0, 4)) : "#0000"
                    color: "#949ba4"; font.pixelSize: 10
                    Layout.fillWidth: true
                }
            }

            // КНОПКА: Микрофон (Мут/Размут)
            Button
            {
                id: micButton
                implicitWidth: 28; implicitHeight: 28
                flat: true
                HoverHandler { cursorShape: Qt.PointingHandCursor }

                background: Rectangle {
                    color: micButton.hovered ? "#35373c" : "transparent"; radius: 4
                }

                // Используем Item как контейнер для абсолютного позиционирования элементов друг над другом
                contentItem: Item
                {
                    implicitWidth: 20
                    implicitHeight: 20

                    // Базовая иконка микрофона
                    Text
                    {
                        text: "🎙️"
                        font.pixelSize: 14
                        anchors.centerIn: parent
                        // Немного приглушаем микрофон, если он выключен (опционально, в стиле Discord)
                        opacity: VoiceEngine.isMuted ? 0.4 : 1.0
                    }

                    // Красная перечеркивающая линия или крестик поверх
                    Text
                    {
                        text: "❌"
                        font.pixelSize: 10 // Делаем крестик чуть меньше микрофона
                        color: "#f23f43"
                        // Сдвигаем в правый нижний угол микрофона для аккуратного наложения
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        anchors.bottomMargin: -2
                        anchors.rightMargin: -2

                        visible: VoiceEngine.isMuted // Показываем только в муте
                    }
                }

                onClicked: VoiceEngine.toggleMicrophone()

                ToolTip.visible: hovered
                ToolTip.text: VoiceEngine.isMuted ? "Включить микрофон" : "Выключить микрофон"
            }

            // КНОПКА: Настройки
            Button
            {
                id: settingsBtn
                implicitWidth: 28; implicitHeight: 28
                flat: true
                HoverHandler { cursorShape: Qt.PointingHandCursor }

                background: Rectangle {
                    color: settingsBtn.hovered ? "#35373c" : "transparent"; radius: 4
                }

                contentItem: Text
                {
                    text: "⚙️"
                    font.pixelSize: 14
                    anchors.centerIn: parent
                }

                onClicked:
                {
                    UIStateManager.MainUIState = 2 // Открываем настройки
                }

                ToolTip.visible: hovered
                ToolTip.text: "Настройки пользователя"
            }
        }
    }
}
