

//Para compilar: mpicc ejemplo
//Para ejecutar: mpirun -np 4 (ruta del ejecutable)


#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define TAG 1

typedef struct matriz{
	int numFilas, numColumnas;
	float ***datos;
}matriz;

typedef struct cabe{
	int numFilas; 
	int numColumnas;
	int finaini, filafin;
	/*Añadir si necesitas fina inicial o fila final*/
}cabe_t;


//Para el esclavo
matriz *recibeMatriz(){
	cabe cabecera;
	MPI_Recv(&cabecera, sizeof(cabe_t), MPI_BYTE, 0, TAG, MPI_COMM_WORLD, &status);
	matriz *m1 = (matriz *)malloc()...;

	m1->numFilas = cabecera.numFilas;
	m1->numColumnas = cabecera.numColumnas;

	//Malloc de los datos
	m1->datos = (float **)malloc(m1->numFilas...)
	for(int i = 0; i < m1->numFilas; i++){
		//Recibir
		m1->datos[i] = (float **)malloc(m1->numColumnas...)
		MPI_Send(m1->datos[i], m1->numColumnas, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD);
	}
}

//Para el maestro
void envia_matriz(matriz *m1, int dest){
	cabe cabecera;
	cabecera.numFilas = m1->numFilas;
	cabecera.numColumnas = m1->numColumnas;
	
	for(int i = 0; i < m1->numFilas; i++){
		//Enviamos cabecera
		MPI_Send(m1->datos[i], m1->numColumnas, MPI_FLOAT, dest, TAG, MPI_COMM_WORLD);
	}
}

//Añadir parametros necesarios para leer (m origen y nombre de matriz resultado)
void maestro(int rank, int nproc, char *m1, char *m2, char *mres){

	//Leer m1
	//Leer m1
	//Dividir matrices (crear paquetes)

	//Por cada esclavo:
		//enviar sub-matriz 

	//Por cada esclavo:
		//Recibir resultados

	//Escribir resultado

}

void esclavo(int rank, int nproc){

	//Rrecibe m1
	//Recibe m2
	//multiplicat (estática o dinámica o gpu)

	//Devolver resultado

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