#pragma once
#include "matriz.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <math.h>
using namespace std;

static void *multiplicarMatricesThreads(void *paqueteDeTrabajo);

paqueteTrabajo* multiplicarMatrices(paqueteTrabajo *paquete, int numeroThreads);

static int multiplicaVector(int *vector1, int *vector2, int numeroRealColumnasACalcular);
