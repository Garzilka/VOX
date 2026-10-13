#include "connection.h"
#include "Core/UIManager/uimanager.h"
#include <QSettings>

QConnection::QConnection(QObject *parent) : QObject(parent)
{
    cryptolib = new QCryptoLib();
    m_socket = new QTcpSocket(this);

    if(DebugMode) return;

    connect(m_socket, &QTcpSocket::connected, this, &QConnection::onConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &QConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &QConnection::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &QConnection::onErrorOccurred);
    connectToServer("127.0.0.1", 55);
}

void QConnection::connectToServer(const QString &host, quint16 port)
{
    if(DebugMode)
    {
        QUIManager::getInstance().setSecuritySuccessfully(true);
        return;
    }
    if (m_socket->state() == QAbstractSocket::ConnectedState) return;
    m_socket->connectToHost(host, port);
}

void QConnection::onConnected()
{
    qDebug() << "Транспорт | Успешное подключение к серверу!";
}
void QConnection::onDisconnected()
{
    qDebug() << "Транспорт | Соединение разорвано.";
    m_buffer.clear();
}

void QConnection::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qCritical() << "Транспорт | Ошибка сокета:" << m_socket->errorString();
}

bool QConnection::sendMessage(const SBaseMessageData &_Message)
{
    if(DebugMode) return true;

    if (m_socket->state() != QAbstractSocket::ConnectedState)
    {
        qWarning() << "Транспорт | Ошибка: Нет соединения с сервером.";
        emit authErrorReceived("Нет соединения с сервером");
        return false;
    }

    QJsonDocument doc(_Message.ToJSON());
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    if (cryptolib->hasSharedSecret()) {
        data = cryptolib->encryptMessage(data);
    }

    quint32 size = data.size();
    QByteArray packet;
    packet.append(reinterpret_cast<const char*>(&size), 4);
    packet.append(data);

    m_socket->write(packet);
    m_socket->flush();
    return true;
}

void QConnection::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    while (m_buffer.size() >= 4)
    {
        QJsonObject MainObject;
        int status = ReadCurrentPacket(MainObject);
        if(status == 2) return;
        if(status == 1) continue;

        SBaseMessageData Data(MainObject);

        switch (Data.TypeMessage)
        {
        case ETypeOfMessage::Crypto:
            ResponseCrypto(MainObject);
            break;
        case ETypeOfMessage::Login:
            ResponseAuth(MainObject);
            break;
        case ETypeOfMessage::Registration:
            ResponseAuth(MainObject);
            break;
        case ETypeOfMessage::Message:
            NewMessage(MainObject);
            break;
        case ETypeOfMessage::ChannelDetails:
            if(Data.Response) ResponseHistoryChannel(MainObject);
            break;
        case ETypeOfMessage::FriendRequest:
            // Входящее уведомление о заявке в друзья в реальном времени
            emit QUIManager::getInstance().GetFriendManager()->setState(3); // Переключить на вкладку заявок
            QMetaObject::invokeMethod(this, "requestDirectChats", Qt::QueuedConnection); // Обновить списки
            break;
        case ETypeOfMessage::FriendAccept:
            // Сигнал, что кто-то принял/отклонил дружбу
            break;
        case ETypeOfMessage::Friends:
            if(Data.Response) ResponseFriendsList(MainObject);
            break;
        case ETypeOfMessage::SearchUsers:
            if(Data.Response) ResponseSearchUsers(MainObject);
            break;
        case ETypeOfMessage::Friendships:
            if(Data.Response) ResponseFriendships(MainObject);
            break;
        case ETypeOfMessage::ChannelCreate:
            if(Data.Response) ResponseOpenOrCreateChat(MainObject);
            break;
        case ETypeOfMessage::ServerCreate:
            if(Data.Response) ResponseCreateServer(MainObject);
            break;
        case ETypeOfMessage::ServerInvite:
            if(Data.Response) ResponseGenerateInvite(MainObject);
            break;
        case ETypeOfMessage::ServerAccept:
            if(Data.Response) ResponseAcceptInvite(MainObject);
            break;
        case ETypeOfMessage::ChannelsPrivate:
            if(Data.Response) ResponseDirectChats(MainObject);
            break;
        case ETypeOfMessage::ServerDetails:
            if(Data.Response) ResponseServerDetails(MainObject);
            break;
        case ETypeOfMessage::UserPassport:
            if(Data.Response) ResponseOpenUsersData(MainObject);
            break;
        case ETypeOfMessage::ServerList:
            if(Data.Response) ResponseServersList(MainObject);
            break;
        // ДОБАВЛЕНО: Обработка прилетевших паспортов серверов
        case ETypeOfMessage::ServerPassport:
            if(Data.Response) ResponseServerShortData(MainObject);
            break;
        case ETypeOfMessage::ChannelJoin: // RequestJoinVoice
            if (Data.Response) ResponseVoiceUserJoined(MainObject);
            break;
        case ETypeOfMessage::ChannelLeave: // RequestLeaveVoice
            if (Data.Response) ResponseVoiceUserLeft(MainObject);
            break;
        default:
            break;
        }
    }
}

void QConnection::requestAuthorization(const QString &Login, const QString &Password, bool rememberMe)
{
    /*g_shouldRememberNextAuth = rememberMe;

    if (DebugMode) {
        setState(1);
        return;
    }

    QSettings settings("MyChatCompany", "MyChatApp");
    bool hasRememberMe = settings.value("auth/remember_me", false).toBool();
    QString savedToken = settings.value("auth/refresh_token", "").toString();

    // СЦЕНАРИЙ 1: Есть валидный токен и мы делаем автоматический вход
    if (hasRememberMe && !savedToken.isEmpty() && Login.isEmpty() && Password.isEmpty())
    {
        qDebug() << "Авторизация | Попытка входа по Refresh-токену...";

#if defined(Q_OS_WIN)
        QString platform = "windows";
#elif defined(Q_OS_MAC)
        QString platform = "macos";
#else
        QString platform = "linux";
#endif

        STokenAuthData tokenPacket(savedToken, platform);
        sendMessage(tokenPacket); // Отправляет пакет с TypeMessage = 25 (TokenAuth)
    }
    // СЦЕНАРИЙ 2: Токена нет (или это ручной вход), авторизуемся по логину и паролю
    else
    {*/
        qDebug() << "Авторизация | Ручной вход по логину:" << Login;

        SAuthorizationData sendData(ETypeOfMessage::Login, Login, "", Password);
        sendMessage(sendData);
    //}
}

void QConnection::requestRegistration(const QString &Login, const QString &Email, const QString &Password, bool rememberMe)
{
    g_shouldRememberNextAuth = rememberMe;
    if(!DebugMode) {
        SAuthorizationData sendData(ETypeOfMessage::Registration, Login, Email, Password);
        sendMessage(sendData);
        return;
    }
    QUIManager::getInstance().setMainUIState(1);
}

void QConnection::ResponseCrypto(QJsonObject &JSONObject)
{
    SCyptsKeysData CryptMessage(JSONObject);
    if (!(CryptMessage.Response && CryptMessage.TypeMessage == ETypeOfMessage::Crypto)) return;
    QByteArray peerKey = QByteArray::fromBase64(CryptMessage.PublicKey.toUtf8());

    if (!cryptolib->generateKeyPair()) qDebug() << "Крипто | Ошибка генерации ключей";

    SCyptsKeysData ResponseCryptMessage(cryptolib->getPublicKeyDER().toBase64());
    sendMessage(ResponseCryptMessage);

    if (!cryptolib->computeSharedSecret(peerKey)) qDebug() << "Крипто | Ошибка общего секрета";

    QUIManager::getInstance().setSecuritySuccessfully(true);
}

void QConnection::ResponseAuth(QJsonObject &JSONObject)
{
    // Используем расширенный парсер ответа
    SAuthorizationData Resp(JSONObject);

    if(Resp.Response)
    {
        UUID = Resp.UUID; // Запоминаем текущий рабочий UUID сессии сокета

        // Если сервер сгенерировал или обновил Refresh-токен
        if (!Resp.longToken.isEmpty())
        {
            QSettings settings("Vox", "VoxApp");
            settings.setValue("auth/refresh_token", Resp.longToken);
            settings.setValue("auth/remember_me", true);
        }

        requestOpenDataUsers({ UUID });
        QUIManager::getInstance().setMainUIState(1); // Прыгаем в главное окно чата
    }
    else
    {
        qWarning() << "Транспорт | Токен невалиден или просрочен.";

        // На случай, если автоматический вход по токену провалился, сбрасываем плохой токен
        QSettings settings("Vox", "VoxApp");
        settings.remove("auth/refresh_token");

        emit authErrorReceived("Сессия истекла. Войдите заново.");
        QUIManager::getInstance().setMainUIState(0);
    }
}

void QConnection::checkSavedSession()
{
    return;
    QSettings settings("Vox", "VoxApp");
    bool rememberMe = settings.value("auth/remember_me", false).toBool();
    QString savedToken = settings.value("auth/refresh_token", "").toString();

    if (rememberMe && !savedToken.isEmpty()) {
        qDebug() << "Авторизация | Найден токен, отправляем запрос на сервер...";

#if defined(Q_OS_WIN)
        QString platform = "windows";
#elif defined(Q_OS_MAC)
        QString platform = "macos";
#else
        QString platform = "linux";
#endif

        //STokenAuthData tokenPacket(savedToken, platform);
        //sendMessage(tokenPacket);
    }
}

// Метод для кнопки "Выйти из аккаунта" в настройках
void QConnection::logout()
{
    QSettings settings("Vox", "VoxApp");
    settings.remove("auth/refresh_token");
    settings.setValue("auth/remember_me", false);

    UUID = "";
    QUIManager::getInstance().setMainUIState(0);
}

void QConnection::requestOpenDataUsers(const QList<QString>& UUIDs)
{
    for(const QString& userUUID : UUIDs)
    {
        SRequestUserPassport msg;
        msg.userUUID = userUUID;
        sendMessage(msg);
    }
}

void QConnection::ResponseOpenUsersData(QJsonObject &JSONObject)
{
    SRequestUserPassport data(JSONObject);
    QVariantList usersList;
    QVariantMap uMap;
    uMap["UUID"] = data.userPassport.UUID;
    uMap["displayName"] = data.userPassport.DisplayName;
    uMap["avatarUrl"] = data.userPassport.AvatarUrl;
    uMap["presenceStatus"] = data.userPassport.PresenceStatus;
    usersList.append(uMap);
    emit batchUsersLoaded(usersList);

}

void QConnection::NewMessage(QJsonObject &JSONObject)
{
    SMessageData msg(JSONObject);
    QVariantMap msgMap;
    msgMap["sender"] = msg.SenderGUID;
    msgMap["messageText"] = msg.Message;
    msgMap["time"] = QTime::currentTime().toString("hh:mm");
    msgMap["avatarColor"] = "#5865f2";

    // Конвертируем вложения для QML
    QVariantList attachmentsList;
    for (const auto& attach : msg.Attachments)
    {
        QVariantMap attachMap;
        attachMap["type"] = static_cast<int>(attach.Type);
        attachMap["url"] = attach.Url;
        attachMap["metaData"] = attach.MetaData;
        attachmentsList.append(attachMap);
    }
    msgMap["attachments"] = attachmentsList; // Добавляем массив в карту сообщения

    QChatManager* chatMgr = QUIManager::getInstance().GetChatManager();
    if (chatMgr) chatMgr->addNewMessage(msg.RoomGUID, msgMap);
}

int QConnection::ReadCurrentPacket(QJsonObject& Result)
{
    quint32 size = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(m_buffer.constData()));

    qDebug() << "Новое сообщение, размер: " << size;
    if (size > 10 * 1024 * 1024)
    {
        qDebug() << "Слишком большой пакет, разрыв соединения";
        m_socket->disconnectFromHost();
        return 2;
    }

    if (m_buffer.size() < 4 + int(size)) return 1;

    QByteArray data = m_buffer.mid(4, size);
    m_buffer.remove(0, 4 + size);

    if (cryptolib->hasSharedSecret())
    {
        data = cryptolib->decryptMessage(data);
        if (data.isEmpty())
        {
            qDebug() << "Расшифровка не удалась";
            return 1;
        }
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Ошибка парсинга JSON:" << error.errorString();
        return 1;
    }
    Result = doc.object();
    return 0;
}
