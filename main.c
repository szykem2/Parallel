#define _GNU_SOURCE
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include "mpi.h"
#include "mpe.h"
#include "mpe_graphics.h"
*/

Data* particlesData;

void printHelp() {
    const char* help = "\nN Body simulation\n\n"
    "Usage: \n"
    "   mpiexec -f nodes ./nbody -df <dataFile>\n"
    "   use `make run` to launch correctly with data file named dataFile\n\n";
    printf("%s", help);
}

int main(int argc, char**argv) {
    char* fname = (char*)malloc(sizeof(char) * 20); 
    if(argc == 3 && strcmp(argv[1], "-df") == 0) 
        sscanf( argv[2], "%s", fname );
    else
        printHelp();

    parse(fname);
    free (fname);
    free(particlesData);
    return 0;
}