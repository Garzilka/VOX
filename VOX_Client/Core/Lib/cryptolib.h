#ifndef CRYPTOLIB_H
#define CRYPTOLIB_H

#include <QObject>
#include <QByteArray>
#include <QIODevice>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <QDebug>

class QCryptoLib : public QObject
{
    Q_OBJECT

public:
    explicit QCryptoLib(QObject *parent = nullptr);
    ~QCryptoLib();

    // Сгенерировать пару ключей ECDH (P-256)
    bool generateKeyPair();

    // Экспортировать публичный ключ в DER (для отправки собеседнику)
    QByteArray getPublicKeyDER() const;

    // Принять чужой публичный ключ (DER) и вычислить общий секрет
    bool computeSharedSecret(const QByteArray &peerPublicKeyDER);

    // Шифрование: [nonce 12 байт][ciphertext + tag 16 байт]
    QByteArray encryptMessage(const QByteArray &plaintext) const;

    // Расшифровка
    QByteArray decryptMessage(const QByteArray &encrypted) const;

    bool hasSharedSecret() const { return m_hasSecret; }

    QByteArray generateSalt(int length = 32);
    QByteArray hashPassword(const QString &password, const QByteArray &salt);
    bool verifyPassword(const QString &password, const QByteArray &salt, const QByteArray &storedHash);

private:
    EVP_PKEY *m_keyPair = nullptr;          // своя пара ключей
    EVP_PKEY *m_peerKey = nullptr;          // чужой публичный ключ
    QByteArray m_sharedSecret;              // вычисленный общий секрет
    QByteArray m_aesKey;                    // ключ AES-256 (32 байта)
    bool m_hasSecret = false;

    static QByteArray evpPkeyToDer(EVP_PKEY *key, bool publicKey);
    static EVP_PKEY* derToEvpPkey(const QByteArray &der, bool publicKey);
    static QByteArray sha256(const QByteArray &data);
};

#endif // CRYPTOLIB_H
