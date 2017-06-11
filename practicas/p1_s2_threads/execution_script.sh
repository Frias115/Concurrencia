#!/bin/bash

# Generation of binary matrixes
cd /home/sergio/Documents/Concurrencia/Concurrencia/practicas/p1_s1/generadorMatrices

./generadorMatrices 2000 2000 1 dosmilidentidad
./generadorMatrices 2000 2000 0 dosmilaleatoria

./generadorMatrices 5000 5000 1 cincomilidentidad
./generadorMatrices 5000 5000 0 cincomilaleatoria

./generadorMatrices 10000 10000 1 diezmilidentidad
./generadorMatrices 10000 10000 0 diezmilaleatoria

# Moving matrixes from current location to execution location


# Execution of 
