#include "AuthClientService.h"
#include "Client.h"
#include "DataBase/BDTaskQueue.h"
#include "Data/BDTaskTypes.h"

bool AuthClientServiceImpl::authClient(const string& userpeerAddress, const SAuthorizationData& data)
{
    if (userpeerAddress.empty())
    {
        return false;
    }

    string error;
    if (!IsValidInput(data, error))
    {
        return false;
    }

    SAuthClientRequest Req;
    Req._UserpeerAddress = userpeerAddress;
    Req._AuthReq = data;

    BDTask newTask(EBDTaskType::Login);
    newTask.payload = Req.ToJSON();
    std::weak_ptr<AuthClientServiceImpl> weakSelf = shared_from_this();

    newTask.onResult = [weakSelf](bool success, const JsonObject& res)
        {
            if (!success) return;

            if (auto self = weakSelf.lock())
            {
                asio::post(self->m_ioContext, [self, res]()
                    {
                        self->dbResponseAuth(res);
                    });
            }
        };

    m_bdQueue.push(std::move(newTask));

    return false;
}

bool AuthClientServiceImpl::regClient(const string& userpeerAddress, const SAuthorizationData& data)
{
    if (userpeerAddress.empty())
    {
        return false;
    }

    string error;
    if (!IsValidInput(data, error))
    {
        return false;
    }


    SAuthClientRequest Req;
    Req._UserpeerAddress = userpeerAddress;
    Req._AuthReq = data;

    BDTask newTask(EBDTaskType::Registration);
    newTask.payload = Req.ToJSON();
    std::weak_ptr<AuthClientServiceImpl> weakSelf = shared_from_this();

    newTask.onResult = [weakSelf](bool success, const JsonObject& res)
        {
            if (!success) return;

            if (auto self = weakSelf.lock())
            {
                asio::post(self->m_ioContext, [self, res]()
                    {
                        self->dbResponseAuth(res);
                    });
            }
        };

    m_bdQueue.push(std::move(newTask));

    return false;
}

void AuthClientServiceImpl::dbResponseAuth(const JsonObject& res)
{
    if (!_CacheClientService) return;
    SAuthClientResponse AuthData = SAuthClientResponse(res);


    if (AuthData._AuthReponse.Response)
    {
        cout << "Успешная авторизация для: " << AuthData._AuthReponse.UUID << " | " << AuthData._UserpeerAddress << endl;
        AuthData._AuthReponse.Response = _CacheClientService->authComplete(AuthData._AuthReponse.UUID, AuthData._UserpeerAddress);
    }
    else
    {
        cout << "Провальная авторизация для: " << AuthData._AuthReponse.UUID << " | " << AuthData._UserpeerAddress << endl;
    }

    if (!notificationService) return;

    notificationService->NotifyClients( { AuthData._AuthReponse.UUID }, AuthData._AuthReponse);
}

bool AuthClientServiceImpl::IsValidInput(const SAuthorizationData& Data, string& outErrorReason)
{
    if (Data.Login.empty()) return false;

    static const std::regex usernameRegex("^[a-zA-Z0-9_]{3,20}$");

    if (!std::regex_match(Data.Login, usernameRegex))
    {
        //TODO: переделать в enum
        outErrorReason = "Логин должен быть от 3 до 20 символов и содержать только латиницу, цифры или знак подчёркивания.";
        return false;
    }

    if (!Data.Email.empty())
    {
        static const std::regex emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

        if (!std::regex_match(Data.Email, emailRegex))
        {
            //TODO: переделать в enum
            outErrorReason = "Неверный формат адреса электронной почты.";
            return false;
        }
    }

    return true;
}
