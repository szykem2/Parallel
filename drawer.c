#include "drawer.h"

#define DESTINY 800
#define CENTER 400
#define MASS 10
#define BODY_COLOR MPE_RED
#define BG_COLOR MPE_WHITE

MPE_XGraph graph;

void openDrawer(MPI_Comm comm) {
    MPE_Open_graphics(&graph, comm, NULL, 0, 0, DESTINY, DESTINY, 1);
}

void drawBuffered(Data* objects, size_t size) {
    double x, y, s;
    for (size_t i=0; i<size; ++i) {
        x = objects[i].position.x*CENTER + CENTER;
        y = objects[i].position.y*CENTER + CENTER;
        s = objects[i].mass / MASS;
        MPE_Fill_circle(graph, x, y, 1, BODY_COLOR);
    }
}

void clean() {
    MPE_Fill_rectangle(graph, 0, 0, DESTINY, DESTINY, BG_COLOR);
}


void draw() {
    MPE_Update(graph);
}

void closeDrawer() {
	MPE_Close_graphics(&graph);
}