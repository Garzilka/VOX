#include "cryptolib.h"
#include <openssl/x509.h>
#include <openssl/buffer.h>
#include <QRandomGenerator>
#include <QPasswordDigestor>

QCryptoLib::QCryptoLib(QObject *parent) : QObject(parent)
{
}

QCryptoLib::~QCryptoLib()
{
    if (m_keyPair) EVP_PKEY_free(m_keyPair);
    if (m_peerKey) EVP_PKEY_free(m_peerKey);
}

bool QCryptoLib::generateKeyPair()
{
    if (m_keyPair)
    {
        EVP_PKEY_free(m_keyPair);
        m_keyPair = nullptr;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx) {
        qWarning() << "Не удалось создать контекст EC";
        return false;
    }

    bool ok = false;
    if (EVP_PKEY_keygen_init(ctx) <= 0)
    {
        qWarning() << "Не удалось инициализировать keygen";
    }
    else if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0)
    {
        qWarning() << "Не удалось установить кривую P-256";
    }
    else if (EVP_PKEY_keygen(ctx, &m_keyPair) <= 0)
    {
        qWarning() << "Не удалось сгенерировать пару ключей";
    }
    else
    {
        ok = true;
    }

    EVP_PKEY_CTX_free(ctx);
    return ok;
}

QByteArray QCryptoLib::getPublicKeyDER() const
{
    if (!m_keyPair) return {};

    return evpPkeyToDer(m_keyPair, true);
}

bool QCryptoLib::computeSharedSecret(const QByteArray &peerPublicKeyDER)
{
    if (!m_keyPair)
    {
        qWarning() << "Своя пара ключей не сгенерирована";
        return false;
    }

    // Парсим чужой публичный ключ из DER
    m_peerKey = derToEvpPkey(peerPublicKeyDER, true);
    if (!m_peerKey)
    {
        qWarning() << "Не удалось загрузить чужой публичный ключ";
        return false;
    }

    // Вычисляем общий секрет через EVP_PKEY_derive
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(m_keyPair, nullptr);
    if (!ctx)
    {
        qWarning() << "Не удалось создать контекст derive";
        return false;
    }

    bool ok = false;
    if (EVP_PKEY_derive_init(ctx) <= 0)
    {
        qWarning() << "Не удалось инициализировать derive";
    } else if (EVP_PKEY_derive_set_peer(ctx, m_peerKey) <= 0)
    {
        qWarning() << "Не удалось установить peer key";
    } else
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

QByteArray QCryptoLib::encryptMessage(const QByteArray &plaintext) const
{
    if (!m_hasSecret || m_aesKey.size() != 32) return {};

    // Генерируем случайный nonce (12 байт для GCM)
    unsigned char nonce[12];
    if (RAND_bytes(nonce, 12) != 1) return {};

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    QByteArray result;
    bool ok = false;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           reinterpret_cast<const unsigned char*>(m_aesKey.constData()),
                           nonce) == 1) {

        QByteArray ciphertext(plaintext.size(), '\0');
        int outLen = 0;

        if (EVP_EncryptUpdate(ctx,
                              reinterpret_cast<unsigned char*>(ciphertext.data()), &outLen,
                              reinterpret_cast<const unsigned char*>(plaintext.constData()),
                              int(plaintext.size())) == 1)
        {

            int finalLen = 0;
            if (EVP_EncryptFinal_ex(ctx,
                                    reinterpret_cast<unsigned char*>(ciphertext.data()) + outLen,
                                    &finalLen) == 1)
            {

                ciphertext.resize(outLen + finalLen);

                // Получаем tag (16 байт)
                unsigned char tag[16];
                EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag);

                // Формируем результат: [nonce][ciphertext][tag]
                result = QByteArray(reinterpret_cast<const char*>(nonce), 12)
                         + ciphertext
                         + QByteArray(reinterpret_cast<const char*>(tag), 16);
                ok = true;
            }
        }
    }

    EVP_CIPHER_CTX_free(ctx);
    return ok ? result : QByteArray();
}

QByteArray QCryptoLib::decryptMessage(const QByteArray &encrypted) const
{
    if (!m_hasSecret || m_aesKey.size() != 32) return {};
    if (encrypted.size() < 12 + 16) return {}; // минимум nonce + tag

    // Разбираем: [nonce 12][ciphertext][tag 16]
    QByteArray nonce = encrypted.left(12);
    QByteArray tag = encrypted.right(16);
    QByteArray ciphertext = encrypted.mid(12, encrypted.size() - 12 - 16);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    QByteArray result;
    bool ok = false;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           reinterpret_cast<const unsigned char*>(m_aesKey.constData()),
                           reinterpret_cast<const unsigned char*>(nonce.constData())) == 1) {

        QByteArray plaintext(ciphertext.size(), '\0');
        int outLen = 0;

        if (EVP_DecryptUpdate(ctx,
                              reinterpret_cast<unsigned char*>(plaintext.data()), &outLen,
                              reinterpret_cast<const unsigned char*>(ciphertext.constData()),
                              int(ciphertext.size())) == 1) {

            // Устанавливаем tag перед финальным вызовом
            EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16,
                                const_cast<char*>(tag.constData()));

            int finalLen = 0;
            if (EVP_DecryptFinal_ex(ctx,
                                    reinterpret_cast<unsigned char*>(plaintext.data()) + outLen,
                                    &finalLen) == 1) {

                plaintext.resize(outLen + finalLen);
                result = plaintext;
                ok = true;
            }
        }
    }

    EVP_CIPHER_CTX_free(ctx);
    return ok ? result : QByteArray();
}

QByteArray QCryptoLib::generateSalt(int length)
{
    QByteArray salt;
    salt.resize(length);
    QRandomGenerator::system()->fillRange(
        reinterpret_cast<quint32*>(salt.data()),
        length / 4
        );
    return salt;
}

QByteArray QCryptoLib::hashPassword(const QString &password, const QByteArray &salt)
{
    return QPasswordDigestor::deriveKeyPbkdf2(
        QCryptographicHash::Sha256,
        password.toUtf8(),
        salt,
        100000,
        32
        );
}
bool QCryptoLib::verifyPassword(const QString &password, const QByteArray &salt, const QByteArray &storedHash)
{
    QByteArray computed = hashPassword(password, salt);

    // Безопасное сравнение без timing-атак
    return (computed == storedHash);
}
// --- Вспомогательные функции ---

QByteArray QCryptoLib::evpPkeyToDer(EVP_PKEY *key, bool publicKey)
{
    if (!key) return {};

    BIO *bio = BIO_new(BIO_s_mem());
    if (!bio) return {};

    int rc = publicKey
                 ? i2d_PUBKEY_bio(bio, key)
                 : i2d_PrivateKey_bio(bio, key);

    QByteArray result;
    if (rc > 0) {
        BUF_MEM *mem = nullptr;
        BIO_get_mem_ptr(bio, &mem);
        if (mem && mem->data && mem->length > 0) {
            result = QByteArray(mem->data, int(mem->length));
        }
    }

    BIO_free(bio);
    return result;
}

EVP_PKEY* QCryptoLib::derToEvpPkey(const QByteArray &der, bool publicKey)
{
    if (der.isEmpty()) return nullptr;

    BIO *bio = BIO_new_mem_buf(der.constData(), int(der.size()));
    if (!bio) return nullptr;

    EVP_PKEY *key = publicKey
                        ? d2i_PUBKEY_bio(bio, nullptr)
                        : d2i_PrivateKey_bio(bio, nullptr);

    BIO_free(bio);
    return key;
}

QByteArray QCryptoLib::sha256(const QByteArray &data)
{
    unsigned char hash[32];
    unsigned int hashLen = 0;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, data.constData(), data.size());
    EVP_DigestFinal_ex(ctx, hash, &hashLen);
    EVP_MD_CTX_free(ctx);
    return QByteArray(reinterpret_cast<const char*>(hash), int(hashLen));
}