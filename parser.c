#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "data.h"

extern Data* particlesData;
extern size_t numOfParticles;

#ifdef _WIN32
#define getline(a,b,c) -1 //only for windows where program won't be launched
#endif

int getLines(const char* fname)
{                                
    FILE *fp = fopen(fname,"r");
    int ch=0;
    int lines=0;

    if (fp == NULL){
        perror(fname);
        exit(EXIT_FAILURE);
    }

    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            lines++;
        }
    }
    fclose(fp);
    return lines;
}

void parse(const char* fname) {
    FILE* f = fopen(fname, "r");
    if(f == NULL) {
        perror(fname);
        exit(EXIT_FAILURE);
    }

    int lines = getLines(fname);
    numOfParticles = lines;
    printf("Number of particles: %zu\n", numOfParticles);
    particlesData = (Data*)calloc(lines, sizeof(Data));

    char * line = NULL;
    size_t len = 0;
    size_t read;
    int i = 0;

    while ((read = getline(&line, &len, f)) != -1) {
        double px, py, vx, vy, m;
        sscanf(line, "%lf,%lf;%lf,%lf;%lf", &px, &py, &vx, &vy, &m);
        particlesData[i].position.x = px;
        particlesData[i].position.y = py;
        particlesData[i].velocity.x = vx;
        particlesData[i].velocity.y = vy;
        particlesData[i].mass = m;
        i++;
    }
    fclose(f);
}
