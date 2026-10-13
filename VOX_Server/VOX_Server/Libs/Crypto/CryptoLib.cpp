#include "CryptoLib.h"


CryptoLib::~CryptoLib()
{
    if (m_keyPair) EVP_PKEY_free(m_keyPair);
    if (m_peerKey) EVP_PKEY_free(m_peerKey);
}

bool CryptoLib::generateKeyPair()
{
    if (m_keyPair)
    {
        EVP_PKEY_free(m_keyPair);
        m_keyPair = nullptr;
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx) {
        cout << "Не удалось создать контекст EC";
        return false;
    }

    bool ok = false;
    if (EVP_PKEY_keygen_init(ctx) <= 0)
    {
        cout << "Не удалось инициализировать keygen";
    }
    else if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0)
    {
        cout << "Не удалось установить кривую P-256";
    }
    else if (EVP_PKEY_keygen(ctx, &m_keyPair) <= 0)
    {
        cout << "Не удалось сгенерировать пару ключей";
    }
    else
    {
        ok = true;
    }

    EVP_PKEY_CTX_free(ctx);
    return ok;
}

ByteArray CryptoLib::getPublicKeyDER() const
{
    if (!m_keyPair) return {};

    return evpPkeyToDer(m_keyPair, true);
}

bool CryptoLib::computeSharedSecret(const ByteArray& peerPublicKeyDER)
{
    if (!m_keyPair)
    {
        cout << "Своя пара ключей не сгенерирована";
        return false;
    }

    // Парсим чужой публичный ключ из DER
    m_peerKey = derToEvpPkey(peerPublicKeyDER, true);
    if (!m_peerKey)
    {
        cout << "Не удалось загрузить чужой публичный ключ";
        return false;
    }

    // Вычисляем общий секрет через EVP_PKEY_derive
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(m_keyPair, nullptr);
    if (!ctx)
    {
        cout << "Не удалось создать контекст derive";
        return false;
    }

    bool ok = false;
    if (EVP_PKEY_derive_init(ctx) <= 0)
    {
        cout << "Не удалось инициализировать derive";
    }
    else if (EVP_PKEY_derive_set_peer(ctx, m_peerKey) <= 0)
    {
        cout << "Не удалось установить peer key";
    }
    else
    {
        size_t secretLen = 0;
        if (EVP_PKEY_derive(ctx, nullptr, &secretLen) > 0 && secretLen > 0)
        {
            m_sharedSecret.resize(int(secretLen));
            if (EVP_PKEY_derive(ctx,
                reinterpret_cast<unsigned char*>(m_sharedSecret.data()),
                &secretLen) > 0)
            {
                // Выводим ключ AES-256 из общего секрета через SHA-256
                m_aesKey = sha256(m_sharedSecret);
                m_hasSecret = true;
                ok = true;
            }
        }
    }

    EVP_PKEY_CTX_free(ctx);
    return ok;
}

ByteArray CryptoLib::encryptMessage(const ByteArray& plaintext) const
{
    // Проверка состояния
    if (!m_hasSecret || m_aesKey.size() != 32) return {};
    

    // 1. Генерируем nonce (12 байт для AES-GCM)
    unsigned char nonce[12];
    if (RAND_bytes(nonce, sizeof(nonce)) != 1) return {};

    // 2. Создаём и инициализируем контекст шифрования
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};    

    const unsigned char* keyPtr = reinterpret_cast<const unsigned char*>(m_aesKey.data());

    // Инициализация EVP для AES-256-GCM
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, keyPtr, nonce) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 3. Выделяем буфер под шифротекст: максимум plaintext.size() байт
    ByteArray ciphertext;
    ciphertext.resize(plaintext.size());

    int outLen = 0;
    const unsigned char* inPtr = reinterpret_cast<const unsigned char*>(plaintext.data());

    // Обновление: шифруем основной блок
    if (EVP_EncryptUpdate(ctx,
        ciphertext.data(),
        &outLen,
        inPtr,
        static_cast<int>(plaintext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 4. Завершение шифрования: получаем остаток (если есть)
    int finalLen = 0;
    if (EVP_EncryptFinal_ex(ctx,
        ciphertext.data() + outLen,
        &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // Корректируем размер шифротекста
    ciphertext.resize(outLen + finalLen);

    // 5. Получаем GCM-тег (16 байт)
    unsigned char tag[16];
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, sizeof(tag), tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);

    // 6. Собираем результат: [nonce (12)][ciphertext][tag (16)]
    ByteArray result;
    result.reserve(sizeof(nonce) + ciphertext.size() + sizeof(tag));
    result.insert(result.end(), nonce, nonce + sizeof(nonce));
    result.insert(result.end(), ciphertext.begin(), ciphertext.end());
    result.insert(result.end(), tag, tag + sizeof(tag));

    return result;
}

ByteArray CryptoLib::decryptMessage(const ByteArray& encrypted) const
{
    if (!m_hasSecret || m_aesKey.size() != 32 || encrypted.size() < 12 + 16) {
        return {}; // минимум: nonce (12) + tag (16); шифротекст может быть пустым
    }

    const size_t nonceLen = 12;
    const size_t tagLen = 16;

    // Разбираем пакет: [nonce][ciphertext][tag]
    const unsigned char* noncePtr = encrypted.data();
    const unsigned char* cipherPtr = noncePtr + nonceLen;
    const size_t cipherSize = encrypted.size() - nonceLen - tagLen;
    const unsigned char* tagPtr = cipherPtr + cipherSize;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    const unsigned char* keyPtr = m_aesKey.data();

    // 1. Инициализация расшифровки с тем же ключом и nonce
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, keyPtr, noncePtr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 2. Устанавливаем тег ДО расшифровки.
    //    EVP_CTRL_GCM_SET_TAG требует неконстантный указатель, поэтому const_cast.
    //    Сами байты мы не меняем — OpenSSL только читает их.
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tagLen,
        const_cast<unsigned char*>(tagPtr)) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 3. Расшифровка основного блока
    ByteArray plaintext(cipherSize);
    int outLen = 0;

    if (EVP_DecryptUpdate(ctx,
        plaintext.data(),
        &outLen,
        cipherPtr,
        static_cast<int>(cipherSize)) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 4. Финализация: если тег не совпал — вернёт ошибку (это и есть проверка подлинности)
    int finalLen = 0;
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + outLen, &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {}; // битые данные или неверный ключ — пустой результат
    }

    EVP_CIPHER_CTX_free(ctx);

    plaintext.resize(static_cast<size_t>(outLen + finalLen));
    return plaintext;
}

ByteArray CryptoLib::generateSalt(int length)
{
    ByteArray salt(static_cast<size_t>(length));
    if (length <= 0) return {};

    if (RAND_bytes(salt.data(), length) != 1) {
        return {}; // генератор случайных чисел не смог — лучше упасть, чем дать слабую соль
    }
    return salt;
}

ByteArray CryptoLib::hashPassword(const string& password, const ByteArray& salt)
{
    const int iterations = 100000;
    const int keyLen = 32;

    ByteArray derived(static_cast<size_t>(keyLen));

    // password уже string (UTF-8) — конвертация не нужна,
    // если пароли хранятся/передаются как UTF-8 строки.
    int ok = PKCS5_PBKDF2_HMAC(
        password.c_str(),
        static_cast<int>(password.size()),
        salt.data(),
        static_cast<int>(salt.size()),
        iterations,
        EVP_sha256(),
        keyLen,
        derived.data()
    );

    if (ok != 1) return {};
    return derived;
}


ByteArray CryptoLib::sha256(const ByteArray& data)
{
    unsigned char hash[EVP_MAX_MD_SIZE]; // 64 байта — с запасом под любой дайджест
    unsigned int hashLen = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return {};

    ByteArray result;
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) == 1 &&
        EVP_DigestUpdate(ctx, data.data(), data.size()) == 1 &&
        EVP_DigestFinal_ex(ctx, hash, &hashLen) == 1)
    {
        result.assign(hash, hash + hashLen); // копируем ровно hashLen байт
    }

    EVP_MD_CTX_free(ctx);
    return result;
}


bool CryptoLib::verifyPassword(const string& password, const ByteArray& salt, const ByteArray& storedHash)
{
    ByteArray computed = hashPassword(password, salt);
    if (computed.empty() || computed.size() != storedHash.size()) return false;

    // CRYPTO_memcompare сравнивает за постоянное время (из libcrypto)
    return CRYPTO_memcmp(computed.data(), storedHash.data(), computed.size()) == 0;
}

ByteArray CryptoLib::evpPkeyToDer(EVP_PKEY* key, bool publicKey)
{
    if (!key) return {};

    int len = 0;
    if (publicKey) 
    {
        len = i2d_PUBKEY(key, nullptr); // Получаем необходимый размер буфера
    }
    else 
    {
        len = i2d_PrivateKey(key, nullptr);
    }

    if (len <= 0) return {};

    ByteArray der(static_cast<size_t>(len));
    unsigned char* p = reinterpret_cast<unsigned char*>(der.data());

    if (publicKey) 
    {
        i2d_PUBKEY(key, &p);
    }
    else 
    {
        i2d_PrivateKey(key, &p);
    }

    return der;
}

EVP_PKEY* CryptoLib::derToEvpPkey(const ByteArray& der, bool publicKey) 
{
    if (der.empty()) return nullptr;

    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(der.data());
    int len = static_cast<int>(der.size());

    // Создаем BIO из памяти (не новый, а "обертку" над существующими данными)
    BIO* bio = BIO_new_mem_buf(ptr, len);
    if (!bio) return nullptr;

    EVP_PKEY* key = nullptr;

    if (publicKey) {
        key = d2i_PUBKEY_bio(bio, nullptr); // nullptr означает "создать новый ключ"
    }
    else {
        key = d2i_PrivateKey_bio(bio, nullptr);
    }

    BIO_free(bio); // Освобождаем только BIO, ключ остается у нас
    return key;
}