#include "DatabaseManager.h"
#include <iostream>
#include <vector>

DatabaseManager::DatabaseManager(const std::string& dbPath)
    : databasePath(dbPath)
{
    initializeDatabase();
}

DatabaseManager::~DatabaseManager() {}

void DatabaseManager::initializeDatabase()
{
    try 
    {
        SQLite::Database db(databasePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        const std::string createTableSQL = R"(
            CREATE TABLE IF NOT EXISTS svg_data (
                fileName TEXT PRIMARY KEY,
                svgData TEXT NOT NULL,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            );
        )";

        db.exec(createTableSQL);
        std::cout << "Database initialized successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
    }
}

void DatabaseManager::saveSVG(const std::string& fileName, const std::string& svgData)
{
    try 
    {
        SQLite::Database db(databasePath, SQLite::OPEN_READWRITE);

        const std::string insertSQL = R"(
            INSERT OR REPLACE INTO svg_data (fileName, svgData) 
            VALUES (?, ?);
        )";

        SQLite::Statement query(db, insertSQL);
        query.bind(1, fileName);
        query.bind(2, svgData);

        query.exec();
        std::cout << "SVG data saved successfully with fileName: " << fileName << std::endl;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error saving SVG data: " << e.what() << std::endl;
    }
}

std::string DatabaseManager::getSVG(const std::string& fileName)
{
    try {
        SQLite::Database db(databasePath, SQLite::OPEN_READONLY);

        const std::string selectSQL = R"(
            SELECT svgData FROM svg_data WHERE fileName = ?;
        )";

        SQLite::Statement query(db, selectSQL);
        query.bind(1, fileName);

        if (query.executeStep()) 
        {
            return query.getColumn(0).getString();
        }
        else 
        {
            std::cerr << "No SVG data found for fileName: " << fileName << std::endl;
        }
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error retrieving SVG data: " << e.what() << std::endl;
    }

    return "";
}

vector<string> DatabaseManager::getFileList()
{
    vector<string> fileList;

    try
    {
        SQLite::Database db(databasePath, SQLite::OPEN_READONLY);

        const std::string selectSQL = R"(
            SELECT fileName FROM svg_data ORDER BY timestamp DESC;
        )";

        SQLite::Statement query(db, selectSQL);

        while (query.executeStep())
        {
            fileList.push_back(query.getColumn(0).getString());
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error retrieving file list: " << e.what() << std::endl;
    }

    return fileList;
}
