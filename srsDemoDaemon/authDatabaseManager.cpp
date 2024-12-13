#include "authDatabaseManager.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <random>
#include <iomanip>
#include <openssl/sha.h>

AuthDatabaseManager::AuthDatabaseManager()
{
    initializeDatabase();
}

AuthDatabaseManager::~AuthDatabaseManager() = default;

void AuthDatabaseManager::initializeDatabase()
{
    databasePath = "srs_database.db";
    sqlite3* db = nullptr;

    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS users (
            userName TEXT PRIMARY KEY,
            password TEXT NOT NULL,
            salt TEXT NOT NULL
        );
    )";

    char* errorMessage = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errorMessage) != SQLITE_OK)
    {
        std::ostringstream errMsg;
        errMsg << "Error creating users table: " << errorMessage;
        sqlite3_free(errorMessage);
        sqlite3_close(db);
        throw std::runtime_error(errMsg.str());
    }

    sqlite3_close(db);
}

bool AuthDatabaseManager::createUser(const std::string& userName, const std::string& password)
{
    if (userName.empty() || password.empty())
    {
        throw std::invalid_argument("User name or password cannot be empty.");
    }

    std::string salt = generateSalt();
    std::string hashedPassword = hashPassword(password, salt);

    sqlite3* db = nullptr;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* insertSQL = R"(
        INSERT INTO users (userName, password, salt)
        VALUES (?, ?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, salt.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

bool AuthDatabaseManager::validateUser(const std::string& userName, const std::string& password)
{
    if (userName.empty() || password.empty())
    {
        throw std::invalid_argument("User name or password cannot be empty.");
    }

    sqlite3* db = nullptr;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* selectSQL = R"(
        SELECT password, salt FROM users WHERE userName = ?;
    )";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);

    std::string storedHashedPassword, storedSalt;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        storedHashedPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        storedSalt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    }
    else
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::string inputHashedPassword = hashPassword(password, storedSalt);
    return inputHashedPassword == storedHashedPassword;
}

std::string AuthDatabaseManager::generateSalt() const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);

    std::ostringstream salt;
    for (int i = 0; i < 16; ++i)
    {
        salt << std::hex << std::setw(2) << std::setfill('0') << dist(gen);
    }

    return salt.str();
}

std::string AuthDatabaseManager::hashPassword(const std::string& password, const std::string& salt) const
{
    std::string saltedPassword = password + salt;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(saltedPassword.c_str()), saltedPassword.size(), hash);

    std::ostringstream hashedPassword;
    for (unsigned char c : hash)
    {
        hashedPassword << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }

    return hashedPassword.str();
}
