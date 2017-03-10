#include<stdio.h>
#include <stdlib.h>
#include <string.h>

int **crearMatriz(int numFilas, numColumnas){

	int **matriz = (int **)malloc(sizeof(int*)*numFilas);
	int i;
	for(i = 0; i < numFilas, i++){
		matriz[i] = (int*)malloc(sizeof(int)*numColumnas);
	}
	return matriz;
}


void rellenarMatriz(int **matriz, int numFilas, numColumnas, identidad){

	int i, j;
	if(identidad)
	{
		for(i = 0; i < numFilas; i++){
			for(j = 0; j < numColumnas, j++){
				if(i == j) matriz([i][j] = 1;
				else(matriz[i][j] = 0;
		
	}else{
	for(i = 0; i < numFilas; i++){
		for(j = 0; j < numColumnas, j++){
			matriz[i][j] = (int)rand();
	}
}


}
	
	return matriz;
}


void escribirMatriz(int **matriz, int numFilas, numColumnas, nombre){

	int i, j;
	FILE* fich = fopen(nombre, "w");
	if(fich == NULL) {
	printf("Error");
	return ;
	}
	
	char charaux[100];

	sprintf(charaux, "%d %d ", numFilas, numColumnas);
	
	fwrite(charaux, sizeof(char), strlen(charaux), fich);	

	for(i = 0; i < numFilas; i++){
		for(j = 0; j < numColumnas, j++){
			sprintf(charaux, "%d ", matriz[i][j]);
			fwrite(charaux, sizeof(char), strlen(charaux), fich);
	}

	fclose(fich);
}



int main(int argc, char **argv){
//USO PROGRAMA: CREARMATRICES <NUMFILAS> <NUMCOLUMNAS> <IDENTIDAD> <NOMBREFICH>

//Lectura numFilas

	int numFilas = atoi(argv[1]);
//Lectura numColumnas
	int numColumnas = atoi(argv[2]);
	int identidad = atoi(argv[3]);
	char *nombreFich = argv([4]);
//Crear matriz

	int **matriz = crearMatriz(numFilas, numColumnas);
//Rellenar
	rellenarMatriz(numFilas, numColumnas, identidad, nombreFich);
//Escribir

return 0;
}
