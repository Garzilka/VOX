#ifndef VOICEROOMDELEGATE_H
#define VOICEROOMDELEGATE_H

#include <QObject>
#include <QDebug>
#include <memory>
#include <QMetaObject>
#include <QAudioSource>
#include <QMediaDevices>
#include <QIODevice>
#include <QtConcurrent>
#include <QAudioSink>
#include "playbackworker.h"

#include "livekit/room.h"
#include "livekit/track.h"
#include "livekit/room_event_types.h"
#include "livekit/room_delegate.h"

class VoiceRoomDelegate : public livekit::RoomDelegate
{

    virtual void onTrackSubscribed(livekit::Room& room, const livekit::TrackSubscribedEvent& Event) override;
};

#endif // VOICEROOMDELEGATE_H
