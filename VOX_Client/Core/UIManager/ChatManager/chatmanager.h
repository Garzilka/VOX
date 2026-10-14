// chatmanager.h
#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QHash>
#include <QDebug>
#include "Core/Data/DataIncluds.h"

class QChatManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString activeRoomGUID READ getActiveRoomGUID WRITE setActiveRoomGUID NOTIFY activeRoomGUIDChanged)
    Q_PROPERTY(QVariantList currentMessages READ getCurrentMessages NOTIFY currentMessagesChanged)

private:
    QString m_activeRoomGUID;
    QString m_lastPrivateRoomGUID;

    QVariantList m_currentMessages;
    QHash<QString, QVariantList> m_chatCache;
    QHash<QString, SChannelPassport> m_directChats; // Хэш-таблица приватных чатов

public:
    QChatManager(QObject *parent = nullptr) : QObject(parent) {};

    void setActiveRoomGUID(const QString& guid);

    void setHistoryFromServer(const QString& roomGUID, const QVariantList& messages)
    {
        m_chatCache[roomGUID] = messages;
        if (m_activeRoomGUID == roomGUID)
        {
            m_currentMessages = messages;
            emit currentMessagesChanged();
        }
    }

    // ОБНОВЛЕННЫЙ МЕТОД: теперь перехватывает новые сообщения и обновляет превью в хэше
    void addNewMessage(const QString& roomGUID, const QVariantMap& message)
    {
        // 1. Сохраняем в историю/кэш сообщений комнаты
        m_chatCache[roomGUID].append(message);
        if (m_activeRoomGUID == roomGUID)
        {
            m_currentMessages = m_chatCache[roomGUID]; // Гарантируем синхронизацию со списком
            emit currentMessagesChanged();
        }

        // 2. ДИНАМИЧЕСКОЕ ОБНОВЛЕНИЕ ПРЕВЬЮ
        /*if (m_directChats.contains(roomGUID))
        {
            // Берем ключ "messageText", который собирает QConnection::NewMessage
            QString textPreview = message["messageText"].toString();

            // Записываем новое превью в С++ структуру данных
            m_directChats[roomGUID].Description = textPreview;

            qDebug() << "ChatManager | Превью для чата" << roomGUID << "обновлено на лету:" << textPreview;

            // Извещаем QML-модель о необходимости вызвать refreshChats()
            emit directChatsChanged();
        }*/
    }

    QVariantList getCurrentMessages() const { return m_currentMessages; }
    QString getActiveRoomGUID() const { return m_activeRoomGUID; }
    QString getLastPrivateRoomGUID() const { return m_lastPrivateRoomGUID; }

    // Методы синхронизации хэш-таблицы
    void clearDirectChats() {
        qDebug() << "CHAT MANAGER LOG | Очистка хэш-таблицы m_directChats";
        m_directChats.clear();
    }

    void addDirectChat(const SChannelPassport& chat)
    {
        qDebug() << "CHAT MANAGER LOG | Добавлен чат в хэш:" << chat.chatUUID
                 << "Тип:" << chat.ChatType
                 << "Участников в базе:" << chat.ParticipantGUIDs.size();
        m_directChats.insert(chat.chatUUID, chat);
    }

    Q_INVOKABLE QVariantList getParticipantGUIDs(const QString& chatGUID) const
    {
        if (m_directChats.contains(chatGUID))
        {
            QVariantList participants;
            for (const QString& guid : m_directChats[chatGUID].ParticipantGUIDs)
            {
                participants.append(guid);
            }
            return participants;
        }
        return QVariantList();
    }

    // Формирование списка для QML
    Q_INVOKABLE QVariantList getDirectChatsList() const
    {
        QVariantList list;
        for (auto it = m_directChats.constBegin(); it != m_directChats.constEnd(); ++it)
        {
            const SChannelPassport& chat = it.value();
            QVariantMap chatMap;
            chatMap["chatGUID"] = chat.chatUUID;
            chatMap["chatType"] = chat.ChatType;
            chatMap["name"] = chat.Name;
            chatMap["description"] = chat.Description; // Сюда уйдет обновленный Description

            QVariantList participants;
            for (const QString& guid : chat.ParticipantGUIDs)
            {
                participants.append(guid);
            }
            chatMap["participantGUIDs"] = participants;
            list.append(chatMap);
        }
        return list;
    }

    Q_INVOKABLE void sendMessageFromUI(const QString& text);
    Q_INVOKABLE void openOrCreateChatFromUI(int chatType, const QString& targetUserGUID, const QString& serverGUID, const QString& name);

signals:
    void historyLoaded(const QVariantList& messages);
    void newMessageReceived(const QVariantMap& message);
    void activeRoomGUIDChanged();
    void currentMessagesChanged();
    void directChatsChanged(); // Сигнал изменения хэш-таблицы
};

#endif // CHATMANAGER_H
