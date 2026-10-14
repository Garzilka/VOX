#include "Client.h"
#include "Core.h"
#include "Libs/Tools/ToolLib.h"
#include "Dispatch/Client_Dispatcher.h"

Client::Client(TCPSoket socket, Core& _Core)
    : _socket(std::move(socket)), _Core(_Core), _writeTimer(_socket.get_executor()) 
{
    _writeTimer.expires_at(std::chrono::steady_clock::time_point::max());
}


Client::~Client(){}

void Client::start() 
{
    if (!_CryptoLib)
    {
        _CryptoLib = std::make_shared<CryptoLib>();
    }
    if (!_CryptoLib->generateKeyPair()) return;

    auto self = shared_from_this();

    //Прежде временно удалялся, продлил жизнь через сильный указатель
    asio::co_spawn(_socket.get_executor(), readLoop(self), asio::detached);
    asio::co_spawn(_socket.get_executor(), WriteLoop(self), asio::detached);

    SCyptsKeysData CryptoMessage(ToolLib::toBase64(_CryptoLib->getPublicKeyDER()));
    sendMessage(CryptoMessage);
}


void Client::disconnect()
{
    if (_isDisconnected.exchange(true)) return;

    notifyDisconnect();
    cout << "[Client] Отключился.\n";

    std::error_code ec;
    if (_socket.is_open())
    {
        _socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        _socket.close(ec);
    }
    _writeTimer.cancel();
}
void Client::subscribeOnDisconnect(DisconnectCallback cb)
{
    _disconnectCallbacks.push_back(std::move(cb));
}

void Client::notifyDisconnect()
{
    auto self = shared_from_this();
    for (auto& cb : _disconnectCallbacks)
    {
        try
        {
            cb(self);
        }
        catch (...) {
            // В продакшене лучше логировать, а не глотать
        }
    }
}
void Client::sendMessage(const SBaseMessageData& message)
{
    if (!_socket.is_open())
    {
        CLIENT_WARNING("Can't add new message UUID: " + _UUID);
        return;
    }
    CLIENT_INFO("Send new message");
    JsonObject doc = message.ToJSON();
    string jsonStr = doc.dump();

    vector<uint8_t> payload(jsonStr.begin(), jsonStr.end());

    if (_CryptoLib->hasSharedSecret())
    {
        auto encrypted = _CryptoLib->encryptMessage(payload);
        if (encrypted.empty()) return;
        payload = std::move(encrypted);
    }

    const uint32_t size = static_cast<uint32_t>(payload.size());
    vector<uint8_t> packet;
    packet.reserve(4 + size);

    packet.push_back((size >> 24) & 0xFF);
    packet.push_back((size >> 16) & 0xFF);
    packet.push_back((size >> 8) & 0xFF);
    packet.push_back(size & 0xFF);
    packet.insert(packet.end(), payload.begin(), payload.end());

    asio::post(_socket.get_executor(), [this, p = std::move(packet)]() mutable 
        {
            std::lock_guard<std::mutex> lock(_writeQueueMutex);
            _writeQueue.push(std::move(p));
            _writeTimer.cancel_one();
        });
}

asio::awaitable<void> Client::WriteLoop(std::shared_ptr<Client> self)
{
    try
    {
        for (;;)
        {


            {
                std::lock_guard<std::mutex> lock(_writeQueueMutex);
                if (_writeQueue.empty())
                {
                    _writeTimer.expires_at(std::chrono::steady_clock::time_point::max());
                }
            }

            if (_writeTimer.expiry() == std::chrono::steady_clock::time_point::max())
            {
                std::error_code ec;
                co_await _writeTimer.async_wait(asio::redirect_error(asio::use_awaitable, ec));
            }

            if (!_socket.is_open()) co_return;

            std::vector<uint8_t> currentPacket;
            bool hasPacket = false;

            {
                std::lock_guard<std::mutex> lock(_writeQueueMutex);
                if (!_writeQueue.empty())
                {
                    currentPacket = std::move(_writeQueue.front());
                    _writeQueue.pop();
                    hasPacket = true;
                }
            }

            if (!hasPacket)
            {
                continue;
            }

            co_await asio::async_write(_socket, asio::buffer(currentPacket), asio::use_awaitable);
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Client " << getUUID() << " writeLoop exception: " << e.what() << "\n";
        disconnect();
    }
}


asio::awaitable<void> Client::readLoop(std::shared_ptr<Client> self)
{
    try
    {
        // Буфер теперь хранит только сырые байты текущего потока
        std::vector<uint8_t> buffer;
        buffer.reserve(4096);

        for (;;)
        {
            CLIENT_INFO("Run read: {}", _UUID);
            while (buffer.size() < 4)
            {
                size_t currentSize = buffer.size();
                size_t need = 4 - currentSize;

                buffer.resize(currentSize + need);

                std::size_t n = co_await asio::async_read(
                    _socket,
                    asio::buffer(buffer.data() + currentSize, need),
                    asio::use_awaitable
                );

                if (n == 0) {
                    disconnect();
                    co_return;
                }

                buffer.resize(currentSize + n);
            }

            // Шаг 2: Читаем размер пакета (Big-Endian)
            uint32_t size = (static_cast<uint32_t>(buffer[0]) << 24) |
                (static_cast<uint32_t>(buffer[1]) << 16) |
                (static_cast<uint32_t>(buffer[2]) << 8) |
                (static_cast<uint32_t>(buffer[3]));

            constexpr uint32_t MAX_PACKET_SIZE = 10 * 1024 * 1024;
            if (size > MAX_PACKET_SIZE)
            {
                std::cout << "Client " << getUUID() << ": слишком большой пакет, разрыв соединения\n";
                disconnect();
                co_return;
            }

            CLIENT_INFO("Size: {}", size);

            const uint32_t totalSize = 4 + size;
            if (buffer.size() < totalSize)
            {
                size_t currentSize = buffer.size();
                size_t need = totalSize - currentSize;

                buffer.resize(totalSize); 

                std::size_t n = co_await asio::async_read(
                    _socket,
                    asio::buffer(buffer.data() + currentSize, need),
                    asio::use_awaitable
                );

                if (n == 0)
                {
                    std::cout << "Client " << getUUID() << ": соединение разорвано при чтении пакета\n";
                    disconnect();
                    co_return;
                }
                buffer.resize(currentSize + n);
            }

            const uint8_t* payloadPtr = buffer.data() + 4;
            std::vector<uint8_t> payload(payloadPtr, payloadPtr + size);

            if (_CryptoLib->hasSharedSecret())
            {
                CLIENT_INFO("Decrypting packet for: " + _UUID);
                auto decrypted = _CryptoLib->decryptMessage(payload);
                std::string jsonStr(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
                spdlog::info("Decrypted JSON string: '{}'", jsonStr);
                if (decrypted.empty())
                {
                    std::cout << "Client " << getUUID() << ": расшифровка не удалась, пропускаем пакет\n";
                    buffer.erase(buffer.begin(), buffer.begin() + totalSize);
                    continue;
                }
                payload = std::move(decrypted);
            }

            CLIENT_INFO("Run parse JSON: " + _UUID);
            JsonObject jsonPacket;
            try
            {
                jsonPacket = JsonObject::parse(payload.begin(), payload.end());
            }
            catch (const nlohmann::json::exception& e)
            {
                std::cout << "Client " << getUUID() << ": ошибка парсинга JSON: " << e.what() << "\n";
                buffer.erase(buffer.begin(), buffer.begin() + totalSize);
                continue;
            }


            if (!jsonPacket.contains("TypeMessage") || jsonPacket["TypeMessage"].is_null()) 
            {
                CLIENT_ERROR("Ошибка, JSON не полный!");
                buffer.erase(buffer.begin(), buffer.begin() + totalSize);
                continue;
            }

            ETypeOfMessage msgType = static_cast<ETypeOfMessage>(jsonPacket["TypeMessage"].get<int>());

            if (msgType == ETypeOfMessage::Crypto)
            {
                bool ok = handleCryptoHandshake(jsonPacket);
                if (!ok)
                {
                    std::cout << "Client " << getUUID() << ": не удалось вычислить общий секрет\n";
                    disconnect();
                    co_return;
                }

                buffer.erase(buffer.begin(), buffer.begin() + totalSize);
                continue;
            }

            co_await _Core._ClientDispatcher->dispatch(msgType, jsonPacket, shared_from_this());
            buffer.erase(buffer.begin(), buffer.begin() + totalSize);
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Client " << getUUID() << " readLoop exception: " << e.what() << "\n";
        disconnect();
    }
}
bool Client::handleCryptoHandshake(const JsonObject& json)
{
    auto pubKeyBase64 = json["PublicKey"].get<string>();
    std::vector<uint8_t> peerKeyBytes = ToolLib::decodeBase64(pubKeyBase64);
    return _CryptoLib->computeSharedSecret(peerKeyBytes);
}