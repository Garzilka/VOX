#pragma once

#include "CheckDBrule.h"

class CheckServersRule : public CheckDBrule 
{

public:
    bool CheckTable(const weak_ptr<SqlDatabase>& dbWeak) override 
    {
        if (!isAlive(dbWeak)) return false;
        

        auto db = dbWeak.lock();
        if (!db) 
        {
            cerr << "Соединение недоступно.\n";
            return false;
        }

        try 
        {
            pqxx::work txn(*db);
            if (isTableExists(txn, "servers"))
            {
                cout << "Таблица 'servers' существует!\n";
                return true;
            }
            const string createTableSql = R"(
                CREATE TABLE IF NOT EXISTS servers (
                    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
                    name VARCHAR(100) NOT NULL,
                    owner_id UUID NOT NULL REFERENCES users(id) ON DELETE RESTRICT,
                    icon_url VARCHAR(2048) DEFAULT NULL,
                    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
                    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL
                );
            )";

            txn.exec(createTableSql);

            txn.exec("DROP TRIGGER IF EXISTS update_servers_updated_at ON servers;");

            const string triggerSql = R"(
                CREATE TRIGGER update_servers_updated_at
                    BEFORE UPDATE ON servers
                    FOR EACH ROW
                    EXECUTE FUNCTION update_updated_at_column();
            )";
            txn.exec(triggerSql);

            txn.commit();

            cout << "Таблица 'servers' успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckServersRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckServersRule.\n";
            return false;
        }
    }
};