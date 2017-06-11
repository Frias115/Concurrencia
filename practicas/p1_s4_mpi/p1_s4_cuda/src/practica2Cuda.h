
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#include "matriz.h"

#ifndef MULTIPLICAMATRICES_H
#define MULTIPLICAMATRICES_H 

#ifdef  __cplusplus
extern "C" {
#endif

	paqueteTrabajo* mainCuda(paqueteTrabajo* paquete);

#ifdef  __cplusplus
}
#endif

#endif