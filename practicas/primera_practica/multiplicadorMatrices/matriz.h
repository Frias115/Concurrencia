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

	Matriz(string nombre);

	void guardarMatriz(string nombre);

	void imprimirMatriz();

	void reservarMemoria();

	~Matriz();
};