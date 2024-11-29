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
