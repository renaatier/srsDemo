import React, { useState } from "react";
import { useWebSocket } from "../context/WebSocketContext";

const AuthForm = () => {
    const { user, isLoggedIn, login, logout, register } = useWebSocket();
    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");
    const [registerMessage, setRegisterMessage] = useState("");

    const handleLogin = () => {
        if (username && password) {
            login(username, password);
        } else {
            alert("Please enter both username and password.");
        }
    };

    const handleRegister = () => {
        if (username && password) {
            register(username, password, setRegisterMessage);
        } else {
            alert("Please enter both username and password.");
        }
    };

    return (
        <div style={{ marginBottom: "20px" }}>
            {isLoggedIn() ? (
                <div>
                    <p>Welcome, {user}!</p>
                    <button onClick={logout}>Logout</button>
                </div>
            ) : (
                <div>
                    <input
                        type="text"
                        placeholder="Username"
                        value={username}
                        onChange={(e) => setUsername(e.target.value)}
                    />
                    <input
                        type="password"
                        placeholder="Password"
                        value={password}
                        onChange={(e) => setPassword(e.target.value)}
                    />
                    <div style={{ marginTop: "10px" }}>
                        <button onClick={handleLogin}>Login</button>
                        <button onClick={handleRegister} style={{ marginLeft: "10px" }}>
                            Register
                        </button>
                    </div>
                    {registerMessage && (
                        <p style={{ color: "green", marginTop: "10px" }}>{registerMessage}</p>
                    )}
                </div>
            )}
        </div>
    );
};

export default AuthForm;
