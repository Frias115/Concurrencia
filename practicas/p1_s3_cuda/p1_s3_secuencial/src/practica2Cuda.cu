/*
 ============================================================================
 Name        : practica2Cuda.cu
 Author      : Sergio Rosello & César Gil
 Version     : 0.1
 Copyright   : If you copy you will fail
 Description : Optimizaciones usando GPU
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define LADO 2000 //lado de la matriz

using namespace std;

void inicializarMatrizConsecutiva(float ** matriz, int tamFilas, int tamColumnas){
	int i, j;
	for (i = 0; i < tamFilas; i++)
		for (j = 0; j < tamColumnas; j++)
			matriz[i][j] = i*tamFilas + j;
}

void inicializarMatrizAleatoria(float **matriz, int tamFilas, int tamColumnas) {
	int i, j;
	for (i = 0; i < tamFilas; i++)
		for (j = 0; j < tamColumnas; j++)
			matriz[i][j] = rand() % 10;
	//matriz[i][j] = 1;
}

void crearMatriz(char* nombre)
{

	int i = 0;
	//int j = 0;

	//Inicializamos memoria para la matriz.
	float **mat = (float **)malloc(sizeof(float *) * LADO);
	for (i = 0; i < LADO; i++) {
		mat[i] = (float *)malloc(sizeof(float) * LADO);
	}

	inicializarMatrizAleatoria(mat, LADO, LADO);

	//Abrimos el fichero binario mat en modo de escritura.
	FILE* fich_bin = fopen(nombre, "w");

	//Escribimos en el archivo binario apuntado por fich_bin

	//Volcamos los datos de la matriz en el archivo binario apuntado por fich_bin
	for (i = 0; i < LADO; i++)
		fwrite(mat[i], sizeof(int), LADO, fich_bin);
	fclose(fich_bin);

	//Liberamos memoria de cada uno de los elementos de la matriz.
	for (i = 0; i < LADO; i++) {
		free(mat[i]);
	}

	//Liberamos memoria de la matriz.
	free(mat);

}

//Imprime la matriz recibida
void imprimirMatriz(float **matriz) {

	FILE* fich = fopen("resultado.bin", "w");
	if(fich == NULL) {
		//printf("Error");
	} else{
		//int aux = LADO;
		//fwrite(&(aux), sizeof(char), sizeof(int), fich);
		//fwrite(&(aux), sizeof(char), sizeof(int), fich);
		int aux = 1;
		for (int i = 0; i < LADO; i++) {
			for (int j = 0; j < LADO; j++){
				//printf("%.0f ", matriz[i][j]);
				fwrite(&(matriz[j][i]), sizeof(float),1, fich);
			}
			//printf("\n");
		}
	}

	fclose(fich);
}

//lee la matriz de un fichero binario
void leerDatosBin(char *nombreFichero, float ***datos, bool leerTraspuesta) {
	FILE* fichero = fopen(nombreFichero, "r");
	//Funciones accesibles: fclose, fread, fwrite
	float **datosLeidos;
	int numFilas, numColumnas;
	int i = 0, j = 0;


	//Inicializamos un array para guardar todos los datos que leemos del fichero.
	datosLeidos = (float **)malloc(LADO * sizeof(float*));

	//multiplicamos por 4 (bytes que ocupa un float)
	for (int i = 0; i < LADO; i++)
		datosLeidos[i] = (float*)malloc(sizeof(float)*LADO);

	if(!leerTraspuesta)
		for (int i = 0; i < LADO; i++)
			for (int j = 0; j < LADO; j++)
				fread(&(datosLeidos[i][j]), sizeof(float), 1, fichero);

	else //Leer la matriz de una forma traspuesta.
		for (int i = 0; i < LADO; i++)
			for (int j = 0; j < LADO; j++)
				fread(&(datosLeidos[j][i]), sizeof(float), 1, fichero);


	//Derreferenciación.
	(*datos) = datosLeidos;
	fclose(fichero);
}

//lamada de la gpu para multiplicar los vectores
__device__ float multiplicarVectores(int lado, float* fila, float* columna){
	if(threadIdx.x == 0 && threadIdx.y == 0)
		printf("Multiplicando vectores\n");
	float resultadoAux = 0;
	for(int i = 0; i<lado; i++){
		resultadoAux += fila[i] * columna[i];
	}

	return resultadoAux;

}

//llamada a la gpu para multiplicar matrices
__global__ void kernel_multiplicarMatrices(int lado, float** matriz1, float** matriz2, float** resultado){
	//printf("estoy multiplicando\n");
	int fila = blockIdx.x * blockDim.y + threadIdx.y;
	int columna = blockIdx.y * blockDim.x + threadIdx.x;

	//control de errores del thread
	if((fila >= lado) || (columna >= lado)){
		//printf("ha ocurrido un error en multiplicacion\n");
		return;
		}

	resultado[fila][columna] = multiplicarVectores(lado, matriz1[fila], matriz2[columna]);
	//printf("cuda thread %d %d %.0f \n",fila,columna,resultado[fila][columna]);
}



int main(){

	//Inicialización de la semilla para los números aleatorios.
	srand(time(NULL));

	bool leerTraspuesta = true;

	//CARGA E INICIALIZACION DE LAS MATRICES
	//CPU
	float** matriz1_host;
	float** matriz2_host;
	float** matrizResultado_host;

	//NEXO (memoria intermedia)
	float** matriz1_nexo;
	float** matriz2_nexo;
	float** matrizResultado_nexo;

	//GPU
	float** matriz1_device;
	float** matriz2_device;
	float** matrizResultado_device;

	//leemos de fichero binario
	leerDatosBin("nuevadosmilaleatoria.bin", &matriz1_host, leerTraspuesta);
	leerDatosBin("nuevadosmilidentidad.bin", &matriz2_host, leerTraspuesta);

	//IMPRIME LAS MATRICES GENERADAS
	//printf("Se van a generar matrices de %d X %d : \n", LADO, LADO);
	//printf("MATRIZ A: \n\n");
	//imprimirMatriz(matriz1_host);
	//printf("MATRIZ B: \n\n");
	//imprimirMatriz(matriz2_host);

	//se hace la traspuesta de la segunda matriz para poder multiplicarla
	leerDatosBin("nuevadosmilidentidad.bin", &matriz2_host, !leerTraspuesta);
	//printf("MATRIZ B traspuesta: \n\n");
	//imprimirMatriz(matriz2_host);

	//Reserva para el resultado del host
	matrizResultado_host = (float**)malloc(LADO * sizeof(float*));
	for(int i=0; i < LADO; i++){
		matrizResultado_host[i] = (float*)malloc(LADO * sizeof(float));
	}

	//Reserva de la memoria intermedia
	matriz1_nexo = (float**)malloc(LADO * sizeof(float*));
	matriz2_nexo = (float**)malloc(LADO * sizeof(float*));
	matrizResultado_nexo = (float**)malloc(LADO * sizeof(float*));

	//Reserva de memoria en GPU
	cudaError_t err1 = cudaMalloc((void**)&matriz1_device, sizeof(float*)* LADO);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));

	err1 = cudaMalloc((void**)&matriz2_device, sizeof(float*)* LADO);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));
	err1 = cudaMalloc((void**)&matrizResultado_device, sizeof(float*)* LADO);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));

	//Reserva de memoria para cada uno de los arrays intermedios
	for(int i = 0; i < LADO; i++){
		err1 = cudaMalloc((void**)&matriz1_nexo[i], sizeof(float)* LADO);
		//printf("matriz1_nexo Run Kernel: %s \n", cudaGetErrorString(err1));
		err1 = cudaMalloc((void**)&matriz2_nexo[i], sizeof(float)* LADO);
		//printf("matriz2_nexo Run Kernel: %s \n", cudaGetErrorString(err1));
		cudaMalloc((void**)&(matrizResultado_nexo[i]), sizeof(float)* LADO);
	}

	//Copia el contenido de los arrays de CPU a los arrays de la matriz intermedia
	for(int i = 0; i < LADO; i++){
			err1 = cudaMemcpy(matriz1_nexo[i], matriz1_host[i], LADO * sizeof(float),cudaMemcpyHostToDevice);
			//printf("cudaMemcoy matriz2_host1 a nexo1 Run Kernel: %s \n", cudaGetErrorString(err1));
			err1 = cudaMemcpy(matriz2_nexo[i], matriz2_host[i], LADO * sizeof(float),cudaMemcpyHostToDevice);
			//printf("cudaMemcoy matriz2_host2 a nexo2 Run Kernel: %s \n", cudaGetErrorString(err1));
	}

	//copia el contenido del array de punteros de CPU a GPU
	err1 = cudaMemcpy(matriz1_device, matriz1_nexo, LADO * sizeof(float*),cudaMemcpyHostToDevice);
	//printf("copia de cpu a gpu array de punteros matriz1 Run Kernel: %s \n", cudaGetErrorString(err1));
	err1 = cudaMemcpy(matriz2_device, matriz2_nexo, LADO * sizeof(float*),cudaMemcpyHostToDevice);
	//printf("copia de cpu a gpu array de punteros matriz2 Run Kernel: %s \n", cudaGetErrorString(err1));
	cudaMemcpy(matrizResultado_device, matrizResultado_nexo, LADO * sizeof(float*),cudaMemcpyHostToDevice);

	//Operaciones en GPU:
	// tamBloque = 32 porque los kernels proporcionan las instrucciones en warps (32 threads)
	//entonces tiene que ser multiplo de 32 para no despediciar threads.
	// dimensionGrid -> (LADO / tamBloque) + 1 para calcular el numero de bloques para la x y para y 1 para la z
	// dimensionBlock -> numero de threads por cada bloque (32*32 = 1024 threads)
	// https://codeyarns.com/2011/02/16/cuda-dim3/
	// http://www.icl.utk.edu/~mgates3/docs/cuda.html
	int tamBloque = 32;

	dim3 dimensionGrid = dim3((int)(LADO / tamBloque) + 1, (int)(LADO / tamBloque) + 1, 1);
	dim3 dimensionBlock = dim3(tamBloque, tamBloque, 1);

	//printf("Antes de multiplicar\n");
	//hace la multiplicacion en GPU
	kernel_multiplicarMatrices <<<dimensionGrid,dimensionBlock>>>(LADO, matriz1_device, matriz2_device, matrizResultado_device);

	//Para que espere hasta que todos los threads terminen (CUDA THREADS SYNCRONIZE)
	cudaError_t error = cudaDeviceSynchronize();
	//printf("Thread synchronization: %s \n", cudaGetErrorString(error));

	//pasamos el resultado de device al host
	for(int i = 0; i < LADO; i++){
		err1 = cudaMemcpy(matrizResultado_host[i], matrizResultado_nexo[i], LADO * sizeof(float),cudaMemcpyDeviceToHost);
		//printf("copia de gpu a cpu final Run Kernel: %s \n", cudaGetErrorString(err1));
	}

	//imprime la matriz resultado na vez copiada al host
	//printf("El resultado es: \n");
	imprimirMatriz(matrizResultado_host);

	//LIBERACION DE MEMORIA DE CPU Y INTERMEDIA
	for(int i = 0; i < LADO; i++){
		//CPU
		free(matriz1_host[i]);
		free(matriz2_host[i]);
		free(matrizResultado_host[i]);

		//Intermedia
		cudaFree(matriz1_nexo[i]);
		cudaFree(matriz2_nexo[i]);
		cudaFree(matrizResultado_nexo[i]);
	}

	//liberacion del array de punteros
	free(matriz1_host);
	free(matriz2_host);
	free(matrizResultado_host);

	free(matriz1_nexo);
	free(matriz2_nexo);
	free(matrizResultado_nexo);

	//free GPU
	cudaFree(matriz1_device);
	cudaFree(matriz2_device);
	cudaFree(matrizResultado_device);

}
