#ifndef AUTHDATABASEMANAGER_H
#define AUTHDATABASEMANAGER_H

#include <string>
#include <optional>

class AuthDatabaseManager
{
public:
    AuthDatabaseManager();
    ~AuthDatabaseManager();

    bool createUser(const std::string& userName, const std::string& password);
    bool validateUser(const std::string& userName, const std::string& password);

private:
    std::string databasePath;
    void initializeDatabase();
    std::string generateSalt() const;
    std::string hashPassword(const std::string& password, const std::string& salt) const;
};

#endif // AUTHDATABASEMANAGER_H
