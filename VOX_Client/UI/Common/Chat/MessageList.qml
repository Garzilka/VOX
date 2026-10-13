import QtQuick
import QtQuick.Controls.Basic

Rectangle
{
    id: root
    color: "#313338"
    clip: true

    ListView
    {
        id: messagesListView
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16
        clip: true
        model: ChatManager.currentMessages

        // ДОБАВЛЕНО ДЛЯ ОПТИМИЗАЦИИ ЧАТА:
        // Включаем переиспользование компонентов и кэш памяти на 500 пикселей вверх/вниз
        reuseItems: true
        cacheBuffer: 500

        delegate: ChatMessageContainer
        {
            // Теперь передаем текст и массив вложений напрямую
            rawContent: modelData["messageText"] ? modelData["messageText"] : ""
            attachmentsModel: modelData["attachments"] !== undefined ? modelData["attachments"] : []

            senderGUID: modelData["sender"] ? modelData["sender"] : ""
            messageTime: modelData["time"] ? modelData["time"] : ""
        }

        // Автоматический скролл вниз при добавлении новых сообщений
        onCountChanged:
        {
            Qt.callLater(messagesListView.positionViewAtEnd)
        }
    }
}
