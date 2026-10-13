#include "chatmanager.h"
#include "Core/connection.h"
#include "Core/UIManager/uimanager.h"

void QChatManager::setActiveRoomGUID(const QString &guid)
{
    if (m_activeRoomGUID != guid)
    {
        m_activeRoomGUID = guid;
        emit activeRoomGUIDChanged();

        // [ПРИВАТНЫЙ ЧАТ] Если этот чат из личных сообщений — сохраняем в историю DM
        if (m_directChats.contains(guid))
        {
            m_lastPrivateRoomGUID = guid;
        }
        else
        {
            // [КАНАЛ СЕРВЕРА] Если чата нет в DM, значит это канал сервера.
            // Узнаем у ServerManager, какой сервер сейчас активен в левой панели
            QString activeServer = QUIManager::getInstance().GetServerManager()->getActiveServerGUID();

            // Если сервер выбран, привязываем этот чат к нему в историю
            if (!activeServer.isEmpty())
            {
                QUIManager::getInstance().GetServerManager()->m_lastServerChannelCache[activeServer] = guid;
            }
        }

        if (m_chatCache.contains(m_activeRoomGUID))
        {
            m_currentMessages = m_chatCache[m_activeRoomGUID];
            qInfo() << "ChatManager | Комната переключена. Данные загружены из КЭША. Сообщений:" << m_currentMessages.size();
        }
        else
        {
            m_currentMessages.clear();
            qInfo() << "ChatManager | Комната переключена. Кэш пуст, ожидается загрузка по сети...";
            QConnection::getInstance().requestHistoryMessage(guid);
        }
        emit currentMessagesChanged();
    }
}

void QChatManager::sendMessageFromUI(const QString &text)
{
    if (m_activeRoomGUID.isEmpty())
    {
        qWarning() << "ChatManager | Попытка отправить сообщение в пустую комнату!";
        return;
    }

    QString trimmedText = text.trimmed();
    if (trimmedText.isEmpty()) return;

    qDebug() << "ChatManager | Отправка сообщения в комнату:" << m_activeRoomGUID;

    // =========================================================================
    // ИСПРАВЛЕНО ПОД КОМПОЗИТНУЮ АРХИТЕКТУРУ ВЛОЖЕНИЙ
    // =========================================================================
    // Передаем: GUID комнаты, наш собственный UUID и текст сообщения.
    // Четвертый параметр (массив Attachments) автоматически инициализируется пустым списком {},
    // что идеально подходит для стандартного текстового сообщения.
    SMessageData msg(m_activeRoomGUID, QConnection::getInstance().getUUID(), trimmedText);

    // Шлем структуру на сервер по TCP
    QConnection::getInstance().sendMessage(msg);
}

// СКОРРЕКТИРОВАНО: Полностью универсальный метод управления комнатами из QML
void QChatManager::openOrCreateChatFromUI(int chatType, const QString &targetUserGUID, const QString &serverGUID, const QString &name)
{
    qDebug() << "ChatManager | UI запросил управление комнатой. Тип:" << chatType
             << "| TargetUser:" << targetUserGUID
             << "| ServerGUID:" << serverGUID
             << "| Name:" << name;

    // Просто делегируем запрос в транспортный слой, бэкенд обработает команду 13
    QConnection::getInstance().requestOpenOrCreateChat(chatType, targetUserGUID, serverGUID, name.trimmed());
}
