#!/bin/sh

DEFAULT_GENERATIONS
DEFAULT_WORLD_SIZE

if [ "$#" -eq 0 ]; then
	WORLD_SIZE=$DEFAULT_WORLD_SIZE
	GENERATIONS=$DEFAULT_GENERATIONS
elif [ "$#" -eq 2 ]; then
	WORLD_SIZE=$1
	GENERATIONS=$2
else
    echo "usage: go.sh <world size> <generations>"
    exit 1
fi


clear
figlet make
make clean
make
figlet run it
./gameoflife $WORLD_SIZE $GENERATIONS
echo
echo
echo

