#include<stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct matriz_t{
	int numFilas;
	int numColumnas;
	int **datos;
}matriz_t;


int **crearMatriz(int numFilas, int numColumnas){

	int **matriz = (int **)malloc(sizeof(int*)*numFilas);
	int i;
	for(i = 0; i < numFilas; i++){
		matriz[i] = (int*)malloc(sizeof(int)*numColumnas);
	}
	return matriz;
}


//Ineficiente --> en binario, no hace falta parsear los datos.
int leeDato(FILE* fich){
	char dato [100];
	char datoLeido;
	int contador = 0;
	do{
		if(!feof(fich)){
			fread(&datoLeido, sizeof(char), 1, fich);
			if(datoLeido != ' '){
				dato[contador] = datoLeido;
				contador++;
			}
		}else datoLeido = ' ';
		
		
	}while(datoLeido != ' ');
	dato[contador] = '\0';
	int datoInteger = atoi(dato);
	return datoInteger;
}

//LeerMatriz
matriz_t LeerMatriz(char *nombreFichero, int traspuesta){
	matriz_t matriz;
	int i, j;
	int numFilas, numColumnas;

	FILE* fichero = fopen(nombreFichero, "r");
	if(fichero == NULL){
		printf("Error\n");
		exit(0);
	}
	numFilas = leeDato(fichero);
	numColumnas = leeDato(fichero);
	matriz.datos = crearMatriz(numFilas, numColumnas);

	if(!traspuesta){
		for (int i = 0; i < numFilas; ++i)
		{
			for (int i = 0; i < numColumnas; ++i)
			{
				matriz.datos[i][j] = leeDato(fichero);
			}
		}
		return matriz;
	}else{
		for (int i = 0; i < numFilas; ++i)
		{
			for (int i = 0; i < numColumnas; ++i)
			{
				matriz.datos[j][i] = leeDato(fichero);
			}
		}
	}

}


int multiplicaVectores(int *v1, int *v2, int size){
	int result = 0;
	int i;
	for (i = 0; i < size; ++i)
	{
		result+= v1[i] * v2[i];
	}
	return result;
}


void escribirMatriz(int **matriz, int numFilas, int numColumnas, char *nombre){

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
		for(j = 0; j < numColumnas; j++){
			sprintf(charaux, "%d ", matriz[i][j]);
			fwrite(charaux, sizeof(char), strlen(charaux), fich);
	}

	fclose(fich);
}




//MultiplicarMatriz
void multiplicaMatrices(matriz_t m1, matriz_t m2, matriz_t mres){
	int i, j;
	for (i = 0; i < m1.numFilas; ++i)
	{
		for (j = 0; j < m2.numColumnas; ++j)
		{
			mres.datos[i][j] = multiplicaVectores(m1.datos[i], m2.datos[j], m2.numColumnas);
		}
	}
}
//escribirMatriz 

void imprimeMatriz(matriz_t m){
	int i, j;
	for (i = 0; i < numFilas; ++i)
	{
		for (j = 0; j < numColumnas; ++j)
		{
			printf("%d ", m.datos[i][j]);
		}
	}printf("\n");
}

int main(int argc, char **argv){
//USO PROGRAMA: multiplicarMatricesSec <MATRIZ1> <MATRIZ2> <MATRIZRESULTADO>

	matriz_t m1, m2, mres;

//Carga de datos
	m1 = LeerMatriz(argv[1], 0);
	m1 = LeerMatriz(argv[2], 1);

//Reserva de resultado 
	mres.numFilas = m1.numFilas;
	mres.numColumnas = m2.numColumnas;
	mres.datos = crearMatriz(mres.numFilas, mres.numColumnas);

//Multiplicamos matrices
	multiplicaMatrices(m1, m2, mres);

	escribirMatriz(mres.datos, mres.numFilas, mres.numColumnas, argv[3]);

	imprimeMatriz(mres);

	return 0;
}
