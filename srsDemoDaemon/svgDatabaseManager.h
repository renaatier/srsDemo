#ifndef SVGDATABASEMANAGER_H
#define SVGDATABASEMANAGER_H

#include <string>
#include <vector>

class SVGDatabaseManager
{
public:
    SVGDatabaseManager();
    ~SVGDatabaseManager();

    void saveSVG(const std::string& fileName, const std::string& userName, const std::vector<unsigned char>& svgData);
    std::vector<unsigned char> getSVG(const std::string& fileName, const std::string& userName);
    std::vector<std::string> getFileList(const std::string& userName);

private:
    std::string databasePath;
    void initializeDatabase();
};

#endif // SVGDATABASEMANAGER_H

