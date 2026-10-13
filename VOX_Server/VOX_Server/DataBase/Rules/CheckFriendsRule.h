#pragma once

#include "CheckDBrule.h"

class CheckFriendsRule : public CheckDBrule 
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

            if (isTableExists(txn, "friends"))
            {
                cout << "Таблица 'friends' существует!\n";
                return true;
            }

            const string createEnumSql = R"(
                DO $$
                BEGIN
                    IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'friend_status_enum') THEN
                        CREATE TYPE friend_status_enum AS ENUM ('pending', 'accepted', 'blocked');
                    END IF;
                END $$;
            )";
            txn.exec(createEnumSql);

            const string createTableSql = R"(
                CREATE TABLE friends (
                    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
                    friend_id UUID REFERENCES users(id) ON DELETE CASCADE,
                    status friend_status_enum NOT NULL DEFAULT 'pending',
                    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
                    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
                    PRIMARY KEY (user_id, friend_id),
                    CONSTRAINT check_self_friendship CHECK (user_id <> friend_id)
                );
            )";
            txn.exec(createTableSql);

            txn.exec("DROP TRIGGER IF EXISTS update_friends_updated_at ON friends;");

            const string triggerSql = R"(
                CREATE TRIGGER update_friends_updated_at
                    BEFORE UPDATE ON friends
                    FOR EACH ROW
                    EXECUTE FUNCTION update_updated_at_column();
            )";
            txn.exec(triggerSql);

            txn.commit();
            cout << "Таблица 'friends' успешно создана!\n";
            return true;
        }

        catch (const pqxx::sql_error& e) 
        {
            cerr << "SQL error in CheckFriendsRule: " << e.what() << "\n";
            return false;
        }
        catch (...) 
        {
            cerr << "Unexpected error in CheckFriendsRule.\n";
            return false;
        }
    }
};
