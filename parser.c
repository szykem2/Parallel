#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "data.h"

extern Data* particlesData;
extern size_t numOfParticles;

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
    int lines = getLines(fname);
    FILE* f = fopen(fname, "r");
    if(f == NULL) {
        perror(fname);
        exit(EXIT_FAILURE);
    }

    numOfParticles = lines+1;
    printf("Number of particles: %zu\n", numOfParticles);
    particlesData = (Data*)calloc(numOfParticles, sizeof(Data));

    int i = 0;
    char str[1000];
    while (fscanf(f, "%s", str) != EOF) {
        double px, py, vx, vy, m;
        sscanf(str, "%lf,%lf;%lf,%lf;%lf", &px, &py, &vx, &vy, &m);
        particlesData[i].position.x = px;
        particlesData[i].position.y = py;
        particlesData[i].velocity.x = vx;
        particlesData[i].velocity.y = vy;
        particlesData[i].mass = m;
        i++;
    }
    //fclose(f);
}
