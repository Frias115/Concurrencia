#include "matriz.h"

Matriz::Matriz(int numFilas, int numColumnas, bool identidad){
	this->numFilas = numFilas;
	this->numColumnas = numColumnas;
	this->identidad = identidad;

	this->datos = (int **)malloc(sizeof(int*)*numFilas);
	for(int i = 0; i < numFilas; i++){
		this->datos[i] = (int*)malloc(sizeof(int)*numColumnas);
	}
}

void Matriz::rellenarMatriz(){
	if(identidad){
		for(int i = 0; i < numFilas; i++){
			for(int j = 0; j < numColumnas; j++){
				if(i == j){
					this->datos[i][j] = 1;
				}
				else{
					this->datos[i][j] = 0;
				}
			}
		}
	}
	else{
		for(int i = 0; i < numFilas; i++){
			for(int j = 0; j < numColumnas; j++){
				this->datos[i][j] = (int)rand();
			}
		}
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

Matriz::~Matriz(){
	for (int i = 0; i < this->numColumnas; ++i){
		free(this->datos[i]);
	}
	free(this->datos);
}
