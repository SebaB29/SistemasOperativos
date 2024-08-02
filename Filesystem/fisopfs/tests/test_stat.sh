#!/bin/bash


echo -e "----- PRUEBA DE ESTADISTICAS DE ARCHIVO -----\n"

echo "Se crea un archivo con contenido: echo hello world >> archivo"
echo hello world >> archivo

echo -e "Se ejecuta: stat archivo\n"

stat archivo

echo -e "\n--- Fin de test estadistica de archivo ---"

rm archivo
