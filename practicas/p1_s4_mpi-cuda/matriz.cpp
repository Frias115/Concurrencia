#include "matriz.h"

int NUM_THREADS = 1;

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

Matriz *Matriz::multiplicarMatrices(Matriz *segundaMatriz, int numeroThreads, int numeroEsclavos) {
	Matriz *resultado = new Matriz(this->numFilas, this->numColumnas);

	if( numeroThreads <= 0){
		NUM_THREADS = 1;
	} else
	{
		NUM_THREADS = numeroThreads;
	}

	for (int i = 1; i < numeroEsclavos; i++){
		paqueteTrabajo *paquete = crearPaquetesDeTrabajo(i, segundaMatriz, resultado);
		
		//envio 
		MPI_Send(paquete, sizeof(paqueteTrabajo), MPI_BYTE, i, 0, MPI_COMM_WORLD);
		MPI_Send(&NUM_THREADS, sizeof(int), MPI_BYTE, i, 0, MPI_COMM_WORLD);
		printf("\n Mensaje enviado a %d", i);

		free(paquete);
	}

	for (int i = 1; i < numeroEsclavos; i++){
		MPI_Status status;
		paqueteTrabajo *paquete = (paqueteTrabajo *)malloc(sizeof(paqueteTrabajo));

		//recibo
		MPI_Recv(paquete, sizeof(paqueteTrabajo), MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);

		//junto resultados
		for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
			for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++) {
				resultado->datos[i+paquete->filaInicial][j] = paquete->resultado[i+paquete->filaInicial][j];
			}
		}

		free(paquete);
	}

	return resultado;
}

paqueteTrabajo *Matriz::crearPaquetesDeTrabajo(int parteMatriz, Matriz *segundaMatriz, Matriz *resultado){
	paqueteTrabajo *paquete = (paqueteTrabajo *)malloc(sizeof(paqueteTrabajo));

	float aux = (float)this->numFilas / (float)NUM_THREADS;
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

	for (int i = 0; i < paquete->numeroRealFilasACalcular; ++i){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; ++j){
			paquete->datosUno[i][j] = this->datos[i+paquete->filaInicial][j];
		}
	}

	paquete->resultado = resultado->datos;

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