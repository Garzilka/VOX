import QtQuick
import QtQuick.Layouts
import VOX_Client

RowLayout
{
    id: root
    width: parent ? parent.width : 0
    spacing: 16
    Layout.alignment: Qt.AlignTop
    property var attachmentsModel: []

    // Входные параметры от ListView
    property int currentType: 0
    property string senderGUID: ""
    property string messageTime: ""
    property string rawContent: ""

    // ИСПРАВЛЕНО: Перевели свойства в динамические переменные (убрали readonly)
    property string senderName: "Загрузка..."
    property color avatarBgColor: "#80848e"

    // Проверяем, загружаются ли данные в текущий момент
    readonly property bool isLoading: root.senderName === "Загрузка..." || root.senderName === ""

    // Логика точечного обновления данных сообщения без дерганья GUID
    function updateMessageSenderData()
    {
        if (!root.senderGUID) return;
        root.senderName = UserManager.getDisplayName(root.senderGUID)
        root.avatarBgColor = UserManager.getStatusColor(root.senderGUID)
    }

    // Триггеры для первоначального вычисления при создании сообщения
    Component.onCompleted: root.updateMessageSenderData()
    onSenderGUIDChanged: root.updateMessageSenderData()

    // ==========================================
    // Аватар (Общий элемент)
    // ==========================================
    Rectangle
    {
        id: avatarRect
        width: 40
        height: 40
        radius: 20
        color: root.isLoading ? "#35363c" : root.avatarBgColor
        Layout.alignment: Qt.AlignTop

        Text
        {
            text: root.isLoading ? "" : (root.senderName ? root.senderName.charAt(0).toUpperCase() : "?")
            color: "#1e1f22"
            font.bold: true
            font.pixelSize: 16
            anchors.centerIn: parent
        }

        // Анимация пульсации для аватарки во время загрузки
        SequentialAnimation on opacity {
            running: root.isLoading
            loops: Animation.Infinite
            NumberAnimation { to: 0.4; duration: 600; easing.type: Easing.InOutQuad }
            NumberAnimation { to: 1.0; duration: 600; easing.type: Easing.InOutQuad }
        }

        // Сброс прозрачности в исходное состояние, когда загрузка завершена
        Connections
        {
             target: root
             function onIsLoadingChanged()
             {
                 if (!root.isLoading)
                 {
                     avatarRect.opacity = 1.0 // Принудительно возвращаем полную непрозрачность
                 }
             }
        }
    }

    // ==========================================
    // Правая часть сообщения
    // ==========================================
    ColumnLayout
    {
        Layout.fillWidth: true
        spacing: 6

        // Метаданные (Общий элемент)
        RowLayout
        {
            spacing: 8

            // Контейнер для имени пользователя / Скелетона
            Item
            {
                Layout.preferredWidth: root.isLoading ? 120 : nameText.implicitWidth
                Layout.preferredHeight: 18

                // Реальный текст имени (виден только когда загружен)
                Text
                {
                    id: nameText
                    text: root.isLoading ? "" : root.senderName
                    color: "#ffffff"
                    font.bold: true
                    font.pixelSize: 14
                    anchors.verticalCenter: parent.verticalCenter
                    visible: !root.isLoading
                }

                // Плавная плашка-скелетон вместо имени
                Rectangle
                {
                    id: nameSkeleton
                    anchors.fill: parent
                    radius: 4
                    color: "#35363c"
                    visible: root.isLoading

                    // Анимация пульсации плашки
                    SequentialAnimation on opacity {
                        running: root.isLoading
                        loops: Animation.Infinite
                        NumberAnimation { to: 0.3; duration: 600; easing.type: Easing.InOutQuad }
                        NumberAnimation { to: 0.8; duration: 600; easing.type: Easing.InOutQuad }
                    }
                }
            }

            // Время отправки сообщения
            Text
            {
                text: root.messageTime
                color: "#949ba4"
                font.pixelSize: 11
                Layout.alignment: Qt.AlignVCenter
            }

            Item
            {
                Layout.fillWidth: true
            }
        }

        // --- Переключаемые контейнеры содержимого ---

        // 1. Основной текст сообщения (отображается всегда, если он не пустой)
                TextMessageContainer
                {
                    Layout.fillWidth: true
                    text: root.rawContent
                    visible: root.rawContent !== ""
                }

                // 2. ДОБАВЛЕНО: Динамический список вложений сообщения
                ColumnLayout
                {
                    Layout.fillWidth: true
                    spacing: 8
                    visible: attachmentsRepeater.count > 0

                    Repeater
                    {
                        id: attachmentsRepeater
                        // Принимает массив [{type: 0, url: "...", metaData: "..."}, ...]
                        model: root.attachmentsModel

                        delegate: Loader
                        {
                            Layout.fillWidth: true

                            // Динамически выбираем нужный QML-компонент в зависимости от типа вложения
                            sourceComponent: {
                                switch (modelData["type"]) {
                                    case 0: return imageAttachmentComp;   // Картинка
                                    case 3: return inviteAttachmentComp;  // Инвайт на сервер
                                    default: return undefined;
                                }
                            }

                            // Компонент инвайта (твоя прошлая InvitationMessageContainer)
                            Component
                            {
                                id: inviteAttachmentComp
                                InvitationMessageContainer
                                {
                                    Layout.fillWidth: true
                                    targetServerGUID: modelData["url"]
                                    inviteToken: modelData["metaData"]
                                }
                            }

                            // Пример нового компонента для картинок
                            Component
                            {
                                id: imageAttachmentComp
                                Image
                                {
                                    source: modelData["url"]
                                    fillMode: Image.PreserveAspectFit
                                    // Для красоты скруглим края вложений как в Discord
                                    layer.enabled: true
                                    layer.effect: ShaderEffect { /* Твой кастомный радиус, если нужен */ }
                                }
                            }
                        }
                    }
                }
    }

    // =====================================================================
    // ИСПРАВЛЕНО: Безопасный триггер обновления профилей без шторма запросов
    // =====================================================================
    Connections
    {
        target: UserManager
        // Используем встроенный синтаксис Qt6: функцию-обработчик сигнала
        function onDataChanged()
        {
            // Принудительно запрашиваем данные из кэша. Если они прилетели —
            // свойства изменятся, интерфейс перерисуется. Сетевой буфер при этом не дергается!
            root.updateMessageSenderData()
        }
    }
}
