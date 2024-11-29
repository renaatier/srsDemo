import React, { useRef, useState, useEffect } from "react";
import Canvas from "./Canvas";
import { w3cwebsocket as W3CWebSocket } from "websocket";

const client = new W3CWebSocket("ws://localhost:8080");

const App = () => {
    const svgRef = useRef();
    const [filename, setFilename] = useState("");
    const [fileList, setFileList] = useState([]);
    const [showFileList, setShowFileList] = useState(false);

    useEffect(() => {
        client.onmessage = (event) => {
            try {
                const payload = JSON.parse(event.data);

                if (payload.fileList) {
                    setFileList(payload.fileList);
                    setShowFileList(true);
                } else if (payload.svgData) {
                    loadCanvas(payload.svgData);
                    setShowFileList(false);
                } else {
                    console.error("Invalid payload received:", payload);
                }
            } catch (error) {
                console.error("Error parsing incoming message:", error);
            }
        };
    }, []);

    const saveCanvas = () => {
        if (svgRef.current) {
            const svgElement = svgRef.current;
            const serializer = new XMLSerializer();
            const svgString = serializer.serializeToString(svgElement);
            console.log(svgString);

            const userInput = prompt(
                `Enter name of file: ${filename}`,
                filename
            );

            if (userInput) {
                setFilename(userInput);

                if (client.readyState === client.OPEN) {
                    const payload = {
                        fileName: userInput,
                        svgData: svgString,
                    };
                    client.send(JSON.stringify(payload));
                    console.log("SVG and filename sent to the server.");
                } else {
                    console.error("WebSocket connection is not open.");
                }
            } else {
                console.error("Filename entry was cancelled.");
            }
        }
    };

    const loadCanvas = (svgData) => {
        if (svgRef.current) {
            svgRef.current.innerHTML = svgData;
            console.log("Canvas updated with new SVG data.");
        } else {
            console.error("SVG reference is not available.");
        }
    };

    const loadFileList = () => {
        if (client.readyState === client.OPEN) {
            client.send(JSON.stringify({ "getFileList": "true" }));
            console.log("Requested file list from the server.");
        } else {
            console.error("WebSocket connection is not open.");
        }
    };

    const handleFileSelect = (selectedFileName) => {
        if (client.readyState === client.OPEN) {
            client.send(JSON.stringify({ "getFileByName": selectedFileName }));
            console.log(`Requested SVG for file: ${selectedFileName}`);
        } else {
            console.error("WebSocket connection is not open.");
        }
    };

    return (
        <div>
            <h1>SVG Drag and Drop Canvas</h1>
            <div ref={svgRef} style={{ border: "1px solid black", width: "500px", height: "500px" }}>
                <Canvas />
            </div>
            <button onClick={saveCanvas}>Save as XML</button>
            <button onClick={loadFileList}>Load XML</button>

            {showFileList && (
                <div
                    style={{
                        marginTop: "20px",
                        border: "1px solid gray",
                        maxHeight: "200px",
                        overflowY: "scroll",
                        width: "300px",
                        padding: "10px",
                    }}
                >
                    <h3>Select a File to Load:</h3>
                    {fileList.length > 0 ? (
                        fileList.map((fileName) => (
                            <div
                                key={fileName}
                                style={{
                                    cursor: "pointer",
                                    padding: "5px",
                                    margin: "5px 0",
                                    backgroundColor: filename === fileName ? "#f0f0f0" : "transparent",
                                }}
                                onClick={() => handleFileSelect(fileName)}
                            >
                                {fileName}
                            </div>
                        ))
                    ) : (
                        <div>No files available</div>
                    )}
                </div>
            )}
        </div>
    );
};

export default App;
