#include "SVGDatabaseManager.h"
#include "AuthDatabaseManager.h"
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
#include <tinyxml2.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include <random>

using json = nlohmann::json;

//Purposefully empty
struct PerConnectionData{};

std::string getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}

void logMessage(const std::string& message, bool isError = false)
{
    static std::ofstream logFile("logfile.txt", std::ios::app);

    if (!logFile.is_open())
    {
        std::cerr << "Failed to open logfile.txt for logging." << std::endl;
        return;
    }

    std::string timestampedMessage = "[" + getCurrentTimestamp() + "] " + (isError ? "!!! " : "") + message;

    if (isError)
    {
        std::cerr << timestampedMessage << std::endl;
    }
    else
    {
        std::cout << timestampedMessage << std::endl;
    }

    logFile << timestampedMessage << std::endl;
}

bool validateXML(const std::string& xmlData)
{
    tinyxml2::XMLDocument doc;
    return doc.Parse(xmlData.c_str()) == tinyxml2::XML_SUCCESS;
}

std::unordered_map<std::string, std::string> sessionStore;
std::mutex sessionMutex;

std::string generateSessionID()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::string sessionID(32, '0');
    for (auto& c : sessionID)
    {
        c = "0123456789abcdef"[dis(gen)];
    }
    return sessionID;
}

void addSession(const std::string& sessionID, const std::string& username)
{
    std::lock_guard<std::mutex> lock(sessionMutex);
    sessionStore[sessionID] = username;
}

bool validateSession(const std::string& sessionID, std::string& username)
{
    std::lock_guard<std::mutex> lock(sessionMutex);
    auto it = sessionStore.find(sessionID);
    if (it != sessionStore.end())
    {
        username = it->second;
        return true;
    }
    return false;
}

void removeSession(const std::string& sessionID)
{
    std::lock_guard<std::mutex> lock(sessionMutex);
    sessionStore.erase(sessionID);
}

void handleLogin(AuthDatabaseManager& authDbManager, const json& payload, auto* ws)
{
    std::string username = payload["username"];
    std::string password = payload["password"];

    if (authDbManager.validateUser(username, password))
    {
        std::string sessionID = generateSessionID();
        addSession(sessionID, username);
        json response = { {"sessionID", sessionID}, {"message", "Login successful"} };
        ws->send(response.dump());
        logMessage("User " + username + " logged in successfully.");
    }
    else
    {
        ws->send(R"({"error": "Invalid credentials"})");
        logMessage("Failed login attempt for user " + username, true);
    }
}

void handleLogout(const json& payload, auto* ws)
{
    std::string sessionID = payload["sessionID"];

    if (!sessionID.empty())
    {
        removeSession(sessionID);
        ws->send(R"({"message": "Logout successful"})");
        logMessage("Session " + sessionID + " logged out.");
    }
    else
    {
        ws->send(R"({"error": "Invalid session"})");
        logMessage("Logout attempt failed due to missing session ID.", true);
    }
}

void handleCreateUser(AuthDatabaseManager& authDbManager, const json& payload, auto* ws)
{
    std::string username = payload["username"];
    std::string password = payload["password"];

    if (authDbManager.createUser(username, password))
    {
        std::string sessionID = generateSessionID();
        addSession(sessionID, username);
        json response = { {"sessionID", sessionID}, {"message", "Registration successful."} };
        ws->send(response.dump());
        logMessage("User " + username + " created successfully.");
    }
    else
    {
        ws->send(R"({"error": "User already exists."})");
        logMessage("Failed registration attempt for user " + username, true);
    }
}

void handleGetFileList(SVGDatabaseManager& dbManager, const json& payload, auto* ws)
{
    std::string sessionID = payload["sessionID"];
    std::string username;

    if (validateSession(sessionID, username))
    {
        std::vector<std::string> fileList = dbManager.getFileList(username);
        json response = { {"fileList", fileList} };
        ws->send(response.dump());
        logMessage("File list sent to user " + username);
    }
    else
    {
        ws->send(R"({"error": "Unauthorized"})");
        logMessage("Unauthorized attempt to access file list.", true);
    }
}

void handleGetSVG(SVGDatabaseManager& dbManager, const json& payload, auto* ws)
{
    std::string sessionID = payload["sessionID"];
    std::string username;

    if (validateSession(sessionID, username))
    {
        std::string fileName = payload["getFileByName"];
        std::vector<unsigned char> svgData = dbManager.getSVG(fileName, username);

        if (!svgData.empty())
        {
            std::string svgDataStr(svgData.begin(), svgData.end());
            json response = { {"svgData", svgDataStr} };
            ws->send(response.dump());
            logMessage("SVG data for " + fileName + " sent to user " + username);
        }
        else
        {
            ws->send(R"({"error": "File not found."})");
            logMessage("User " + username + " got empty result when trying to access file: " + fileName, true);
        }
    }
    else
    {
        ws->send(R"({"error": "Unauthorized"})");
        logMessage("Unauthorized attempt to retrieve SVG.", true);
    }
}


void handleMessage(SVGDatabaseManager& dbManager, AuthDatabaseManager& authDbManager, std::string_view message, auto* ws)
{
    try
    {
        auto payload = json::parse(message);

        if (payload.contains("login"))
        {
            handleLogin(authDbManager, payload, ws);
        }
        else if (payload.contains("logout"))
        {
            handleLogout(payload, ws);
        }
        else if (payload.contains("createUser"))
        {
            handleCreateUser(authDbManager, payload, ws);
        }
        else if (payload.contains("getFileList"))
        {
            handleGetFileList(dbManager, payload, ws);
        }
        else if (payload.contains("getFileByName"))
        {
            handleGetSVG(dbManager, payload, ws);
        }
        else
        {
            ws->send(R"({"error": "Invalid request"})");
            logMessage("Invalid payload received.", true);
        }
    }
    catch (const json::exception& e)
    {
        logMessage("JSON parsing error: " + std::string(e.what()), true);
        ws->send(R"({"error": "Error parsing JSON"})");
    }
    catch (const std::exception& e)
    {
        logMessage("Unexpected error: " + std::string(e.what()), true);
        ws->send(R"({"error": "Internal server error"})");
    }
}

void run_server()
{
    try
    {
        SVGDatabaseManager dbManager;
        AuthDatabaseManager authDbManager;

        uWS::App()
            .ws<PerConnectionData>("/*", {
                .open = [](auto* ws)
                {
                    logMessage("Connection opened.");
                },
                .message = [&dbManager, &authDbManager](auto* ws, std::string_view message, uWS::OpCode opCode)
                {
                    logMessage("Received message: " + std::string(message));
                    handleMessage(dbManager, authDbManager, message, ws);
                },
                .close = [](auto* ws, int code, std::string_view message)
                {
                    logMessage("Connection closed. Code: " + std::to_string(code) + ", Message: " + std::string(message));
                }
                })
            .listen(8080, [](auto* token)
                {
                    if (token)
                    {
                        logMessage("Server listening on port 8080.");
                    }
                    else
                    {
                        logMessage("Failed to bind server to port 8080.", true);
                        throw std::runtime_error("Unable to bind server to port.");
                    }
                })
            .run();
    }
    catch (const std::exception& e)
    {
        logMessage("Fatal error: " + std::string(e.what()), true);
    }
}

int main()
{
    run_server();
    return 0;
}
