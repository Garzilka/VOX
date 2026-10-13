#ifndef MICCAPTURE_H
#define MICCAPTURE_H

#pragma once
#include <QObject>
#include <QDebug>
#include <QAudioSource>
#include <QMediaDevices>
#include <QIODevice>
#include <memory>
#include "livekit/livekit.h"

class QMicCapture : public QObject
{
    Q_OBJECT

public:
    explicit QMicCapture(std::shared_ptr<livekit::AudioSource> src, QObject* parent = nullptr)
        : QObject(parent), m_source(std::move(src)) {}

    void start()
    {
        QAudioFormat fmt;
        fmt.setSampleRate(48000);
        fmt.setChannelCount(1);
        fmt.setSampleFormat(QAudioFormat::Int16);

        QAudioDevice Voice = QMediaDevices::defaultAudioInput();
        if (Voice.isNull())
        {
            qWarning() << "MIC CAPTURE | Нет доступного микрофона";
            return;
        }
        m_audio = std::make_unique<QAudioSource>(Voice, fmt);
        m_io = m_audio->start();

        if (!m_io)
        {
            qWarning() << "MIC CAPTURE | Не удалось открыть микрофон";
            return;
        }
        connect(m_io, &QIODevice::readyRead, this, &QMicCapture::handleReadyRead);
        qInfo() << "MIC CAPTURE | Start microphone caprure";
    }

    void stop()
    {
        if (m_audio)
        {
            m_audio->stop();
            m_audio.reset();
        }
        m_io = nullptr;
        m_buffer.clear();
    }
    void setMuted(bool muted) {
        m_isMuted = muted;
    }
protected:
    void handleReadyRead()
    {
        QByteArray newData = m_io->readAll();

        if (m_isMuted) {
            m_buffer.clear();
            return;
        }

        m_buffer.append(newData);
        const int FRAME_SIZE_BYTES = 960;

        while (m_buffer.size() >= FRAME_SIZE_BYTES)
        {
            livekit::AudioFrame frame(
                std::vector<int16_t>(reinterpret_cast<const int16_t*>(m_buffer.constData()),
                                     reinterpret_cast<const int16_t*>(m_buffer.constData()) + 480),
                48000, 1, 480);
            m_source->captureFrame(frame);
            m_buffer.remove(0, FRAME_SIZE_BYTES);
        }
    }

private:
    std::shared_ptr<livekit::AudioSource> m_source;
    std::unique_ptr<QAudioSource> m_audio;
    QByteArray m_buffer;
    QIODevice* m_io = nullptr;
    std::atomic<bool> m_isMuted{false};
};


#endif // MICCAPTURE_H
