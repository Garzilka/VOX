#pragma once
#include "Data/MessagesIncluds.h"

class CheckDBrule
{
public:
    CheckDBrule() {};
    virtual ~CheckDBrule() = default;


    virtual bool CheckTable(const weak_ptr<SqlDatabase>& dbWeak) { return false; };

    bool isTableExists(pqxx::work& txn, const string& tableName)
    {
        try
        {
            // Используем переданную транзакцию, новую НЕ создаем!
            string sql = "SELECT EXISTS ("
                "  SELECT FROM information_schema.tables "
                "  WHERE table_schema = 'public' "
                "    AND table_name = " + txn.quote(tableName) +
                ");";

            auto result = txn.exec(sql);

            if (!result.empty())
            {
                return result[0][0].as<bool>();
            }
            return false;
        }
        catch (const pqxx::sql_error& e)
        {
            std::cout << "REPOSITORY SQL ERROR: " << e.what() << "\n";
            return false;
        }
        catch (const std::exception& e)
        {
            std::cout << "REPOSITORY ERROR: " << e.what() << "\n";
            return false;
        }
        return false;
    }

    bool isAlive(const weak_ptr<SqlDatabase>& dbWeak)
    {
        auto db = dbWeak.lock();
        if (db && db->is_open()) return true;

        cerr << "Соединение с БД недоступно.\n";
        return false;
    }
};