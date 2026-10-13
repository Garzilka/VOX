#pragma once
#include "CheckDBrule.h"

class CheckVoiceStatesRule : public CheckDBrule 
{
public:
    CheckVoiceStatesRule() = default;
    ~CheckVoiceStatesRule() override = default;

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
            if (isTableExists(txn, "voice_states"))
            {
                cout << "Таблица 'voice_states' существует!\n";
                return true;
            }
            const string sql = R"(
                CREATE TABLE IF NOT EXISTS voice_states (
                    user_id UUID PRIMARY KEY REFERENCES users(id) ON DELETE CASCADE,
                    server_id UUID REFERENCES servers(id) ON DELETE CASCADE,
                    channel_id UUID REFERENCES channels(id) ON DELETE CASCADE,
                    is_muted BOOLEAN DEFAULT false,
                    is_deafened BOOLEAN DEFAULT false,
                    joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                );
            )";
            txn.exec(sql);

            txn.commit();
            cout << "Таблица 'voice_states' успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckVoiceStatesRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckVoiceStatesRule.\n";
            return false;
        }
    }
};
