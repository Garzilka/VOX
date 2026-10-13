#include "friendmanager.h"
#include "Core/connection.h"

void QFriendManager::handleFriendAction(const QString &targetGUID, int action)
{
    qDebug() << "FriendsManager | Действие с заявкой:" << (action == 0 ? "Принять" : "Отклонить") << "для GUID:" << targetGUID;

    // 1. Optimistic UI: Быстро удаляем карточку из списка "Ожидание" на клиенте, не дожидаясь ответа сервера
    for (int i = 0; i < m_pendingRequests.size(); ++i)
    {
        QVariantMap userMap = m_pendingRequests[i].toMap();
        // СКОРРЕКТИРОВАНО: Проверка по нижнему регистру ключа "guid", как приходит из QConnection
        if (userMap["guid"].toString() == targetGUID)
        {
            m_pendingRequests.removeAt(i);
            qDebug() << "FriendsManager | Карточка пользователя локально удалена из UI ожидания";
            emit pendingRequestsChanged();
            break;
        }
    }

    // 2. Отправляем сетевой пакет через обновленный QConnection шлюз для фиксации изменений в БД
    QConnection::getInstance().requestFriendAction(targetGUID, action);
}
