#!/bin/bash
chmod +x tests/test_create.sh
chmod +x tests/test_eliminar_archivo.sh
chmod +x tests/test_escritura_lectura.sh
chmod +x tests/test_mkdir.sh
chmod +x tests/test_eliminar_dir.sh
chmod +x tests/test_stat.sh

cd prueba
echo -e "\n\n"
echo -e "\e[32m-----------------TESTS-----------------\e[0m"

echo -e "\n\n"
./../tests/test_create.sh
echo -e "\n\n"
./../tests/test_eliminar_archivo.sh
echo -e "\n\n"
./../tests/test_escritura_lectura.sh
echo -e "\n\n"
./../tests/test_mkdir.sh
echo -e "\n\n"
./../tests/test_eliminar_dir.sh
echo -e "\n\n"
./../tests/test_stat.sh
echo -e "\n\n"

echo -e "\e[32m-----------------FIN TESTS-----------------\e[0m"
