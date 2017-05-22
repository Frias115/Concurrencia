#!/bin/bash

IPS="10.4.0.29 10.4.0.44"
PATH="/home/alumno/profesor"
EXEC_PATH="/home/alumno/ejempliMPI_inicio/ejemplo"

for I in $IPS
do
	/usr/bin/ssh $I mkdir $PATH
	/usr/bin/scp -r 
#	/usr/bin/scp -r
#	/usr/bin/ssh $I

done