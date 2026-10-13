#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H
#pragma once

#include <QObject>
#include <QHash>
#include <QDebug>
#include "Core/Data/DataIncluds.h"


struct SRoomNotificationState
{
    int UnreadCount = 0;   // Общее число новых сообщений (для серой подсветки)
    int MentionCount = 0;  // Число личных пингов (для красного кружка)
    bool IsMuted = false;  // В муте ли этот канал/чат
};

// 2. Иерархический контейнер уведомлений для целого сервера
struct SServerNotificationGroup
{
    QString ServerGUID;

    // Внутренний хэш: Ключ: ChatGUID канала -> Значение: Статус пингов этого канала
    QHash<QString, SRoomNotificationState> ChannelsNotifications;

    // Быстрый подсчет всех личных пингов на сервере за O(N_channels)
    int getTotalServerMentions() const {
        int sum = 0;
        for (const auto& state : ChannelsNotifications) {
            sum += state.MentionCount;
        }
        return sum;
    }
};

class QNotificationManager : public QObject
{
    Q_OBJECT

private:
    // ХЭШ-ТАБЛИЦА 1: Уведомления серверов (Группировка по ServerGUID)
    QHash<QString, SServerNotificationGroup> m_serverNotifications;

    // ХЭШ-ТАБЛИЦА 2: Уведомления личных и групповых чатов (Ключ: ChatGUID)
    QHash<QString, SRoomNotificationState> m_directNotifications;

    explicit QNotificationManager(QObject *parent = nullptr) : QObject(parent) {};

public:
    QNotificationManager& operator=(const QNotificationManager&) = delete;
    QNotificationManager(const QNotificationManager&) = delete;

    static QNotificationManager& getInstance()
    {
        static QNotificationManager instance(nullptr);
        return instance;
    };

    // --- МЕТОДЫ-ПУСТЫШКИ (Для будущей логики обработки сети и UI) ---

    // Вызывается из QConnection при получении real-time пакета SChatNotificationEvent (команда 21)
    void handleNotificationEvent(const SChatNotificationEvent& event)
    {
        // Тут будет логика раскладки данных по двум хэш-таблицам
        Q_UNUSED(event);
    }

    // Вызывается из UI, когда пользователь кликает на чат и читает его (для сброса счетчиков)
    Q_INVOKABLE void markChatAsRead(const QString& chatGUID, const QString& serverGUID = "") {
        Q_UNUSED(chatGUID);
        Q_UNUSED(serverGUID);
    }

    // Вызывается из QML для отрисовки красной точки на круглой иконке сервера
    Q_INVOKABLE int getServerMentionsCount(const QString& serverGUID) {
        if (m_serverNotifications.contains(serverGUID)) {
            return m_serverNotifications[serverGUID].getTotalServerMentions();
        }
        return 0;
    }

    // Вызывается из QML для отрисовки красной точки на общей кнопке Личных сообщений (DM) вверху слева
    Q_INVOKABLE int getTotalDirectMentionsCount()
    {
        int sum = 0;
        for (const auto& state : m_directNotifications)
        {
            sum += state.MentionCount;
        }
        return sum;
    }
};

#endif // NOTIFICATIONMANAGER_H
