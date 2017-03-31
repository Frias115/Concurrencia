#include "matriz.h"

pthread_mutex_t cerrojo;
list<paqueteTrabajo*> *paquetesGlobal = new list<paqueteTrabajo*>();
int NUM_THREADS;
int NUM_FILAS_POR_PAQUETE;

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

Matriz *Matriz::multiplicarMatrices(Matriz *segundaMatriz, int numeroThreads, int numeroFilasPaquete) {
	Matriz *resultado = new Matriz(this->numFilas, this->numColumnas);

	if( numeroThreads <= 0){
		NUM_THREADS = 1;
	} else
	{
		NUM_THREADS = numeroThreads;
	}

	if( numeroFilasPaquete <= 0){
		NUM_FILAS_POR_PAQUETE = 1;
	} else
	{
		NUM_FILAS_POR_PAQUETE = numeroFilasPaquete;
	}
	

	pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t)*NUM_THREADS);
	pthread_mutex_init(&cerrojo,NULL);

	int numeroPaquetes = calcularYReservarPaquetes(segundaMatriz, resultado);
	int i = 0;

	do{	
		if( i >= NUM_THREADS){
			i = 0;
			void* vacio;
			for(int j = 0; j < NUM_THREADS; j++){
				pthread_join(thread[j], &vacio);
			}
		}

		pthread_create(&(thread[i]), NULL, multiplicarMatricesThreads, NULL);
		i++;

	}while(!paquetesGlobal->empty());
	
	void* vacio;
	for(int j = 0; j < i; j++){
		pthread_join(thread[j], &vacio);
	}

	pthread_mutex_destroy(&cerrojo);
	delete(paquetesGlobal);
	return resultado;
}

void *Matriz::multiplicarMatricesThreads(void *o) {

	pthread_mutex_lock(&cerrojo);

	paqueteTrabajo *paquete;
	if(!paquetesGlobal->empty()){
		paquete = paquetesGlobal->front();
	
		paquetesGlobal->pop_front();
		
	} else{
		pthread_mutex_unlock(&cerrojo);
		pthread_exit(NULL);
	}
	pthread_mutex_unlock(&cerrojo);

	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++) {
			paquete->resultado[i+paquete->filaInicial][j] = multiplicaVector(paquete->datosUno[i], paquete->datosDos[j], paquete->numeroRealColumnasACalcular);
		}
	}
	
	free(paquete);
	pthread_exit(NULL);
}


int Matriz::multiplicaVector(int *vector1, int *vector2, int numeroRealColumnasACalcular) {
	int resultado = 0;
	for (int k = 0; k < numeroRealColumnasACalcular; k++){
		resultado += vector1[k] * vector2[k];
	}
	return resultado;
}

int Matriz::calcularYReservarPaquetes(Matriz *segundaMatriz, Matriz *resultado){
	int numeroPaquetes = ceil((float)this->numFilas/ (float)NUM_FILAS_POR_PAQUETE);

	for (int i = 0; i < numeroPaquetes; ++i){
		crearPaqueteDeTrabajo(i, numeroPaquetes, segundaMatriz, resultado);
	}
	return numeroPaquetes;
}

void Matriz::crearPaqueteDeTrabajo(int parteMatriz, int numeroPaquetes, Matriz *segundaMatriz, Matriz *resultado){
	paqueteTrabajo *paquete = (paqueteTrabajo *)malloc(sizeof(paqueteTrabajo));

	float aux = (float)this->numFilas / (float)numeroPaquetes;
	if(ceil(aux) <= (this->numFilas - (ceil(aux) * parteMatriz))){
		paquete->numeroRealFilasACalcular = ceil(aux);
		paquete->filaInicial = paquete->numeroRealFilasACalcular*parteMatriz;
	}else{
		paquete->numeroRealFilasACalcular = (this->numFilas - (ceil(aux) * parteMatriz));
		paquete->filaInicial = (ceil(aux))*parteMatriz;
	}
	paquete->numeroRealColumnasACalcular = this->numColumnas;

	paquete->datosUno = (int **)malloc(sizeof(int*)*paquete->numeroRealFilasACalcular);
	for(int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		paquete->datosUno[i] = (int*)malloc(sizeof(int)*paquete->numeroRealColumnasACalcular);
	}

	paquete->datosDos = segundaMatriz->datos;

	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++){
			paquete->datosUno[i][j] = this->datos[i+paquete->filaInicial][j];
		}
	}

	paquete->resultado = resultado->datos;

	paquetesGlobal->push_back(paquete);
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
	for (int i = 0; i < this->numColumnas; i++){
	free(this->datos[i]);
	}
	free(this->datos);
}