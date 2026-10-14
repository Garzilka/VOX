#pragma once

#include "../../Data/MessagesIncluds.h"

class Channel
{
private:
    SChannelPassport _ChannelPassport;
    SChannelDetails  _ChannelDetails;

    unordered_set<string> _PassportPendingClients;
    unordered_set<string> _DetailsPendingClients;

    vector<std::function<void(string)>> _PassportCallbacks;
    vector<std::function<void(string)>> _DetailsCallbacks;

    mutable mutex _ChannelMutex;

    bool _isPassportValid = false;
    bool _isDetailsValid = false;

public:
    explicit Channel() = default;

    void addParticipant(const string& userUUID);
    void removeParticipant(const string& userUUID);

    bool getChannelPassport(SChannelPassport& outPassport, const string& clientUUID);
    void warmupPassport(std::function<void(string)>& callback);
    bool setChannelPassport(SChannelPassport newPassport, vector<string>& outPendings);

    // Детали канала
    bool getChannelDetails(SChannelDetails& outDetails, const string& clientUUID);
    void warmupDetails(std::function<void(string)>& callback);
    bool setChannelDetails(SChannelDetails newDetails, vector<string>& outPendings);

    bool isPassportValid() const { return _isPassportValid; }
    bool isDetailsValid() const { return _isDetailsValid; }
};