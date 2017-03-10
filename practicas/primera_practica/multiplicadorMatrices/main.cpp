#include "matriz.h"

using namespace std;

int main(int argc, char **argv){
//USO PROGRAMA: multiplicarMatricesSec <MATRIZ1> <MATRIZ2> <MATRIZRESULTADO>

	matriz m1, m2, mres;

//Carga de datos
	m1 = LeerMatriz(argv[1], 0);
	m1 = LeerMatriz(argv[2], 1);

//Reserva de resultado 
	mres.numFilas = m1.numFilas;
	mres.numColumnas = m2.numColumnas;
	mres.datos = crearMatriz(mres.numFilas, mres.numColumnas);

//Multiplicamos matrices
	multiplicaMatrices(m1, m2, mres);

	escribirMatriz(mres.datos, mres.numFilas, mres.numColumnas, argv[3]);

	imprimeMatriz(mres);

	return 0;
}
