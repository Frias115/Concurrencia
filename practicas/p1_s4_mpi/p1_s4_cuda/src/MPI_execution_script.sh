#!/bin/bash

directory=$(pwd)

if [ ! -e milaleatoria ] || [ ! -e milidentidad ] || [ ! -e diezmilaleatoria ] || [ ! -e diezmilidentidad ]; then
	
	# Ve al directorio donde se generan las matrices
	cd ..
	cd ..
	cd ..
	cd p1_s1/generadorMatrices

	# Genera matrices de 1000x1000 y 10000x10000
	./generadorMatrices 1000 1000 0 milaleatoria
	./generadorMatrices 1000 1000 1 milidentidad

	./generadorMatrices 10000 10000 0 diezmilaleatoria
	./generadorMatrices 10000 10000 1 diezmilidentidad

	# Mueve las matrices al directorio de ejecucion
	mv milaleatoria milidentidad diezmilaleatoria diezmilidentidad $directory

	#Sube a el directorio de ejecucion
	cd $directory

fi


# Compila los archivos necesarios para la ejecucion
/usr/local/cuda/bin/nvcc -I/usr/lib/openmpi/include -L/usr/lib/openmpi/lib -c practica2Cuda.cu -o practica2Cuda.o	
mpiCC  -c main.cpp -o main.o
mpiCC  -c matriz.cpp -o matriz.o 
mpiCC *.o -L/usr/local/cuda/lib64  -lcudart -o mulMatrixMPI


# Ejecuta el programa con los distintos argumantos
# optirun mpirun -np 3 mulMatrixMPI dosmilaleatoria dosmilidentidad 4








