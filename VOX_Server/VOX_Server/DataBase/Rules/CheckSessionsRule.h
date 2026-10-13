#pragma once

#include "CheckDBrule.h"


class CheckSessionsRule : public CheckDBrule
{
public:
    CheckSessionsRule() = default;
    ~CheckSessionsRule() override = default;

    bool CheckTable(const std::weak_ptr<SqlDatabase>& dbWeak) override 
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
            if (isTableExists(txn, "user_sessions"))
            {
                cout << "Таблица 'user_sessions' существует!\n";
                return true;
            }
            const string createTableSql = R"(
                CREATE TABLE IF NOT EXISTS user_sessions (
                    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
                    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
                    refresh_token VARCHAR(255) NOT NULL UNIQUE,
                    device_info VARCHAR(255) DEFAULT NULL,
                    expires_at TIMESTAMP WITH TIME ZONE NOT NULL,
                    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL
                );
            )";
            txn.exec(createTableSql);

            const string idxSql = R"(
                CREATE INDEX IF NOT EXISTS idx_sessions_token ON user_sessions (refresh_token);
            )";
            txn.exec(idxSql);

            txn.commit();
            cout << "Таблица 'user_sessions' успешно создана!\n";
            return true;

        }

        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in SessionsCheckRule: " << e.what() << "\n";
            return false;
        }
        catch (...)
        {
            cerr << "Unexpected error in SessionsCheckRule.\n";
            return false;
        }
    }
};
