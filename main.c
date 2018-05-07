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
#include <stddef.h>
#include "drawer.h"
#include <unistd.h>

#define MASTER 0
#define dt 0.001;

/*Zmienne globalne*/
Data* particlesData;
size_t numOfParticles;
MPI_Datatype Particle;
double* masses;
double tm = 50;

/*
Funkcja wypisuje pomoc w przypadku błędnego podania argumentów
*/
void printHelp() {
    const char* help = "\nN Body simulation\n\n"
    "Usage: \n"
    "   mpiexec -f nodes ./nbody -df <dataFile> [-t <time of simulation>]\n"
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
Funkcja tworząca typ środowiska MPI
*/
void createType() {
    int blocklengths[3] = {2, 2, 1}; //ilości typów
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};//typy występujące w strukturze
    MPI_Aint offsets[3];//odległości od początku struktury do danej zmiennej

    //wypełnienie
    offsets[0] = offsetof(Data, position);
    offsets[1] = offsetof(Data, velocity);
    offsets[2] = offsetof(Data, mass);

    //utworzenie typu
    MPI_Type_create_struct(3, blocklengths, offsets, types, &Particle);
    //zatwierdzenie typu    
    MPI_Type_commit(&Particle);
}

/*
Punkt startowy programu
*/
int main(int argc, char**argv) {
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

    int myid, nodeCount;
    //inicjalizacja
    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nodeCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    
    if(myid == MASTER) {
        printf("N-body problem simulation.\n");
    }
    openDrawer(MPI_COMM_WORLD);//otworzenie ekranu do rysowania

    createType();
    if(myid == MASTER) {
        parse(fname);//parsowanie pliku wejściowego
    }
    MPI_Bcast(&numOfParticles,1,MPI_INT,0,MPI_COMM_WORLD);//rozsyłanie informacji o ilości cząstek
    if(myid != MASTER) {
        particlesData = (Data*)malloc(sizeof(Data)*numOfParticles);//alokacja pamięci
    }
    int* sizes = (int*)malloc(sizeof(int) * nodeCount);//alokacja pamięci
    int* disp = (int*)malloc(sizeof(int) * nodeCount);//alokacja pamięci
    int size = numOfParticles/nodeCount + (numOfParticles % nodeCount <= myid ? 0 :1); //wyliczenie ilości cząstek dla węzła
    Data* sendBuffer = (Data*)malloc(sizeof(Data) * size);//alokacja pamięci

    MPI_Gather(&size,1,MPI_INT,sizes,1,MPI_INT,0,MPI_COMM_WORLD); //zbieranie rozmiarów
    MPI_Bcast(sizes,nodeCount,MPI_INT,0,MPI_COMM_WORLD); //rozsyłanie rozmiarów
    disp[0] = 0;
    for(int i = 1; i < nodeCount; i++)
        disp[i] = disp[i-1] + sizes[i-1];//wypełnianie tablicy przesunięc

    MPI_Bcast(particlesData,numOfParticles,Particle,0,MPI_COMM_WORLD); //rozsyłanie informacji o cząsteczkach
    masses = (double*)malloc(sizeof(double) * numOfParticles);
    for(int i = 0; i < numOfParticles; i++) 
        masses[i] = particlesData[i].mass;//przypisanie mas
    Tree* tree = NULL;
    createTree(&tree);//tworzenie drzewa
    double t = 0;
    while(t < tm) {
        t += dt;
        buildTree(tree, particlesData, numOfParticles); //budowanie drzewa
        computeCOM(tree); //liczenie środka masy
        int l = 0;
        for(int i = disp[myid]; i < disp[myid] + sizes[myid]; i++) {
            memcpy(&sendBuffer[l],&particlesData[i],sizeof(Data));
            Vector2D force = calculateForce(tree, &particlesData[i]); //siła działająca na cząstkę

            sendBuffer[l].position.x += sendBuffer[l].velocity.x * dt; //ds=v*dt
            sendBuffer[l].position.y += sendBuffer[l].velocity.y * dt;
            sendBuffer[l].mass = masses[i];
            sendBuffer[l].velocity.x += force.x / sendBuffer[l].mass * dt; //a=F/m  dv=a*dt
            sendBuffer[l].velocity.y += force.y / sendBuffer[l].mass * dt;
            l++;
        }
	    MPI_Allgatherv(sendBuffer,size,Particle,particlesData,sizes,disp,Particle,MPI_COMM_WORLD);//zbieranie informacji o cząstkach
        clearTree(tree); //czyszczenie drzewa
        if(myid == MASTER)
            clean();

        if(strcmp(fname, "planets.data") == 0) 
            usleep(2000); //jeśli rysujemy planety dodawane jest opóźnienie

        MPI_Barrier(MPI_COMM_WORLD);
        draw(sendBuffer, size);
    }

    if(myid == MASTER)
        saveParticles(); //master zapisuje stan końcowy cząstek

    //zwalnianie pamięci i czyszczenie
    deleteTree(tree);
    free(sizes);
    free(disp);
    free(sendBuffer);
    free (fname);
    free(particlesData);
    free(masses);
    closeDrawer();
    MPI_Type_free(&Particle);
    MPI_Finalize();
    return 0;
}
