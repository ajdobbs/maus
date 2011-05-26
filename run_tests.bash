#!/bin/bash
set -e

echo "Your current directory is:"
pwd
echo
echo "Your MAUS_ROOT_DIR is:"
echo ${MAUS_ROOT_DIR}
echo
echo "These should agree"

nosetests --with-coverage build

# generated by: find bin -type f -executable | grep -v \~
#bin/load_test.py
#bin/simulate_mice.py