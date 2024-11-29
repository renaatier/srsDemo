import React, { useState } from 'react';

const Canvas = () => {
    const [shapes, setShapes] = useState([
        { id: 1, x: 50, y: 50, width: 100, height: 100, type: 'rect', fill: 'blue' },
        { id: 2, x: 200, y: 200, r: 50, type: 'circle', fill: 'green' }
    ]);

    const [dragging, setDragging] = useState(null);
    const [resizing, setResizing] = useState(null);
    const [dragOffset, setDragOffset] = useState({ x: 0, y: 0 });

    // Handling Dragging
    const handleMouseDown = (e, id) => {
        const shape = shapes.find(shape => shape.id === id);
        const offsetX = e.clientX - shape.x;
        const offsetY = e.clientY - shape.y;

        setDragOffset({ x: offsetX, y: offsetY });
        setDragging(id);
    };

    const handleMouseMove = (e) => {
        if (dragging !== null) {
            const updatedShapes = shapes.map(shape => {
                if (shape.id === dragging) {
                    return { ...shape, x: e.clientX - dragOffset.x, y: e.clientY - dragOffset.y };
                }
                return shape;
            });
            setShapes(updatedShapes);
        }
        if (resizing !== null) {
            const updatedShapes = shapes.map(shape => {
                if (shape.id === resizing) {
                    if (shape.type === 'rect') {
                        return { ...shape, width: shape.width + (e.clientX - (shape.x + shape.width)), height: shape.height + (e.clientY - (shape.y + shape.height + 400)) };
                    }
                    if (shape.type === 'circle') {
                        const radius = Math.sqrt(Math.pow(e.clientX - shape.x, 2) + Math.pow(e.clientY - shape.y - 400, 2));
                        return { ...shape, r: radius };
                    }
                }
                return shape;
            });
            setShapes(updatedShapes);
        }
    };

    const handleMouseUp = () => {
        setDragging(null);
        setResizing(null);
    };

    // Handling Resizing
    const handleResizeMouseDown = (e, id) => {
        setResizing(id);
        e.stopPropagation(); // Prevent drag event from being triggered
    };

    return (
        <svg
            width="800"
            height="600"
            style={{ border: '1px solid black' }}
            onMouseMove={handleMouseMove}
            onMouseUp={handleMouseUp}
        >
            {shapes.map(shape => (
                shape.type === 'rect' ? (
                    <g key={shape.id}>
                        <rect
                            x={shape.x}
                            y={shape.y}
                            width={shape.width}
                            height={shape.height}
                            fill={shape.fill}
                            onMouseDown={(e) => handleMouseDown(e, shape.id)}
                        />
                        {/* Resize Handle (Bottom-right corner) */}
                        <circle
                            cx={shape.x + shape.width}
                            cy={shape.y + shape.height}
                            r={8}
                            fill="red"
                            onMouseDown={(e) => handleResizeMouseDown(e, shape.id)}
                        />
                    </g>
                ) : (
                    <g key={shape.id}>
                        <circle
                            cx={shape.x}
                            cy={shape.y}
                            r={shape.r}
                            fill={shape.fill}
                            onMouseDown={(e) => handleMouseDown(e, shape.id)}
                        />
                        {/* Resize Handle (On the edge of the circle) */}
                        <circle
                            cx={shape.x + shape.r}
                            cy={shape.y}
                            r={8}
                            fill="red"
                            onMouseDown={(e) => handleResizeMouseDown(e, shape.id)}
                        />
                    </g>
                )
            ))}
        </svg>
    );
};

export default Canvas;
