#include "Channel.h"

bool Channel::getChannelPassport(SChannelPassport& outPassport, const string& clientUUID)
{
    std::lock_guard<mutex> lock(_ChannelMutex);
    if (!_isPassportValid)
    {
        _PassportPendingClients.insert(clientUUID);
        return false;
    }
    outPassport = _ChannelPassport;
    return true;
}

void Channel::warmupPassport(std::function<void(string)>& callback)
{
    vector<std::function<void(string)>> localCallbacks;
    string channelUUID = "";

    {
        std::lock_guard<mutex> lock(_ChannelMutex);
        _PassportCallbacks.push_back(callback);
        if (!_isPassportValid) return;

        localCallbacks = _PassportCallbacks;
        channelUUID = _ChannelPassport.ChatUUID;
        _PassportCallbacks.clear();
    }

    for (const auto& cb : localCallbacks)
    {
        if (cb) cb(channelUUID);
    }
}

bool Channel::setChannelPassport(SChannelPassport newPassport, vector<string>& outPendings)
{
    vector<std::function<void(string)>> localCallbacks;
    string channelUUID = "";

    {
        std::lock_guard<mutex> lock(_ChannelMutex);
        _ChannelPassport = std::move(newPassport);
        _isPassportValid = !_ChannelPassport.ChatUUID.empty();

        if (!_isPassportValid) return false;

        ToolLib::fillPendingList(outPendings, _PassportPendingClients);

        channelUUID = _ChannelPassport.ChatUUID;
        localCallbacks = _PassportCallbacks;
        _PassportCallbacks.clear();
    }

    if (channelUUID.empty()) return _isPassportValid;

    for (const auto& cb : localCallbacks)
    {
        if (cb) cb(channelUUID);
    }
    return true;
}

bool Channel::getChannelDetails(SChannelDetails& outDetails, const string& clientUUID)
{
    std::lock_guard<mutex> lock(_ChannelMutex);
    if (!_isDetailsValid)
    {
        _DetailsPendingClients.insert(clientUUID);
        return false;
    }
    outDetails = _ChannelDetails;
    return true;
}

void Channel::warmupDetails(std::function<void(string)>& callback)
{
    vector<std::function<void(string)>> localCallbacks;
    string channelUUID = "";

    {
        std::lock_guard<mutex> lock(_ChannelMutex);
        _DetailsCallbacks.push_back(callback);
        if (!_isDetailsValid) return;

        localCallbacks = _DetailsCallbacks;
        channelUUID = _ChannelDetails.RoomUUID;
        _DetailsCallbacks.clear();
    }

    for (const auto& cb : localCallbacks)
    {
        if (cb) cb(channelUUID);
    }
}

bool Channel::setChannelDetails(SChannelDetails newDetails, vector<string>& outPendings)
{
    vector<std::function<void(string)>> localCallbacks;
    string channelUUID = "";

    {
        std::lock_guard<mutex> lock(_ChannelMutex);
        _ChannelDetails = std::move(newDetails);
        _isDetailsValid = !_ChannelDetails.RoomUUID.empty();

        if (!_isDetailsValid) return false;

        ToolLib::fillPendingList(outPendings, _DetailsPendingClients);

        channelUUID = _ChannelDetails.RoomUUID;
        localCallbacks = _DetailsCallbacks;
        _DetailsCallbacks.clear();
    }

    if (channelUUID.empty()) return _isDetailsValid;

    for (const auto& cb : localCallbacks)
    {
        if (cb) cb(channelUUID);
    }
    return true;
}

void Channel::addParticipant(const string& userUUID)
{
    std::lock_guard<mutex> lock(_ChannelMutex);
    if (!_isPassportValid) return;

    auto& participants = _ChannelPassport.ParticipantUUIDs;
    if (std::find(participants.begin(), participants.end(), userUUID) == participants.end())
    {
        participants.push_back(userUUID);
    }
}

void Channel::removeParticipant(const string& userUUID)
{
    std::lock_guard<mutex> lock(_ChannelMutex);
    if (!_isPassportValid) return;

    auto& participants = _ChannelPassport.ParticipantUUIDs;
    auto it = std::find(participants.begin(), participants.end(), userUUID);
    if (it != participants.end())
    {
        participants.erase(it);
    }
}