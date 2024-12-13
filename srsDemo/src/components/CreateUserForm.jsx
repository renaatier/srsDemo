import React, { useState } from "react";
import { useAuth } from "../context/AuthContext";

const CreateUserForm = () => {
    const { register, isLoggedIn } = useAuth();
    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");
    const [confirmPassword, setConfirmPassword] = useState("");
    const [message, setMessage] = useState("");
    const [showForm, setShowForm] = useState(false);

    const handleRegister = () => {
        if (!username || !password || !confirmPassword) {
            setMessage("All fields are required.");
            return;
        }

        if (password !== confirmPassword) {
            setMessage("Passwords do not match.");
            return;
        }

        register(username, password, setMessage);
    };

    if (isLoggedIn()) {
        return null;
    }

    return (
        <div>
            {!showForm && (
                <button onClick={() => setShowForm(true)}>Register</button>
            )}
            {showForm && (
                <div>
                    <h3>Create New User</h3>
                    <div>
                        <input
                            type="text"
                            placeholder="Username"
                            value={username}
                            onChange={(e) => setUsername(e.target.value)}
                        />
                    </div>
                    <div>
                        <input
                            type="password"
                            placeholder="Password"
                            value={password}
                            onChange={(e) => setPassword(e.target.value)}
                        />
                    </div>
                    <div>
                        <input
                            type="password"
                            placeholder="Confirm Password"
                            value={confirmPassword}
                            onChange={(e) => setConfirmPassword(e.target.value)}
                        />
                    </div>
                    <button onClick={handleRegister}>Submit</button>
                    <button onClick={() => setShowForm(false)}>Cancel</button>
                    {message && <p>{message}</p>}
                </div>
            )}
        </div>
    );
};

export default CreateUserForm;
