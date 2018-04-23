#include "mpi.h"
#include "mpe.h"
#define START_BCAST 0
#define END_BCAST 1
#define START_GATHER 2
#define END_GATHER 3
#define START_ALLGATHV 4
#define END_ALLGATHV 5

#ifdef MPI_PROFILE
int MPI_Init(int *argc, char ***argv)
{
	int ret, myid;
	ret = PMPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPE_Init_log();

	if(myid == 0) {
		MPE_Describe_state(START_BCAST, END_BCAST, "broadcast", "red");
		MPE_Describe_state(START_GATHER, END_GATHER, "gather", "blue");
		MPE_Describe_state(START_ALLGATHV, END_ALLGATHV, "allgatherv", "green");
	}
	
	MPE_Start_log();
	return ret;
}

int MPI_Finalize(void)
{
	int ret;
   	MPE_Finish_log("mpe-profile");
	ret = PMPI_Finalize();
	return ret;
}

int MPI_Bcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm) 
{ 
	int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);

	MPE_Log_event(START_BCAST, myid, "start broadcast");
	ret = PMPI_Bcast( buf, count, datatype, root, comm ); 
   	MPE_Log_event(END_BCAST, myid, "end broadcast");
    return ret; 
} 

int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) {
	int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPE_Log_event(START_GATHER, myid, "start gather");
	ret = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
   	MPE_Log_event(END_GATHER, myid, "end gather");
    return ret; 
}

int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm) {
	int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPE_Log_event(START_ALLGATHV, myid, "start allgatherv");
	ret = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
   	MPE_Log_event(END_ALLGATHV, myid, "end allgatherv");
    return ret; 
}
#endif
