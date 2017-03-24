#include "matriz.h"

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

void *Matriz::multiplicaVector(void *threadParams) {

	thread_params *params = (thread_params*)threadParams;

	int resultado = 0;
	for (int k = 0; k < this->numColumnas; k++)
		resultado += params->vectorUno[k] * params->vectorDos[k];
	pthread_exit(&resultado);
	delete(params);
}

//http://stackoverflow.com/questions/28205116/correct-way-to-pass-a-struct-to-pthread-within-a-for-loop

Matriz *Matriz::multiplicarMatrices(Matriz *segundaMatriz) {
	Matriz *resultado = new Matriz(this->numFilas, this->numColumnas);

	pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t)*this->numColumnas);


	for (int i = 0; i < this->numFilas; i++){
		for (int j = 0; j < this->numColumnas; j++) {
			
			thread_params *params = new thread_params;

			params->vectorUno = this->datos[i];
			params->vectorDos = this->datos[j];

			//resultado->datos[i][j] = multiplicaVector(this->datos[i], segundaMatriz->datos[j]);
			resultado->datos[i][j] = pthread_create(&(thread[j]), NULL, multiplicaVector, (void *)params);
			
		}

		void *vacio;
		for (int j = 0; j < this->numColumnas; j++)
			pthread_join(thread[j], &vacio);

	}

	return resultado;
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