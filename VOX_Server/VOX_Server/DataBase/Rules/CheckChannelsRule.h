#pragma once
#include "CheckDBrule.h"

class CheckChannelsRule : public CheckDBrule 
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
            if (isTableExists(txn, "channels"))
            {
                cout << "Таблица 'channels' существует!\n";
                return true;
            }
            const string createTableSql = R"(
                CREATE TABLE IF NOT EXISTS channels (
                    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
                    type SMALLINT NOT NULL,
                    server_id UUID NULL REFERENCES servers(id) ON DELETE CASCADE,
                    name VARCHAR(100) NULL,
                    parent_id UUID NULL,
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL
                );
            )";
            txn.exec(createTableSql);

            txn.commit();
            cout << "Таблица 'channels' успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckChannelsRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckChannelsRule.\n";
            return false;
        }
    }
};
