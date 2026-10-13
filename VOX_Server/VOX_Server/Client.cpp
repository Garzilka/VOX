#include "Client.h"
#include "Core.h"
#include "Libs/Tools/ToolLib.h"
#include "Dispatch/Client_Dispatcher.h"

Client::Client(TCPSoket socket, Core& _Core)
    : _socket(std::move(socket)), _Core(_Core), _writeTimer(_socket.get_executor()) 
{
    _writeTimer.expires_at(std::chrono::steady_clock::time_point::max());
}


Client::~Client()
{
    disconnect();
}

void Client::start() 
{
    if (!_CryptoLib)
    {
        _CryptoLib = std::make_shared<CryptoLib>();
    }
    if (!_CryptoLib->generateKeyPair()) return;

    auto executor = _socket.get_executor();
    asio::co_spawn(_socket.get_executor(), readLoop(), asio::detached);
    asio::co_spawn(_socket.get_executor(), WriteLoop(), asio::detached);

    SCyptsKeysData CryptoMessage(ToolLib::toBase64(_CryptoLib->getPublicKeyDER()));
    sendMessage(CryptoMessage);

}


void Client::disconnect()
{
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
    if (!_socket.is_open()) return;

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

    // Записываем длину в формате Big-Endian (сетевой порядок)
    packet.push_back((size >> 24) & 0xFF);
    packet.push_back((size >> 16) & 0xFF);
    packet.push_back((size >> 8) & 0xFF);
    packet.push_back(size & 0xFF);
    packet.insert(packet.end(), payload.begin(), payload.end());

    asio::post(_socket.get_executor(), [this, p = std::move(packet)]() mutable {
        _writeQueue.push(std::move(p));
        _writeTimer.cancel_one(); // Пробуждаем writeLoop, если он спал
        });
}

asio::awaitable<void> Client::WriteLoop()
{
    try
    {
        for (;;)
        {
            if (_writeQueue.empty())
            {
                _writeTimer.expires_at(std::chrono::steady_clock::time_point::max());
                std::error_code ec;
                co_await _writeTimer.async_wait(asio::redirect_error(asio::use_awaitable, ec));
            }

            if (!_socket.is_open()) co_return;

            std::vector<uint8_t> currentPacket;

            if (!_writeQueue.empty())
            {
                currentPacket = std::move(_writeQueue.front());
                _writeQueue.pop();
            }
            else
            {
                continue;
            }

            // Безопасно отправляем: currentPacket гарантированно живет до конца строки
            co_await asio::async_write(_socket, asio::buffer(currentPacket), asio::use_awaitable);
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Client " << getUUID() << " writeLoop exception: " << e.what() << "\n";
        disconnect();
    }
}


asio::awaitable<void> Client::readLoop()
{
    try
    {
        // Входной буфер: храним «хвосты» между чтениями
        std::vector<char> buffer;
        buffer.reserve(4096);

        for (;;)
        {
            if (buffer.size() < 4)
            {
                buffer.resize(4096); // временный буфер для чтения
                std::size_t n = co_await _socket.async_read_some(asio::buffer(buffer), asio::use_awaitable);
                buffer.resize(n);    // реальная длина прочитанного

                if (n == 0)
                {
                    disconnect();
                    co_return;
                }
                // Дальше будем копировать в основной буфер
            }

            // Читаем длину пакета (4 байта)
            uint32_t size = 0;
            std::memcpy(&size, buffer.data(), 4);

            constexpr uint32_t MAX_PACKET_SIZE = 10 * 1024 * 1024;
            if (size > MAX_PACKET_SIZE)
            {
                std::cout << "Client " << getUUID() << ": слишком большой пакет, разрыв соединения\n";
                disconnect();
                co_return;
            }

            // Проверяем, хватает ли данных для полного пакета
            const uint32_t totalSize = 4 + size;
            if (buffer.size() < totalSize)
            {
                // Не хватает — читаем ещё, пока не доберёмся до полного пакета
                while (buffer.size() < totalSize)
                {
                    buffer.resize(totalSize); // резервируем место
                    std::size_t need = totalSize - buffer.size();
                    // Читаем только недостающее
                    auto readBuffer = asio::buffer(buffer.data() + buffer.size(), need);
                    std::size_t n = co_await _socket.async_read_some(readBuffer, asio::use_awaitable);

                    if (n == 0)
                    {
                        std::cout << "Client " << getUUID() << ": соединение разорвано при чтении пакета\n";
                        disconnect();
                        co_return;
                    }
                    buffer.resize(buffer.size() + n);
                }
            }

            const char* payloadPtr = buffer.data() + 4;
            std::vector<uint8_t> payload(payloadPtr, payloadPtr + size);

            // Шифрование/дешифрование
            if (_CryptoLib->hasSharedSecret())
            {
                auto decrypted = _CryptoLib->decryptMessage(payload);
                if (decrypted.empty())
                {
                    std::cout << "Client " << getUUID() << ": расшифровка не удалась, пропускаем пакет\n";
                    // Удаляем обработанный пакет из начала буфера
                    buffer.erase(buffer.begin(), buffer.begin() + totalSize);
                    continue; // следующий цикл
                }
                payload = std::move(decrypted);
            }

            // Парсим JSON
            JsonObject jsonPacket;
            try
            {
                jsonPacket = JsonObject::parse(
                    reinterpret_cast<const char*>(payload.data()),
                    reinterpret_cast<const char*>(payload.data() + payload.size())
                );
            }
            catch (const nlohmann::json::exception& e)
            {
                std::cout << "Client " << getUUID() << ": ошибка парсинга JSON: " << e.what() << "\n";
                buffer.erase(buffer.begin(), buffer.begin() + totalSize);
                continue;
            }

            // Достаём тип сообщения
            ETypeOfMessage msgType = static_cast<ETypeOfMessage>(jsonPacket["TypeMessage"].get<int>());

            // Обработка Crypto-сообщения (обмен ключами)
            if (msgType == ETypeOfMessage::Crypto)
            {
                // Здесь логика как в Qt: парсим ключи, вычисляем общий секрет
                // Если всё ок — можно даже не вызывать dispatch, а сразу вернуть
                bool ok = handleCryptoHandshake(jsonPacket);
                if (!ok)
                {
                    std::cout << "Client " << getUUID() << ": не удалось вычислить общий секрет\n";
                    disconnect();
                    co_return;
                }

                // Удаляем обработанный пакет
                buffer.erase(buffer.begin(), buffer.begin() + totalSize);
                continue;
            }

            // Диспетчеризация

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