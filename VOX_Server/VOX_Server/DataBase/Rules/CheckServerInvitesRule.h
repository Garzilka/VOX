#pragma once

#include "CheckDBrule.h"

class CheckServerInvitesRule : public CheckDBrule 
{
public:
    bool CheckTable(const weak_ptr<SqlDatabase>& dbWeak) override 
    {
        

        auto db = dbWeak.lock();
        if (!db) 
        {
            cerr << "Соединение недоступно.\n";
            return false;
        }

        try 
        {
            pqxx::work txn(*db);
            if (isTableExists(txn, "server_invites")) {
                cout << "Таблица 'server_invites' существует!\n";
                return true;
            }
            const string createTableSql = R"(
                CREATE TABLE IF NOT EXISTS server_invites (
                    token VARCHAR(50) PRIMARY KEY,
                    server_id UUID NOT NULL REFERENCES servers(id) ON DELETE CASCADE,
                    inviter_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
                    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
                    expires_at TIMESTAMP WITH TIME ZONE DEFAULT (CURRENT_TIMESTAMP + INTERVAL '3 days') NOT NULL
                );
            )";
            txn.exec(createTableSql);

            const string idxSql = R"(
                CREATE INDEX IF NOT EXISTS idx_server_invites_expiry ON server_invites(expires_at);
            )";
            txn.exec(idxSql);

            txn.commit();
            cout << "Таблица 'server_invites' успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckServerInvitesRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckServerInvitesRule.\n";
            return false;
        }
    }
};
