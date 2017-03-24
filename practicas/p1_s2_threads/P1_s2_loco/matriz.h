#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;

class Matriz{

public:
	int numFilas;
	int numColumnas;
	int **datos;

	Matriz(int numFilas, int numColumnas);

	Matriz(string nombre, bool leerTraspuesta);

	void guardarMatriz(string nombre);

	void imprimirMatriz();

	void reservarMemoria();

	int multiplicaVector(int *vector1, int *vector2);

	Matriz *multiplicarMatrices(Matriz *segundaMatriz);

	~Matriz();
};