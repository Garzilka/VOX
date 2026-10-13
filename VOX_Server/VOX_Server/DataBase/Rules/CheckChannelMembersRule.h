#pragma once
#include "CheckDBrule.h"

class CheckChannelMembersRule : public CheckDBrule 
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
            if (isTableExists(txn, "channel_members"))
            {
                cout << "Таблица 'channel_members' существует!\n";
                return true;
            }
            const string createTableSql = R"(
                CREATE TABLE IF NOT EXISTS channel_members (
                    channel_id UUID REFERENCES channels(id) ON DELETE CASCADE,
                    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
                    joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
                    last_read_message_id UUID NULL,
                    PRIMARY KEY (channel_id, user_id)
                );
            )";
            txn.exec(createTableSql);

            txn.commit();
            cout << "Таблица 'channel_members' успешно создана!\n";
            return true;

        }
        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckChannelMembersRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckChannelMembersRule.\n";
            return false;
        }
    }
};
