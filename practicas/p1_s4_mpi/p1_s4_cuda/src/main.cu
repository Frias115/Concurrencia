#include "debug_time.h"
#include "/usr/include/openmpi/mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <complex>

using namespace std;

int NUM_THREADS = 1;

struct paqueteTrabajo {
	int filaInicial;
	int numeroRealFilasACalcular;
	int numeroRealColumnasACalcular;
    int **datosUno;
    int **datosDos;
    int **resultado;
};

paqueteTrabajo* mainCuda(paqueteTrabajo* paquete);
__host__ __device__ float kernel_multiplicarVectores(int lado, float* fila, float* columna);
__global__ void kernel_multiplicarMatrices(int lado, float** matriz1, float** matriz2, float** resultado);

class Matriz{

public:
	int numFilas;
	int numColumnas;
	int **datos;

	Matriz(int numFilas, int numColumnas);

	Matriz(string nombre, bool leerTraspuesta);

	void guardarMatriz(string nombre);

	void imprimirMatriz();

	void reservarMemoria();

	Matriz *multiplicarMatrices(Matriz *segundaMatriz, int numeroThreads, int numeroEsclavos);

	void enviarDatosAEsclavo(int parteMatriz, Matriz *segundaMatriz, int numeroEsclavos);

	~Matriz();
};

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

//Añadir parametros necesarios para leer (m origen y nombre de matriz resultado)
void maestro(int rank, int nproc, Matriz *m1, Matriz *m2, int numeroThreads){

	int source = rank;

	DEBUG_TIME_INIT;
	DEBUG_TIME_START;

	{
		DEBUG_TIME_INIT;
		DEBUG_TIME_START;
		//dividir matriz // crear paquetes de trabajo
			//por cada esclavo
			//enviar submatriz
			//por cada esclavo
		//recibir resultados

		//escribir resultado
		cout << "Ejecuto multiplicarMatrices" << endl;
		Matriz *resultado = m1->multiplicarMatrices(m2, numeroThreads, nproc);
		cout << "Acaba multiplicarMatrices" << endl;
		DEBUG_TIME_END;
		DEBUG_PRINT_FINALTIME("Tiempo multiplicarMatrices(): \n\t");
		resultado->guardarMatriz("resultado");
	}

	DEBUG_TIME_END;

	DEBUG_PRINT_FINALTIME("Tiempo Total: \n\t");
}

void esclavo(int rank, int nproc){

	int dest = rank;
	MPI_Status status;

	paqueteTrabajo *paquete = (paqueteTrabajo *)malloc(sizeof(paqueteTrabajo));
	int numeroThreads = 0;
	int filaInicial = 0;
	int numeroRealFilasACalcular = 0;
	int numeroRealColumnasACalcular = 0;

	//Recibe la informacion 
	cout << rank << " -> Espera a los datos" << endl;
	MPI_Recv(&numeroThreads, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&filaInicial, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&numeroRealFilasACalcular, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&numeroRealColumnasACalcular, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);

	Matriz *matriz1 = new Matriz(numeroRealFilasACalcular, numeroRealColumnasACalcular);
	Matriz *matriz2 = new Matriz(numeroRealColumnasACalcular, numeroRealColumnasACalcular); 
	Matriz *resultado = new Matriz(numeroRealColumnasACalcular, numeroRealColumnasACalcular);

	for (int i = 0; i < numeroRealFilasACalcular; ++i){
		MPI_Recv(matriz1->datos[i], numeroRealColumnasACalcular, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	}

	for (int i = 0; i < numeroRealColumnasACalcular; ++i){
		MPI_Recv(matriz2->datos[i], numeroRealColumnasACalcular, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	}

	cout << rank << " -> Recibe los datos" << endl;

	//Mete en un paquete de trabajo la informacion
	paquete->filaInicial = filaInicial;
	paquete->numeroRealFilasACalcular = numeroRealFilasACalcular;
	paquete->numeroRealColumnasACalcular = numeroRealColumnasACalcular;
	paquete->datosUno = matriz1->datos;
	paquete->datosDos = matriz2->datos;
	paquete->resultado = resultado->datos;

	paqueteTrabajo *paqueteAux;
	//Multiplicar (estatica/dinamica/gpu)
	cout << rank << " -> Multiplica los datos" << endl;
	paqueteAux = mainCuda(paquete);
	cout << rank << " -> Recibe el resultado de multiplicar" << endl;

	//Enviar informacion de vuelta al maestro
	cout << "paqueteAux->filaInicial " << paqueteAux->filaInicial << endl;
	cout << "paqueteAux->numeroRealFilasACalcular" << paqueteAux->numeroRealFilasACalcular << endl;
	cout << rank << " -> Devuelve datos al maestro" << endl;
	MPI_Send(&paqueteAux->filaInicial, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
	MPI_Send(&paqueteAux->numeroRealFilasACalcular, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
	MPI_Send(&paqueteAux->numeroRealColumnasACalcular, sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD);


	for (int i = 0; i < numeroRealColumnasACalcular; i++){
		MPI_Send(paqueteAux->resultado[i], numeroRealColumnasACalcular, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	free(paqueteAux);
	cout << rank << " -> Acaba" << endl;

}

int main(int arg, char **argv){

	//Para c
	//Para compilar: mpicc ejemplo

	//Para c++
	//para compilar: mpiCC ejemplo.cpp -o ejemplo

	//Para ejecutar: mpirun -np 4 (ruta del ejecutable) (parametros)

	//Para ejecutar en cluster: mpirun -np 4 (ruta del ejecutable) -hostfile hostfile.txt

	//Orden parametros: <MATRIZ1> <MATRIZ2> <NUM_THREADS>

	int nproc, rank;
	MPI_Init(&arg, &argv);
	//MPI_COMM_WORLD: Comunicador
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Status status;
	cout << "Leo las matrices" << endl;
	bool leerTraspuesta = true;

	//Hay que enviar la matriz por cachos

	//leer matrices
	Matriz *m1 = new Matriz(argv[1], !leerTraspuesta);
	Matriz *m2 = new Matriz(argv[2], leerTraspuesta);
	int numeroThreads = atoi(argv[3]);

	cout << "Termino de leer las matrices" << endl;

	if(rank == 0){
		cout << rank << " -> Entra al maestro" << endl;
		maestro(rank, nproc, m1, m2, numeroThreads);
	}
	else //if(rank == 1)
	{
		cout << rank << " -> Entra al esclavo" << endl;
		esclavo(rank, nproc);
	}
	MPI_Finalize();
	return 0;
}

/*

si se quejade specified application es que esta mal la ruta


mpirun -np 4 -hostfile hostfile.txt /(ruta del programa)

hostfile.txt:

10.4.0.29
10.4.0.44

*/


/*
 ============================================================================
 Name        : practica2Cuda.cu
 Author      : Ana Casado, Roberto Frias & Sergio Rosello
 Version     : 1.0
 Copyright   : MIT
 Description : Optimizaciones usando GPU
 ============================================================================
 */


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
