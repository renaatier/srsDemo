v0.3a
Changes:
-Client: UI for Login/Register
-Client: authorization stuff for client/server socket, storing session, etc
-Server: Handle auth/create/logout requests from client
-Server: users table, SHA. svg table is now FK'd to users.

Known issues:
-React is the worst. AuthProvider causing mysterious React JS console errors.

v0.2
Changes:
-Cross platform support, removed all Windows specific stuff
-More robust daemon with data checks, error handling, logging
-Rewrote React code to fix issue with context being lost after updating the canvas
-Replaced boost with uwebsockets and sqlitecpp with sqlite3
-Removed loading files from curses from server, it was just used for practice anyway

v0.1
# srsDemo
A simple client-server application for SVG editing. 
The server is implemented in C++ with WebSocket support, and the client is built using React.
The client is currently hardcoded to run on the user's localhost for demo purposes

## Features
- Drag-and-drop SVG shapes
- Resize and save shapes to the server
- Load saved SVG files from the server

## How to Run
- Start the server: 
./srsDemoDaemon/x64/Debug/srsDemoDaemon.exe

- Start the client: 
./srsDemo/npm install
./srsDemo/npm run dev
This will output port number, navigate in browser to localhost:<port>
