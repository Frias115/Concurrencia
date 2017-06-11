
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
		cout << "Ejecuto multiplicarMatrices" << endl;
		Matriz *resultado = m1->multiplicarMatrices(m2, numeroThreads, nproc);
		cout << "Acaba multiplicarMatrices" << endl;
		DEBUG_TIME_END;
		cout << "***************************************************" << endl;
		DEBUG_PRINT_FINALTIME("Tiempo multiplicarMatrices(): \n\t");
		cout << "***************************************************" << endl;
		resultado->guardarMatriz("resultado");
	}

	DEBUG_TIME_END;

	cout << "***************************************************" << endl;
	DEBUG_PRINT_FINALTIME("Tiempo Total: \n\t");
	cout << "***************************************************" << endl;
}

void esclavo(int rank, int nproc){

	int dest = rank;
	MPI_Status status;

	paqueteTrabajo *paquete = (paqueteTrabajo *)malloc(sizeof(paqueteTrabajo));
	int numeroThreads = 0;
	int filaInicial = 0;
	int numeroRealFilasACalcular = 0;
	int numeroRealColumnasACalcular = 0;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		DEBUG_TIME_INIT;
		DEBUG_TIME_START;

		//Recibe la informacion 
		cout << rank << " -> Espera a los datos" << endl;
		MPI_Recv(&numeroThreads, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&filaInicial, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&numeroRealFilasACalcular, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&numeroRealColumnasACalcular, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);

		DEBUG_TIME_END;
		cout << "---------------------------------------------------" << endl;
		DEBUG_PRINT_FINALTIME("Tiempo recibir cabecera esclavo-maestro: \n\t");
		cout << "---------------------------------------------------" << endl;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Matriz *matriz1 = new Matriz(numeroRealFilasACalcular, numeroRealColumnasACalcular);
	Matriz *matriz2 = new Matriz(numeroRealColumnasACalcular, numeroRealColumnasACalcular); 
	Matriz *resultado = new Matriz(numeroRealColumnasACalcular, numeroRealColumnasACalcular);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		DEBUG_TIME_INIT;
		DEBUG_TIME_START;

		for (int i = 0; i < numeroRealFilasACalcular; ++i){
			MPI_Recv(matriz1->datos[i], numeroRealColumnasACalcular, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		}

		for (int i = 0; i < numeroRealColumnasACalcular; ++i){
			MPI_Recv(matriz2->datos[i], numeroRealColumnasACalcular, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		}

		DEBUG_TIME_END;
		cout << "---------------------------------------------------" << endl;
		DEBUG_PRINT_FINALTIME("Tiempo recibir matrices esclavo-maestro: \n\t");
		cout << "---------------------------------------------------" << endl;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cout << rank << " -> Recibe los datos" << endl;

	//Mete en un paquete de trabajo la informacion
	paquete->filaInicial = filaInicial;
	paquete->numeroRealFilasACalcular = numeroRealFilasACalcular;
	paquete->numeroRealColumnasACalcular = numeroRealColumnasACalcular;
	paquete->datosUno = matriz1->datos;
	paquete->datosDos = matriz2->datos;
	paquete->resultado = resultado->datos;

	paqueteTrabajo *paqueteAux;
	//Multiplicar (estatica/dinamica/gpu)
	cout << rank << " -> Multiplica los datos" << endl;
	paqueteAux = multiplicarMatrices(paquete, numeroThreads);
	cout << rank << " -> Recibe el resultado de multiplicar" << endl;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		DEBUG_TIME_INIT;
		DEBUG_TIME_START;

	//Enviar informacion de vuelta al maestro
	cout << "paqueteAux->filaInicial " << paqueteAux->filaInicial << endl;
	cout << "paqueteAux->numeroRealFilasACalcular" << paqueteAux->numeroRealFilasACalcular << endl;
	cout << rank << " -> Devuelve datos al maestro" << endl;
	MPI_Send(&paqueteAux->filaInicial, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
	MPI_Send(&paqueteAux->numeroRealFilasACalcular, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
	MPI_Send(&paqueteAux->numeroRealColumnasACalcular, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD);


	for (int i = 0; i < numeroRealColumnasACalcular; i++){
		MPI_Send(paqueteAux->resultado[i], numeroRealColumnasACalcular, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

		DEBUG_TIME_END;
		cout << "---------------------------------------------------" << endl;
		DEBUG_PRINT_FINALTIME("Tiempo enviar cabecera y matriz resultado esclavo-maestro: \n\t");
		cout << "---------------------------------------------------" << endl;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	free(paqueteAux);
	cout << rank << " -> Acaba" << endl;

}

int main(int arg, char **argv){

	//Para c
	//Para compilar: mpicc ejemplo

	//Para c++
	//para compilar: mpiCC ejemplo.cpp -o ejemplo

	//Para ejecutar: mpirun -np 4 (ruta del ejecutable) (parametros)

	//Para ejecutar en cluster: mpirun -np 4 (ruta del ejecutable) -hostfile hostfile.txt

	//Orden parametros: <MATRIZ1> <MATRIZ2> <NUM_THREADS>

	int nproc, rank;
	MPI_Init(&arg, &argv);
	//MPI_COMM_WORLD: Comunicador
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Status status;
	cout << "Leo las matrices" << endl;
	bool leerTraspuesta = true;

	//Hay que enviar la matriz por cachos

	//leer matrices
	Matriz *m1 = new Matriz(argv[1], !leerTraspuesta);
	Matriz *m2 = new Matriz(argv[2], leerTraspuesta);
	int numeroThreads = atoi(argv[3]);

	cout << "Termino de leer las matrices" << endl;

	if(rank == 0){
		cout << rank << " -> Entra al maestro" << endl;
		maestro(rank, nproc, m1, m2, numeroThreads);
	}
	else //if(rank == 1)
	{
		cout << rank << " -> Entra al esclavo" << endl;
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

