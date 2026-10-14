#ifndef FRIENDDATA_H
#define FRIENDDATA_H
#include "CoreData.h"


struct SFriendRequest : public SBaseMessageData
{
    QString SenderGUID;
    QString TargetGUID;

    SFriendRequest(QString _SenderGUID, QString _TargetUsername, QString _TargetGUID = "", ETypeOfMessage _TypeMessage = ETypeOfMessage::FriendRequest)
        : SBaseMessageData(_TypeMessage), SenderGUID(_SenderGUID), TargetGUID(_TargetGUID) {};

    SFriendRequest(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        SenderGUID = InJSON["SenderGUID"].toString();
        TargetGUID = InJSON["TargetGUID"].toString();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["SenderGUID"] = SenderGUID;
        messageObj["TargetGUID"] = TargetGUID;
        return messageObj;
    }
};

struct SFriendResponse : public SBaseMessageData
{
    QString TargetGUID;
    bool Action = false;        // 0 - Принять, 1 - Отклонить/Удалить

    SFriendResponse(QString _TargetGUID, int _Action, ETypeOfMessage _TypeMessage = ETypeOfMessage::FriendAccept)
        : SBaseMessageData(_TypeMessage), TargetGUID(_TargetGUID), Action(_Action) {};

    SFriendResponse(const QJsonObject& InJSON) : SBaseMessageData(InJSON)
    {
        TargetGUID = InJSON["TargetGUID"].toString();
        Action = InJSON["Action"].toBool();
    };

    virtual QJsonObject ToJSON() const override
    {
        QJsonObject messageObj = SBaseMessageData::ToJSON();
        messageObj["TargetGUID"] = TargetGUID;
        messageObj["Action"] = Action;
        return messageObj;
    }
};

#endif // FRIENDDATA_H
