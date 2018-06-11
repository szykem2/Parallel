#include "mpi.h"
#include "mpe.h"
#define START_BCAST 0
#define END_BCAST 1
#define START_GATHER 2
#define END_GATHER 3
#define START_ALLGATHV 4
#define END_ALLGATHV 5
#define START_BARRIER 6
#define END_BARRIER 7
/*
Profilowanie uaktywnia się tylko gdy przy kompilacji użyta zostanie flaga -DMPI_PROFILE
*/

#ifdef MPI_PROFILE
int MPI_Init(int *argc, char ***argv)
{
	int ret, myid;
	ret = PMPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPE_Init_log(); //inicjalizacja logowania

	if(myid == 0) {
        //opisy stanów
		MPE_Describe_state(START_BCAST, END_BCAST, "broadcast", "red");
		MPE_Describe_state(START_GATHER, END_GATHER, "gather", "blue");
		MPE_Describe_state(START_ALLGATHV, END_ALLGATHV, "allgatherv", "green");
        MPE_Describe_state(START_BARRIER, END_BARRIER, "barrier", "yellow");
	}
	
	MPE_Start_log(); //rozpoczęcie logowania
	return ret;
}

int MPI_Finalize(void)
{
	int ret;
   	MPE_Finish_log("mpe-profile"); //zakończenie logowania i zapis do pliku
	ret = PMPI_Finalize();
	return ret;
}

int MPI_Bcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm) 
{ 
	int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    //rejestacja wydażenia broadcast
	MPE_Log_event(START_BCAST, myid, "start broadcast");
	ret = PMPI_Bcast( buf, count, datatype, root, comm ); 
   	MPE_Log_event(END_BCAST, myid, "end broadcast");
    return ret; 
} 

int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
	int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    ///rejestracja wydażenia gather
	MPE_Log_event(START_GATHER, myid, "start gather");
	ret = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
   	MPE_Log_event(END_GATHER, myid, "end gather");
    return ret; 
}

int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
	int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    //rejestracja wydażenia allgatherv
	MPE_Log_event(START_ALLGATHV, myid, "start allgatherv");
	ret = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
   	MPE_Log_event(END_ALLGATHV, myid, "end allgatherv");
    return ret; 
}

int MPI_Barrier(MPI_Comm comm) {
    int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    //rejestracja wydażenia barrier
	MPE_Log_event(START_BARRIER, myid, "start barrier");
	ret = PMPI_Barrier(comm);
   	MPE_Log_event(END_BARRIER, myid, "end barrier");
    return ret; 
}
#endif
