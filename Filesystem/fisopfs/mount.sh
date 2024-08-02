#!/bin/bash
make clean
mkdir prueba
make
./fisopfs -f ./prueba
rmdir prueba