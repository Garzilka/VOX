#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <random>
#include <sstream>
#include <iomanip>
#include <functional>
#include <memory>
#include <typeinfo>
#include <stdexcept>
#include <typeindex>
#include <ctime>

#include <thread>
#include <vector>

#include "spdlog/spdlog.h"
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include <openssl/hmac.h>
#include <openssl/sha.h>

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;
using std::optional;
using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::cout;
using std::cerr;
using std::queue;
using std::endl;
using std::mutex;
using std::function;

using JsonObject = nlohmann::json;
using string = std::string;
using SqlDatabase = pqxx::connection;

using ByteArray = std::vector<std::uint8_t>;

#define INDEX_NONE -1

#define INFO_CLIENT(msg, ...) spdlog::info("[CLIENT]: " msg, ##__VA_ARGS__);
#define WARNING_CLIENT(msg, ...) spdlog::warn("[CLIENT]: " msg, ##__VA_ARGS__)
#define ERROR_CLIENT(msg, ...) spdlog::error("[CLIENT]: " msg, ##__VA_ARGS__)


#define LOG_INFO(msg, ...) spdlog::info("CLIENT MANAGER | " msg, ##__VA_ARGS__);
#define LOG_WARNING(msg, ...) spdlog::warn("CLIENT MANAGER | " msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) spdlog::error("CLIENT MANAGER | " msg, ##__VA_ARGS__)