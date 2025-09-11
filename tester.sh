#!/bin/bash

(cd src/ej1 && make valgrind_abi && echo -e '\033[0;32mTests EJ1 corrieron exitosamente\033[0m')
EJ1_STATUS=$?

(cd src/ej2 && make valgrind_abi && echo -e '\033[0;32mTests EJ2 corrieron exitosamente\033[0m')
EJ2_STATUS=$?

# If either test failed, exit with non-zero
if [ $EJ1_STATUS -ne 0 ] || [ $EJ2_STATUS -ne 0 ]; then
    echo
    echo -e '\033[0;31mAl menos uno de los tests falló\033[0m'
    exit 1
else
    echo
    echo -e '\033[0;32mTodos los tests corrieron exitosamente\033[0m'
    exit 0
fi
