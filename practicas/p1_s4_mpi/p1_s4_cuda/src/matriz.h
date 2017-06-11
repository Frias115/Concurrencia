#pragma once
#include "mpi.h"
#include "debug_time.h"
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

	Matriz *multiplicarMatrices(Matriz *segundaMatriz, int numeroEsclavos);

	void enviarDatosAEsclavo(int parteMatriz, Matriz *segundaMatriz, int numeroEsclavos);

	~Matriz();
};

