#include "shapes.h"

#include <math.h>


void COL_RenderCircle(SDL_Renderer *renderer, float radius, float x, float y) {
    float start = x - radius;
    float end = x + radius;
    float y_low, y_high, y_length;
    float dx;

    int i;
    for (i = start; i <= end; i++) {
        dx = i - x;
        y_length = (float)sqrt(radius*radius - dx*dx);
        y_high = y + y_length;
        y_low = y - y_length;
        SDL_RenderLine(renderer, i, y_low, i, y_high);
    }
}