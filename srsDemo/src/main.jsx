import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import App from './App.jsx';
import { WebSocketProvider } from './context/WebSocketContext';
import './main.css';

createRoot(document.getElementById('root')).render(
    <StrictMode>
        <WebSocketProvider>
            <App />
        </WebSocketProvider>
    </StrictMode>
);
