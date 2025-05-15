#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <cmath>
#include <stdexcept>
#include <GL/freeglut.h>
#include "CVector.h" 
#include "global.h"

#define PI 3.1415926

inline float random_float(float min, float max) {//Ëæ»ú¸¡µãÊý
    static std::mt19937 engine(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(engine);
}



#pragma once
