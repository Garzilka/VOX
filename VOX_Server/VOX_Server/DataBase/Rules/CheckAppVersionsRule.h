#pragma once
#include "CheckDBrule.h"

class CheckAppVersionsRule : public CheckDBrule 
{
public:
    CheckAppVersionsRule() = default;
    ~CheckAppVersionsRule() override = default;

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
            if (isTableExists(txn, "app_versions"))
            {
                cout << "Таблица 'app_versions' существует!\n";
                return true;
            }
            const string sql = R"(
                CREATE TABLE IF NOT EXISTS app_versions (
                    id SERIAL PRIMARY KEY,
                    version_code INT NOT NULL,
                    version_name VARCHAR(16) NOT NULL,
                    platform VARCHAR(16) NOT NULL,
                    download_url TEXT NOT NULL,
                    sha256_hash VARCHAR(64) NOT NULL,
                    released_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                );
                CREATE INDEX IF NOT EXISTS idx_versions_platform_code
                ON app_versions(platform, version_code DESC);
            )";

            txn.exec(sql);
            txn.commit();

            cout << "Таблица 'app_versions' успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckAppVersionsRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckAppVersionsRule.\n";
            return false;
        }
    }
};
