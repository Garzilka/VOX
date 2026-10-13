#ifndef FRIENDMANAGER_H
#define FRIENDMANAGER_H
#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QDebug>

class QFriendManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int State READ getState WRITE setState NOTIFY StateChanged)
    Q_PROPERTY(QVariantList searchResults READ getSearchResults NOTIFY searchResultsChanged)
    Q_PROPERTY(QVariantList pendingRequests READ getPendingRequests NOTIFY pendingRequestsChanged)
    Q_PROPERTY(QVariantList friendsList READ getFriendsList NOTIFY friendsListChanged)

private:
    int FriendState = 0; // 0 - Friends | 1 - Chat | 2 - Search | 3 - RequestFriend
    QVariantList m_searchResults;
    QVariantList m_pendingRequests;
    QVariantList m_friendsList;

public:
    QFriendManager(QObject *parent = nullptr) : QObject(parent) {};

    int getState() { return FriendState; }
    QVariantList getSearchResults() const { return m_searchResults; }
    QVariantList getPendingRequests() const { return m_pendingRequests; }
    QVariantList getFriendsList() const { return m_friendsList; }

    // Метод для обновления результатов поиска из класса QConnection
    void setSearchResults(const QVariantList& results)
    {
        m_searchResults = results;
        qDebug() << "FriendsManager | Обновлен список результатов поиска, элементов:" << m_searchResults.size();
        emit searchResultsChanged();
    }

    // Обновление списка ожидающих заявок (Входящие / Исходящие)
    void setPendingRequests(const QVariantList& requests)
    {
        m_pendingRequests = requests;
        qDebug() << "FriendsManager | Обновлен список заявок (Ожидание), элементов:" << m_pendingRequests.size();
        emit pendingRequestsChanged();
    }

    // Обновление подтвержденного списка друзей
    void setFriendsList(const QVariantList& friends)
    {
        m_friendsList = friends;
        qDebug() << "FriendsManager | Обновлен список друзей (Все контакты), элементов:" << m_friendsList.size();
        emit friendsListChanged();
    }

    Q_INVOKABLE void setState(int NewNum)
    {
        if (FriendState != NewNum)
        {
            FriendState = NewNum;
            emit StateChanged();
        }
    }

    // Обработка кнопок "Принять" / "Отклонить" в UI
    Q_INVOKABLE void handleFriendAction(const QString& targetGUID, int action);

signals:
    void StateChanged();
    void searchResultsChanged();
    void pendingRequestsChanged();
    void friendsListChanged();
};

#endif // FRIENDMANAGER_H
