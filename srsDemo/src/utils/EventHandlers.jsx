export const useDrag = (shape, onUpdate) => {
    const handleDragStart = (e) => {
        shape.dragging = true;
        shape.offsetX = e.clientX - shape.x;
        shape.offsetY = e.clientY - shape.y;
    };

    const handleDragMove = (e) => {
        if (shape.dragging) {
            const updatedShape = {
                ...shape,
                x: e.clientX - shape.offsetX,
                y: e.clientY - shape.offsetY,
            };
            onUpdate(updatedShape);
        }
    };

    return { handleDragStart, handleDragMove };
};

export const useResize = (shape, onUpdate) => {
    const handleResizeStart = (e, offset) => {
        shape.resizing = offset;
    };

    const handleResizeMove = (e) => {
        if (shape.resizing) {
            const { dx = 0, dy = 0 } = shape.resizing;
            const updatedShape = {
                ...shape,
                width: shape.width + e.movementX * dx,
                height: shape.height + e.movementY * dy,
            };
            onUpdate(updatedShape);
        }
    };

    return { handleResizeStart, handleResizeMove };
};
