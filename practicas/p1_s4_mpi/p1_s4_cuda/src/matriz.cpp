
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
	cout << "Tengo "  << numeroEsclavos-1 << " esclavos"<< endl;
	if( numeroThreads <= 0){
		NUM_THREADS = 1;
	} else
	{
		NUM_THREADS = numeroThreads;
	}

	for (int i = 1; i < numeroEsclavos; i++){
		enviarDatosAEsclavo((i -1), segundaMatriz, (numeroEsclavos -1));
		
		printf("\n Mensaje enviado a %d \n", i);

	}

	cout << "Los esclavos ya tienen la informacion " << endl;

	for (int i = 1; i < numeroEsclavos; i++){
		MPI_Status status;
		
		int filaInicial;
		int numeroRealFilasACalcular;
		int numeroRealColumnasACalcular;

		//recibo
		MPI_Recv(&filaInicial, sizeof(int), MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&numeroRealFilasACalcular, sizeof(int), MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&numeroRealColumnasACalcular, sizeof(int), MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);

		int** aux = (int **)malloc(sizeof(int*)*numeroRealColumnasACalcular);
		for(int j = 0; j < numeroRealColumnasACalcular; j++){
			aux[j] = (int*)malloc(sizeof(int)*numeroRealColumnasACalcular);
		}

		for (int j = 0; j < numeroRealColumnasACalcular; j++){
			MPI_Recv(aux[j], numeroRealColumnasACalcular, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		}

		cout << "Recibo el paquete del esclavo " << i << endl;
		//junto resultados
		for (int j = 0; j < numeroRealColumnasACalcular; j++){
			resultado->datos[j+filaInicial] = aux[j+filaInicial];
		}
		free(aux);
	}

	return resultado;
}

void Matriz::enviarDatosAEsclavo(int parteMatriz, Matriz *segundaMatriz, int numeroEsclavos){

	int filaInicial;
	int numeroRealFilasACalcular;
	int numeroRealColumnasACalcular;

	float aux = (float)this->numFilas / (float)numeroEsclavos;

	if(ceil(aux) <= (this->numFilas - (ceil(aux) * parteMatriz))){
		numeroRealFilasACalcular = ceil(aux);
		filaInicial = numeroRealFilasACalcular*parteMatriz;
	}else{
		numeroRealFilasACalcular = (this->numFilas - (ceil(aux) * parteMatriz));
		filaInicial = (ceil(aux))*parteMatriz;
	}
	numeroRealColumnasACalcular = this->numColumnas;
	
	//Envio informacion a los esclavos
	MPI_Send(&NUM_THREADS, sizeof(int), MPI_BYTE, parteMatriz+1, 0, MPI_COMM_WORLD);
	MPI_Send(&filaInicial, sizeof(int), MPI_BYTE, parteMatriz+1, 0, MPI_COMM_WORLD);
	MPI_Send(&numeroRealFilasACalcular, sizeof(int), MPI_BYTE, parteMatriz+1, 0, MPI_COMM_WORLD);
	MPI_Send(&numeroRealColumnasACalcular, sizeof(int), MPI_BYTE, parteMatriz+1, 0, MPI_COMM_WORLD);

	for (int i = 0; i < numeroRealFilasACalcular; ++i){
		MPI_Send(this->datos[i+filaInicial], numeroRealColumnasACalcular, MPI_INT, parteMatriz+1, 0, MPI_COMM_WORLD);
	}

	for (int i = 0; i < this->numFilas; ++i){
		MPI_Send(segundaMatriz->datos[i], numeroRealColumnasACalcular, MPI_INT, parteMatriz+1, 0, MPI_COMM_WORLD);
	}
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