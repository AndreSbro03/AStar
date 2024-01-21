#!/bin/sh
set -xe

CFLAGS="-Wall -Wextra"
DFLAGS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11" #-lX11
NAME="A_path_finder"

gcc -o Run run_as_deamon.c
gcc $CFLAGS -o $NAME main.c $DFLAGS

