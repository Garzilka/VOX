#include "BD_Dispatcher.h"

BD_Dispatcher::BD_Dispatcher(const shared_ptr<Repository>& result_Repository)
{
    registerHandler(EBDTaskType::Login, std::make_shared<CmdRequestLogin>(result_Repository));
    registerHandler(EBDTaskType::Registration, std::make_shared<CmdRequestReg>(result_Repository));
    registerHandler(EBDTaskType::CreateServer, std::make_shared<CmdCreateServer>(result_Repository));
    registerHandler(EBDTaskType::LoadServerPassport, std::make_shared<CmdRequestServerPassport>(result_Repository));
    registerHandler(EBDTaskType::LoadServerDetails, std::make_shared<CmdRequestServerDetails>(result_Repository));
    registerHandler(EBDTaskType::LoadAppVersion, std::make_shared<CmdRequestAppVersion>(result_Repository));
    registerHandler(EBDTaskType::LoadRelation, std::make_shared<CmdLoadRelation>(result_Repository));
    registerHandler(EBDTaskType::FriendAccept, std::make_shared<CmdFriendRequest>(result_Repository));
    registerHandler(EBDTaskType::FriendRequest, std::make_shared<CmdFriendAccept>(result_Repository));
    registerHandler(EBDTaskType::LoadUserPassport, std::make_shared<CmdRequestUserPassport>(result_Repository));
    registerHandler(EBDTaskType::LoadUserPassport, std::make_shared<CmdRequestUserPassport>(result_Repository));
    registerHandler(EBDTaskType::SearchUser, std::make_shared<CmdSearchUsers>(result_Repository));
}
