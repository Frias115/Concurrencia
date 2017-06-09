
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "debug_time.h"
#include "mpi.h"
#include "matriz.h"

#ifndef MULTIPLICAMATRICES_H
#define MULTIPLICAMATRICES_H 

#ifdef  __cplusplus
extern "C" {
#endif

	paqueteTrabajo* mainCuda(paqueteTrabajo* paquete);
	float kernel_multiplicarVectores(int lado, float* fila, float* columna);
	void kernel_multiplicarMatrices(int lado, float** matriz1, float** matriz2, float** resultado);

#ifdef  __cplusplus
}
#endif

#endif