#!/bin/bash

echo -e "----- PRUEBA DE ESCRITURA Y LECTURA DE ARCHIVO -----\n"

echo "Se ejecuta: echo hello world > nuevo_archivo"
echo hello world > archivo

echo -e "Se ejecuta ls:\n"
ls

echo -e "\nSe ejecuta: cat archivo"
echo -e "Se debe mostrar: hello world\n"
cat archivo

echo -e "\nSe ejecuta: echo bye world >> archivo"
echo bye world >> archivo

echo "Se ejecuta: cat archivo"
echo "Se debe mostrar: hello world\n bye world"
echo
cat archivo

echo -e "\n--- Fin de test escritura y lectura de archivo ---"

rm archivo
