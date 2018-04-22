#define _GNU_SOURCE
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "tree.h"
#include "mpi.h"
#include "mpe.h"
#include "mpe_graphics.h"
#include <omp.h>
#include <stddef.h>
#include "drawer.h"


#define MASTER 0
#define dt 0.001;

Data* particlesData;
size_t numOfParticles;
MPI_Datatype Particle;

void printHelp() {
    const char* help = "\nN Body simulation\n\n"
    "Usage: \n"
    "   mpiexec -f nodes ./nbody -df <dataFile>\n"
    "   use `make run` to launch correctly with data file named dataFile\n\n";
    printf("%s", help);
}

void createType() {
    int blocklengths[3] = {2, 2, 1};
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Aint offsets[3];

    offsets[0] = offsetof(Data, position);
    offsets[1] = offsetof(Data, velocity);
    offsets[2] = offsetof(Data, mass);

    MPI_Type_create_struct(3, blocklengths, offsets, types, &Particle);
    MPI_Type_commit(&Particle);
}

void calculate(unsigned int position) {
    Vector2D force;
    force.x = 0;
    force.y = 0;
    for(int i = 0; i < numOfParticles; i++) {
        if(i == position) 
            continue;
        
        Vector2D f;
        f.x = 0;
        f.y = 0;
        double r = pow(particlesData[i].position.x - particlesData[position].position.x, 2) + pow(particlesData[i].position.y - particlesData[position].position.y, 2);//r^2
        double value = G * particlesData[i].mass * particlesData[position].mass / r; //F=G*M*m/r^2
        f.x = value * (particlesData[i].position.x - particlesData[position].position.x) / sqrt(r); //Fx = F*cos(phi)=F*x/r
        f.y = value * (particlesData[i].position.y - particlesData[position].position.y) / sqrt(r); //Fy = F*sin(phi)=F*y/r
        force.x += f.x;
        force.y += f.y;
    }
    //printf("Force iterative: %lf, %lf\n", force.x, force.y);
    particlesData[position].position.x += particlesData[position].velocity.x * dt; //ds=v*dt
    particlesData[position].position.y += particlesData[position].velocity.y * dt;
    particlesData[position].velocity.x += force.x / particlesData[position].mass * dt ; //a=F/m dv=a*dt
    particlesData[position].velocity.y += force.y/ particlesData[position].mass * dt ;
}

int main(int argc, char**argv) {
    char* fname = (char*)malloc(sizeof(char) * 20); 
    if(argc == 3 && strcmp(argv[1], "-df") == 0) 
        sscanf(argv[2], "%s", fname);
    else
        printHelp();

    int myid, nodeCount;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &nodeCount);
    if(myid == MASTER) {
        printf("N-body problem simulation.\n");
    }
    openDrawer(MPI_COMM_WORLD);

    createType();
    if(myid == MASTER) {
        parse(fname);
    }
    MPI_Bcast(&numOfParticles,1,MPI_INT,0,MPI_COMM_WORLD);
    if(myid != MASTER) {
        particlesData = (Data*)malloc(sizeof(Data)*numOfParticles);
    }
    int* sizes = (int*)malloc(sizeof(int) * nodeCount);
    int* disp = (int*)malloc(sizeof(int) * nodeCount);
    int size = numOfParticles/nodeCount + (numOfParticles % nodeCount <= myid ? 0 :1);
    Data* sendBuffer = (Data*)malloc(sizeof(Data) * size);

    MPI_Gather(&size,1,MPI_INT,sizes,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(sizes,nodeCount,MPI_INT,0,MPI_COMM_WORLD);
    disp[0] = 0;
    for(int i = 1; i < nodeCount; i++)
        disp[i] = disp[i-1] + sizes[i-1];

    MPI_Bcast(particlesData,numOfParticles,Particle,0,MPI_COMM_WORLD);
    Tree* tree = NULL;
    createTree(&tree);
    const double tm = 100;
    double t = 99;
    while(t < tm) {
        t += dt;
        /*buildTree(tree, particlesData, numOfParticles);
        computeCOM(tree);*/
        createTree(&tree);
        buildTree(tree, particlesData, numOfParticles);
        computeCOM(tree);
        int l = 0;
        for(int i = disp[myid]; i < disp[myid] + sizes[myid]; i++) {
            calculate(i);
            /*memcpy(&sendBuffer[l],&particlesData[i],sizeof(Data));
            Vector2D force = calculateForce(tree, &particlesData[i]);

            sendBuffer[l].position.x += sendBuffer[l].velocity.x * dt; //ds=v*dt
            sendBuffer[l].position.y += sendBuffer[l].velocity.y * dt;
            sendBuffer[l].velocity.x += force.x / sendBuffer[l].mass * dt; //a=F/m  dv=a*dt
            sendBuffer[l].velocity.y += force.y / sendBuffer[l].mass * dt;
            l++;*/
        }
        if(myid == MASTER)
            clean();
        drawBuffered(sendBuffer, size);
        draw();
        MPI_Allgatherv(sendBuffer,size,Particle,particlesData,sizes,disp,Particle,MPI_COMM_WORLD);
        deleteTree(tree);
    }
    free(sizes);
    free(disp);
    free(sendBuffer);
    free (fname);
    free(particlesData);
    closeDrawer();
    MPI_Type_free(&Particle);
    MPI_Finalize();
    return 0;
}
