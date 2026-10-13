#pragma once

#include "CheckDBrule.h"

class CheckMessagesRule : public CheckDBrule
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

            // 1. Проверяем и создаем таблицу 'messages'
            if (!isTableExists(txn, "messages"))
            {
                const string createMessagesSql = R"(
                    CREATE TABLE messages (
                        id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
                        channel_id UUID NOT NULL REFERENCES channels(id) ON DELETE CASCADE,
                        sender_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
                        message_text TEXT NOT NULL DEFAULT '',
                        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
                        updated_at TIMESTAMP NULL
                    );
                )";
                txn.exec(createMessagesSql);
                cout << "Таблица 'messages' успешно создана!\n";
            }
            else
            {
                cout << "Таблица 'messages' уже существует.\n";
            }

            // 2. Проверяем и создаем таблицу 'message_attachments'
            if (!isTableExists(txn, "message_attachments"))
            {
                const string createAttachmentsSql = R"(
                    CREATE TABLE message_attachments (
                        id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
                        message_id UUID NOT NULL REFERENCES messages(id) ON DELETE CASCADE,
                        attachment_type SMALLINT NOT NULL,
                        url TEXT NOT NULL,
                        meta_data TEXT NOT NULL DEFAULT '',
                        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL
                    );
                )";
                txn.exec(createAttachmentsSql);

                const string idxSql = R"(
                    CREATE INDEX IF NOT EXISTS idx_attachments_message_id ON message_attachments(message_id);
                )";
                txn.exec(idxSql);

                cout << "Таблица 'message_attachments' успешно создана!\n";
            }
            else
            {
                cout << "Таблица 'message_attachments' уже существует.\n";
            }

            // 3. Безопасное добавление внешнего ключа в 'channel_members'
            // Проверяем, существует ли уже ограничение с именем 'fk_last_read_message'
            const string checkConstraintSql = R"(
                SELECT 1 FROM pg_constraint WHERE conname = 'fk_last_read_message';
            )";
            pqxx::result constraintRes = txn.exec(checkConstraintSql);

            if (constraintRes.empty())
            {
                const string alterSql = R"(
                    ALTER TABLE channel_members
                    ADD CONSTRAINT fk_last_read_message
                    FOREIGN KEY (last_read_message_id) REFERENCES messages(id) ON DELETE SET NULL;
                )";
                txn.exec(alterSql);
                cout << "Ограничение fk_last_read_message успешно добавлено.\n";
            }
            else
            {
                cout << "Ограничение fk_last_read_message уже существует — пропускаем.\n";
            }

            txn.commit();
            return true;
        }
        catch (const pqxx::sql_error& e)
        {
            cerr << "SQL error in CheckMessagesRule: " << e.what() << "\n";
            return false;
        }
        catch (...)
        {
            cerr << "Unexpected error in CheckMessagesRule.\n";
            return false;
        }
    }
};
