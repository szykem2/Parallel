#define _GNU_SOURCE
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tree.h"
#include "mpi.h"
#include "mpe.h"
#include "mpe_graphics.h"

#define MASTER 0
#define dt 0.001;
#define MAX_IN_QUAD 10

Data* particlesData;
size_t numOfParticles;

void printHelp() {
    const char* help = "\nN Body simulation\n\n"
    "Usage: \n"
    "   mpiexec -f nodes ./nbody -df <dataFile>\n"
    "   use `make run` to launch correctly with data file named dataFile\n\n";
    printf("%s", help);
}

void calculate(unsigned int position) {
    Vector2D force;
    for(int i = 0; i < numOfParticles; i++) {
        if(i == position) 
            continue;
        
        Vector2D f;
        double r = pow(particlesData[i].position.x - particlesData[position].position.x, 2) + pow(particlesData[i].position.y - particlesData[position].position.y, 2);//r^2
        double value = G * particlesData[i].mass * particlesData[position].mass / r / 100; //F=G*M*m/r^2
        f.x = value * (particlesData[i].position.x - particlesData[position].position.x) / sqrt(r); //Fx = F*cos(phi)=F*x/r
        f.y = value * (particlesData[i].position.y - particlesData[position].position.y) / sqrt(r); //Fy = F*sin(phi)=F*y/r
        force.x += f.x;
        force.y += f.y;
    }
    printf("Force iterative: %lf, %lf\n", force.x, force.y);
    /*particlesData[position].position.x += particlesData[position].velocity.x * dt; //ds=v*dt
    particlesData[position].position.y += particlesData[position].velocity.y * dt;
    particlesData[position].velocity.x += force.x / particlesData[position].mass * dt ; //a=F/m dv=a*dt
    particlesData[position].velocity.y += force.y/ particlesData[position].mass * dt ;*/
}

int main(int argc, char**argv) {
    char* fname = (char*)malloc(sizeof(char) * 20); 
    if(argc == 3 && strcmp(argv[1], "-df") == 0) 
        sscanf( argv[2], "%s", fname );
    else
        printHelp();

    int myid;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    if(myid == MASTER) {
        printf("N-body problem simulation.\n");
    }

    parse(fname);

    Tree* tree = NULL;
    createTree(&tree);
    buildTree(tree, particlesData, 3);
    computeCOM(tree);
    for(int i = 0; i < numOfParticles; i++) {
        Vector2D forces = calculateForce(tree, &particlesData[i]);
        printf("Forces tree: %lf, %lf\n", forces.x, forces.y);
    }

    for(int i = 0; i < numOfParticles; i++) {
        calculate(i);
    }

    //TODO: divide the data somehow, send it and visualize

    
    free (fname);
    free(particlesData);
    deleteTree(tree);
    MPI_Finalize();
    return 0;
}