#include "voiceroomdelegate.h"
#include "voiceengine.h"

void VoiceRoomDelegate::onTrackSubscribed(livekit::Room &room, const livekit::TrackSubscribedEvent &Event)
{
    if (!Event.track || Event.track->kind() != livekit::TrackKind::KIND_AUDIO)
        return;

    auto track = Event.track;
    auto* engine = &QVoiceEngine::getInstance();

    QMetaObject::invokeMethod(engine, [engine, track]() {
        engine->handleSubscribedTrack(track);
    }, Qt::QueuedConnection);
}
