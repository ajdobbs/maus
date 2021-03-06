#!/usr/bin/env bash
ver=1.8.0
directory=googletest-release-${ver}
filename=${directory}.tar.gz
download_filename=release-${ver}.tar.gz
url=https://github.com/google/googletest/archive/${download_filename}

echo
echo 'INFO: Installing third party library GTest 1.8.0'
echo '------------------------------------------------'
echo

while [[ $# > 1 ]]
do
key="$1"
case $key in
    -j|--num-threads)
    if expr "$2" : '-\?[0-9]\+$' >/dev/null
    then
        MAUS_NUM_THREADS="$2"
    fi
    shift
    ;;
esac
shift
done
if [ -z "$MAUS_NUM_THREADS" ]; then
  MAUS_NUM_THREADS=1
fi

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
        echo "INFO: Found source archive in 'source' directory"
    else
        echo "INFO: Source archive doesn't exist.  Downloading..."

  wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source/ ${url}
  mv ${MAUS_ROOT_DIR}/third_party/source/${download_filename} ${MAUS_ROOT_DIR}/third_party/source/${filename}

    fi

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
      echo "INFO: Source archive exists."
      echo
      echo "INFO: Checking MD5 checksum (otherwise the file didn't"
      echo "INFO: download properly):"
      echo
      cd ${MAUS_ROOT_DIR}/third_party/source
      md5sum -c ${filename}.md5 || { echo "FATAL: Failed to download:" >&2; echo "FATAL: ${filename}." >&2; echo "FATAL: MD5 checksum failed.">&2; echo "FATAL: Try rerunning this command to redownload, or check" >&2; echo "FATAL: internet connection"  >&2; rm -f ${filename}; exit 1; }
      sleep 1
      echo
      echo "INFO: Unpacking:"
      echo
      rm -Rf ${MAUS_ROOT_DIR}/third_party/source/${directory}
      sleep 1
      tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/source > /dev/null


      echo
      echo "INFO: Configuring"
      echo
      rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
      mkdir ${MAUS_ROOT_DIR}/third_party/build/${directory}
      cd ${MAUS_ROOT_DIR}/third_party/build/${directory}

      gcc_bin=`which gcc`
      gxx_bin=`which g++`
      cmake ${MAUS_ROOT_DIR}/third_party/source/${directory} \
            -DCMAKE_C_COMPILER=${gcc_bin} \
            -DCMAKE_CXX_COMPILER=${gxx_bin} \
            -DCMAKE_INSTALL_PREFIX=${MAUS_ROOT_DIR}/third_party/install

      echo
      echo "INFO: Making"
      echo
      make -j$MAUS_NUM_THREADS
      echo
      echo "INFO: Installing within MAUS's third party directory:"
      echo
      make install
      echo "INFO: The package should be locally build now in your"
      echo "INFO: third_party directory, which the rest of MAUS will"
      echo "INFO: find."
    else
      echo "FATAL: Source archive still doesn't exist.  Please file a bug report with your operating system,">&2
      echo "FATAL: distribution, and any other useful information at:" >&2
      echo "FATAL: " >&2
      echo "FATAL: http://micewww.pp.rl.ac.uk/projects/maus/issues/" >&2
      echo "FATAL:" >&2
      echo "FATAL: Giving up, sorry..." >&2
    	exit 1
    fi

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
