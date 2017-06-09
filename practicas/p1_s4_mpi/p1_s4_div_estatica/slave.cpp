#include "slave.h"

paqueteTrabajo* multiplicarMatrices(paqueteTrabajo *paquete, int numeroThreads) {
		
	cout << "Entro en el multiplicar matrices de slave con " << numeroThreads << "threads y " << paquete->numeroRealFilasACalcular << " filas a calcular." << endl;
	if( numeroThreads <= 0){
		numeroThreads = 1;
	}

	pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t)*numeroThreads);
	int threadID = 0;
	cout << "Creo los threads" << endl;
	for (int i = 0; i < numeroThreads; i++){
		
		pthread_create(&(thread[i]), NULL, multiplicarMatricesThreads, paquete);

	}
	cout << "Acaban los threads y el numero de filas a calcular es " << paquete->numeroRealFilasACalcular << endl;

	void* vacio;
	for(int j = 0; j < numeroThreads; j++){
		pthread_join(thread[j], &vacio);
	}
	cout << "Recibo los datos de los threads" << endl;
	return paquete; 
}

void* multiplicarMatricesThreads(void *paqueteDeTrabajo) {

	paqueteTrabajo *paquete = (paqueteTrabajo*)paqueteDeTrabajo;
	cout << "numero real filas " << paquete->numeroRealFilasACalcular << endl;
	cout << "numero real columnas " << paquete->numeroRealColumnasACalcular << endl;

	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++) {
			paquete->resultado[i+paquete->filaInicial][j] = multiplicaVector(paquete->datosUno[i], paquete->datosDos[j], paquete->numeroRealColumnasACalcular);
		}
	}
	cout << "Acaba la multiplicacion en el thread" << endl;
	pthread_exit(NULL);
}


int multiplicaVector(int *vector1, int *vector2, int numeroRealColumnasACalcular) {
	int resultado = 0;
	for (int k = 0; k < numeroRealColumnasACalcular; k++)
		resultado += vector1[k] * vector2[k];
	return resultado;
}