import React from 'react';
import { useDrag, useResize } from '../utils/EventHandlers';

const Circle = ({ shape, onUpdate }) => {
    const { handleDragStart, handleDragMove } = useDrag(shape, onUpdate);
    const { handleResizeStart, handleResizeMove } = useResize(shape, onUpdate);

    return (
        <g>
            <circle
                cx={shape.x}
                cy={shape.y}
                r={shape.r}
                fill={shape.fill}
                onMouseDown={handleDragStart}
                onMouseMove={handleDragMove}
            />
            {}
            <circle
                cx={shape.x + shape.r}
                cy={shape.y}
                r={6}
                fill="red"
                onMouseDown={(e) => handleResizeStart(e, { dx: 1 })}
                onMouseMove={(e) => handleResizeMove(e, { dx: 1 })}
            />
        </g>
    );
};

export default Circle;
