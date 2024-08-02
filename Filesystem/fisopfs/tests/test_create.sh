#!/bin/bash

echo "----- PRUEBA DE CREACION DE ARCHIVO -----"
echo

echo -e "Se crea archivo con touch: touch archivo"
touch archivo
echo "Lo que se espera que muestre ls: archivo"
echo -e "Se ejecuta ls: \n"
ls

echo -e "\n--- Fin de test creacion de archivo ---"

rm archivo
