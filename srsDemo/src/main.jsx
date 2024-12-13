import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import App from './App.jsx';
import { AuthProvider } from './context/AuthContext';
import { WebSocketProvider } from './context/WebSocketContext';
import './main.css';

createRoot(document.getElementById('root')).render(
    <StrictMode>
        <AuthProvider>
            <WebSocketProvider>
                <App />
            </WebSocketProvider>
        </AuthProvider>
    </StrictMode>
);
