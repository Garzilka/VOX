#pragma once
#include "Data/Common.h"
#include <openssl/x509.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/rand.h>
#include <openssl/err.h>

class CryptoLib
{

public:
    explicit CryptoLib() = default;
    ~CryptoLib();

    // Сгенерировать пару ключей ECDH (P-256)
    bool generateKeyPair();

    // Экспортировать публичный ключ в DER (для отправки собеседнику)
    ByteArray getPublicKeyDER() const;

    // Принять чужой публичный ключ (DER) и вычислить общий секрет
    bool computeSharedSecret(const ByteArray& peerPublicKeyDER);

    // Шифрование: [nonce 12 байт][ciphertext + tag 16 байт]
    ByteArray encryptMessage(const ByteArray& plaintext) const;

    // Расшифровка
    ByteArray decryptMessage(const ByteArray& encrypted) const;

    bool hasSharedSecret() const { return m_hasSecret; }

    static ByteArray generateSalt(int length = 32);
    static ByteArray hashPassword(const string& password, const ByteArray& salt);
    bool verifyPassword(const string& password, const ByteArray& salt, const ByteArray& storedHash);

private:
    EVP_PKEY* m_keyPair = nullptr;          // своя пара ключей
    EVP_PKEY* m_peerKey = nullptr;          // чужой публичный ключ
    ByteArray m_sharedSecret;              // вычисленный общий секрет
    ByteArray m_aesKey;                    // ключ AES-256 (32 байта)
    bool m_hasSecret = false;

    static ByteArray evpPkeyToDer(EVP_PKEY* key, bool publicKey);
    static EVP_PKEY* derToEvpPkey(const ByteArray& der, bool publicKey);
    static ByteArray sha256(const ByteArray& data);
};
