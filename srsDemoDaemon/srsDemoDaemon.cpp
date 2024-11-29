#include "DatabaseManager.h"
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include <thread>
#include <chrono>
#include <curses.h>


namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;

using namespace std;
using json = nlohmann::json;

void run_server();
void run_curses_menu(DatabaseManager& dbManager, websocket::stream<asio::ip::tcp::socket>& ws);

int main() {
    run_server();
    return 0;
}

void run_server() {
    try {
        DatabaseManager dbManager("svg_database.db");

        asio::io_context ioc;
        asio::ip::tcp::acceptor acceptor(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8080));
        asio::ip::tcp::socket socket(ioc);

        cout << "Waiting for connection on localhost:8080..." << endl;
        acceptor.accept(socket);

        websocket::stream<asio::ip::tcp::socket> ws(move(socket));
        ws.accept();

        //Launch curses menu in a separate thread
        thread curses_thread(run_curses_menu, ref(dbManager), ref(ws));

        while (true) {
            beast::flat_buffer buffer;
            cout << "Waiting for bytes..." << endl;
            ws.read(buffer);

            string received_data = beast::buffers_to_string(buffer.data());
            cout << "Received data: " << received_data << endl;

            try {
                auto payload = json::parse(received_data);

                if (payload.contains("fileName") && payload.contains("svgData")) {
                    string fileName = payload["fileName"];
                    string svgData = payload["svgData"];
                    dbManager.saveSVG(fileName, svgData);
                    cout << "Data saved to database successfully." << endl;
                }
                else if (payload.contains("getFileList")) {
                    vector<string> fileList = dbManager.getFileList();
                    json response = { {"fileList", fileList} };
                    ws.write(boost::asio::buffer(response.dump()));
                    cout << "File list sent to the client." << endl;
                }
                else if (payload.contains("getFileByName")) {
                    string fileName = payload["getFileByName"];
                    string svgData = dbManager.getSVG(fileName);
                    if (!svgData.empty()) {
                        json response = { {"svgData", svgData} };
                        ws.write(boost::asio::buffer(response.dump()));
                        cout << "SVG data for " << fileName << " sent to the client." << endl;
                    }
                    else {
                        json response = { {"error", "File not found"} };
                        ws.write(boost::asio::buffer(response.dump()));
                        cout << "Error: File not found." << endl;
                    }
                }
                else {
                    cerr << "Invalid payload received." << endl;
                }
            }
            catch (const json::exception& e) {
                cerr << "JSON parsing error: " << e.what() << endl;
            }
        }

        // Ensure the PDCurses thread is joined before exiting
        curses_thread.join();
    }
    catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void run_curses_menu(DatabaseManager& dbManager, websocket::stream<asio::ip::tcp::socket>& ws) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);  // Enable keypad input for special keys

    vector<string> fileList = dbManager.getFileList();
    int choice = 0;
    int highlight = 0;

    while (true) {
        clear();
        mvprintw(0, 0, "PDCurses Menu - Select a File to Load");

        for (size_t i = 0; i < fileList.size(); ++i) {
            if (i == highlight) {
                attron(A_REVERSE);
            }
            mvprintw(i + 1, 0, fileList[i].c_str());
            if (i == highlight) {
                attroff(A_REVERSE);
            }
        }

        int input = getch();

        switch (input) {
        case KEY_UP:
            if (highlight > 0) highlight--;
            break;
        case KEY_DOWN:
            if (highlight < fileList.size() - 1) highlight++;
            break;
        case 10:  //Enter key
            choice = highlight;
            {
                string selectedFile = fileList[choice];
                json request = { {"getFileByName", selectedFile} };
                ws.write(asio::buffer(request.dump()));
                mvprintw(fileList.size() + 2, 0, ("Requested file: " + selectedFile).c_str());
            }
            break;
        default:
            break;
        }

        if (input == 'q') {
            break;
        }
    }

    endwin();
}
