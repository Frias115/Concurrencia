/* Aquí ya tenemos las matrices en memoria de la GPU.
Lo del doble puntero en los parámetros es opcional. Podemos hacerlo como queramos.
*/

__global__ void multMatrix(float** m1, float** m2, float** result, int numFilas, int numColumnas){
    // Solo vamos a utilizar el eje X de los threads de CUDA
    int globalId;
    globalId = blockId.x * blockDim.x + threadIdx.x; // Creo que falta algo que quiere que pongamos nosotros
    
    if(globalId > (numFilas * numColumnas)){
        return;
    }

    for(int i = 0; i < numFilas; i++){
        result[i][j] = 0;
        for(int j = 0; j < numColumnas; j++0){
            for(int k = 0; k < numFilas; k++){
                result[i][j] += m1[i][k] * m2[j][k];    // La segunda matriz debe estar transpuesta
            }
        }
    }
}


int main(int argc, char **argv){
    int numFilas = 4;
    int numColumnas = 4;
    // Leemos las matrices
    float m1[4][4] = {{0,1,2,3}, {0,1,2,3}, {0,1,2,3}, {0,1,2,3}};
    float m2[4][4] = {{0,1,2,3}, {0,1,2,3}, {0,1,2,3}, {0,1,2,3}};
    float mres[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};


    // Nos llevamos las matrices desde la CPU a la GPU
    float** d_m1;
    float** d_m2;
    float** d_mres;

    // HostDevice (matrices intermedias)
    float** hd_m1;
    float** hd_m2;
    float** hd_mres;

    hd_m1 = (float**)malloc(sizeof(float*) * numFilas);
    hd_m2 = (float**)malloc(sizeof(float*) * numFilas);
    hd_mres = (float**)malloc(sizeof(float*) * numFilas);

    // Reservamos memoria para las matrices intermedias en la GPU
    for(int i = 0; i < numFilas; i++){
       cudaMalloc(&(hd_m1[i]), sizeof(float*) * numFilas));
       cudaMalloc(&(hd_m2[i]), sizeof(float*) * numFilas));
       cudaMalloc(&(hd_mres[i]), sizeof(float*) * numFilas));
    }

    // Reserva en la GPU
    cudaMalloc(&(d_m1), sizeof(float*) * numFilas);
    cudaMalloc(&(d_m2), sizeof(float*) * numFilas);
    cudaMalloc(&(d_mres), sizeof(float*) * numFilas);

    // Copiamos a la GPU
    cudaMemcpy(d_m1, hd_m1, sizeof(float*) * numFilas, cudaDeviceHostToDevice);
    cudaMemcpy(d_m2, hd_m2, sizeof(float*) * numFilas, cudaDeviceHostToDevice);
    cudaMemcpy(d_mres, hd_mres, sizeof(float*) * numFilas, cudaDeviceHostToDevice);

    // Copiar a la CPU los resultados. No hace falta la matriz resultado porque ahora mismo está vacía.
    for(int i = 0; i < numFilas; i++){
        cudaMemcpy(hd_m1[i], d_m1[i], sizeof(float*) * numFilas, cudaMemcpyHostToDevice);
        cudaMemcpy(hd_m2[i], d_m2[i], sizeof(float*) * numFilas, cudaMemcpyHostToDevice);
    }

    // Número de bloques y threads y lanzar la aplicación. TO-DO.
    int numBlock, numThreads;   // Podríamos usar un máximo de 1024 threads. Con menos threads habrá menos accesos a RAM (y por tanto, mejor).
    // Por ejemplo
    numThreads = 1;
    numBlock = 1;
    multMatrix<<<numBlock, numThreads>>>(d_m1, d_m2, d_mres, numFilas, numColumnas);

    for(int i = 0; i < numFilas; i++){
        cudaMemcpy(mres[i], d_mres[i], sizeof(float*) * numFilas, cudaMemcpyDeviceToHost);
    }

    // Sincronizamos
    cudaDeviceSynchronize();

    for(int i = 0; i < numFilas; i++){
        printf("\n");
        for(int j = 0; j < numColumnas; j++){
            printf("%f ", mres[i][j]);
        }
    }

    getc(stdin);

    // Faltan los cudafree. TO-DO.
}

Como control de errores, comprobar que las matrices van a caber en la GPU.