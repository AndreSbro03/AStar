#!/bin/sh
set -xe

CFLAGS="-Wall -Wextra -O3"
DFLAGS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11" #-lX11
NAME="AStarAlgorithm"

gcc -o RunAsDeamon run_as_deamon.c
gcc $CFLAGS -o $NAME main.c $DFLAGS

