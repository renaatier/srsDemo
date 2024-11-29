import React from 'react';
import { Rect } from 'react-konva'; // Using Konva for shapes

const Shape = ({ x, y, width, height, fill }) => {
    return (
        <Rect x={x} y={y} width={width} height={height} fill={fill} draggable />
    );
};

export default Shape;
