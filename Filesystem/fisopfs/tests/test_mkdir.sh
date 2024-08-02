#!/bin/bash

echo -e "----- PRUEBA DE CREACION DE DIRECTORIO -----\n"

echo "Se ejecuta: mkdir carpeta1"
mkdir carpeta1

echo "Lo que se espera que muestre ls: carpeta1"
echo -e "Se ejecuta ls:\n"
ls

echo -e "\n--- Fin de test creacion de directorio ---"

rmdir carpeta1
