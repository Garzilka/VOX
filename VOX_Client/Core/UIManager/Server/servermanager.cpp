#include "servermanager.h"
#include "Core/connection.h"
#include "Core/UIManager/uimanager.h"
#include "Core/UIManager/ChatManager/chatmanager.h"
#include "Core/VoiceEngine/voiceengine.h"

QServerManager::QServerManager(QObject *parent) : QObject(parent)
{
    m_batchTimer = new QTimer(this);
    m_batchTimer->setSingleShot(true);
    m_batchTimer->setInterval(50); // Ждем 50мс для склейки запросов с экрана в один пакет
    connect(m_batchTimer, &QTimer::timeout, this, &QServerManager::sendBatchPassportRequest);
}

// Формирует динамическую QML модель для левой панели на основе плоского списка m_myServers
QVariantList QServerManager::getServerList() const
{
    QVariantList list;
    /*for (const QString& guid : m_myServers)
    {
        QVariantMap sMap;
        sMap["serverGUID"] = guid;
        // Если паспорт уже загружен — отдаем имя, иначе — заглушку под скелетон
        if (m_serverRegistry.contains(guid))
        {
            sMap["name"] = m_serverRegistry[guid].Name;
            sMap["iconUrl"] = m_serverRegistry[guid].IconUrl;
        }
        else
        {
            sMap["name"] = "";
            sMap["iconUrl"] = "";
        }
        list.append(sMap);
    }*/
    return list;
}

// ГЛОВНЫЙ ЛЕЗИ-ПОИСК ИМЕНИ: Вызывается отовсюду (иконки, инвайты)
QString QServerManager::getServerName(const QString& serverGUID)
{
    if (serverGUID.isEmpty()) return "";

    /*if (m_serverRegistry.contains(serverGUID))
    {
        return m_serverRegistry[serverGUID].Name;
    }*/

    if (!m_pendingPassportRequests.contains(serverGUID))
    {
        m_pendingPassportRequests.insert(serverGUID);
        if (!m_batchTimer->isActive())
        {
            m_batchTimer->start();
        }
    }

    return "Загрузка...";
}

bool QServerManager::isUserMemberOfServer(const QString& serverGUID) const
{
    return m_myServers.contains(serverGUID);
}


void QServerManager::sendBatchPassportRequest()
{
    /*if (m_pendingPassportRequests.isEmpty()) return;

    SServersList requestPacket(ETypeOfMessage::RequestServerShortData);
    requestPacket.GUIDs = m_pendingPassportRequests.values();

    QConnection::getInstance().sendMessage(requestPacket);

    m_pendingPassportRequests.clear();*/
}


void QServerManager::handleServerPassportsLoaded(const QList<SServerPassport>& passports)
{
    /*for (const auto& shortInfo : passports)
    {
        SServerPassport passport;
        passport.ServerGUID = shortInfo.ServerGUID;
        passport.Name = shortInfo.Name;
        passport.IconUrl = shortInfo.IconUrl;

        m_serverRegistry[shortInfo.ServerGUID] = passport;
    }


    emit serverRegistryUpdated();
    emit serverListChanged();*/
}


void QServerManager::initMyServersList(const QStringList& guids)
{
    m_myServers = guids;

    emit serverListChanged();


    for (const QString& guid : m_myServers)
    {
        getServerName(guid);
    }
}
void QServerManager::selectActiveChannelForServer(const QString& serverGUID, const QList<SChannelPassport>& channels)
{
    /*QString targetChannelGUID = "";


    if (m_lastServerChannelCache.contains(serverGUID))
    {
        targetChannelGUID = m_lastServerChannelCache[serverGUID];
    }
    else
    {

        for (const auto& chan : channels)
        {
            if (chan.ChatType == 2)
            {
                targetChannelGUID = chan.ChatGUID;
                break; // Нашли первый текстовый, выходим из цикла
            }
        }
    }


    if (!targetChannelGUID.isEmpty())
    {
        QUIManager::getInstance().GetChatManager()->setActiveRoomGUID(targetChannelGUID);
    }*/
}
// Клик по серверу на левой панели: Лениво запрашиваем детальную структуру каналов
void QServerManager::setActiveServerGUID(const QString &serverGUID)
{
    /*if (m_activeServerGUID == serverGUID) return;

    m_activeServerGUID = serverGUID;
    emit activeServerGUIDChanged();


    if (m_serverDetailsCache.contains(m_activeServerGUID))
    {
        const auto& cachedDetails = m_serverDetailsCache[m_activeServerGUID];
        updateUIWithServerDetails(cachedDetails);
        qInfo() << "ServerManager | Каналы сервера подняты из кэша. Накатываем активный чат.";


        selectActiveChannelForServer(m_activeServerGUID, cachedDetails.Channels);
    }
    else
    {

        m_currentChannels.clear();
        m_currentParticipants.clear();
        emit currentChannelsChanged();
        emit currentParticipantsChanged();

        qInfo() << "ServerManager | Сетевой запрос структуры каналов для сервера:" << serverGUID;
        QConnection::getInstance().requestServerDetails(serverGUID);
    }*/
}

void QServerManager::setServerDetailsFromServer(const SServerDetailsData& details)
{
    /*SServerFullDetails fullDetails;
    fullDetails.ServerGUID = details.ServerGUID;
    fullDetails.Channels = details.Channels;
    fullDetails.ParticipantGUIDs = details.UserGUIDs;

    m_serverDetailsCache[details.ServerGUID] = fullDetails;

    if (m_activeServerGUID == details.ServerGUID)
    {
        updateUIWithServerDetails(fullDetails);
        qInfo() << "ServerManager | Структура сервера получена из сети. Накатываем активный чат.";


        selectActiveChannelForServer(details.ServerGUID, details.Channels);
    }*/
}

void QServerManager::handleServerCreated(const SCreateServerData& serverData)
{
    /*if (serverData.ServerGUID.isEmpty() || serverData.ServerName.isEmpty()) return;


    SServerPassport passport;
    passport.ServerGUID = serverData.ServerGUID;
    passport.Name = serverData.ServerName;
    m_serverRegistry.insert(serverData.ServerGUID, passport);


    m_myServers.append(serverData.ServerGUID);
    emit serverListChanged();


    setState(1);
    setActiveServerGUID(serverData.ServerGUID);*/
}

void QServerManager::updateUIWithServerDetails(const SServerFullDetails& details)
{
    /*m_currentChannels.clear();
    for (const auto& chan : details.Channels)
    {
        QVariantMap cMap;
        cMap["chatGUID"] = chan.ChatGUID;
        cMap["chatType"] = chan.ChatType;
        cMap["name"] = chan.Name;
        cMap["parentGUID"] = chan.ParentGUID;
        m_currentChannels.append(cMap);
    }
    m_currentParticipants = details.ParticipantGUIDs;

    emit currentChannelsChanged();
    emit currentParticipantsChanged();*/
}

void QServerManager::joinVoiceChannelFromUI(const QString &serverGUID, const QString &channelGUID)
{
    if (QVoiceEngine::getInstance().currentRoomGUID() == channelGUID)
    {
        qInfo() << "ServerManager | Игнорируем запрос: пользователь уже подключен к каналу" << channelGUID;
        return; // Просто выходим, ничего не делая
    }

    // Если канал новый — шлем сетевой запрос на подключение как обычно
    qInfo() << "ServerManager | Отправка сетевого запроса на вход в голос. Сервер:" << serverGUID << "Канал:" << channelGUID;
    QConnection::getInstance().requestJoinVoice(serverGUID, channelGUID);
}

void QServerManager::leaveVoiceChannelFromUI(const QString &serverGUID, const QString &channelGUID)
{
    QConnection::getInstance().requestLeaveVoice(serverGUID, channelGUID);
}

void QServerManager::handleVoiceUserJoined(const QString& channelGUID, const QString& userGUID, const QString& webRTCToken)
{
    // 1. Если зашли мы сами — подключаем наш WebRTC движок LiveKit
    if (userGUID == QConnection::getInstance().getUUID())
    {
        QVoiceEngine::getInstance().connectToVoiceServer(channelGUID, webRTCToken);
    }

    // 2. ИСПРАВЛЕНИЕ: Удаляем пользователя абсолютно ИЗ ВСЕХ голосовых комнат во всем кэше,
    // чтобы он не двоился при переключении между каналами
    QStringList affectedChannels;
    for (auto it = m_voiceUsersCache.begin(); it != m_voiceUsersCache.end(); ++it)
    {
        if (it.value().contains(userGUID))
        {
            it.value().removeAll(userGUID);
            affectedChannels.append(it.key()); // запоминаем, из каких комнат он "исчез"
        }
    }

    // 3. Добавляем в новую комнату
    if (!m_voiceUsersCache[channelGUID].contains(userGUID))
    {
        m_voiceUsersCache[channelGUID].append(userGUID);
        affectedChannels.append(channelGUID);
    }

    // 4. ИСПРАВЛЕНИЕ: Триггерим сигналы обновления для каждой изменившейся комнаты
    for (const QString& chanGUID : affectedChannels)
    {
        emit voiceUsersChanged(chanGUID);
    }

    forceRefreshChannels(); // Оставляем для совместимости со структурой
}

void QServerManager::handleVoiceUserLeft(const QString& channelGUID, const QString& userGUID)
{
    // 1. Если вышли мы — глушим WebRTC
    if (userGUID == QConnection::getInstance().getUUID())
    {
        QVoiceEngine::getInstance().disconnectFromVoiceServer();
    }

    // 2. Удаляем из кэша канала
    if (m_voiceUsersCache.contains(channelGUID))
    {
        m_voiceUsersCache[channelGUID].removeAll(userGUID);
    }

    // 3. ИСПРАВЛЕНИЕ: Оповещаем интерфейс, что конкретно этот канал опустел
    emit voiceUsersChanged(channelGUID);

    forceRefreshChannels();
}
