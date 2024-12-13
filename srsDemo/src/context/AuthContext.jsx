import React, { createContext, useContext, useState } from "react";
import { useWebSocket } from "./WebSocketContext";

const AuthContext = createContext();

export const AuthProvider = ({ children }) => {
    const [sessionId, setSessionId] = useState(null);
    const [user, setUser] = useState(null);
    const { client } = useWebSocket();

    const login = async (username, password) => {
        if (client?.readyState === client.OPEN) {
            const credentials = { action: "login", username, password };
            client.send(JSON.stringify(credentials));
            client.onmessage = (event) => {
                const response = JSON.parse(event.data);
                if (response.sessionId) {
                    setSessionId(response.sessionId);
                    setUser(username);
                    localStorage.setItem("sessionId", response.sessionId);
                } else if (response.error) {
                    console.error(response.error);
                }
            };
        } else {
            console.error("WebSocket is not open.");
        }
    };

    const logout = () => {
        if (client?.readyState === client.OPEN && sessionId) {
            client.send(JSON.stringify({ action: "logout", sessionId }));
            setSessionId(null);
            setUser(null);
            localStorage.removeItem("sessionId");
        } else {
            console.error("WebSocket is not open or no active session.");
        }
    };

    const register = (username, password, setMessage) => {
        if (client?.readyState === client.OPEN) {
            const payload = {
                action: "createUser",
                username,
                password,
            };

            client.send(JSON.stringify(payload));
            client.onmessage = (event) => {
                const response = JSON.parse(event.data);
                if (response.success) {
                    setMessage("User created successfully!");
                    login(username, password);
                } else if (response.error) {
                    setMessage(response.error);
                }
            };
        } else {
            setMessage("WebSocket connection is not open.");
        }
    };

    const isLoggedIn = () => !!sessionId;

    return (
        <AuthContext.Provider value={{ user, isLoggedIn, login, logout, register }}>
            {children}
        </AuthContext.Provider>
    );
};

export const useAuth = () => useContext(AuthContext);
