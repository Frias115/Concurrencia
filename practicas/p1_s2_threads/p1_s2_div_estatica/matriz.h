#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <pthread.h>
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

	static void *multiplicaVector(void *thread_params);

	Matriz *multiplicarMatrices(Matriz *segundaMatriz);

	~Matriz();
};

struct thread_params {
    //int threadId;
    int numFilas;
    int numColumnas;
    int *vectorUno;
    int *vectorDos;
    int resultado;
};