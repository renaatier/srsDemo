import React, { useState } from "react";
import { useAuth } from "../context/AuthContext";

const AuthForm = () => {
    const { user, login, logout, isLoggedIn } = useAuth();
    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");

    const handleLogin = () => {
        if (username && password) {
            login(username, password);
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
                    <button onClick={handleLogin}>Login</button>
                </div>
            )}
        </div>
    );
};

export default AuthForm;
