import React from "react";
import Circle from "./Circle";
import Rectangle from "./Rectangle";

const ShapeRenderer = ({ shape, onUpdate }) => {
    switch (shape.type) {
        case "circle":
            return <Circle shape={shape} onUpdate={onUpdate} />;
        case "rectangle":
            return <Rectangle shape={shape} onUpdate={onUpdate} />;
        default:
            return null;
    }
};

export default ShapeRenderer;
