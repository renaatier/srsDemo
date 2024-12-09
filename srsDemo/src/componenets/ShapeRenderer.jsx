import React from 'react';
import Rectangle from './Rectangle';
import Circle from './Circle';

const ShapeRenderer = ({ shape, onUpdate }) => {
    if (shape.type === 'rect') {
        return <Rectangle shape={shape} onUpdate={onUpdate} />;
    }
    if (shape.type === 'circle') {
        return <Circle shape={shape} onUpdate={onUpdate} />;
    }
    return null;
};

export default ShapeRenderer;
