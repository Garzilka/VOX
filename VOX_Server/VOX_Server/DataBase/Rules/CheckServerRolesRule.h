#pragma once

#include "CheckDBrule.h"

class CheckServerRolesRule : public CheckDBrule 
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
            if (isTableExists(txn, "server_roles"))
            {
                cout << "Таблица 'server_roles' существует!\n";
                return true;
            }
            const string createTableSql = R"(
                CREATE TABLE IF NOT EXISTS server_roles (
                    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
                    server_id UUID NOT NULL REFERENCES servers(id) ON DELETE CASCADE,
                    name VARCHAR(50) NOT NULL,
                    color VARCHAR(7) DEFAULT '#99aab5',
                    permissions BIGINT DEFAULT 0 NOT NULL,
                    position SMALLINT DEFAULT 0 NOT NULL,
                    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL
                );
            )";
            txn.exec(createTableSql);

            txn.commit();
            cout << "Таблица 'server_roles' успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckServerRolesRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckServerRolesRule.\n";
            return false;
        }
    }
};
