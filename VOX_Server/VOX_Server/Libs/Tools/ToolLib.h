#pragma once
#include "Data/Common.h"

class ToolLib
{
public:

    static string generateUuid()
    {
        static std::mt19937_64 rng(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        );

        std::uniform_int_distribution<uint64_t> dist;

        auto r1 = dist(rng);
        auto r2 = dist(rng);

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');

        // Простой формат UUID без дефисов (как QUuid::WithoutBraces)
        // Можно сделать стандартный формат с дефисами, если нужно.
        oss << std::setw(16) << (r1 & 0xFFFFFFFFFFFFFFFFULL);
        oss << std::setw(16) << (r2 & 0xFFFFFFFFFFFFFFFFULL);

        return oss.str();
    }

    static string toBase64(const ByteArray bytes)
    {
        static const char kBase64StandardChars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        string result;
        result.reserve((bytes.size() + 2) / 3 * 4);

        size_t i = 0;
        // Обработка полных троек байт
        for (; i + 2 < bytes.size(); i += 3)
        {
            uint32_t triple = (static_cast<uint32_t>(bytes[i]) << 16) |
                (static_cast<uint32_t>(bytes[i + 1]) << 8) |
                static_cast<uint32_t>(bytes[i + 2]);

            result.push_back(kBase64StandardChars[(triple >> 18) & 0x3F]);
            result.push_back(kBase64StandardChars[(triple >> 12) & 0x3F]);
            result.push_back(kBase64StandardChars[(triple >> 6) & 0x3F]);
            result.push_back(kBase64StandardChars[triple & 0x3F]);
        }

        // Правильная и безопасная обработка остатка (1 или 2 байта)
        if (i < bytes.size())
        {
            size_t remaining = bytes.size() - i;
            uint32_t triple = static_cast<uint32_t>(bytes[i]) << 16;
            if (remaining == 2)
            {
                triple |= static_cast<uint32_t>(bytes[i + 1]) << 8;
            }

            result.push_back(kBase64StandardChars[(triple >> 18) & 0x3F]);
            result.push_back(kBase64StandardChars[(triple >> 12) & 0x3F]);

            if (remaining == 2)
            {
                result.push_back(kBase64StandardChars[(triple >> 6) & 0x3F]);
                result.push_back('='); // Паддинг для классического Base64
            }
            else
            {
                result.push_back('=');
                result.push_back('=');
            }
        }

        return result;
    }

    // 2. Base64Url без паддинга (строго по стандарту RFC 4648 для JWT)
    static string toBase64Url(const ByteArray bytes)
    {
        string b64 = toBase64(bytes);

        for (char& c : b64)
        {
            if (c == '+') c = '-';
            else if (c == '/') c = '_';
        }

        while (!b64.empty() && b64.back() == '=')
        {
            b64.pop_back();
        }

        return b64;
    }

    static ByteArray decodeBase64(const string& input)
    {
        // Поддерживаем стандартный алфавит с '-' и '_' (Base64Url)
        static const char kBase64Chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

        int charToIndex[256];
        std::fill(std::begin(charToIndex), std::end(charToIndex), -1);

        for (int i = 0; kBase64Chars[i] != '\0'; ++i) {
            charToIndex[static_cast<unsigned char>(kBase64Chars[i])] = i;
        }
        // Поддержка классического Base64 (если в строке придут '+' или '/')
        charToIndex[static_cast<unsigned char>('+')] = 62;
        charToIndex[static_cast<unsigned char>('/')] = 63;

        ByteArray result;
        result.reserve((input.size() * 3) / 4);

        uint32_t buffer = 0;
        int bitsCollected = 0;

        for (char c : input)
        {
            if (c == '=') break; // Конец данных (паддинг)

            int idx = charToIndex[static_cast<unsigned char>(c)];
            if (idx == -1)
            {
                // Игнорируем пробелы/переносы строк или кидаем ошибку
                continue;
            }

            buffer = (buffer << 6) | idx;
            bitsCollected += 6;

            if (bitsCollected >= 8)
            {
                bitsCollected -= 8;
                result.push_back(static_cast<unsigned char>((buffer >> bitsCollected) & 0xFF));
            }
        }

        return result;
    }
    static int64_t currentSecondsSinceEpochPrecise()
    {
        using namespace std::chrono;
        return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    }

    static string hmacSha256Base64Url(const string& data, const string& key)
    {
        unsigned char digest[SHA256_DIGEST_LENGTH];
        HMAC(
            EVP_sha256(),
            reinterpret_cast<const unsigned char*>(key.c_str()),
            static_cast<int>(key.size()),
            reinterpret_cast<const unsigned char*>(data.c_str()),
            static_cast<int>(data.size()),
            digest,
            nullptr
        );

        ByteArray vec(digest, digest + SHA256_DIGEST_LENGTH);
        return ToolLib::toBase64Url(vec);
    }

    template<typename T>
    static inline T jsonGetValue(const JsonObject& j, const char* key, const T& def = T{})
    {
        try
        {
            return j.value(key, def);
        }
        catch (...)
        {
            return def;
        }
    }
};