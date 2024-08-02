#!/bin/bash

echo -e "----- PRUEBA DE ELIMINAR ARCHIVOS -----\n"

echo -e "Se crean un par de archivos: archivo, archivo2, archivo3"

touch archivo
touch archivo2
touch archivo3

echo -e "Se ejecuta ls: \n"
ls

echo -e "\nSe elimina 'archivo' con unlink"
unlink archivo

echo "Lo que se espera que muestre ls: archivo2 archivo3"
echo -e "Se ejecuta ls: \n"
ls

echo -e "\nSe elimina 'archivo2' con rm"
rm archivo2
echo "Lo que se espera que muestre ls: archivo3"
echo -e "Se ejecuta ls: \n"
ls

echo -e "\n--- Fin de test eliminar archivo --- "

rm archivo3
