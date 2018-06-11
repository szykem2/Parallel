#include <upc.h>
#include <upc_relaxed.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "data.h"
#include "tree.h"
#include "mpi.h"
#include "mpe.h"
#include "mpe_graphics.h"
#include <stddef.h>
#include "drawer.h"
#include <unistd.h>

#define MASTER 0
#define dt 0.001;

/*Zmienne globalne*/
shared Data* shared particlesData;
shared size_t numOfParticles;
shared double* shared masses;
Data* localParticles;
double tm = 50;

/*
Funkcja wypisuje pomoc w przypadku błędnego podania argumentów
*/
void printHelp() {
    const char* help = "\nN Body simulation\n\n"
    "Usage: \n"
    "   UPC_NODEFILE=nodes upcrun -c 1 -N 4 -n 4  ./nbody -df  <dataFile> [-t <time of simulation>]\n"
    "   use `make run` to launch correctly with data file named dataFile\n\n";
    printf("%s", help);
}

/*
Funkcja zapisuje stan układu do pliku
*/
void saveParticles() {
    FILE* f = fopen("output.data", "w");
    for(int i = 0; i < numOfParticles; i++) {
        fprintf(f, "%.6lf,%.6lf;%.6lf,%.6lf;%.6lf\n",particlesData[i].position.x, particlesData[i].position.y, particlesData[i].velocity.x, particlesData[i].velocity.y, particlesData[i].mass);
    }
    fclose(f);
}

/*
Punkt startowy programu
*/
int main(int argc, char**argv) {
    //MPI_Init(&argc, &argv);
    char* fname = (char*)malloc(sizeof(char) * 30);//alokacja pamięci
    //parsowanie argumentów 
    if(argc == 3 && strcmp(argv[1], "-df") == 0) 
        sscanf(argv[2], "%s", fname);
    else if(argc == 5 && strcmp(argv[1], "-df") == 0 && strcmp(argv[3], "-t") == 0){
        sscanf(argv[2], "%s", fname);
        sscanf(argv[4], "%lf", &tm);
    }
    else
        printHelp();

    
    if(MYTHREAD == MASTER) {
        printf("N-body problem simulation.\n");
    }
    openDrawer(MPI_COMM_WORLD);//otworzenie ekranu do rysowania

    if(MYTHREAD == MASTER) {
        parse(fname);//parsowanie pliku wejściowego
        masses = (shared double* shared)upc_global_alloc(numOfParticles, sizeof(double));
    }
    upc_barrier;
    localParticles = (Data*)malloc(sizeof(Data) * numOfParticles);
    upc_forall(int i = 0; i < numOfParticles; i++; &masses[i]) {
        masses[i] = particlesData[i].mass;//przypisanie mas
    }
    Tree* tree = NULL;
    createTree(&tree);//tworzenie drzewa
    double t = 0;
    while(t < tm) {
        t += dt;
        for(int i = 0; i < numOfParticles; i++) {
            localParticles[i] = particlesData[i];
        }
        buildTree(tree, localParticles, numOfParticles); //budowanie drzewa
        computeCOM(tree); //liczenie środka masy

        upc_barrier;

        upc_forall(int i = 0; i < numOfParticles; i++; &particlesData[i]) {
            //printf("PP: %lf,%lf,%lf,%lf,%lf\n", particlesData[i].position.x, particlesData[i].position.y,particlesData[i].velocity.x, particlesData[i].velocity.y,particlesData[i].mass);
            Data* particle = (Data*)malloc(sizeof(Data));
            *particle = particlesData[i];
            Vector2D force = calculateForce(tree, particle); //siła działająca na cząstkę
            
            particle->position.x += particle->velocity.x * dt; //ds=v*dt
            particle->position.y += particle->velocity.y * dt;
            particle->velocity.x += force.x / particle->mass * dt; //a=F/m  dv=a*dt
            particle->velocity.y += force.y / particle->mass * dt;
            //printf("DP: %lf,%lf,%lf,%lf,%lf\n", particle->position.x, particle->position.y,particle->velocity.x, particle->velocity.y,particle->mass);
            upc_memput(&particlesData[i], particle, sizeof(Data)/sizeof(char));
            //printf("PA: %lf,%lf,%lf,%lf,%lf\n", particlesData[i].position.x, particlesData[i].position.y,particlesData[i].velocity.x, particlesData[i].velocity.y,particlesData[i].mass);
            free(particle);
        }

        clearTree(tree); //czyszczenie drzewa
        upc_barrier;

        if(MYTHREAD == MASTER){
            clean();
            draw();
        }
        //break;
        if(strcmp(fname, "planets.data") == 0) 
            usleep(5000); //jeśli rysujemy planety dodawane jest opóźnienie

    }

    if(MYTHREAD == MASTER)
        saveParticles(); //master zapisuje stan końcowy cząstek

    //zwalnianie pamięci i czyszczenie
    deleteTree(tree);
    free (fname);
    free(localParticles);
    upc_free(particlesData);
    upc_free(masses);
    closeDrawer();
    return 0;
}
