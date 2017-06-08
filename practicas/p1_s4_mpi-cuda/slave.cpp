#include "slave.h"

int NUM_THREADS = 1;

paqueteTrabajo* multiplicarMatrices(paqueteTrabajo *paquete, int numeroThreads) {
	if( numeroThreads <= 0){
		NUM_THREADS = 1;
	} else
	{
		NUM_THREADS = numeroThreads;
	}
	

	pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t)*NUM_THREADS);
	int threadID = 0;

	for (int i = 0; i < NUM_THREADS; i++){
		
		pthread_create(&(thread[i]), NULL, multiplicarMatricesThreads, paquete);

	}

	void* vacio;
	for(int j = 0; j < NUM_THREADS; j++){
		pthread_join(thread[j], &vacio);
	}
	return paquete; 
}

void* multiplicarMatricesThreads(void *paqueteDeTrabajo) {

	paqueteTrabajo *paquete = (paqueteTrabajo*)paqueteDeTrabajo;

	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++) {
			paquete->resultado[i+paquete->filaInicial][j] = multiplicaVector(paquete->datosUno[i], paquete->datosDos[j], paquete->numeroRealColumnasACalcular);
		}
	}
	free(paquete);
	pthread_exit(NULL);
}


int multiplicaVector(int *vector1, int *vector2, int numeroRealColumnasACalcular) {
	int resultado = 0;
	for (int k = 0; k < numeroRealColumnasACalcular; k++)
		resultado += vector1[k] * vector2[k];

	return resultado;
}