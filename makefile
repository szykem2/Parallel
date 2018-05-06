CC=/opt/nfs/mpich-3.2/bin/mpicc
SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
OBJS=$(SRCS:.c=.o)
PROJ=nbody
CFLAGS= -std=c99 -c -g -Wall -I/opt/nfs/mpe2-2.4.9b/include -pthread
LDFLAGS= -L/opt/nfs/mpe2-2.4.9b/lib
LIBS= -lm -pthread -lmpe -lX11 -lm
DISPLAY = $(shell hostname | sed -e 's/206-0/1/'):1

all:$(PROJ)

prof: CFLAGS += -DMPI_PROFILE
prof: $(PROJ)

$(PROJ): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROJ) $(LIBS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -f *.o *.slog2 *.clog2 *~ $(PROJ)

.PHONY:run
run:
	/opt/nfs/mpich-3.2/bin/mpiexec -f nodes -env DISPLAY $(DISPLAY) ./$(PROJ) -df dataFile


.PHONY:planets
planets:
	/opt/nfs/mpich-3.2/bin/mpiexec -f nodes -env DISPLAY $(DISPLAY)  ./$(PROJ) -df dataFile -t 100

.PHONY:profile
profile:
	/opt/nfs/mpich-3.2/bin/mpiexec -f nodes -n 4 ./$(PROJ) -df dataFile -t 10
	/opt/nfs/mpe2-2.4.9b/bin/clog2TOslog2 mpe-profile.clog2
	/opt/nfs/mpe2-2.4.9b/bin/jumpshot mpe-profile.slog2
