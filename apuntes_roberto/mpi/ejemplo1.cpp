//mpiCC si es con c++

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct matriz
{
	int numfilas;
	int numcolumnas;
	int filaini, filafin;
}matriz;

typedef struct cabe
{
	int numfilas;
	int numcolumnas;
	int filaini,filafin;
}cabe_t;

void enviarMatriz(matriz * m1, int dest){

	cabe cabezera;
	cabezera.numfilas = m1->numfilas;
	cabezera.numcolumnas = m1->numfilas;
	MPI_Send(&cabezera,sizeof(cabe), MPI_BYTE, dest, TAG, MPI_COMM_WORLD);
	for(int i= 0;i<m1->numfilas;i++)
	{
		memset(m1->datos[i],1, sizeof(float)*m1->numcolumnas);// rellenado de datos, esto lo hace por el ejemplo, nosotros lo harias normal
		MPI_Send(m1->datos[i],numcolumnas, MPI_FLOAT, dest, TAG, MPI_COMM_WORLD);
	}
}

matriz* recibeMatriz()
{

	cabe cabezera;
	MPI_Recv(&cabezera,sizeof(cabe_t), MPI_BYTE, dest, TAG, MPI_COMM_WORLD, &status);
	matriz* m1 = (matriz*) malloc(...);

	m1->numfilas=cabezera.numfilas;
	m1->numcolumnas=cabezera.numcolumnas;

	m1->datos =(float**)malloc(m1->numfilas...);

	for(int i= 0;i<m1->numfilas;i++)
	{
		m1->datos[i]=(float*)malloc(m1->numcolumnas...);
		MPI_Recv(m1->datos[i],m1->numcolumnas, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD,&status);
	}
}

void maestro(int rank, int nproc, char* m1, char* m2, char* res){

	char* msg=(char*) malloc(100);
	int source = rank;
	int dest = 1;
	sprintf();

	//Hay que enviar la matriz por cachos

	//leer matriz 1
	//leer matriz 2

	//dividir matriz // crear paquetes de trabajo
	//por cada esclavo
		//enviar submatriz

	//por cada esclavo
		//recibir resultados

	//escribir resultado

	for(int i=1; i<nproc;i++){
		MPI_Send(msg,100,MPI_CHAR,i,TAG,MPI_COMM_WORLD);

		printf();
	}

}

void esclavo(int rank, int nproc){
	MPI_Status status;
	char* msg=(char*) malloc(100);
	int dest = rank;


	//recibe matriz 1
	//recibe matriz 2

	//multiplicar (estatica/dinamica/gpu)

	//devolver resultado

	MPI_Recv(msg,100,MPI_CHAR,0,TAG,MPI_COMM_WORLD,&status)
	printf();
	printf();

}

int main (int argc, char **argv)
{
	int nproc, rank;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

		

		

		if(rank==0)
		{
			maestro(rank,nproc);
		}
		else
		{
			esclavo(rank,nproc);

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