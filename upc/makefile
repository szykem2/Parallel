CC=upcc -network=mpi
SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
OBJS=$(SRCS:.c=.o)
PROJ=nbody
CFLAGS= -c -I/opt/nfs/mpe2-2.4.9b/include -I/opt/nfs/mpich-3.2/include
LDFLAGS= -L/opt/nfs/mpe2-2.4.9b/lib -L/opt/nfs/mpich-3.2/lib 
LIBS= -lm -lmpe -lX11 -lmpi --uses-mpi

all:$(PROJ)

$(PROJ): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROJ) $(LIBS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -f *.o *.slog2 *.clog2 *~ $(PROJ)

.PHONY:run
run:
	UPC_NODEFILE=nodes upcrun -c 1 -N 6 -n 6  ./$(PROJ) -df dataFile


.PHONY:planets
planets:
	UPC_NODEFILE=nodes upcrun -c 1 -N 1 -n 1 ./$(PROJ) -df planets.data -t 100
