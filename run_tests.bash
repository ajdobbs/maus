#!/bin/bash
set -e

if [ -z "${MAUS_ROOT_DIR}" ]; then  # see if the variable exists yet
    echo "ERROR: \$MAUS_ROOT_DIR not set"
    exit 1;
fi

echo "Your current directory is:"
pwd
echo
echo "Your MAUS_ROOT_DIR is:"
echo ${MAUS_ROOT_DIR}
echo
echo "These should agree"

# To run lcov:-
# (a) Uncomment lines below
# (b) Uncomment some CCFLAGS in SConstruct

#if which lcov >& /dev/null; then 
#    echo Clearing lcov
#    lcov -q -b ${MAUS_ROOT_DIR} --directory src --zerocounters -q
#fi
nosetests --with-coverage build
#if which lcov >& /dev/null; then 
#    echo Building lcov output
#    lcov  -q -b ${MAUS_ROOT_DIR} --directory src --capture --output-file maus.info
#    genhtml -o doc/coverage/ maus.info
#fi

# generated by: find bin -type f -executable | grep -v \~
#bin/load_test.py
#bin/simulate_mice.py
