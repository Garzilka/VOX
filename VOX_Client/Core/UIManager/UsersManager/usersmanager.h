#ifndef USERSMANAGER_H
#define USERSMANAGER_H
#pragma once

#include <QObject>
#include <QHash>
#include <QSet>
#include <QTimer>
#include <QDateTime>
#include <QVariantMap>
#include "Core/Data/DataIncluds.h"
#include "Core/connection.h"


struct SClientUserCache
{
    SUserPassport Data;
    QDateTime ExpiresAt;
};

class QUsersManager : public QObject
{
    Q_OBJECT
private:
    QHash<QString, SClientUserCache> m_usersCache;
    QSet<QString> m_pendingRequests;
    QTimer* m_batchTimer;

public:

    explicit QUsersManager(QObject *parent = nullptr) : QObject(parent)
    {
        m_batchTimer = new QTimer(this);
        m_batchTimer->setSingleShot(true);
        m_batchTimer->setInterval(500); // Ждем 50мс, собирая GUID-ы со всего экрана, и шлем ОДИН пакет
        connect(m_batchTimer, &QTimer::timeout, this, &QUsersManager::sendBatchRequest);

        // Подключаем транспортный сигнал из QConnection (команда 18)
        connect(&QConnection::getInstance(), &QConnection::batchUsersLoaded,
                this, &QUsersManager::onBatchUsersLoaded);
    }

    Q_INVOKABLE QString getDisplayName(const QString& guid)
    {
        if (guid.isEmpty()) return "";

        if (checkAndQueueUser(guid))
        {
            return m_usersCache[guid].Data.DisplayName;
        }
        return "Загрузка...";
    }

    Q_INVOKABLE QString getStatusColor(const QString& guid)
    {
        if (!m_usersCache.contains(guid)) return "#80848e";

        switch (m_usersCache[guid].Data.PresenceStatus)
        {
            case 1: return "#23a55a"; // Online (Зеленый)
            case 2: return "#f0b232"; // Idle (Желтый)
            case 3: return "#f23f43"; // Dnd (Красный)
            default: return "#80848e"; // Offline (Серый)
        }
    }

private:
    bool checkAndQueueUser(const QString& guid)
    {
        if(guid.isEmpty()) return false;

        bool exists = m_usersCache.contains(guid);
        bool isExpired = exists && (QDateTime::currentDateTime() > m_usersCache[guid].ExpiresAt);

        if (!exists || isExpired)
        {
            if (!m_pendingRequests.contains(guid))
            {
                m_pendingRequests.insert(guid);
                if (!m_batchTimer->isActive())
                {
                    m_batchTimer->start();
                }
            }
            return exists;
        }
        return true;
    }

    void sendBatchRequest()
    {
        if (m_pendingRequests.isEmpty()) return;

        QList<QString> guidsList = m_pendingRequests.values();
        QConnection::getInstance().requestUserPassport(guidsList);
        m_pendingRequests.clear();
    }

    void onBatchUsersLoaded(const QVariantList& users)
    {
        for (const auto& var : users)
        {
            QVariantMap uMap = var.toMap();
            SUserPassport uData;
            uData.UUID = uMap["guid"].toString();
            uData.DisplayName = uMap["displayName"].toString();
            uData.AvatarUrl = uMap["avatarUrl"].toString();
            uData.PresenceStatus = uMap["presenceStatus"].toInt();

            SClientUserCache cacheItem;
            cacheItem.Data = uData;
            cacheItem.ExpiresAt = QDateTime::currentDateTime().addSecs(3 * 60 * 60); // TTL = 3 часа

            m_usersCache[uData.UUID] = cacheItem;
        }

        emit dataChanged();
    }

signals:
    void dataChanged();
};

#endif // USERSMANAGER_H
