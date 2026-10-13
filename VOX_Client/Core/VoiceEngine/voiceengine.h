#ifndef VOICEENGINE_H
#define VOICEENGINE_H
#pragma once

#include <QObject>
#include <QDebug>
#include <memory>
#include <QMetaObject>
#include <QAudioSource>
#include <QMediaDevices>
#include <QIODevice>
#include <QtConcurrent>
#include <QAudioSink>
#include "miccapture.h"

// ПОДКЛЮЧАЕМ ОФИЦИАЛЬНЫЕ ХЕДЕРЫ LIVEKIT SDK
#include "livekit/livekit.h"
#include "livekit/room.h"
#include "livekit/track.h"
#include "livekit/room_event_types.h"
#include "livekit/room_delegate.h"
#include "livekit/participant.h"
#include "livekit/remote_audio_track.h"
#include "voiceroomdelegate.h"


class QVoiceEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentRoomGUID READ currentRoomGUID NOTIFY voiceRoomChanged)
    Q_PROPERTY(bool isMuted READ isMuted NOTIFY muteStatusChanged)

private:
    QString m_currentRoomGUID;
    QString m_liveKitServerUrl = "ws://212.109.198.185:7880";
    bool m_isMicrophoneMuted = false;

    // ЖИВЫЕ ОБЪЕКТЫ LIVEKIT SDK
    std::shared_ptr<livekit::Room> m_room;
    std::shared_ptr<livekit::LocalAudioTrack> m_localAudioTrack;
    std::shared_ptr<livekit::AudioSource> m_audioSource;
    QMicCapture* m_MicCapture = nullptr;

    // ИСПРАВЛЕНО: Делегат теперь хранится как монолитный объект класса.
    // Он гарантированно создан и НЕ равен nullptr в памяти никогда!
    VoiceRoomDelegate m_roomDelegate;

    explicit QVoiceEngine(QObject *parent = nullptr) : QObject(parent)
    {
        // Инициализируем WebRTC фабрики кодеков
        livekit::initialize();
        livekit::initialize(livekit::LogLevel::Debug);
        m_room = std::make_shared<livekit::Room>();
    };

    QThread m_playbackThread;
    QVector<PlaybackWorker*> m_workers;

public:
    void stopAll()
    {
        // 1. Сначала переводим флаг воркеров в false, чтобы циклы в потоках завершились
        for (auto* w : m_workers)
        {
            // Напрямую меняем атомарный флаг (если m_running public) или вызываем неблокирующий stop.
            QMetaObject::invokeMethod(w, "stop", Qt::QueuedConnection);
        }

        // 2. Гасим поток воспроизведения Qt
        if (m_playbackThread.isRunning())
        {
            m_playbackThread.quit();
            m_playbackThread.wait(); // Ждем завершения цикла событий Qt
        }

        // 3. Безопасно удаляем воркеров через планировщик Qt
        for (auto* w : m_workers)
        {
            w->deleteLater();
        }
        m_workers.clear();
    }

    static QVoiceEngine& getInstance() {
        static QVoiceEngine instance(nullptr);
        return instance;
    }
    ~QVoiceEngine()
    {
        if (m_MicCapture)
        {
            m_MicCapture->stop();
            m_MicCapture->deleteLater(); // безопаснее, чем delete вручную
            m_MicCapture = nullptr;
        }
    }
    QString currentRoomGUID() const { return m_currentRoomGUID; }
    bool isMuted() const { return m_isMicrophoneMuted; }

    void handleSubscribedTrack(std::shared_ptr<livekit::Track> track)
    {
        // здесь: AudioStream::fromTrack, QAudioSink, PlaybackWorker, moveToThread —
        // всё то, что сейчас лежит в onTrackSubscribed
        if (!track || track->kind() != livekit::TrackKind::KIND_AUDIO)
            return;

        livekit::AudioStream::Options opts;
        opts.capacity = 100; // кольцевой буфер ~1 сек кадров, старые дропаются

        std::shared_ptr<livekit::AudioStream> stream =
            livekit::AudioStream::fromTrack(track, opts);

        auto* reader = new PlaybackWorker(stream, nullptr);
        reader->moveToThread(&m_playbackThread);
        if (!m_playbackThread.isRunning())
            m_playbackThread.start();
        QMetaObject::invokeMethod(reader, "start", Qt::QueuedConnection);

        m_workers.append(reader);
    }

    Q_INVOKABLE void toggleMicrophone()
    {
        m_isMicrophoneMuted = !m_isMicrophoneMuted;

        // 1. Управляем самим WebRTC треком (для индикации у других участников)
        if (m_localAudioTrack)
        {
            m_localAudioTrack->setMuted(m_isMicrophoneMuted);
        }

        // 2. ИСПРАВЛЕНИЕ: Физически прекращаем слать байты из нашего захватчика
        if (m_MicCapture)
        {
            m_MicCapture->setMuted(m_isMicrophoneMuted);
        }

        emit muteStatusChanged();
    }

    void connectToVoiceServer(const QString& roomGUID, const QString& jwtToken)
    {
        // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Если мы уже в канале — жестко и полностью сбрасываем старую сессию!
        if (!m_currentRoomGUID.isEmpty()) {
            qInfo() << "VOICE ENGINE | Обнаружено активное подключение. Сброс старой сессии перед переподключением...";
            disconnectFromVoiceServer();
        }

        qDebug() << "token len:" << jwtToken.size();
        qDebug() << "jwtToken:" << jwtToken;
        m_currentRoomGUID = roomGUID;
        emit voiceRoomChanged();

        qInfo() << "VOICE ENGINE | Запуск асинхронного WebRTC потока...";

        m_room->setDelegate(&m_roomDelegate);

        livekit::RoomOptions options;
        options.auto_subscribe = true;

        bool success = m_room->connect(m_liveKitServerUrl.toStdString(),
                                       jwtToken.toStdString(), options);

        if (!success) {
            qCritical() << "VOICE ENGINE CRITICAL | Ошибка WebRTC подключения к LiveKit!";
            m_currentRoomGUID = "";
            emit voiceRoomChanged();
            return;
        }

        qInfo() << "VOICE ENGINE | Успешный сетевой Handshake с медиа-сервером!";

        auto localParticipant = m_room->localParticipant().lock();
        if (localParticipant)
        {
            qInfo() << "VOICE ENGINE | Инициализация аудиокарты Windows и захват микрофона...";

            m_audioSource = std::make_shared<livekit::AudioSource>(48000, 1);

            livekit::TrackPublishOptions pub;
            pub.source = livekit::TrackSource::SOURCE_MICROPHONE;

            m_localAudioTrack = livekit::LocalAudioTrack::createLocalAudioTrack("microphone", m_audioSource);

            if (m_localAudioTrack)
            {
                localParticipant->publishTrack(m_localAudioTrack, pub);

                QMetaObject::invokeMethod(this, [this]() {
                    // Страховка от утечки памяти старого захватчика
                    if (m_MicCapture) {
                        m_MicCapture->stop();
                        m_MicCapture->deleteLater();
                    }
                    m_MicCapture = new QMicCapture(m_audioSource);
                    m_MicCapture->setMuted(m_isMicrophoneMuted); // Учитываем ваш первый вариант мута
                    m_MicCapture->start();
                }, Qt::QueuedConnection);

                m_localAudioTrack->setMuted(m_isMicrophoneMuted);
                qInfo() << "VOICE ENGINE | Микрофон Windows успешно захвачен и опубликован в WebRTC!";
            }
            else
            {
                qCritical() << "VOICE ENGINE ERROR | Не удалось инициализировать LocalAudioTrack!";
            }
        }
    }

    void disconnectFromVoiceServer()
    {
        qDebug() << "VOICE ENGINE | Закрытие медиа-сессии.";

        stopAll();

        if (m_room) {
            m_room->disconnect();
        }

        if (m_localAudioTrack) {
            m_localAudioTrack.reset();
        }

        if (m_audioSource) {
            m_audioSource.reset();
        }

        if (m_MicCapture)
        {
            m_MicCapture->stop();
            m_MicCapture->deleteLater(); // безопаснее, чем delete вручную
            m_MicCapture = nullptr;
        }
        m_currentRoomGUID = "";
        emit voiceRoomChanged();
    }

signals:
    void voiceRoomChanged();
    void muteStatusChanged();
};
#endif // VOICEENGINE_H
