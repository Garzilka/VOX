#ifndef LAUNCHERCONTROLLER_H
#define LAUNCHERCONTROLLER_H
#pragma once

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>

class QLauncherController : public QObject
{
    Q_OBJECT

    // Свойства для реактивной привязки в QML
    Q_PROPERTY(int uiState READ uiState NOTIFY uiStateChanged) // 0 - Поиск обновлений, 1 - Скачивание
    Q_PROPERTY(double downloadProgress READ downloadProgress NOTIFY downloadProgressChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString versionName READ versionName NOTIFY versionNameChanged)

private:
    int m_uiState = 0;
    double m_downloadProgress = 0.0;
    QString m_statusText = "Проверка обновлений...";
    QString m_currentVersionName = "0.0.0.1";
    QTcpSocket* m_tcpSocket;
    QNetworkAccessManager* m_httpManager;
    QNetworkReply* m_httpReply;

    // ХАРДКОД ЛОКАЛЬНОЙ ВЕРСИИ ЛАУНЧЕРА (При каждой сборке нового релиза увеличиваем это число)
    int m_currentVersionCode = 100; // Теперь значение будет считываться из файла
    int m_latestServerVersionCode = 100; // Сюда сохраним то, что пришлет сервер
    QString m_configPath;
    const QString m_platform = "windows"; // 'windows', 'linux', 'macos'

    QString m_downloadUrl;
    QString m_expectedHash;
    QString m_archivePath;


    explicit QLauncherController(QObject *parent = nullptr) : QObject(parent)
    {
        m_tcpSocket = new QTcpSocket(this);
        m_httpManager = new QNetworkAccessManager(this);
        m_archivePath = QDir::currentPath() + "/update.zip";

        // Путь к файлу config.ini в папке с лаунчером
        m_configPath = QDir::currentPath() + "/config.ini";

        // Читаем текущую локальную версию из конфига
        // Если файла нет, QSettings автоматически вернет дефолтное значение 100
        QSettings settings(m_configPath, QSettings::IniFormat);
        m_currentVersionCode = settings.value("App/version_code", 100).toInt();
        m_currentVersionName = settings.value("App/version_name", "0.0.0.1").toString();
        qInfo() << "LAUNCHER | Локальная версия из config.ini:" << m_currentVersionCode << "(" << m_currentVersionName << ")";

        connect(m_tcpSocket, &QTcpSocket::connected, this, &QLauncherController::onTcpConnected);
        connect(m_tcpSocket, &QTcpSocket::readyRead, this, &QLauncherController::onTcpReadyRead);

        m_tcpSocket->connectToHost("212.109.198.185", 55);
    }
public:
    QString versionName() const { return m_currentVersionName; }

    QLauncherController& operator=(const QLauncherController&) = delete;
    QLauncherController(const QLauncherController&) = delete;
    static QLauncherController& getInstance()
    {
        static QLauncherController instance(nullptr);
        return instance;
    };

    int uiState() const { return m_uiState; }
    double downloadProgress() const { return m_downloadProgress; }
    QString statusText() const { return m_statusText; }

private:
    bool m_isUpdateRequired = false;
    bool m_isTimerFinished = false;
    QByteArray m_networkBuffer;
    quint32 m_expectedPacketSize = 0;

    void onTcpConnected()
    {
        // Сразу запускаем таймер на 1 секунду для удержания экрана загрузки
        QTimer::singleShot(1000, this, [this]() {
            m_isTimerFinished = true;
            // Если проверка сети уже прошла и обновлений не нужно — запускаем чат
            if (!m_isUpdateRequired && m_tcpSocket->state() == QAbstractSocket::UnconnectedState) {
                launchMainClientAndQuit();
            }
        });

        // Отправляем пакет запроса версии (Команда 23)
        QJsonObject packet;
        packet["TypeMessage"] = 23;
        packet["Response"] = false;
        packet["ClientVersionCode"] = m_currentVersionCode;
        packet["Platform"] = m_platform;

        QJsonDocument doc(packet);
        QByteArray rawData = doc.toJson(QJsonDocument::Compact);

        quint32 size = rawData.size();
        m_tcpSocket->write(reinterpret_cast<const char*>(&size), 4);
        m_tcpSocket->write(rawData);
        m_tcpSocket->flush();
    }

    void onTcpReadyRead()
    {
        // 1. Дописываем все вновь пришедшие байты в наш накопительный буфер
        m_networkBuffer.append(m_tcpSocket->readAll());

        // 2. Запускаем цикл обработки, пока в буфере есть хотя бы заголовок размера (4 байта)
        while (m_networkBuffer.size() >= 4)
        {
            // Читаем размер пакета из первых 4 байт
            quint32 packetSize = *reinterpret_cast<const quint32*>(m_networkBuffer.constData());

            // Защита от аномально больших пакетов (как в QConnection::ReadCurrentPacket)
            if (packetSize > 10 * 1024 * 1024)
            {
                qWarning() << "LAUNCHER | Слишком большой пакет, разрыв соединения";
                m_tcpSocket->disconnectFromHost();
                m_networkBuffer.clear();
                return;
            }

            // Если весь пакет (4 байта заголовка + тело) еще не дополз целиком —
            // выходим из цикла и ждем следующего события readyRead от сокета
            if (m_networkBuffer.size() < static_cast<int>(4 + packetSize))
            {
                return;
            }

            // ПАКЕТ ДОЛЕТЕЛ ПОЛНОСТЬЮ! Вырезаем чистый JSON (пропуская первые 4 байта)
            QByteArray jsonData = m_networkBuffer.mid(4, packetSize);

            // ВАЖНО: Удаляем из буфера ТОЛЬКО обработанный пакет, оставляя остальное на следующий шаг цикла
            m_networkBuffer.remove(0, 4 + packetSize);

            // Парсим JSON
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
            if (error.error != QJsonParseError::NoError)
            {
                qWarning() << "LAUNCHER | Ошибка парсинга JSON:" << error.errorString();
                continue; // Пробуем читать следующий пакет, если он есть
            }

            QJsonObject response = doc.object();
            int msgType = response["TypeMessage"].toInt();

            // Пропускаем и игнорируем проактивный пакет крипто-ключей (тип 5 или любой кроме 24)
            if (msgType != 24)
            {
                qDebug() << "LAUNCHER | Пропущен системный пакет (Type" << msgType << "). Ожидаем пакет версии...";
                continue; // Переходим к следующему пакету в цикле while
            }

            // Обработка целевого пакета версии (TypeMessage == 24)
            qDebug() << "LAUNCHER | Получен ПОЛНЫЙ JSON от сервера:" << doc.toJson(QJsonDocument::Compact);

            m_isUpdateRequired = response["Response"].toBool();

            if (m_isUpdateRequired)
            {
                m_downloadUrl = response["DownloadUrl"].toString();
                m_expectedHash = response["Sha256Hash"].toString();
                m_latestServerVersionCode = response["LatestVersionCode"].toInt();
                m_currentVersionName = response["LatestVersionName"].toString();
                emit versionNameChanged(); // Обновляем UI

                if (m_downloadUrl.isEmpty() || !m_downloadUrl.contains("://"))
                {
                    m_statusText = "Ошибка: Сервер прислал невалидный URL!";
                    emit statusTextChanged();
                    m_tcpSocket->disconnectFromHost();
                    return;
                }

                m_tcpSocket->disconnectFromHost();
                startHttpDownload();
                return; // Завершаем выполнение, так как уходим на скачивание
            }
            else
            {
                m_tcpSocket->disconnectFromHost();
                if (m_isTimerFinished)
                {
                    launchMainClientAndQuit();
                } else
                {
                    m_statusText = "Запуск Vox...";
                    emit statusTextChanged();
                }
                return; // Завершаем выполнение
            }
        }
    }


    void startHttpDownload()
    {
        m_uiState = 1; // Переключаем стейт интерфейса на стиль "Epic Games Launcher" (окно расширится)
        m_statusText = "Скачивание обновления...";
        emit uiStateChanged();
        emit statusTextChanged();

        QNetworkRequest request((QUrl(m_downloadUrl)));
        m_httpReply = m_httpManager->get(request);

        // Подключаем асинхронные сигналы HTTP-скачивания с прогресс-баром
        connect(m_httpReply, &QNetworkReply::downloadProgress, this, &QLauncherController::onDownloadProgress);
        connect(m_httpReply, &QNetworkReply::finished, this, &QLauncherController::onDownloadFinished);
    }

    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
    {
        if (bytesTotal > 0) {
            m_downloadProgress = static_cast<double>(bytesReceived) / bytesTotal;
            emit downloadProgressChanged(); // QML Прогресс-бар плавно задвигается
        }
    }

    void onDownloadFinished()
    {
        if (m_httpReply->error() != QNetworkReply::NoError) {
            m_statusText = "Ошибка скачивания: " + m_httpReply->errorString();
            emit statusTextChanged();
            return;
        }

        // Сохраняем скачанный ZIP на диск
        QFile file(m_archivePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_httpReply->readAll());
            file.close();
        }
        m_httpReply->deleteLater();

        // 4. Проверяем целостность файла по SHA-256
        if (!verifyArchiveHash()) {
            m_statusText = "Критическая ошибка: Хэш файла не совпадает!";
            emit statusTextChanged();
            return;
        }

        m_statusText = "Установка обновления...";
        emit statusTextChanged();

        // 5. Распаковываем и заменяем файлы
        if (extractAndInstallUpdate())
        {
            launchMainClientAndQuit();
        } else {
            m_statusText = "Не удалось распаковать файлы обновления.";
            emit statusTextChanged();
        }
    }

    bool verifyArchiveHash()
    {
        QFile file(m_archivePath);
        if (!file.open(QIODevice::ReadOnly)) return false;

        QCryptographicHash hash(QCryptographicHash::Sha256);
        if (hash.addData(&file)) {
            QString calculatedHash = hash.result().toHex();
            return (calculatedHash.toLower() == m_expectedHash.toLower());
        }
        return false;
    }

    bool extractAndInstallUpdate()
    {
        m_statusText = "Очистка старой версии приложения...";
        emit statusTextChanged();

        // Шаг 1: Производим умную очистку перед распаковкой архива
        cleanCurrentDirectory(QDir::currentPath());

        m_statusText = "Распаковка пакетов обновления...";
        emit statusTextChanged();

        // Шаг 2: Распаковываем новый релиз из архива через tar
        QStringList arguments;
        arguments << "-xf" << m_archivePath << "-C" << QDir::currentPath();

        int exitCode = QProcess::execute("tar", arguments);

        // Удаляем временный архив за собой после успешной или неуспешной установки
        QFile::remove(m_archivePath);

        if (exitCode == 0)
        {
            QSettings settings(m_configPath, QSettings::IniFormat);
            settings.setValue("App/version_code", m_latestServerVersionCode);
            settings.setValue("App/version_name", m_currentVersionName); // Динамически пишем имя версии с сервера
            settings.sync();
            emit versionNameChanged(); // Обновляем UI

            qInfo() << "LAUNCHER | Конфиг обновлен. Текущая версия записана:" << m_latestServerVersionCode << "(" << m_currentVersionName << ")";
            return true;
        }

        qCritical() << "LAUNCHER ERROR | Не удалось распаковать файлы через системный tar. Код:" << exitCode;
        return false;
    }

    void launchMainClientAndQuit()
    {
        // Запускаем обновленный или актуальный основной мессенджер в независимом процессе
#ifdef Q_OS_WIN
        QProcess::startDetached(QDir::currentPath() + "/VOX_Client.exe");
#else
        QProcess::startDetached(QDir::currentPath() + "/VOX_Client");
#endif
        // Полностью гасим лаунчер, он выполнил свою задачу
        QCoreApplication::quit();
    }
    void cleanCurrentDirectory(const QString& path)
    {
        QDir dir(path);
        if (!dir.exists()) return;

        // Получаем имя текущего запущенного бинарника лаунчера
        QString launcherExeName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
        QString configName = QFileInfo(m_configPath).fileName();

        // =====================================================================
        // ОБНОВЛЕНО: Белый список файлов, которые КАТЕГОРИЧЕСКИ нельзя удалять
        // =====================================================================
        QStringList protectedFiles = {
            launcherExeName,    // Сам Лаунчер
            configName,         // config.ini (файл текущей версии)
            "update.zip",       // Сам архив обновления (нужен для tar)
            "unins000.exe",     // Исполняемый файл деинсталлятора Inno Setup
            "unins000.dat"      // База данных деинсталлятора (список удаляемых файлов)
        };

        // Настраиваем фильтры: файлы, папки, скрытые элементы, исключая ссылки "." и ".."
        dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
        QFileInfoList list = dir.entryInfoList();

        for (const QFileInfo& fileInfo : list)
        {
            // Если имя файла или папки в белом списке — пропускаем её
            if (protectedFiles.contains(fileInfo.fileName())) {
                continue;
            }

            if (fileInfo.isDir())
            {
                // Рекурсивно чистим поддиректорию
                cleanCurrentDirectory(fileInfo.absoluteFilePath());
                // Удаляем саму пустую папку
                dir.rmdir(fileInfo.fileName());
            }
            else
            {
                // Удаляем файл
                dir.remove(fileInfo.fileName());
            }
        }
    }
signals:
    void uiStateChanged();
    void statusTextChanged();
    void downloadProgressChanged();
    void versionNameChanged();
};

#endif // LAUNCHERCONTROLLER_H
