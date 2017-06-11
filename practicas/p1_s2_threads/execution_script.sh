#!/bin/bash

#Store current directory
wd=$(pwd)
pwd

if [ ! -e dosmilidentidad ] || [ ! -e dosmilaleatoria ] || [ ! -e cincomilidentidad ] || [ ! -e cincomilaleatoria ] || [ ! -e diezmilaleatoria ] || [ ! -e diezmilidentidad ]; then
	
	# Ve al directorio donde se generan las matrices
	pwd
	cd ..
	pwd
	cd p1_s1/generadorMatrices

	# Genera matrices de 2000x2000, 5000x5000 y 10000x10000
	./generadorMatrices 2000 2000 1 dosmilidentidad
	./generadorMatrices 2000 2000 0 dosmilaleatoria

	./generadorMatrices 5000 5000 1 cincomilidentidad
	./generadorMatrices 5000 5000 0 cincomilaleatoria

	./generadorMatrices 10000 10000 1 diezmilidentidad
	./generadorMatrices 10000 10000 0 diezmilaleatoria

	# Mueve las matrices al directorio de ejecucion
	mv dosmilidentidad dosmilaleatoria cincomilidentidad cincomilaleatoria diezmilaleatoria diezmilidentidad $wd

	#Sube a el directorio de ejecucion
	cd $wd

fi




# Pasa al primer directorio de ejecucion (p1_s2_loco)
cd p1_s2_loco

# Build executable
if [[ -e *.o ]]; then
	make clean
	make
else
	make
fi

# Start execution

# Multiplicar matrices de 2000x2000
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad > res_loco_dosmil.txt && diff $wd/dosmilaleatoria resultado >> res_loco_dosmil.txt

# Multiplicar matrices de 5000x5000
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad > res_loco_cincomil.txt && diff $wd/cincomilaleatoria resultado >> res_loco_cincomil.txt

# Multiplicar matrices de 10000x10000
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad > res_loco_diezmil.txt && diff $wd/diezmilaleatoria resultado >> res_loco_diezmil.txt






# Pasa al segundo directorio de ejecucion (p1_s2_div_estatica)
cd ..
cd p1_s2_div_estatica

# Build executable
if [[ -e *.o ]]; then
	make clean
	make
else
	make
fi

# Start execution with 1, 2, 4 and 8 threads

# Multiplicar matrices de 2000x2000 con 1 thread
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad 1 > res_div_estatica_dosmil_1.txt && diff $wd/dosmilaleatoria resultado >> res_div_estatica_dosmil_1.txt
# Multiplicar matrices de 2000x2000 con 2 threads
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad 2 > res_div_estatica_dosmil_2.txt && diff $wd/dosmilaleatoria resultado >> res_div_estatica_dosmil_2.txt
# Multiplicar matrices de 2000x2000 con 4 threads
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad 4 > res_div_estatica_dosmil_4.txt && diff $wd/dosmilaleatoria resultado >> res_div_estatica_dosmil_4.txt
# Multiplicar matrices de 2000x2000 con 8 threads
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad 8 > res_div_estatica_dosmil_8.txt && diff $wd/dosmilaleatoria resultado >> res_div_estatica_dosmil_8.txt

# Multiplicar matrices de 5000x5000 con 1 thread
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad 1 > res_div_estatica_cincomil_1.txt && diff $wd/cincomilaleatoria resultado >> res_div_estatica_cincomil_1.txt
# Multiplicar matrices de 5000x5000 con 2 thread
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad 2 > res_div_estatica_cincomil_2.txt && diff $wd/cincomilaleatoria resultado >> res_div_estatica_cincomil_2.txt
# Multiplicar matrices de 5000x5000 con 4 thread
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad 4 > res_div_estatica_cincomil_4.txt && diff $wd/cincomilaleatoria resultado >> res_div_estatica_cincomil_4.txt
# Multiplicar matrices de 5000x5000 con 8 thread
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad 8 > res_div_estatica_cincomil_8.txt && diff $wd/cincomilaleatoria resultado >> res_div_estatica_cincomil_8.txt

# Multiplicar matrices de 10000x10000 con 1 thread
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad 1 > res_div_estatica_diezmil_1.txt && diff $wd/diezmilaleatoria resultado >> res_div_estatica_diezmil_1.txt
# Multiplicar matrices de 10000x10000 con 2 thread
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad 2 > res_div_estatica_diezmil_2.txt && diff $wd/diezmilaleatoria resultado >> res_div_estatica_diezmil_2.txt
# Multiplicar matrices de 10000x10000 con 4 thread
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad 4 > res_div_estatica_diezmil_4.txt && diff $wd/diezmilaleatoria resultado >> res_div_estatica_diezmil_4.txt
# Multiplicar matrices de 10000x10000 con 8 thread
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad 8 > res_div_estatica_diezmil_8.txt && diff $wd/diezmilaleatoria resultado >> res_div_estatica_diezmil_8.txt




# Pasa al tercer directorio de ejecucion (p1_s2_balan_carga)
cd ..
cd p1_s2_balan_carga

# Build executable
if [[ -e *.o ]]; then
	make clean
	make
else
	make
fi

# Start execution with 1, 2, 4 and 8 threads y 50 filas por paquete

# Multiplicar matrices de 2000x2000 con 1 thread
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad 1 50 > res_balan_carga_dosmil_1_50.txt && diff $wd/dosmilaleatoria resultado >> res_balan_carga_dosmil_1_50.txt
# Multiplicar matrices de 2000x2000 con 2 threads
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad 2 50 > res_balan_carga_dosmil_2_50.txt && diff $wd/dosmilaleatoria resultado >> res_balan_carga_dosmil_2_50.txt
# Multiplicar matrices de 2000x2000 con 4 threads
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad 4 50 > res_balan_carga_dosmil_4_50.txt && diff $wd/dosmilaleatoria resultado >> res_balan_carga_dosmil_4_50.txt
# Multiplicar matrices de 2000x2000 con 8 threads
./multiplicadorMatrices $wd/dosmilaleatoria $wd/dosmilidentidad 8 50 > res_balan_carga_dosmil_8_50.txt && diff $wd/dosmilaleatoria resultado >> res_balan_carga_dosmil_8_50.txt

# Multiplicar matrices de 5000x5000 con 1 thread
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad 1 50 > res_balan_carga_cincomil_1_50.txt && diff $wd/cincomilaleatoria resultado >> res_balan_carga_cincomil_1_50.txt
# Multiplicar matrices de 5000x5000 con 2 thread
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad 2 50 > res_balan_carga_cincomil_2_50.txt && diff $wd/cincomilaleatoria resultado >> res_balan_carga_cincomil_2_50.txt
# Multiplicar matrices de 5000x5000 con 4 thread
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad 4 50 > res_balan_carga_cincomil_4_50.txt && diff $wd/cincomilaleatoria resultado >> res_balan_carga_cincomil_4_50.txt
# Multiplicar matrices de 5000x5000 con 8 thread
./multiplicadorMatrices $wd/cincomilaleatoria $wd/cincomilidentidad 8 50 > res_balan_carga_cincomil_8_50.txt && diff $wd/cincomilaleatoria resultado >> res_balan_carga_cincomil_8_50.txt

# Multiplicar matrices de 10000x10000 con 1 thread
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad 1 50 > res_balan_carga_diezmil_1_50.txt && diff $wd/diezmilaleatoria resultado >> res_balan_carga_diezmil_1_50.txt
# Multiplicar matrices de 10000x10000 con 2 thread
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad 2 50 > res_balan_carga_diezmil_2_50.txt && diff $wd/diezmilaleatoria resultado >> res_balan_carga_diezmil_2_50.txt
# Multiplicar matrices de 10000x10000 con 4 thread
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad 4 50 > res_balan_carga_diezmil_4_50.txt && diff $wd/diezmilaleatoria resultado >> res_balan_carga_diezmil_4_50.txt
# Multiplicar matrices de 10000x10000 con 8 thread
./multiplicadorMatrices $wd/diezmilaleatoria $wd/diezmilidentidad 8 50 > res_balan_carga_diezmil_8_50.txt && diff $wd/diezmilaleatoria resultado >> res_balan_carga_diezmil_8_50.txt


