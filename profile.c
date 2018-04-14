#define MPI_PROFILE
#include "mpi.h"
#include "mpe.h"
#define START_BCAST 0
#define END_BCAST 1
#define START_ALLRED 2
#define END_ALLRED 3
#define START_RECV 4
#define END_RECV 5
#define START_SEND 6
#define END_SEND 7
#ifdef MPI_PROFILE
int MPI_Init(int *argc, char ***argv)
{
    int ret, myid;
	ret = PMPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPE_Init_log();

	if(myid == 0) {
		MPE_Describe_state(START_BCAST, END_BCAST, "broadcast", "red");
		MPE_Describe_state(START_ALLRED, END_ALLRED, "allreduce", "blue");
		MPE_Describe_state(START_RECV, END_RECV, "receive", "yellow");
		MPE_Describe_state(START_SEND, END_SEND, "send", "green");
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

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{ 
    int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   	MPE_Log_event(START_SEND, myid, "start send");
    ret = PMPI_Send(buf, count, datatype, dest, tag, comm );
   	MPE_Log_event(END_SEND, myid, "end send");
    return ret; 
} 

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)                           
{ 
    int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   	MPE_Log_event(START_RECV, myid, "start recv");
    ret = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
   	MPE_Log_event(END_RECV, myid, "end recv");
    return ret; 
} 


int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{ 
    int ret, myid;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   	MPE_Log_event(START_ALLRED, myid, "start allreduce");
    ret = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
   	MPE_Log_event(END_ALLRED, myid, "end allreduce");
    return ret; 
} 
#endif