#include "DatabaseManager.h"
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
#include <tinyxml2.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

using json = nlohmann::json;

//Purposefully empty
struct PerConnectionData {};

std::string getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}

void logMessage(const std::string& message, bool isError = false)
{
    static std::ofstream logFile("logfile.txt", std::ios::app); // Open log file in append mode

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

void handleSaveSVG(DatabaseManager& dbManager, const json& payload, auto* ws)
{
    std::string fileName = payload["fileName"];
    std::string svgData = payload["svgData"];

    if (validateXML(svgData))
    {
        dbManager.saveSVG(fileName, std::vector<unsigned char>(svgData.begin(), svgData.end()));
        logMessage("SVG data saved successfully: " + fileName);
        ws->send("SVG saved successfully.");
    }
    else
    {
        logMessage("Invalid XML format for file: " + fileName, true);
        ws->send(R"({"error": "Invalid XML format."})");
    }
}

void handleGetFileList(DatabaseManager& dbManager, auto* ws)
{
    std::vector<std::string> fileList = dbManager.getFileList();
    json response = { {"fileList", fileList} };
    ws->send(response.dump());
    logMessage("File list sent to the client.");
}

void handleGetSVG(DatabaseManager& dbManager, const json& payload, auto* ws)
{
    std::string fileName = payload["getFileByName"];
    try
    {
        std::vector<unsigned char> svgData = dbManager.getSVG(fileName);
        std::string svgDataStr(svgData.begin(), svgData.end());
        json response = { {"svgData", svgDataStr} };
        ws->send(response.dump());
        logMessage("SVG data for " + fileName + " sent to the client.");
    }
    catch (const std::runtime_error& e)
    {
        logMessage("Error retrieving SVG: " + std::string(e.what()), true);
        ws->send(R"({"error": "File not found."})");
    }
}

void handleMessage(DatabaseManager& dbManager, std::string_view message, auto* ws)
{
    try
    {
        auto payload = json::parse(message);

        if (payload.contains("fileName") && payload.contains("svgData"))
        {
            handleSaveSVG(dbManager, payload, ws);
        }
        else if (payload.contains("getFileList"))
        {
            handleGetFileList(dbManager, ws);
        }
        else if (payload.contains("getFileByName"))
        {
            handleGetSVG(dbManager, payload, ws);
        }
        else
        {
            ws->send(R"({"error": "Invalid request."})");
            logMessage("Invalid payload received.", true);
        }
    }
    catch (const json::exception& e)
    {
        logMessage("JSON parsing error: " + std::string(e.what()), true);
        ws->send(R"({"error": "Error parsing JSON."})");
    }
    catch (const std::exception& e)
    {
        logMessage("Unexpected error: " + std::string(e.what()), true);
        ws->send(R"({"error": "Internal server error."})");
    }
}

void run_server()
{
    try
    {
        DatabaseManager dbManager("svg_database.db");

        uWS::App()
            .ws<PerConnectionData>("/*", {
                .open = [](auto* ws)
                {
                    logMessage("Connection opened.");
                },
                .message = [&dbManager](auto* ws, std::string_view message, uWS::OpCode opCode)
                {
                    logMessage("Received message: " + std::string(message));
                    handleMessage(dbManager, message, ws);
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
