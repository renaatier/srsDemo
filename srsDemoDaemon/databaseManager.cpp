#include "DatabaseManager.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

DatabaseManager::DatabaseManager(const std::string& dbPath) : databasePath(dbPath)
{
    initializeDatabase();
}

DatabaseManager::~DatabaseManager() = default;

void DatabaseManager::initializeDatabase()
{
    sqlite3* db = nullptr;

    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        std::ostringstream errMsg;
        errMsg << "Error opening database: " << sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error(errMsg.str());
    }

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS svg_data (
            fileName TEXT PRIMARY KEY,
            svgData BLOB NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
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

void DatabaseManager::saveSVG(const std::string& fileName, const std::vector<unsigned char>& svgData) 
{
    if (fileName.empty() || svgData.empty())
    {
        throw std::invalid_argument("File name or SVG data cannot be empty.");
    }

    sqlite3* db = nullptr;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* insertSQL = R"(
        INSERT OR REPLACE INTO svg_data (fileName, svgData)
        VALUES (?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, fileName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, svgData.data(), (int)svgData.size(), SQLITE_STATIC);

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

std::vector<unsigned char> DatabaseManager::getSVG(const std::string& fileName)
{
    if (fileName.empty())
    {
        throw std::invalid_argument("File name cannot be empty.");
    }

    sqlite3* db = nullptr;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* selectSQL = R"(
        SELECT svgData FROM svg_data WHERE fileName = ?;
    )";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, fileName.c_str(), -1, SQLITE_STATIC);

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
        throw std::runtime_error("No SVG data found for fileName: " + fileName);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return svgData;
}

std::vector<std::string> DatabaseManager::getFileList()
{
    sqlite3* db = nullptr;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK)
    {
        throw std::runtime_error("Error opening database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* selectSQL = R"(
        SELECT fileName FROM svg_data ORDER BY timestamp DESC;
    )";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error("Error preparing statement: " + std::string(sqlite3_errmsg(db)));
    }

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
