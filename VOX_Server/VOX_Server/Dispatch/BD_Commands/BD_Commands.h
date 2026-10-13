#pragma once
#include "DataBase/Repository.h"
#include "Data/BDTaskTypes.h"


class IDBCommandHandler
{
protected:
    shared_ptr<Repository> _Repository;
public:
    IDBCommandHandler(const shared_ptr<Repository>& result_Repository) : _Repository(result_Repository) {};
    virtual ~IDBCommandHandler() = default;
    virtual void execute(const BDTask& data) = 0;

protected:
    void Exit(const BDTask& data)
    {
        if (!data.onResult) return;

        data.onResult(false, JsonObject());
    }
};

class CmdCreateServer : public IDBCommandHandler
{
public:
    CmdCreateServer(const shared_ptr<Repository>& result_Repository) : IDBCommandHandler(result_Repository) {};
    virtual void execute(const BDTask& data) override;
};

class CmdRequestServerPassport : public IDBCommandHandler
{
public:
    CmdRequestServerPassport(const shared_ptr<Repository>& result_Repository) : IDBCommandHandler(result_Repository) {};
    virtual void execute(const BDTask& data) override;
};

class CmdRequestServerDetails : public IDBCommandHandler
{
public:
    CmdRequestServerDetails(const shared_ptr<Repository>& result_Repository) : IDBCommandHandler(result_Repository) {};
    virtual void execute(const BDTask& data) override;
};

class CmdRequestAppVersion : public IDBCommandHandler
{
public:
    CmdRequestAppVersion(const shared_ptr<Repository>& result_Repository) : IDBCommandHandler(result_Repository) {};
    virtual void execute(const BDTask& data) override;
};

class CmdRequestLogin : public IDBCommandHandler
{
public:
    CmdRequestLogin(const shared_ptr<Repository>& result_Repository) : IDBCommandHandler(result_Repository) {};
    virtual void execute(const BDTask& data) override;
};

class CmdRequestReg : public IDBCommandHandler
{
public:
    CmdRequestReg(const shared_ptr<Repository>& result_Repository) : IDBCommandHandler(result_Repository) {};
    virtual void execute(const BDTask& data) override;
};