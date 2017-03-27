#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <math.h>
using namespace std;

struct paqueteTrabajo {
	int filaInicial;
	int numeroRealFilasACalcular;
	int numeroRealColumnasACalcular;
    int **datosUno;
    int **datosDos;
    int **resultado;
};


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

	static int multiplicaVector(int *vector1, int *vector2, int numeroRealColumnasACalcular);

	Matriz *multiplicarMatrices(Matriz *segundaMatriz);

	static void *multiplicarMatricesThreads(void *segundaMatriz);

	int calcularYReservarPaquetes(Matriz *segundaMatriz);

	void crearPaqueteDeTrabajo(int parteMatriz, int numeroPaquetes, Matriz *segundaMatriz);

	~Matriz();
};

