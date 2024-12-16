import React, { useEffect, useState } from "react";
import ShapeRenderer from "./ShapeRenderer";
import { useWebSocket } from "../context/WebSocketContext";

const Canvas = () => {
    const { svgData } = useWebSocket();

    const defaultShapes = [
        {
            id: "circle-1",
            type: "circle",
            x: 100,
            y: 100,
            r: 50,
            fill: "blue",
        },
        {
            id: "rect-1",
            type: "rectangle",
            x: 300,
            y: 200,
            width: 100,
            height: 100,
            fill: "green",
        },
    ];

    const [shapes, setShapes] = useState(defaultShapes);

    useEffect(() => {
        if (svgData) {
            const parsedShapes = JSON.parse(svgData);
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

