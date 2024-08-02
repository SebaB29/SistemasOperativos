#!/bin/bash

echo -e "----- PRUEBA DE ELIMINAR DIRECTORIO -----\n"

echo -e "Se crea un directorio: mkdir carpeta_a_eliminar"
mkdir carpeta_a_eliminar

echo -e "Se ejecuta ls:\n"
ls

echo -e "\nSe ejecuta: rmdir carpeta_a_eliminar"
rmdir carpeta_a_eliminar

echo -e "Se ejecuta ls: \n"
ls

echo -e "\n--- Fin de test eliminar directorio ---"
