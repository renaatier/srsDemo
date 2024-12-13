import React, { useRef } from "react";
import Canvas from "./components/Canvas";
import AuthForm from "./components/AuthForm";
import { AuthProvider } from "./context/AuthContext";
import { useWebSocket } from "./context/WebSocketContext";

const App = () => {
    const { fileList, requestFileList, requestSvgByFileName, saveSvg } = useWebSocket();
    const svgRef = useRef();

    const handleSaveCanvas = () => {
        if (svgRef.current) {
            const svgElement = svgRef.current;
            const serializer = new XMLSerializer();
            const svgString = serializer.serializeToString(svgElement);
            const fileName = prompt("Enter the file name:");
            if (fileName) {
                saveSvg(fileName, svgString);
            }
        }
    };

    return (
        <AuthProvider>
            <div>
                <h1>SVG Editor</h1>
                <AuthForm />
                <div ref={svgRef} style={{ border: "1px solid black", width: "800px", height: "600px" }}>
                    <Canvas />
                </div>
                <button onClick={handleSaveCanvas}>Save as XML</button>
                <button onClick={requestFileList}>Load Files</button>
                {fileList.length > 0 && (
                    <div style={{ marginTop: "20px" }}>
                        <h3>Available Files</h3>
                        {fileList.map((file) => (
                            <div
                                key={file}
                                style={{
                                    cursor: "pointer",
                                    padding: "5px",
                                    margin: "5px 0",
                                    background: "#f9f9f9",
                                }}
                                onClick={() => requestSvgByFileName(file)}
                            >
                                {file}
                            </div>
                        ))}
                    </div>
                )}
            </div>
        </AuthProvider>
    );
};

export default App;
