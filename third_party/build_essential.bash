#!/usr/bin/env bash
# one can use 'set -u' to have bash exit whenever a variable
# is found that isn't defined.  I prefer to catch it, alert
# the user about the problem, then exit with an error code.
# This is more dangerous and requires that people be careful
# when using environmental variables
# set -u

# Exit the script if ANY command fails
set -e

if [ -n "${MAUS_ROOT_DIR+x}" ]; then
    # One could do the line below, but the problem is error codes.  Xargs doesn't exit nicely
    # (at least the xargs on most machines) if one of the many commands die.  Not using find
    # also happens to make the code a little more readable

    "${MAUS_ROOT_DIR}/third_party/bash/03python.bash"
    "${MAUS_ROOT_DIR}/third_party/bash/10swig.bash"
    "${MAUS_ROOT_DIR}/third_party/bash/11jsoncpp.bash"
    "${MAUS_ROOT_DIR}/third_party/bash/12setuptools.bash"
    "${MAUS_ROOT_DIR}/third_party/bash/13suds.bash"
else
echo
echo "FATAL: MAUS_ROOT_DIR is not set, which is required to" >&2
echo "FATAL: know where to install this package.  You have two" >&2
echo "FATAL: options:" >&2
echo "FATAL:" >&2
echo "FATAL: 1. Set the MAUS_ROOT_DIR from the command line by" >&2
echo "FATAL: (if XXX is the directory where MAUS is installed):" >&2
echo "FATAL:" >&2
echo "FATAL:        MAUS_ROOT_DIR=XXX ${0}" >&2
echo "FATAL:" >&2
echo "FATAL: 2. Run the './configure' script in the MAUS ROOT" >&2
echo "FATAL: directory, run 'source env.sh' then rerun this" >&2
echo "FATAL: command ">&2
exit 1
fi

