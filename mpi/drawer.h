#ifndef __DRAWER_H__
#define __DRAWER_H__

#include "data.h"
#include "mpi.h"
#include "mpe.h"
#include "mpe_graphics.h"


void openDrawer(MPI_Comm comm);
void draw(Data* objects, size_t size);
void clean();
void closeDrawer();

#endif




