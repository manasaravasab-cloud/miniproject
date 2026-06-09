#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 40
#define HEIGHT 20

char canvas[HEIGHT][WIDTH];

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

int main() {
    /* Rectangle */
    initCanvas();
    drawFilledRectangle(WIDTH/2, HEIGHT/2, 12, 6);
    printf("Filled Rectangle (centered):\n");
    displayCanvas();

    /* Circle (Now renders with beautifully rounded top/bottom boundaries) */
    initCanvas();
    drawFilledCircle(WIDTH/2, HEIGHT/2, 8, 2.0f);
    printf("Filled Circle (centered):\n");
    displayCanvas();

    /* Line */
    initCanvas();
    drawLine(WIDTH/2, HEIGHT/2, 20);
    printf("Line (centered diagonal):\n");
    displayCanvas();

    /* Triangle */
    initCanvas();
    drawFilledTriangle(WIDTH/2, HEIGHT/2, 12);
    printf("Filled Triangle (centered upright):\n");
    displayCanvas();

    return 0;
}