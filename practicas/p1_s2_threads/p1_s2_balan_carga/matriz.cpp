#include "matriz.h"
#define NUM_THREADS 4
#define NUM_FILAS_POR_PAQUETE 11

int contador = 0;
pthread_mutex_t cerrojo;
paqueteTrabajo **paquetesGlobal;

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

	int numeroPaquetes = calcularYReservarPaquetes(segundaMatriz);

	for (int i = 0; i < numeroPaquetes; i++){	

		for (int j = 0; j < this->numFilas; j++){
			for (int k = 0; k < this->numColumnas; k++) {
				if(paquetesGlobal[i]->datosDos[j][k] >= 10)
					cout << "PRE:  j: "  << j<< " k: " << k <<"  paquetesGlobal[i]->datosDos[j][k]: " << paquetesGlobal[i]->datosDos[j][k] << endl;
			}
		}
		pthread_create(&(thread[i]), NULL, multiplicarMatricesThreads, paquetesGlobal[i]);
		pthread_join(thread[i], (void **)&paquetesGlobal[i]);


		for (int j = 0; j < this->numFilas; j++){
			for (int k = 0; k < this->numColumnas; k++) {
				if(paquetesGlobal[i]->datosDos[j][k] >= 10)
					cout << "POST:  j: "  << j<< " k: " << k <<"  paquetesGlobal[i]->datosDos[j][k]: " << paquetesGlobal[i]->datosDos[j][k] << endl;
			}
		}

		for (int j = 0; j < paquetesGlobal[i]->numeroRealFilasACalcular; j++){
			for (int k = 0; k < paquetesGlobal[i]->numeroRealColumnasACalcular; k++){
				resultado->datos[j+paquetesGlobal[i]->filaInicial][k] = paquetesGlobal[i]->resultado[j][k];
			}
		}

	}


	for (int i = 0; i < 1; ++i){
		cout << " paquetesGlobal[numeroPaquetes-i]->numeroRealFilasACalcular: "<< paquetesGlobal[32]->numeroRealFilasACalcular << endl;
		for (int j = 0; j < paquetesGlobal[32]->numeroRealFilasACalcular; ++j){
			for (int k = 0; k < paquetesGlobal[32]->numeroRealColumnasACalcular; ++k){
				cout << j <<" "<< k << " paquetesGlobal[numeroPaquetes-i]->resultado[j][k]: " << paquetesGlobal[32]->resultado[j][k];
				cout << " paquetesGlobal[numeroPaquetes-i]->datosUno[j][k]: " << paquetesGlobal[32]->datosUno[j][k] << endl;
			}
		}
		cout << " paquetesGlobal[numeroPaquetes-i]->filaInicial: " << paquetesGlobal[33]->filaInicial << endl;
	}
/*
		for (int j = 0; j < this->numFilas; ++j){
			for (int k = 0; k < this->numColumnas; ++k){
				cout << j <<" "<< k << " resultado->datos[j][k]: " << resultado->datos[j][k] << " this->datos[j][k]: " << this->datos[j][k] << endl;
			}
		}
		*/

	return resultado;
}

void *Matriz::multiplicarMatricesThreads(void *paqueteDeTrabajo) {

	paqueteTrabajo *paquete = (paqueteTrabajo*)paqueteDeTrabajo;

	pthread_mutex_lock(&cerrojo);
/*
	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++) {
			if(paquete->datosUno[i][j] >= 10)
				cout << " i: "  << i<< " j: " << j <<"  paquete->datosUno[i][j]: " << paquete->datosUno[i][j] << endl;
		}
	}



	for (int j = 0; j < 200; j++){
		for (int k = 0; k < 200; k++) {
				cout << "THREAD DENTRO:  j: "  << j << " k: " << k <<"  paquete->datosDos[j][k]: " << paquete->datosDos[j][k] << endl;
		}
	}
*/
	for (int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		for (int j = 0; j < paquete->numeroRealColumnasACalcular; j++) {
			paquete->resultado[i][j] = multiplicaVector(paquete->datosUno[i], paquete->datosDos[j], paquete->numeroRealColumnasACalcular);
		}
	}

	pthread_mutex_unlock(&cerrojo);

	pthread_exit(paquete);
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

int Matriz::calcularYReservarPaquetes(Matriz *segundaMatriz){
	int numeroPaquetes = ceil((float)this->numFilas/ (float)NUM_FILAS_POR_PAQUETE);
	paquetesGlobal = (paqueteTrabajo**)malloc(sizeof(paqueteTrabajo*)*numeroPaquetes);
	for (int i = 0; i < numeroPaquetes; ++i){
		crearPaqueteDeTrabajo(i, numeroPaquetes, segundaMatriz);
	}
	return numeroPaquetes;
}

void Matriz::crearPaqueteDeTrabajo(int parteMatriz, int numeroPaquetes, Matriz *segundaMatriz){
	paqueteTrabajo *paquete = (paqueteTrabajo *)malloc(sizeof(paqueteTrabajo));

	float resultado = (float)this->numFilas / (float)numeroPaquetes;
	if(ceil(resultado) <= (this->numFilas - (ceil(resultado) * parteMatriz))){
		paquete->numeroRealFilasACalcular = ceil(resultado);
		paquete->filaInicial = paquete->numeroRealFilasACalcular*parteMatriz;
	}else{
		paquete->numeroRealFilasACalcular = (this->numFilas - (ceil(resultado) * parteMatriz));
		paquete->filaInicial = (ceil(resultado))*parteMatriz;
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

	for (int j = 0; j < this->numFilas; j++){
		for (int k = 0; k < this->numColumnas; k++) {
			if(paquete->datosDos[j][k] >= 10)
				cout << "paqueteTrabajo:  j: "  << j<< " k: " << k <<"  paquete->datosDos[j][k]: " << paquete->datosDos[j][k] << endl;
		}
	}



	paquete->resultado = (int **)malloc(sizeof(int*)*paquete->numeroRealFilasACalcular);
	for(int i = 0; i < paquete->numeroRealFilasACalcular; i++){
		paquete->resultado[i] = (int*)malloc(sizeof(int)*paquete->numeroRealColumnasACalcular);
	}
	paquetesGlobal[parteMatriz] = paquete;
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