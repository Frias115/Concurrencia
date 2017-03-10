#include "matriz.h"

Matriz::Matriz(string nombre){

	FILE *fichero = fopen(nombre.c_str(), "r+");
	if(fich == NULL) {
		printf("Error");
		return;
	}

	fread(&this->numFilas, sizeof(char), sizeof(int), fichero);
	fread(&this->numColumnas, sizeof(char), sizeof(int), fichero);

	this->reservaMemoria();

	for(int i = 0; i < this->numFilas; i++){
		for(int j = 0; j < this->numColumnas; j++){
			fread(&this->datos[i][j], sizeof(char), sizeof(int), fichero);
		}
	}
	fclose(fichero);
}

void Matriz::reservaMemoria(){
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

void Matriz::imprimirMatriz(){
	for(int i = 0; i < this->numFilas; i++){
		for(int j = 0; j < this->numColumnas; j++){
			cout << this->datos[i][j] << " ";
		}
		cout << endl;
	}
}
