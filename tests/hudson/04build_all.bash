#!/bin/bash
./configure
source env.sh
scons -c &&  REQUIREALL=1 scons all