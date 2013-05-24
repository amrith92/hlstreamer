CC		= mpic++
C_OPTS	= -Wall -I/usr/lib/openmpi/include -L/usr/lib/openmpi/lib -lboost_mpi -lboost_serialization *.cpp -o main
HEADERS	= *.hpp

CPU		= 8

all:
	$(CC) $(C_OPTS)

run:
	mpirun -n $(CPU) ./main

