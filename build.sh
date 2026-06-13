#!/bin/bash


if [ $1 = "--full" ]; then
	if [ -d "./build" ]; then
		rm -r ./build
	fi

	mkdir ./build

fi
cd ./build
cmake ..
make
cd ..
./build/SensorDatabaseProgram
