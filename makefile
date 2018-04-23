CC=mpicc
SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
OBJS=$(SRCS:.c=.o)
PROJ=nbody
CFLAGS= -std=c99 -c -g -Wall -fopenmp -I/opt/nfs/mpe2-2.4.9b/include -pthread
LDFLAGS= -L/opt/nfs/mpe2-2.4.9b/lib
LIBS= -lm -pthread -lmpe -lX11 -lm -fopenmp

all:$(PROJ)

prof: CFLAGS += -DMPI_PROFILE
prof: $(PROJ)

$(PROJ): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROJ) $(LIBS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -f *.o *~ $(PROJ)

.PHONY:run
run:
	. /opt/nfs/config/source_mpich32.sh
	/opt/nfs/config/station_name_list.sh 101 116 > nodes
	mpiexec -f nodes -n 4 ./$(PROJ) -df dataFile
	#display :
	# mpiexec -f nodes -n $(( 2 * $(cat nodes | wc -l) + 1 )) -env DISPLAY $( hostname | sed -e 's/206-0/1/' ):1 ./nbody -df dataFile
	# 

.PHONY:profile
profile:
	export PATH=/opt/nfs/mpe2-2.4.9b/bin/:$PATH
	clog2TOslog2 mpe-profile.clog2
	jumpshot mpe-profile.slog2
