valgrind --leak-check=yes mpiexec -f nodes -n $(( 2 * $(cat nodes | wc -l) + 1 )) \
	-env DISPLAY $( hostname | sed -e 's/206-0/1/' ):1 ./nbody -df dataFile