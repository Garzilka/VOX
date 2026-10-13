#pragma once

#include "CheckDBrule.h"

class CheckServerMembersRule : public CheckDBrule 
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
            if (isTableExists(txn, "server_members"))
            {
                cout << "Таблица 'server_members' существует!\n";
                return true;
            }
            const string createTableSql = R"(
                CREATE TABLE server_members (
                    server_id UUID REFERENCES servers(id) ON DELETE CASCADE,
                    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
                    nickname VARCHAR(50) DEFAULT NULL,
                    role_id UUID NULL REFERENCES server_roles(id) ON DELETE SET NULL,
                    joined_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
                    PRIMARY KEY (server_id, user_id)
                );
            )";
            txn.exec(createTableSql);

            const string idxSql = R"(
                CREATE INDEX IF NOT EXISTS idx_server_members_user ON server_members(user_id);
            )";
            txn.exec(idxSql);

            txn.commit();
            cout << "Таблица 'server_members' (с поддержкой ролей) успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckServerMembersRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckServerMembersRule.\n";
            return false;
        }
    }
};
