import React from 'react';
import { useDrag, useResize } from '../utils/EventHandlers';

const Rectangle = ({ shape, onUpdate }) => {
    const { handleDragStart, handleDragMove } = useDrag(shape, onUpdate);
    const { handleResizeStart, handleResizeMove } = useResize(shape, onUpdate);

    return (
        <g>
            <rect
                x={shape.x}
                y={shape.y}
                width={shape.width}
                height={shape.height}
                fill={shape.fill}
                onMouseDown={handleDragStart}
                onMouseMove={handleDragMove}
            />
            {}
            {[{ dx: 1, dy: 1 }, { dx: 1, dy: -1 }, { dx: -1, dy: 1 }, { dx: -1, dy: -1 }].map((offset, index) => (
                <circle
                    key={index}
                    cx={shape.x + shape.width * offset.dx}
                    cy={shape.y + shape.height * offset.dy}
                    r={6}
                    fill="red"
                    onMouseDown={(e) => handleResizeStart(e, offset)}
                    onMouseMove={(e) => handleResizeMove(e, offset)}
                />
            ))}
        </g>
    );
};

export default Rectangle;
