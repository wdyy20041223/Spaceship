

typedef struct star {
    CVector place;
    CVector color;
    CVector speed;
    float size;
    float colorDelta;
}; 

void initstar();
void drawStars();
void checkStar();

#pragma once
