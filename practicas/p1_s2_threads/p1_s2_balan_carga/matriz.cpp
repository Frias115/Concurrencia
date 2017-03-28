#include "matriz.h"
#define NUM_THREADS 4
#define NUM_FILAS_POR_PAQUETE 6

int contador = 0;
pthread_mutex_t cerrojo;
//paqueteTrabajo **paquetesGlobal;
list<paqueteTrabajo*> *paquetesGlobal = new list<paqueteTrabajo*>();

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

	int numeroPaquetes = calcularYReservarPaquetes(segundaMatriz, resultado);
	int i = 0;
	do{	

		// maximo numero thread ordenador sergio 62240
		if(i > 60000){
			void* vacio;
			for(int j = 0; j < NUM_THREADS; j++){
				pthread_join(thread[j], &vacio);
			}
			i = 0;
		}

		pthread_create(&(thread[i]), NULL, multiplicarMatricesThreads, NULL);
		i++;


	}while(!paquetesGlobal->empty());
	
	void* vacio;
	for(int j = 0; j < NUM_THREADS; j++){
		pthread_join(thread[j], &vacio);
	}

	return resultado;
}

void *Matriz::multiplicarMatricesThreads(void *prueba) {

	//paqueteTrabajo *paquete = (paqueteTrabajo*)paqueteDeTrabajo;
	//list<paqueteTrabajo*> *listaPaquetes = (list<paqueteTrabajo*>*)paquetesGlobal;


	pthread_mutex_lock(&cerrojo);
	paqueteTrabajo *paquete = paquetesGlobal->front();
	paquetesGlobal->pop_front();
	pthread_mutex_unlock(&cerrojo);

	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++) {
			paquete->resultado[i+paquete->filaInicial][j] = multiplicaVector(paquete->datosUno[i], paquete->datosDos[j], paquete->numeroRealColumnasACalcular);
		}
	}

	//free(paquete);
	pthread_exit(NULL);
}


int Matriz::multiplicaVector(int *vector1, int *vector2, int numeroRealColumnasACalcular) {
	int resultado = 0;
	for (int k = 0; k < numeroRealColumnasACalcular; k++){
		resultado += vector1[k] * vector2[k];
		//cout << "Resultado: " << resultado << " += " << vector1[k] << " * " << vector2[k] << endl;
	}
	//cout << "Resultado: " << resultado << endl;
	return resultado;
}

int Matriz::calcularYReservarPaquetes(Matriz *segundaMatriz, Matriz *resultado){
	int numeroPaquetes = ceil((float)this->numFilas/ (float)NUM_FILAS_POR_PAQUETE);
	//paquetesGlobal = (paqueteTrabajo**)malloc(sizeof(paqueteTrabajo*)*numeroPaquetes);
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




	paquete->datosDos = (int **)malloc(sizeof(int*)*segundaMatriz->numFilas);
	for(int i = 0; i < segundaMatriz->numFilas; i++){
		paquete->datosDos[i] = (int*)malloc(sizeof(int)*segundaMatriz->numColumnas);
	}


	//paquete->datosDos = segundaMatriz->datos;


	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++){
			paquete->datosUno[i][j] = this->datos[i+paquete->filaInicial][j];
		}
	}

	for (int i = 0; i < segundaMatriz->numFilas; i++){
		for (int j = 0; j < segundaMatriz->numColumnas; j++){
			paquete->datosDos[i][j] = segundaMatriz->datos[i][j];
		}
	}
/*
	for (int j = 0; j < this->numFilas; j++){
		for (int k = 0; k < this->numColumnas; k++) {
			if(paquete->datosDos[j][k] >= 10)
				cout << "paqueteTrabajo:  j: "  << j<< " k: " << k <<"  paquete->datosDos[j][k]: " << paquete->datosDos[j][k] << endl;
		}
	}*/



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