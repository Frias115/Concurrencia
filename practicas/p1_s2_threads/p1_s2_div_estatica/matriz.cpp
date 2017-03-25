#include "matriz.h"
#define NUM_THREADS 4

int contador = 0;
pthread_mutex_t cerrojo;

Matriz::Matriz(string nombre, bool leerTraspuesta){

	FILE *fichero = fopen(nombre.c_str(), "r+");
	if(fichero == NULL) {
		printf("Error");
		return;
	}

	fread(&this->numFilas, sizeof(char), sizeof(int), fichero);
	fread(&this->numColumnas, sizeof(char), sizeof(int), fichero);

	this->reservarMemoria();

	if(leerTraspuesta){
		for (int i = 0; i < this->numColumnas; i++){
			for (int j = 0; j < this->numFilas; j++){
				fread(&(this->datos[j][i]), sizeof(char), sizeof(int), fichero);
			}
		}
	}
	else{
		for (int i = 0; i < this->numFilas; i++) 
			fread(this->datos[i], sizeof(float), this->numColumnas, fichero);
	}

	fclose(fichero);
}

Matriz::Matriz(int numFilas, int numColumnas){
	this->numFilas = numFilas;
	this->numColumnas = numColumnas;

	this->datos = (int **)malloc(sizeof(int*)*numFilas);
	for(int i = 0; i < numFilas; i++){
		this->datos[i] = (int*)malloc(sizeof(int)*numColumnas);
	}
}

void Matriz::reservarMemoria(){
	this->datos = (int **)malloc(sizeof(int*)*this->numFilas);
	for(int i = 0; i < this->numFilas; i++){
		this->datos[i] = (int*)malloc(sizeof(int)*this->numColumnas);
	}
}

void Matriz::guardarMatriz(string nombre){

	FILE* fich = fopen(nombre.c_str(), "w");
	if(fich == NULL) {
		printf("Error");
		return;
	}
	
	fwrite(&this->numFilas, sizeof(char), sizeof(int), fich);
	fwrite(&this->numColumnas, sizeof(char), sizeof(int), fich);
	for(int i = 0; i < this->numFilas; i++){
		for(int j = 0; j < this->numColumnas; j++){
			fwrite(&this->datos[i][j], sizeof(char), sizeof(int), fich);
		}
	}
	fclose(fich);
}

Matriz *Matriz::multiplicarMatrices(Matriz *segundaMatriz) {
	Matriz *resultado = new Matriz(this->numFilas, this->numColumnas);

	pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t)*NUM_THREADS);
	int threadID = 0;

	for (int i = 0; i < NUM_THREADS; ++i){
		paqueteTrabajo *paquete = crearPaquetesDeTrabajo(i, segundaMatriz);
		
		pthread_create(&(thread[i]), NULL, multiplicarMatricesThreads, paquete);
		pthread_join(thread[i], (void **)&paquete);

		for (int j = 0; j < paquete->numeroRealFilasACalcular; ++j){
			for (int k = 0; k < paquete->numeroRealColumnasACalcular; ++k){
				resultado->datos[j+paquete->filaInicial][k] = paquete->resultado[j][k];
			}
		}
	}
	return resultado;
}

void *Matriz::multiplicarMatricesThreads(void *paqueteDeTrabajo) {

	paqueteTrabajo *paquete = (paqueteTrabajo*)paqueteDeTrabajo;

	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++) {
			paquete->resultado[i][j] = multiplicaVector(paquete->datosUno[i], paquete->datosDos[j], paquete->numeroRealColumnasACalcular);
		}
	}
	pthread_exit(paquete);
}


int Matriz::multiplicaVector(int *vector1, int *vector2, int numeroRealColumnasACalcular) {
	int resultado = 0;
	for (int k = 0; k < numeroRealColumnasACalcular; k++)
		resultado += vector1[k] * vector2[k];
	return resultado;
}

paqueteTrabajo *Matriz::crearPaquetesDeTrabajo(int parteMatriz, Matriz *segundaMatriz){
	paqueteTrabajo *paquete = (paqueteTrabajo *)malloc(sizeof(paqueteTrabajo));

	float resultado = (float)this->numFilas / (float)NUM_THREADS;
	if(ceil(resultado) <= (this->numFilas - (ceil(resultado) * parteMatriz))){
		paquete->numeroRealFilasACalcular = ceil(resultado);
	}else{
		paquete->numeroRealFilasACalcular = (this->numFilas - (ceil(resultado) * parteMatriz));
		parteMatriz++;
	}

	paquete->numeroRealColumnasACalcular = this->numColumnas;

	paquete->filaInicial = paquete->numeroRealFilasACalcular*parteMatriz;

	paquete->datosUno = (int **)malloc(sizeof(int*)*paquete->numeroRealFilasACalcular);
	for(int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		paquete->datosUno[i] = (int*)malloc(sizeof(int)*paquete->numeroRealColumnasACalcular);
	}

	paquete->datosDos = segundaMatriz->datos;

	for (int i = 0; i < paquete->numeroRealFilasACalcular; ++i){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; ++j){
			paquete->datosUno[i][j] = this->datos[i+paquete->filaInicial][j];
		}
	}

	paquete->resultado = (int **)malloc(sizeof(int*)*paquete->numeroRealFilasACalcular);
	for(int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		paquete->resultado[i] = (int*)malloc(sizeof(int)*paquete->numeroRealColumnasACalcular);
	}
	return paquete;
}

void Matriz::imprimirMatriz(){
	for(int i = 0; i < this->numFilas; i++){
		for(int j = 0; j < this->numColumnas; j++){
			cout << this->datos[i][j] << " ";
		}
		cout << endl;
	}
}

Matriz::~Matriz(){
	for (int i = 0; i < this->numColumnas; ++i){
	free(this->datos[i]);
	}
	free(this->datos);
}