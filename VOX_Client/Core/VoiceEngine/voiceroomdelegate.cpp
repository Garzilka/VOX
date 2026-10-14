#include "voiceroomdelegate.h"
#include "voiceengine.h"

void VoiceRoomDelegate::onTrackSubscribed(livekit::Room &room, const livekit::TrackSubscribedEvent &Event)
{
    if (!Event.track || Event.track->kind() != livekit::TrackKind::KIND_AUDIO)
        return;

    // Копируем shared_ptr, выходим из SDK-потока
    auto track = Event.track;
    auto* engine = &QVoiceEngine::getInstance();

    QMetaObject::invokeMethod(engine, [engine, track]() {
        engine->handleSubscribedTrack(track); // дальше всё в GUI-потоке
    }, Qt::QueuedConnection);
}
