import React, { createContext, useContext, useEffect, useState } from "react";
import { w3cwebsocket as W3CWebSocket } from "websocket";

const WebSocketContext = createContext();

export const WebSocketProvider = ({ children }) => {
    const [client, setClient] = useState(null);
    const [user, setUser] = useState(null);
    const [fileList, setFileList] = useState([]);
    const [svgData, setSvgData] = useState(null);
    const isLoggedIn = () => !!localStorage.getItem("sessionId");

    useEffect(() => {
        const socket = new W3CWebSocket("ws://localhost:8080");
        setClient(socket);

        socket.onmessage = (event) => {
            try {
                const payload = JSON.parse(event.data);

                if (payload.error) {
                    alert(payload.error);
                    return;
                }

                switch (payload.action) {
                    case "login":
                        if (payload.sessionId) {
                            setUser(payload.username);
                            localStorage.setItem("sessionId", payload.sessionId);
                        }
                        break;

                    case "createUser":
                        alert(payload.message || "User created successfully!");
                        setUser(payload.username);
                        localStorage.setItem("sessionId", payload.sessionId);
                        break;

                    case "fileList":
                        setFileList(payload.fileList);
                        break;

                    case "svgData":
                        setSvgData(payload.svgData);
                        break;

                    default:
                        console.warn("Unknown action:", payload.action);
                }
            } catch (error) {
                console.error("Error parsing incoming message:", error);
            }
        };

        socket.onopen = () => console.log("WebSocket connection established.");
        socket.onclose = () => console.log("WebSocket connection closed.");

        return () => socket.close();
    }, []);

    const sendPayload = (payload) => {
        if (client?.readyState === client.OPEN) {
            client.send(JSON.stringify(payload));
        } else {
            console.error("WebSocket is not open.");
        }
    };

    const login = (username, password) => {
        sendPayload({ action: "login", username, password });
    };

    const logout = () => {
        sendPayload({ action: "logout", sessionId: localStorage.getItem("sessionId") });
        setUser(null);
        localStorage.removeItem("sessionId");
    };

    const register = (username, password) => {
        sendPayload({ action: "createUser", username, password });
    };

    const requestFileList = () => {
        sendPayload({ action: "getFileList", sessionId: localStorage.getItem("sessionId") });
    };

    const requestSvgByFileName = (fileName) => {
        sendPayload({ action: "getFileByName", fileName, sessionId: localStorage.getItem("sessionId") });
    };

    const saveSvg = (fileName, svgString) => {
        sendPayload({ action: "saveSVG", fileName, svgData: svgString, sessionId: localStorage.getItem("sessionId") });
    };

    return (
        <WebSocketContext.Provider
            value={{
                client,
                user,
                isLoggedIn,
                login,
                logout,
                register,
                fileList,
                svgData,
                requestFileList,
                requestSvgByFileName,
                saveSvg,
            }}
        >
            {children}
        </WebSocketContext.Provider>
    );
};

export const useWebSocket = () => useContext(WebSocketContext);
