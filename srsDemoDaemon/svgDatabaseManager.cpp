#include "SVGDatabaseManager.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

SVGDatabaseManager::SVGDatabaseManager()
{
    initializeDatabase();
}

SVGDatabaseManager::~SVGDatabaseManager() = default;

void SVGDatabaseManager::initializeDatabase()
{
    databasePath = "srs_database.db";
    sqlite3* db = nullptr;

    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS svg_data (
            userName TEXT NOT NULL,
            fileName TEXT NOT NULL,
            svgData BLOB NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (userName, fileName),
            FOREIGN KEY (userName) REFERENCES users(userName) ON DELETE CASCADE
        );
    )";

    char* errorMessage = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errorMessage) != SQLITE_OK)
    {
        std::ostringstream errMsg;
        errMsg << "Error creating table: " << errorMessage;
        sqlite3_free(errorMessage);
        sqlite3_close(db);
        throw std::runtime_error(errMsg.str());
    }

    sqlite3_close(db);
}

void SVGDatabaseManager::saveSVG(const std::string& fileName, const std::string& userName, const std::vector<unsigned char>& svgData)
{
    if (fileName.empty() || svgData.empty() || userName.empty())
    {
        throw std::invalid_argument("File name, user name, or SVG data cannot be empty.");
    }

    sqlite3* db = nullptr;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* insertSQL = R"(
        INSERT OR REPLACE INTO svg_data (userName, fileName, svgData)
        VALUES (?, ?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, fileName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, svgData.data(), (int)svgData.size(), SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::ostringstream errMsg;
        errMsg << "Error executing statement: " << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw std::runtime_error(errMsg.str());
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

std::vector<unsigned char> SVGDatabaseManager::getSVG(const std::string& fileName, const std::string& userName)
{
    if (fileName.empty() || userName.empty())
    {
        throw std::invalid_argument("File name or user name cannot be empty.");
    }

    sqlite3* db = nullptr;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* selectSQL = R"(
        SELECT svgData FROM svg_data WHERE userName = ? AND fileName = ?;
    )";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, fileName.c_str(), -1, SQLITE_STATIC);

    std::vector<unsigned char> svgData;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const void* blobData = sqlite3_column_blob(stmt, 0);
        int blobSize = sqlite3_column_bytes(stmt, 0);
        svgData.assign(static_cast<const unsigned char*>(blobData), static_cast<const unsigned char*>(blobData) + blobSize);
    }
    else
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        throw std::runtime_error("No SVG data found for userName and fileName.");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return svgData;
}

std::vector<std::string> SVGDatabaseManager::getFileList(const std::string& userName)
{
    if (userName.empty())
    {
        throw std::invalid_argument("User name cannot be empty.");
    }

    sqlite3* db = nullptr;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* selectSQL = R"(
        SELECT fileName FROM svg_data WHERE userName = ? ORDER BY timestamp DESC;
    )";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);

    std::vector<std::string> fileList;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* fileName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        fileList.emplace_back(fileName);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return fileList;
}
