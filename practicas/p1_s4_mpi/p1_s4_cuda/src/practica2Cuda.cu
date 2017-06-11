/*
 ============================================================================
 Name        : practica2Cuda.cu
 Author      : Ana Casado, Roberto Frias & Sergio Rosello
 Version     : 1.0
 Copyright   : MIT
 Description : Optimizaciones usando GPU
 ============================================================================
 */

#include "practica2Cuda.h"
//#define LADO 5000 //lado de la matriz
//2249 max extraño

using namespace std;

//lamada de la gpu para multiplicar los vectores
__device__ int kernel_multiplicarVectores(int lado, int* fila, int* columna){
	if(threadIdx.x == 0 && threadIdx.y == 0){
		//printf("Multiplicando vectores\n");
	}
	int resultadoAux = 0;
	for(int i = 0; i<lado; i++){
		//printf("fila: %.0f columna: %.0f", fila[i], columna[i]);
		resultadoAux += fila[i] * columna[i];
	}

	//printf("resultado: %.0f \n", resultadoAux);

	return resultadoAux;

}

//llamada a la gpu para multiplicar matrices
__global__ void kernel_multiplicarMatrices(int lado, int** matriz1, int** matriz2, int** resultado){
	//printf("estoy multiplicando\n");

	int fila = blockIdx.x * blockDim.y + threadIdx.y;
	int columna = blockIdx.y * blockDim.x + threadIdx.x;

	//control de errores del thread
	if((fila >= lado) || (columna >= lado)){
		//printf("ha ocurrido un error en multiplicacion\n");
		return;
		}

	resultado[fila][columna] = kernel_multiplicarVectores(lado, matriz1[fila], matriz2[columna]);
	//printf("cuda thread %d %d %.0f \n",fila,columna,resultado[fila][columna]);

}

paqueteTrabajo* mainCuda(paqueteTrabajo* paquete){

	//Inicialización de la semilla para los números aleatorios.
	srand(time(NULL));

	//bool leerTraspuesta = true;

	//CARGA E INICIALIZACION DE LAS MATRICES
	//CPU
	cout << "Recibo las matrices en CUDA" << endl;
	int** matriz1_host;
	int** matriz2_host;
	int** matrizResultado_host;

	//NEXO (memoria intermedia)
	int** matriz1_nexo;
	int** matriz2_nexo;
	int** matrizResultado_nexo;

	//GPU
	int** matriz1_device;
	int** matriz2_device;
	int** matrizResultado_device;

	int size = paquete->numeroRealColumnasACalcular;

	//leemos de fichero binario
	//leerDatosBin(argv[2], &matriz1_host, leerTraspuesta, size);
	//leerDatosBin("nuevadosmilidentidad.bin", &matriz2_host, leerTraspuesta);

	//IMPRIME LAS MATRICES GENERADAS
	//printf("Se van a generar matrices de %d X %d : \n", LADO, LADO);
	//printf("MATRIZ A: \n\n");
	//imprimirMatriz(matriz1_host);
	//printf("MATRIZ B: \n\n");
	//imprimirMatriz(matriz2_host);

	//se hace la traspuesta de la segunda matriz para poder multiplicarla
	//leerDatosBin(argv[3], &matriz2_host, !leerTraspuesta, size);
	//printf("MATRIZ B traspuesta: \n\n");
	//imprimirMatriz(matriz2_host);

	matriz1_host = (int **)malloc(sizeof(int *) * size);
	for (int i = 0; i < size; i++) {
		matriz1_host[i] = (int *)malloc(sizeof(int) * size);
	}

	matriz1_host = paquete->datosUno;

	matriz2_host = (int **)malloc(sizeof(int *) * size);
	for (int i = 0; i < size; i++) {
		matriz2_host[i] = (int *)malloc(sizeof(int) * size);
	}

	matriz2_host = paquete->datosDos;
	
	//Reserva para el resultado del host
	matrizResultado_host = (int**)malloc(size * sizeof(int*));
	for(int i=0; i < size; i++){
		matrizResultado_host[i] = (int*)malloc(size * sizeof(int));
	}

	//Reserva de la memoria intermedia
	matriz1_nexo = (int**)malloc(size * sizeof(int*));
	matriz2_nexo = (int**)malloc(size * sizeof(int*));
	matrizResultado_nexo = (int**)malloc(size * sizeof(int*));

	//Reserva de memoria en GPU
	cudaError_t err1 = cudaMalloc((void**)&matriz1_device, sizeof(int*)* size);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));

	err1 = cudaMalloc((void**)&matriz2_device, sizeof(int*)* size);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));
	err1 = cudaMalloc((void**)&matrizResultado_device, sizeof(int*)* size);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));


	//Reserva de memoria para cada uno de los arrays intermedios
	for(int i = 0; i < size; i++){
		err1 = cudaMalloc((void**)&matriz1_nexo[i], sizeof(int)* size);
		//printf("matriz1_nexo Run Kernel: %s \n", cudaGetErrorString(err1));
		err1 = cudaMalloc((void**)&matriz2_nexo[i], sizeof(int)* size);
		//printf("matriz2_nexo Run Kernel: %s \n", cudaGetErrorString(err1));
		cudaMalloc((void**)&(matrizResultado_nexo[i]), sizeof(int)* size);
	}

	//Copia el contenido de los arrays de CPU a los arrays de la matriz intermedia
	for(int i = 0; i < size; i++){
		err1 = cudaMemcpy(matriz1_nexo[i], matriz1_host[i], size * sizeof(int),cudaMemcpyHostToDevice);
		//printf("cudaMemcoy matriz2_host1 a nexo1 Run Kernel: %s \n", cudaGetErrorString(err1));
		err1 = cudaMemcpy(matriz2_nexo[i], matriz2_host[i], size * sizeof(int),cudaMemcpyHostToDevice);
		//printf("cudaMemcoy matriz2_host2 a nexo2 Run Kernel: %s \n", cudaGetErrorString(err1));
	}

	//copia el contenido del array de punteros de CPU a GPU
	err1 = cudaMemcpy(matriz1_device, matriz1_nexo, size * sizeof(int*),cudaMemcpyHostToDevice);
	//printf("copia de cpu a gpu array de punteros matriz1 Run Kernel: %s \n", cudaGetErrorString(err1));
	err1 = cudaMemcpy(matriz2_device, matriz2_nexo, size * sizeof(int*),cudaMemcpyHostToDevice);
	//printf("copia de cpu a gpu array de punteros matriz2 Run Kernel: %s \n", cudaGetErrorString(err1));
	cudaMemcpy(matrizResultado_device, matrizResultado_nexo, size * sizeof(int*),cudaMemcpyHostToDevice);

	//Operaciones en GPU:
	// tamBloque = 32 porque los kernels proporcionan las instrucciones en warps (32 threads)
	//entonces tiene que ser multiplo de 32 para no despediciar threads.
	// dimensionGrid -> (LADO / tamBloque) + 1 para calcular el numero de bloques para la x y para y 1 para la z
	// dimensionBlock -> numero de threads por cada bloque (32*32 = 1024 threads)
	// https://codeyarns.com/2011/02/16/cuda-dim3/
	// http://www.icl.utk.edu/~mgates3/docs/cuda.html
	int tamBloque = 32;

	dim3 dimensionGrid = dim3((int)(size / tamBloque) + 1, (int)(size / tamBloque) + 1, 1);
	dim3 dimensionBlock = dim3(tamBloque, tamBloque, 1);

	//printf("Antes de multiplicar\n");
	//hace la multiplicacion en GPU
	kernel_multiplicarMatrices <<<dimensionGrid,dimensionBlock>>>(size, matriz1_device, matriz2_device, matrizResultado_device);

	//Para que espere hasta que todos los threads terminen (CUDA THREADS SYNCRONIZE)
	cudaError_t error = cudaDeviceSynchronize();
	printf("Thread synchronization: %s \n", cudaGetErrorString(error));

	//pasamos el resultado de device al host
	for(int i = 0; i < size; i++){
		err1 = cudaMemcpy(matrizResultado_host[i], matrizResultado_nexo[i], size * sizeof(int),cudaMemcpyDeviceToHost);
		//printf("copia de gpu a cpu final Run Kernel: %s \n", cudaGetErrorString(err1));
	}

	cudaDeviceSynchronize();

	//imprime la matriz resultado na vez copiada al host
	//printf("El resultado es: \n");
	//guardarMatriz(matrizResultado_host, size);

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			paquete->resultado[i][j] = matrizResultado_host[i][j];
		}
	}

	//LIBERACION DE MEMORIA DE CPU Y INTERMEDIA
	for(int i = 0; i < size; i++){
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

	return paquete;
	
}