#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int arg, char **argv){
	int nproc, rank;
	MPI_Init(&argc, &argv);
	//MPI_COMM_WORLD: Comunicador
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Cuerpo del programa
		printf("Hola mundo desde el nodo: %d\n", rank);

	MPI_Finalize();
	return 0;
}