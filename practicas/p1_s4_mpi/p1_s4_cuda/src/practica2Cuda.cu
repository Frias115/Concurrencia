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
__device__ float kernel_multiplicarVectores(int lado, float* fila, float* columna){
	if(threadIdx.x == 0 && threadIdx.y == 0){
		//printf("Multiplicando vectores\n");
	}
	float resultadoAux = 0;
	for(int i = 0; i<lado; i++){
		//printf("fila: %.0f columna: %.0f", fila[i], columna[i]);
		resultadoAux += fila[i] * columna[i];
	}

	//printf("resultado: %.0f \n", resultadoAux);


	return resultadoAux;

}

//llamada a la gpu para multiplicar matrices
__global__ void kernel_multiplicarMatrices(int lado, float** matriz1, float** matriz2, float** resultado){
	//printf("estoy multiplicando\n");
	int fila = blockIdx.x * blockDim.y + threadIdx.y;
	int columna = blockIdx.y * blockDim.x + threadIdx.x;


 	//HABRIA QUE HACERLO CON MEMORIA COMPARTIDA PARA QUE LOS ACCESOS SEAN MUCHO MENOS https://devblogs.nvidia.com/parallelforall/using-shared-memory-cuda-cc/
 	//LA IDEA ES PILLAR UNAS CUANTAS FILAS O COLUMNAS Y UTILIZARLAS PARA REALIZAR TODOS LOS CALCULOS, DE MANERA QUE PILLAMOS UN BLOQUE DE X COLUMNAS QUE QUEPA 
 	// EN LOS 40K QUE ACEPTAN 1024 THREADS(32*32) Y ASI UTILIZARLA EN TODOS LOS CALCULOS

 	//TAMBIEN HAY QUE COLOCAR LA PARTE DE CREAR MATRICES EN OTRO LADO

 	//NO ESTARIA MAL TAMPOCO INTENTAR ARREGLARLO PARA QUE FUNCIONE CON NUESTRAS MATRICES ANTERIORES, AUNQUE TAMPOCO ES NECESARIO

 	//HAY QUE VOLVER A HACER TODOS LOS CALCULOS EN LAS OTRAS ENTREGAS PARA DAR BIEN LOS TIEMPOS Y EXPLICAR PORQUE DAN QUE ERA POR LO DEL NUMERO DE CORES DEL
 	//PROCESADOR 

	//control de errores del thread
	if((fila >= lado) || (columna >= lado)){
		//printf("ha ocurrido un error en multiplicacion\n");
		return;
		}

	resultado[fila][columna] = kernel_multiplicarVectores(lado, matriz1[fila], matriz2[columna]);
	//printf("cuda thread %d %d %.0f \n",fila,columna,resultado[fila][columna]);
}

paqueteTrabajo* mainCuda(paqueteTrabajo* paquete){


	DEBUG_TIME_INIT;
	DEBUG_TIME_START;

	//Inicialización de la semilla para los números aleatorios.
	srand(time(NULL));

	//bool leerTraspuesta = true;

	//CARGA E INICIALIZACION DE LAS MATRICES
	//CPU
	float** matriz1_host = (float**)paquete->datosUno;
	float** matriz2_host = (float**)paquete->datosDos;
	float** matrizResultado_host;

	//NEXO (memoria intermedia)
	float** matriz1_nexo;
	float** matriz2_nexo;
	float** matrizResultado_nexo;

	//GPU
	float** matriz1_device;
	float** matriz2_device;
	float** matrizResultado_device;

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

	//Reserva para el resultado del host
	matrizResultado_host = (float**)malloc(size * sizeof(float*));
	for(int i=0; i < size; i++){
		matrizResultado_host[i] = (float*)malloc(size * sizeof(float));
	}

	//Reserva de la memoria intermedia
	matriz1_nexo = (float**)malloc(size * sizeof(float*));
	matriz2_nexo = (float**)malloc(size * sizeof(float*));
	matrizResultado_nexo = (float**)malloc(size * sizeof(float*));

	//Reserva de memoria en GPU
	cudaError_t err1 = cudaMalloc((void**)&matriz1_device, sizeof(float*)* size);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));

	err1 = cudaMalloc((void**)&matriz2_device, sizeof(float*)* size);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));
	err1 = cudaMalloc((void**)&matrizResultado_device, sizeof(float*)* size);
	//printf("Run Kernel: %s \n", cudaGetErrorString(err1));

	//Reserva de memoria para cada uno de los arrays intermedios
	for(int i = 0; i < size; i++){
		err1 = cudaMalloc((void**)&matriz1_nexo[i], sizeof(float)* size);
		//printf("matriz1_nexo Run Kernel: %s \n", cudaGetErrorString(err1));
		err1 = cudaMalloc((void**)&matriz2_nexo[i], sizeof(float)* size);
		//printf("matriz2_nexo Run Kernel: %s \n", cudaGetErrorString(err1));
		cudaMalloc((void**)&(matrizResultado_nexo[i]), sizeof(float)* size);
	}

	//Copia el contenido de los arrays de CPU a los arrays de la matriz intermedia
	for(int i = 0; i < size; i++){
			err1 = cudaMemcpy(matriz1_nexo[i], matriz1_host[i], size * sizeof(float),cudaMemcpyHostToDevice);
			//printf("cudaMemcoy matriz2_host1 a nexo1 Run Kernel: %s \n", cudaGetErrorString(err1));
			err1 = cudaMemcpy(matriz2_nexo[i], matriz2_host[i], size * sizeof(float),cudaMemcpyHostToDevice);
			//printf("cudaMemcoy matriz2_host2 a nexo2 Run Kernel: %s \n", cudaGetErrorString(err1));
	}

	//copia el contenido del array de punteros de CPU a GPU
	err1 = cudaMemcpy(matriz1_device, matriz1_nexo, size * sizeof(float*),cudaMemcpyHostToDevice);
	//printf("copia de cpu a gpu array de punteros matriz1 Run Kernel: %s \n", cudaGetErrorString(err1));
	err1 = cudaMemcpy(matriz2_device, matriz2_nexo, size * sizeof(float*),cudaMemcpyHostToDevice);
	//printf("copia de cpu a gpu array de punteros matriz2 Run Kernel: %s \n", cudaGetErrorString(err1));
	cudaMemcpy(matrizResultado_device, matrizResultado_nexo, size * sizeof(float*),cudaMemcpyHostToDevice);

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
	{
		DEBUG_TIME_INIT;
		DEBUG_TIME_START;

		kernel_multiplicarMatrices <<<dimensionGrid,dimensionBlock>>>(size, matriz1_device, matriz2_device, matrizResultado_device);

		//Para que espere hasta que todos los threads terminen (CUDA THREADS SYNCRONIZE)
		cudaError_t error = cudaDeviceSynchronize();
		printf("Thread synchronization: %s \n", cudaGetErrorString(error));

		//CUDA MEMCHECK SE ASEGURA DE LOS ACCESOS A MEMORIA ESTEN BIEN
		DEBUG_TIME_END;
		DEBUG_PRINT_FINALTIME("Tiempo multiplicarMatrices(): \n\t");

	}

	//pasamos el resultado de device al host
	for(int i = 0; i < size; i++){
		err1 = cudaMemcpy(matrizResultado_host[i], matrizResultado_nexo[i], size * sizeof(float),cudaMemcpyDeviceToHost);
		//printf("copia de gpu a cpu final Run Kernel: %s \n", cudaGetErrorString(err1));
	}

	cudaDeviceSynchronize();

	//imprime la matriz resultado na vez copiada al host
	//printf("El resultado es: \n");
	//guardarMatriz(matrizResultado_host, size);

	paquete->resultado = (int**)matrizResultado_host;

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

	DEBUG_TIME_END;

	DEBUG_PRINT_FINALTIME("Tiempo Total: \n\t");




	return paquete;
	
}