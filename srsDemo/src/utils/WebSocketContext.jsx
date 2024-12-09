import React, { createContext, useContext, useEffect, useState } from "react";
import { w3cwebsocket as W3CWebSocket } from "websocket";

const WebSocketContext = createContext();

export const WebSocketProvider = ({ children }) => {
    const [client, setClient] = useState(null);
    const [fileList, setFileList] = useState([]);
    const [svgData, setSvgData] = useState(null);

    useEffect(() => {
        const socket = new W3CWebSocket("ws://localhost:8080");
        setClient(socket);

        socket.onmessage = (event) => {
            try {
                const payload = JSON.parse(event.data);
                if (payload.fileList) {
                    setFileList(payload.fileList);
                } else if (payload.svgData) {
                    setSvgData(payload.svgData);
                }
            } catch (error) {
                console.error("Error parsing incoming message:", error);
            }
        };

        return () => socket.close();
    }, []);

    const requestFileList = () => {
        if (client?.readyState === client.OPEN) {
            client.send(JSON.stringify({ getFileList: "true" }));
        } else {
            console.error("WebSocket is not open.");
        }
    };

    const requestSvgByFileName = (fileName) => {
        if (client?.readyState === client.OPEN) {
            client.send(JSON.stringify({ getFileByName: fileName }));
        } else {
            console.error("WebSocket is not open.");
        }
    };

    const saveSvg = (fileName, svgString) => {
        if (client?.readyState === client.OPEN) {
            client.send(JSON.stringify({ fileName, svgData: svgString }));
        } else {
            console.error("WebSocket is not open.");
        }
    };

    return (
        <WebSocketContext.Provider
            value={{ client, fileList, svgData, requestFileList, requestSvgByFileName, saveSvg }}
        >
            {children}
        </WebSocketContext.Provider>
    );
};

export const useWebSocket = () => useContext(WebSocketContext);
