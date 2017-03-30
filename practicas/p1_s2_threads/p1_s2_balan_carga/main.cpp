#include "matriz.h"
#include "debug_time.h"

using namespace std;

int main(int argc, char **argv){
//USO PROGRAMA: multiplicadorMatrices <MATRIZ1> <MATRIZ2> <NUM_THREADS> <NUM_FILAS_POR_PAQUETE>

	DEBUG_TIME_INIT;
	DEBUG_TIME_START;
	bool leerTraspuesta = true;

	Matriz *m1 = new Matriz(argv[1], !leerTraspuesta);
	Matriz *m2 = new Matriz(argv[2], leerTraspuesta);
	int numeroThreads = atoi(argv[3]);
	int numeroFilasPaquete = atoi(argv[4]);
	{
		DEBUG_TIME_INIT;
		DEBUG_TIME_START;
		Matriz *resultado = m1->multiplicarMatrices(m2, numeroThreads, numeroFilasPaquete);
		DEBUG_TIME_END;
		DEBUG_PRINT_FINALTIME("Tiempo multiplicarMatrices(): \n\t");
		resultado->guardarMatriz("resultado");
	}

	DEBUG_TIME_END;

	DEBUG_PRINT_FINALTIME("Tiempo Total: \n\t");


	return 0;
}
