#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <vector>

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    void saveSVG(const std::string& fileName, const std::vector<unsigned char>& svgData);
    std::vector<unsigned char> getSVG(const std::string& fileName);
    std::vector<std::string> getFileList();

private:
    std::string databasePath;
    void initializeDatabase();
};

#endif // DATABASEMANAGER_H
