#pragma once

#include "CheckDBrule.h"

class UsersCheckRule : public CheckDBrule {
public:
    UsersCheckRule() = default;
    ~UsersCheckRule() override = default;

    bool CheckTable(const weak_ptr<SqlDatabase>& dbWeak) override
    {
        

        auto db = dbWeak.lock();
        if (!db) return false;
        

        try 
        {
            pqxx::work txn(*db);
            if (isTableExists(txn, "users"))
            {
                cout << "Таблица 'users' существует!\n";
                return true;
            }
            const string createTableSql = R"(
                CREATE TABLE IF NOT EXISTS users (
                    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
                    username VARCHAR(32) NOT NULL UNIQUE CHECK (username ~ '^[a-zA-Z0-9_]+$'),
                    display_name VARCHAR(50) NOT NULL,
                    email VARCHAR(255) NOT NULL UNIQUE CHECK (email ~* '^[A-Za-z0-9._%-]+@[A-Za-z0-9.-]+[A-Za-z]{2,4}$'),
                    password_hash VARCHAR(255) NOT NULL,
                    salt VARCHAR(255) NOT NULL,
                    avatar_url VARCHAR(2048) DEFAULT NULL,
                    status SMALLINT NOT NULL DEFAULT 0,
                    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
                    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL
                );
            )";

            txn.exec(createTableSql);

            const string idxSql = R"(
                CREATE INDEX IF NOT EXISTS idx_users_username ON users (username);
            )";
            txn.exec(idxSql);

            const string funcSql = R"(
                CREATE OR REPLACE FUNCTION update_updated_at_column()
                RETURNS TRIGGER AS $$
                BEGIN
                    NEW.updated_at = CURRENT_TIMESTAMP;
                    RETURN NEW;
                END;
                $$ LANGUAGE plpgsql;
            )";
            txn.exec(funcSql);

            txn.exec("DROP TRIGGER IF EXISTS update_users_updated_at ON users;");

            const string triggerSql = R"(
                CREATE TRIGGER update_users_updated_at
                    BEFORE UPDATE ON users
                    FOR EACH ROW
                    EXECUTE FUNCTION update_updated_at_column();
            )";
            txn.exec(triggerSql);
            txn.commit();

            cout << "Таблица 'users' успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in UsersCheckRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in UsersCheckRule.\n";
            return false;
        }
    }
};
