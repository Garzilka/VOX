#include "friendmanager.h"
#include "Core/connection.h"

void QFriendManager::handleFriendAction(const QString &targetGUID, int action)
{
    qDebug() << "FriendsManager | Действие с заявкой:" << (action == 0 ? "Принять" : "Отклонить") << "для GUID:" << targetGUID;

    for (int i = 0; i < m_pendingRequests.size(); ++i)
    {
        if (m_pendingRequests[i].toString() == targetGUID)
        {
            m_pendingRequests.removeAt(i);
            qDebug() << "FriendsManager | Карточка пользователя локально удалена из UI ожидания";
            emit pendingRequestsChanged();
            break;
        }
    }

    QConnection::getInstance().requestFriendAction(targetGUID, action);
}
