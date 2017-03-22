#include "matriz.h"
#include <string>
#include <time.h>

using namespace std;

//USO PROGRAMA: GENERADORMATRICES <NUMFILAS> <NUMCOLUMNAS> <IDENTIDAD> <NOMBREFICH>
int main(int argc, char **argv){

	srand(time(NULL));

//Lectura parámetros
	int numFilas = atoi(argv[1]);
	int numColumnas = atoi(argv[2]);
	int identidad = atoi(argv[3]);
	string nombreFich = argv[4];
	
//Crear matriz
	Matriz *matriz = new Matriz(numFilas, numColumnas, identidad);
//Rellenar matriz
	matriz->rellenarMatriz();
//Guardar matriz
	matriz->guardarMatriz(nombreFich);
//Imprime para comprobar
//	matriz->imprimirMatriz();

return 0;
}
