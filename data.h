#ifndef __DATA_H__
#define __DATA_H__
#include <stdio.h>
#include <stdlib.h>

typedef struct Vector2D {
    double x;
    double y;
} Vector2D;

typedef struct Data {
    Vector2D position;
    Vector2D velocity;
    double mass;
} Data;

void parse(const char* fname);
#endif