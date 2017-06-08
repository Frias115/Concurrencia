
//Para c
//Para compilar: mpicc ejemplo

//Para c++
//para compilar: mpiCC ejemplo.cpp -o ejemplo

//Para ejecutar: mpirun -np 4 (ruta del ejecutable)

//Para ejecutar en cluster: mpirun -np 4 (ruta del ejecutable) -hostfile hostfile.txt

#include "matriz.h"
#include "debug_time.h"
#include "mpi.h"
#include "slave.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//Añadir parametros necesarios para leer (m origen y nombre de matriz resultado)
void maestro(int rank, int nproc, Matriz *m1, Matriz *m2, int numeroThreads){

	int source = rank;

	DEBUG_TIME_INIT;
	DEBUG_TIME_START;

	{
		DEBUG_TIME_INIT;
		DEBUG_TIME_START;
		//dividir matriz // crear paquetes de trabajo
			//por cada esclavo
			//enviar submatriz
			//por cada esclavo
		//recibir resultados

		//escribir resultado
		Matriz *resultado = m1->multiplicarMatrices(m2, numeroThreads, nproc);
		DEBUG_TIME_END;
		DEBUG_PRINT_FINALTIME("Tiempo multiplicarMatrices(): \n\t");
		resultado->guardarMatriz("resultado");
	}

	DEBUG_TIME_END;

	DEBUG_PRINT_FINALTIME("Tiempo Total: \n\t");
}

void esclavo(int rank, int nproc){

	int dest = rank;
	MPI_Status status;

	paqueteTrabajo *paquete = (paqueteTrabajo *)malloc(sizeof(paqueteTrabajo));
	int numeroThreads = 0;

	//recibe paquete de trabajo y numero threads
	MPI_Recv(paquete, sizeof(paqueteTrabajo), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&numeroThreads, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);

	//multiplicar (estatica/dinamica/gpu)
	paquete = multiplicarMatrices(paquete, numeroThreads);

	//enviar paquete de trabajo
	MPI_Send(paquete, sizeof(paqueteTrabajo), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

	free(paquete);
}

int main(int arg, char **argv){

	//USO PROGRAMA: multiplicadorMatrices <MATRIZ1> <MATRIZ2> <NUM_THREADS>

	int nproc, rank;
	MPI_Init(&arg, &argv);
	//MPI_COMM_WORLD: Comunicador
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Status status;

	bool leerTraspuesta = true;

	//Hay que enviar la matriz por cachos

	//leer matrices
	Matriz *m1 = new Matriz(argv[1], !leerTraspuesta);
	Matriz *m2 = new Matriz(argv[2], leerTraspuesta);
	int numeroThreads = atoi(argv[3]);

	if(rank == 0){
		maestro(rank, nproc, m1, m2, numeroThreads);
	}
	else //if(rank == 1)
	{
		esclavo(rank, nproc);
	}
	MPI_Finalize();
	return 0;
}


/*

si se quejade specified application es que esta mal la ruta


mpirun -np 4 -hostfile hostfile.txt /(ruta del programa)

hostfile.txt:

10.4.0.29
10.4.0.44

*/

