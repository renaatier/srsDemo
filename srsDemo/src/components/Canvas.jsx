import React, { useEffect, useState } from "react";
import ShapeRenderer from "./ShapeRenderer";
import { useWebSocket } from "../context/WebSocketContext";

const Canvas = () => {
    const { svgData } = useWebSocket();
    const [shapes, setShapes] = useState([]);

    useEffect(() => {
        if (svgData) {
            const parsedShapes = JSON.parse(svgData); // Ensure SVG is parsed to shapes
            setShapes(parsedShapes);
        }
    }, [svgData]);

    const updateShape = (updatedShape) => {
        setShapes((prevShapes) =>
            prevShapes.map((shape) => (shape.id === updatedShape.id ? updatedShape : shape))
        );
    };

    return (
        <svg width="800" height="600" style={{ border: "1px solid black" }}>
            {shapes.map((shape) => (
                <ShapeRenderer key={shape.id} shape={shape} onUpdate={updateShape} />
            ))}
        </svg>
    );
};

export default Canvas;
