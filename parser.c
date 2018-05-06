#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

/*
Zmienne globalne z pliku main.c
*/
extern Data* particlesData;
extern size_t numOfParticles;

/*
Funkcja sprawdza ile linii znajduje się w pliku, nie uwzględnia linii pustych.
*/
int getLines(const char* fname)
{                                
    FILE *fp = fopen(fname,"r");
    int lines=0;

    if (fp == NULL) {
        perror(fname);
        exit(EXIT_FAILURE);
    }

    char str[1000];
    while (fscanf(fp, "%s", str) != EOF) {//dopóki nie pobrane zostało wszystko z pliku
        if(strcmp(str, "\n") == 0){
            continue;
        }
        lines++;
    }
    fclose(fp);
    return lines;
}

/*
Funckja parsuje plik i przypisuje cząstkom wartości z pliku
*/
void parse(const char* fname) {
    int lines = getLines(fname); //ile cząstek
    FILE* f = fopen(fname, "r");
    if(f == NULL) {
        perror(fname);
        exit(EXIT_FAILURE);
    }

    numOfParticles = lines;
    printf("Number of particles: %zu\n", numOfParticles);
    particlesData = (Data*)calloc(numOfParticles, sizeof(Data)); //alokacja pamięci

    int i = 0;
    char str[1000];
    while (fscanf(f, "%s", str) != EOF) {//dopóki nie pobrane zostało wszystko z pliku
        if(strcmp(str, "\n") == 0){
            continue;
        }
        double px, py, vx, vy, m;
        sscanf(str, "%lf,%lf;%lf,%lf;%lf", &px, &py, &vx, &vy, &m);//pobieranie danych
        //zapis do tablicy
        particlesData[i].position.x = px;
        particlesData[i].position.y = py;
        particlesData[i].velocity.x = vx;
        particlesData[i].velocity.y = vy;
        particlesData[i].mass = m;
        i++;
    }
    fclose(f);
}
