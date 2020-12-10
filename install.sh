#! /bin/bash

if [ "$#" -ne 1 ]; 
then
	echo "Illegal number of arguments"
	exit
fi

name="$1"

cd Build

cmake ..

make

mv main "${name}"

mv "${name}" /usr/bin
 
