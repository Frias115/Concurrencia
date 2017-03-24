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
	bool identidad;
	int **datos;

	Matriz(int numFilas, int numColumnas, bool identidad);

	void rellenarMatriz();

	void guardarMatriz(string nombre);

	void imprimirMatriz();

	~Matriz();
};