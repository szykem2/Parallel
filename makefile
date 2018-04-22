CC=mpicc
SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
OBJS=$(SRCS:.c=.o)
PROJ=nbody
CFLAGS= -std=c11 -c -g -Wall -fopenmp -O3 -I/opt/nfs/mpe2-2.4.9b/include -L/opt/nfs/mpe2-2.4.9b/lib 
LIBS= -lm -pthread -lmpe -lX11 -lm -lmpi -fopenmp

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
	mpiexec -f nodes -n 7 ./$(PROJ) -df dataFile
	#display :
	# mpiexec -f nodes -n $(( 2 * $(cat nodes | wc -l) + 1 )) -env DISPLAY $( hostname | sed -e 's/206-0/1/' ):1 ./nbody -df dataFile
	# 
