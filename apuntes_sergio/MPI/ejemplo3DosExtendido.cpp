

//Para compilar: mpicc ejemplo
//Para ejecutar: mpirun -np 4 (ruta del ejecutable)


#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define TAG 1

typedef struct matriz{
	float ***datos;
}matriz;

typedef struct cabe{
	int numFilas; 
	int numColumnas;
	int finaini, filafin;
}cabe_t;

void maestro(int rank, int nproc){

	char *msg = (char  *)malloc(100);
	int source = rank;
	int dest = 1;

	sprintf(msg, "\n\n\t Esto es un mensaje del proceso %d al proceso %d", source, dest);

	//Esto así no funcionaría
	matriz * m1 = crearMatriz("matrix1.bin");
	for (int i = 1; i < nproc; i++){
		MPI_Send(&cabecera, sizeof(cabe_t), MPI_BYTE, i, TAG, MPI_COMM_WORLD);
		printf("\n Mensaje enviado a %d", i);
	}
}

void esclavo(int rank, int nproc){

	char *msg = (char  *)malloc(100);

	int dest = rank;
	cabe_t cabecera;

	MPI_Recv(&cabecera, sizeof(cabe_t), MPI_BYTE, 0, TAG, MPI_COMM_WORLD, &status);
	cabecera.numFilas;

	//Malloc matriz 1, malloc matriz 2 etc...
	printf("\n Mensaje recibido en %d", dest);
	printf("%s\n", msg);
}

int main(int arg, char **argv){
	int nproc, rank;
	MPI_Init(&argc, &argv);
	//MPI_COMM_WORLD: Comunicador
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Status status;

	if(rank == 0){
		maestro(rank, nproc);
	}
	else //if(rank == 1)
	{
		esclavo(rank, nproc);
	}
	MPI_Finalize();
	return 0;
}