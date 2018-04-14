#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "data.h"

extern Data* particlesData;


int getLines(const char* fname)
{                                
    FILE *fp = fopen(fname,"r");
    int ch=0;
    int lines=0;

    if (fp == NULL);
        return 0;

    lines++;
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
    particlesData = (Data*)malloc(sizeof(Data) * lines);

    char * line = NULL;
    size_t len = 0;
    size_t read;
    int i = 0;

    while ((read = getline(&line, &len, f)) != -1) {
        if(i==4)
            continue;
        printf("Retrieved line of length %zu\n", read);
        printf("%s", line);
        double px, py, vx, vy, m;
        sscanf(line, "%lf,%lf;%lf,%lf;%lf\n", &px, &py, &vx, &vy, &m);
        particlesData[i].position.x = px;
        particlesData[i].position.y = py;
        particlesData[i].velocity.x = vx;
        particlesData[i].velocity.y = vy;
        particlesData[i].mass = m;
        printf("%lf,%lf;%lf,%lf;%lf\n", particlesData[i].position.x, particlesData[i].position.y, particlesData[i].velocity.x, particlesData[i].velocity.y, particlesData[i].mass);
        i++;
    }

    printf("test");
    fclose(f);
}