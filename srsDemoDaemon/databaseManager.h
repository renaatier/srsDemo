#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

//Why won't it compile without this?
#define SQLITECPP_COMPILE_DLL

#include <string>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

using namespace std;

class DatabaseManager {
public:
    DatabaseManager(const string& dbPath);
    ~DatabaseManager();

    void saveSVG(const string& fileName, const string& svgData);
    string getSVG(const string& fileName);
    vector<string> getFileList();

private:
    string databasePath;
    void initializeDatabase();
};

#endif // DATABASEMANAGER_H
