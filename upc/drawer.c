#include "drawer.h"
#include <upc_relaxed.h>

#define DESTINY 800
#define CENTER 400
#define BODY_COLOR MPE_RED
#define BG_COLOR MPE_WHITE

extern shared Data* shared particlesData;
extern shared size_t numOfParticles;

//objekt służący do rysowania
MPE_XGraph graph;

/*
Funkcja otwierająca okno
*/
void openDrawer(MPI_Comm comm) {
    MPE_Open_graphics(&graph, comm, NULL, 0, 0, DESTINY, DESTINY, 1);
}

/*
Funkcja orysująca
*/
void draw() {
    double x, y, s;
    for(size_t i = 0; i < numOfParticles; ++i) { //dla wszystkich cząstek w buforze
        x = particlesData[i].position.x * CENTER + CENTER;
        y = particlesData[i].position.y * CENTER + CENTER;
        s = 2;
        if(particlesData[i].mass > 10)
            s = 5;

        MPE_Fill_circle(graph, x, y, s, BODY_COLOR); //rysowanie
    }
    MPE_Update(graph);
}

/*
Funkcja czyszcząca obszar rysowania
*/
void clean() {
    MPE_Fill_rectangle(graph, 0, 0, DESTINY, DESTINY, BG_COLOR);
}

/*
Funkcja zamykająca okno
*/
void closeDrawer() {
	MPE_Close_graphics(&graph);
}
