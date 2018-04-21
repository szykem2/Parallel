CC=mpicc
SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
OBJS=$(SRCS:.c=.o)
PROJ=nbody
CFLAGS= -std=c11 -c -g -Wall -fopenmp -I/opt/nfs/mpe2-2.4.9b/include -L/opt/nfs/mpe2-2.4.9b/lib 
LIBS= -lm -pthread -lX11 -lm -lmpe -lmpi

all:$(PROJ)

profile: CFLAGS += -DMPI_PROFILE
profile: $(PROJ)

$(PROJ): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROJ) $(LIBS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -f *.o *~ $(PROJ)

.PHONY:run
run:
	#sourcempi
	#/opt/nfs/config/station_name_list.sh 101 116 > nodes
	mpiexec -f nodes -n 1 ./$(PROJ) -df dataFile
