#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define WIDTH 40
#define HEIGHT 20
#define MAX_SHAPES 100
char canvas[HEIGHT][WIDTH];
// Shape definitions to support adding, deleting, and modifying objects
typedef enum {
    SHAPE_RECTANGLE = 1,
    SHAPE_CIRCLE,
    SHAPE_LINE,
    SHAPE_TRIANGLE
} ShapeType;
typedef struct {
    ShapeType type;
    int cx;
    int cy;
    union {
        struct {
            int w;
            int h;
        } rect;
        struct {
            int r;
            float aspect;
        } circle;
        struct {
            int len;
        } line;
        struct {
            int size;
        } tri;
    } data;
} Shape;
Shape shapes[MAX_SHAPES];
int shapeCount = 0;
void initCanvas() {
    int i, j;
    for (i = 0; i < HEIGHT; i++)
        for (j = 0; j < WIDTH; j++)
            canvas[i][j] = '_';
}
void displayCanvas() {
    int i, j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (canvas[i][j] == '*') {
                /* Print '*' in green, then reset to default color */
                printf("\033[32m*\033[0m");
            } else {
                printf("%c", canvas[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}
void drawPoint(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        canvas[y][x] = '*';
}
/* Filled rectangle centered */
void drawFilledRectangle(int cx, int cy, int w, int h) {
    int x0 = cx - w/2;
    int y0 = cy - h/2;
    int i, j;
    for (i = 0; i < h; i++)
        for (j = 0; j < w; j++)
            drawPoint(x0+j, y0+i);
}
/* Filled circle centered (with half-pixel offset fix) */
void drawFilledCircle(int cx, int cy, int r, float aspect) {
    /* Use r + 0.5 to offset for pixel centers and round the top/bottom poles smoothly */
    float r_outer = r + 0.5f; 
    int x_bound = (int)(r_outer * aspect);
    float r_scaled_sq = (r_outer * aspect) * (r_outer * aspect);
    int y, x;
    if (r < 0) {
        return;
    }
    for (y = -r; y <= r; y++) {
        /* Scale y once per row to optimize performance */
        float y_scaled_sq = (y * aspect) * (y * aspect);
        
        for (x = -x_bound; x <= x_bound; x++) {
            /* Circle formula: x^2 + (y * aspect)^2 <= (r_outer * aspect)^2 */
            if (x * x + y_scaled_sq <= r_scaled_sq) {
                drawPoint(cx + x, cy + y);
            }
        }
    }
}
/* Line centered (diagonal) */
void drawLine(int cx, int cy, int len) {
    int x1 = cx - len/2, y1 = cy - len/2;
    int x2 = cx + len/2, y2 = cy + len/2;
    int dx = abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = -abs(y2-y1), sy = y1<y2 ? 1 : -1;
    int err = dx+dy, e2;
    while (1) {
        drawPoint(x1,y1);
        if (x1==x2 && y1==y2) break;
        e2 = 2*err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}
/* Filled triangle centered */
void drawFilledTriangle(int cx, int cy, int size) {
    int halfSize, topY, baseY, height, y, x;
    if (size <= 0) {
        return;
    }
    halfSize = size / 2;
    topY = cy - halfSize;
    baseY = cy + halfSize;
    height = baseY - topY;
    /* Handle size = 1 to prevent division by zero */
    if (height <= 0) {
        drawPoint(cx, cy);
        return;
    }
    for (y = topY; y <= baseY; y++) {
        int dy = y - topY;
        int halfWidth = (dy * halfSize + height / 2) / height;
        int xa = cx - halfWidth;
        int xb = cx + halfWidth;
        for (x = xa; x <= xb; x++) {
            drawPoint(x, y);
        }
    }
}
// Redraw canvas with all active shapes
void redrawCanvas() {
    initCanvas();
    int i;
    for (i = 0; i < shapeCount; i++) {
        switch (shapes[i].type) {
            case SHAPE_RECTANGLE:
                drawFilledRectangle(shapes[i].cx, shapes[i].cy, shapes[i].data.rect.w, shapes[i].data.rect.h);
                break;
            case SHAPE_CIRCLE:
                drawFilledCircle(shapes[i].cx, shapes[i].cy, shapes[i].data.circle.r, shapes[i].data.circle.aspect);
                break;
            case SHAPE_LINE:
                drawLine(shapes[i].cx, shapes[i].cy, shapes[i].data.line.len);
                break;
            case SHAPE_TRIANGLE:
                drawFilledTriangle(shapes[i].cx, shapes[i].cy, shapes[i].data.tri.size);
                break;
        }
    }
}
// Helper function to read a valid integer within a range safely
int getIntegerInput(const char* prompt, int minVal, int maxVal) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        buffer[strcspn(buffer, "\r\n")] = '\0';
        
        char* endptr;
        long parsed = strtol(buffer, &endptr, 10);
        if (endptr == buffer || *endptr != '\0') {
            printf("Invalid input. Please enter a valid integer.\n");
            continue;
        }
        if (parsed < minVal || parsed > maxVal) {
            printf("Input out of range. Please enter a value between %d and %d.\n", minVal, maxVal);
            continue;
        }
        val = (int)parsed;
        break;
    }
    return val;
}
// List all active shapes in the picture
void listShapes() {
    if (shapeCount == 0) {
        printf("No objects currently in the picture.\n");
        return;
    }
    printf("\n=================== CURRENT SHAPES LIST ===================\n");
    int i;
    for (i = 0; i < shapeCount; i++) {
        printf("[%d] ", i + 1);
        switch (shapes[i].type) {
            case SHAPE_RECTANGLE:
                printf("Filled Rectangle: Center (%d, %d), Width %d, Height %d\n",
                       shapes[i].cx, shapes[i].cy, shapes[i].data.rect.w, shapes[i].data.rect.h);
                break;
            case SHAPE_CIRCLE:
                printf("Filled Circle: Center (%d, %d), Radius %d\n",
                       shapes[i].cx, shapes[i].cy, shapes[i].data.circle.r);
                break;
            case SHAPE_LINE:
                printf("Line (centered diagonal): Center (%d, %d), Length %d\n",
                       shapes[i].cx, shapes[i].cy, shapes[i].data.line.len);
                break;
            case SHAPE_TRIANGLE:
                printf("Filled Triangle: Center (%d, %d), Size %d\n",
                       shapes[i].cx, shapes[i].cy, shapes[i].data.tri.size);
                break;
        }
    }
    printf("===========================================================\n");
}
// Add a shape to the editor
void addShapeMenu() {
    if (shapeCount >= MAX_SHAPES) {
        printf("Maximum shape limit reached. Please delete some shapes first.\n");
        return;
    }
    printf("\nSelect shape to add:\n");
    printf("1. Rectangle\n");
    printf("2. Circle\n");
    printf("3. Line\n");
    printf("4. Triangle\n");
    printf("5. Cancel\n");
    int choice = getIntegerInput("Enter choice (1-5): ", 1, 5);
    if (choice == 5) {
        printf("Cancelled.\n");
        return;
    }
    Shape newShape;
    newShape.type = (ShapeType)choice;
    newShape.cx = getIntegerInput("Enter Center X coordinate (0 to 39): ", 0, WIDTH - 1);
    newShape.cy = getIntegerInput("Enter Center Y coordinate (0 to 19): ", 0, HEIGHT - 1);
    switch (newShape.type) {
        case SHAPE_RECTANGLE:
            newShape.data.rect.w = getIntegerInput("Enter width (1 to 40): ", 1, WIDTH);
            newShape.data.rect.h = getIntegerInput("Enter height (1 to 20): ", 1, HEIGHT);
            break;
        case SHAPE_CIRCLE:
            newShape.data.circle.r = getIntegerInput("Enter radius (0 to 20): ", 0, 20);
            newShape.data.circle.aspect = 2.0f;
            break;
        case SHAPE_LINE:
            newShape.data.line.len = getIntegerInput("Enter length (1 to 40): ", 1, WIDTH);
            break;
        case SHAPE_TRIANGLE:
            newShape.data.tri.size = getIntegerInput("Enter size (1 to 20): ", 1, 20);
            break;
    }
    shapes[shapeCount++] = newShape;
    printf("Shape added successfully!\n");
    redrawCanvas();
    displayCanvas();
}
// Delete a shape from the editor
void deleteShapeMenu() {
    if (shapeCount == 0) {
        printf("No shapes to delete.\n");
        return;
    }
    listShapes();
    int idx = getIntegerInput("Enter the shape index to delete (or 0 to cancel): ", 0, shapeCount);
    if (idx == 0) {
        printf("Cancelled.\n");
        return;
    }
    int i;
    for (i = idx - 1; i < shapeCount - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shapeCount--;
    printf("Shape deleted successfully!\n");
    redrawCanvas();
    displayCanvas();
}
// Modify an existing shape's parameters
void modifyShapeMenu() {
    if (shapeCount == 0) {
        printf("No shapes to modify.\n");
        return;
    }
    listShapes();
    int idx = getIntegerInput("Enter the shape index to modify (or 0 to cancel): ", 0, shapeCount);
    if (idx == 0) {
        printf("Cancelled.\n");
        return;
    }
    Shape *shape = &shapes[idx - 1];
    printf("\nModifying Shape [%d]:\n", idx);
    printf("Currently: ");
    switch (shape->type) {
        case SHAPE_RECTANGLE:
            printf("Rectangle at (%d, %d), size %dx%d\n", shape->cx, shape->cy, shape->data.rect.w, shape->data.rect.h);
            break;
        case SHAPE_CIRCLE:
            printf("Circle at (%d, %d), radius %d\n", shape->cx, shape->cy, shape->data.circle.r);
            break;
        case SHAPE_LINE:
            printf("Line at (%d, %d), length %d\n", shape->cx, shape->cy, shape->data.line.len);
            break;
        case SHAPE_TRIANGLE:
            printf("Triangle at (%d, %d), size %d\n", shape->cx, shape->cy, shape->data.tri.size);
            break;
    }
    printf("\nSelect modification option:\n");
    printf("1. Modify center coordinates (X, Y)\n");
    printf("2. Modify dimensions/size\n");
    printf("3. Change shape type & parameters\n");
    printf("4. Cancel\n");
    int choice = getIntegerInput("Enter choice (1-4): ", 1, 4);
    if (choice == 4) {
        printf("Cancelled.\n");
        return;
    }
    if (choice == 1 || choice == 3) {
        shape->cx = getIntegerInput("Enter new Center X coordinate (0 to 39): ", 0, WIDTH - 1);
        shape->cy = getIntegerInput("Enter new Center Y coordinate (0 to 19): ", 0, HEIGHT - 1);
    }
    if (choice == 2) {
        switch (shape->type) {
            case SHAPE_RECTANGLE:
                shape->data.rect.w = getIntegerInput("Enter new width (1 to 40): ", 1, WIDTH);
                shape->data.rect.h = getIntegerInput("Enter new height (1 to 20): ", 1, HEIGHT);
                break;
            case SHAPE_CIRCLE:
                shape->data.circle.r = getIntegerInput("Enter new radius (0 to 20): ", 0, 20);
                break;
            case SHAPE_LINE:
                shape->data.line.len = getIntegerInput("Enter new length (1 to 40): ", 1, WIDTH);
                break;
            case SHAPE_TRIANGLE:
                shape->data.tri.size = getIntegerInput("Enter new size (1 to 20): ", 1, 20);
                break;
        }
    } else if (choice == 3) {
        printf("\nSelect new shape type:\n");
        printf("1. Rectangle\n");
        printf("2. Circle\n");
        printf("3. Line\n");
        printf("4. Triangle\n");
        int typeChoice = getIntegerInput("Enter choice (1-4): ", 1, 4);
        shape->type = (ShapeType)typeChoice;
        switch (shape->type) {
            case SHAPE_RECTANGLE:
                shape->data.rect.w = getIntegerInput("Enter width (1 to 40): ", 1, WIDTH);
                shape->data.rect.h = getIntegerInput("Enter height (1 to 20): ", 1, HEIGHT);
                break;
            case SHAPE_CIRCLE:
                shape->data.circle.r = getIntegerInput("Enter radius (0 to 20): ", 0, 20);
                shape->data.circle.aspect = 2.0f;
                break;
            case SHAPE_LINE:
                shape->data.line.len = getIntegerInput("Enter length (1 to 40): ", 1, WIDTH);
                break;
            case SHAPE_TRIANGLE:
                shape->data.tri.size = getIntegerInput("Enter size (1 to 20): ", 1, 20);
                break;
        }
    }
    printf("Shape modified successfully!\n");
    redrawCanvas();
    displayCanvas();
}
int main() {
    initCanvas();
    printf("===========================================\n");
    printf("      WELCOME TO 2D ASCII GRAPHICS EDITOR  \n");
    printf("===========================================\n");
    printf("Canvas size: %dx%d cells\n", WIDTH, HEIGHT);
    printf("Background is '_', shapes are drawn using '*'.\n");
    while (1) {
        printf("\n------------- MAIN MENU -------------\n");
        printf("1. Display Canvas / Picture\n");
        printf("2. Add Shape Object\n");
        printf("3. Delete Shape Object\n");
        printf("4. Modify Shape Object\n");
        printf("5. List All Shape Objects\n");
        printf("6. Exit\n");
        printf("-------------------------------------\n");
        int choice = getIntegerInput("Enter choice (1-6): ", 1, 6);
        if (choice == 6) {
            printf("\nThank you for using the 2D Graphics Editor. Goodbye!\n");
            break;
        }
        switch (choice) {
            case 1:
                redrawCanvas();
                displayCanvas();
                break;
            case 2:
                addShapeMenu();
                break;
            case 3:
                deleteShapeMenu();
                break;
            case 4:
                modifyShapeMenu();
                break;
            case 5:
                listShapes();
                break;
        }
    }
    return 0;
}
