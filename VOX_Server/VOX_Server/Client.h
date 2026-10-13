#pragma once
#include "Data/MessagesIncluds.h"
#include "Libs/Crypto/CryptoLib.h"
#include "Data/AsioThreads.h"

class Core;

class Client : public std::enable_shared_from_this<Client>
{
    Core& _Core;
    shared_ptr<CryptoLib> _CryptoLib;
    ByteArray _buffer;
    TCPSoket _socket;
    string _UUID;
    string _Nickname;


    bool _writingInProgress = false;
    queue<vector<uint8_t>> _writeQueue;
    steady_timer _writeTimer;

public:
    explicit Client(TCPSoket socket, Core& _Core);
    ~Client();
    using DisconnectCallback = std::function<void(const std::shared_ptr<Client>&)>; 
    void subscribeOnDisconnect(DisconnectCallback cb);
    void unsubscribeOnDisconnect(const DisconnectCallback& cb);

    void sendMessage(const SBaseMessageData &_Message);
    string peerAddress() const;
    void setUUID(string UUID) {_UUID = UUID;}
    string getUUID() { return _UUID; }
    void onReadyRead();

    string getSocketDescriptor() const
    {
        return _socket.remote_endpoint().address().to_string();
    }

    const TCPSoket* getSocket() {return &_socket;};

    void start();
    void disconnect();


private:
    std::vector<DisconnectCallback> _disconnectCallbacks;
    void notifyDisconnect();
    void receiveMessage(JsonObject& data, ETypeOfMessage& TypeMessage);
    bool handleCryptoHandshake(const JsonObject& json);

    // Корутина C++20 для чтения данных из сокета
    asio::awaitable<void> readLoop();

    // Корутина C++20 для записи данных (обслуживает очередь отправки)
    asio::awaitable<void> WriteLoop();
};
