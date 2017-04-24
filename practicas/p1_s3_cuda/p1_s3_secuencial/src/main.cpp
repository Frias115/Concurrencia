#include "matriz.h"

using namespace std;

int main(int argc, char **argv){
//USO PROGRAMA: multiplicadorMatrices <MATRIZ1> <MATRIZ2>
	bool leerTraspuesta = true;

	Matriz *m1 = new Matriz(argv[1], !leerTraspuesta);
	Matriz *m2 = new Matriz(argv[2], leerTraspuesta);
	Matriz *resultado = m1->multiplicarMatrices(m2);

	//cout << "Matriz 1: " << endl;
	//m1->imprimirMatriz();
	//cout <<  "Matriz 2 traspuesta: " << endl;
	//m2->imprimirMatriz();
	//cout << "Matriz resultado: " << endl;
	//resultado->imprimirMatriz();

	resultado->guardarMatriz("Resultado");
	return 0;
}
