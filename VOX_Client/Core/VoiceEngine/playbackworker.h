#ifndef PLAYBACKWORKER_H
#define PLAYBACKWORKER_H

#include <QObject>
#include <QDebug>
#include <memory>
#include <QAudioSink>
#include <QMediaDevices>
#include <QIODevice>
#include <QByteArray>
#include <thread>
#include <atomic>

// ПОДКЛЮЧАЕМ ОФИЦИАЛЬНЫЕ ХЕДЕРЫ LIVEKIT SDK
#include "livekit/room.h"
#include "livekit/track.h"
#include "livekit/room_event_types.h"
#include "livekit/room_delegate.h"

class PlaybackWorker : public QObject {
    Q_OBJECT

public:
    PlaybackWorker(std::shared_ptr<livekit::AudioStream> stream, QObject* parent = nullptr)
        : QObject(parent), m_stream(std::move(stream)), m_running(false), m_sink(nullptr), m_sinkDevice(nullptr)
    {
    }

    ~PlaybackWorker()
    {
        stop();
        destroySink();
    }

public slots:
    void start()
    {
        if (m_running) return;
        m_running = true;

        // Запускаем низкоуровневый поток для блокирующего чтения из LiveKit SDK
        m_workerThread = std::thread(&PlaybackWorker::audioLoop, this);
        qDebug() << "VOICE PLAYBACK | Рабочий поток воспроизведения успешно запущен.";
    }

    void stop()
    {
        if (!m_running) return;
        m_running = false;

        if (m_stream)
        {
            m_stream.reset();
        }

        if (m_workerThread.joinable())
        {
            m_workerThread.join();
        }

        qDebug() << "VOICE PLAYBACK | Рабочий поток воспроизведения остановлен.";
    }

private slots:
    // Этот слот будет вызываться БЕЗОПАСНО в потоке, где живет QAudioSink (через invokeMethod)
    void playFrameData(const QByteArray& pcmData, int sampleRate, int channels)
    {
        // Инициализируем аудиовыход лениво под РЕАЛЬНЫЙ формат пришедшего звука
        if (!m_sink)
        {
            QAudioFormat fmt;
            fmt.setSampleRate(sampleRate);
            fmt.setChannelCount(channels);
            fmt.setSampleFormat(QAudioFormat::Int16);

            auto device = QMediaDevices::defaultAudioOutput();
            if (!device.isFormatSupported(fmt)) {
                fmt = device.preferredFormat();
            }

            m_sink = new QAudioSink(device, fmt);
            // Буфер на 40 мс предотвратит заикания сети
            m_sink->setBufferSize(fmt.sampleRate() * fmt.channelCount() * sizeof(int16_t) * 40 / 1000);
            m_sinkDevice = m_sink->start();

            qDebug() << "VOICE PLAYBACK | Инициализирован QAudioSink:" << sampleRate << "Hz," << channels << "channels";
        }

        if (m_sinkDevice && m_sink->state() != QAudio::StoppedState) {
            m_sinkDevice->write(pcmData);
        }
    }

private:
    void audioLoop()
    {
        while (m_running)
        {
            // Делаем локальную копию, чтобы избежать race condition при сбросе из GUI потока
            auto currentStream = m_stream;
            if (!currentStream)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            livekit::AudioFrameEvent frameEvent;

            // БЛОКИРУЮЩЕЕ ЧТЕНИЕ
            bool hasFrame = currentStream->read(frameEvent);

            // Если во время ожидания пришел стоп или стрим закрыли — выходим
            if (!hasFrame || !m_running || !m_stream)
            {
                continue;
            }

            const auto& frame = frameEvent.frame;
            int numSamples = frame.samplesPerChannel() * frame.numChannels();

            if (numSamples <= 0) {
                continue;
            }

            QByteArray pcm(reinterpret_cast<const char*>(frame.data().data()), numSamples * sizeof(int16_t));

            QMetaObject::invokeMethod(this, "playFrameData", Qt::QueuedConnection,
                                      Q_ARG(QByteArray, pcm),
                                      Q_ARG(int, frame.sampleRate()),
                                      Q_ARG(int, frame.numChannels()));
        }
    }

    void destroySink()
    {
        if (m_sink) {
            m_sink->stop();
            delete m_sink;
            m_sink = nullptr;
            m_sinkDevice = nullptr;
        }
    }

    std::shared_ptr<livekit::AudioStream> m_stream;
    std::thread m_workerThread;
    std::atomic<bool> m_running;

    QAudioSink* m_sink;
    QIODevice* m_sinkDevice;
};

#endif // PLAYBACKWORKER_H
