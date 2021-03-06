#!/usr/bin/env bash
directory=root_v5.34.36
filename=${directory}.source.tar.gz 
url=https://root.cern.ch/download/${filename}

echo
echo 'INFO: Installing third party library ROOT 5.34.36'
echo '-------------------------------------------------'
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
      wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source ${url}
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
      rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
      mkdir ${MAUS_ROOT_DIR}/third_party/build/${directory}
      sleep 1
      tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/source > /dev/null
      mv ${MAUS_ROOT_DIR}/third_party/source/root ${MAUS_ROOT_DIR}/third_party/source/${directory}


      echo
      echo "INFO: Patching:"
      echo
      cd ${MAUS_ROOT_DIR}/third_party/source/${directory}
      patch -p1 < ${MAUS_ROOT_DIR}/third_party/source/gcc6.patch


      echo
      echo "INFO: Configuring:"
      echo
      cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
      sleep 1
      # Tell cmake explicitly about third_party/install/include, third_party/install/lib and third_party/install/lib64  to look for headers and libs
      sed -i "s@include(MacroEnsureVersion)@include(MacroEnsureVersion)\ninclude_directories(${MAUS_ROOT_DIR}/third_party/install/include)\nlink_directories(${MAUS_ROOT_DIR}/third_party/install/lib ${MAUS_ROOT_DIR}/third_party/install/lib64)@" ${MAUS_ROOT_DIR}/third_party/source/${directory}/CMakeLists.txt

      # Used to force cmake to pick up the correct gcc
      gcc_bin=`which gcc`
      gxx_bin=`which g++`

      cmake ${MAUS_ROOT_DIR}/third_party/source/${directory} \
            -DCMAKE_C_COMPILER=${gcc_bin} \
            -DCMAKE_CXX_COMPILER=${gxx_bin} \
            -DEO_SOURCE_DIR:PATH=${MAUS_ROOT_DIR}/third_party/install/include \
            -Dgsl_shared=ON \
            -DGSL_CONFIG_EXECUTABLE=${MAUS_ROOT_DIR}/third_party/install/bin/gsl-config \
            -Dminuit2=ON \
            -Dbuiltin_freetype=ON \
            -Dx11=ON \
            -Dxrootd=OFF

#       ./configure --disable-xrootd --enable-gsl-shared --enable-minuit2 \
#                 --enable-builtin-freetype \
#                 --with-gsl-incdir=${MAUS_ROOT_DIR}/third_party/install/include \
#                 --with-gsl-libdir=${MAUS_ROOT_DIR}/third_party/install/lib \
#                 --with-x11-libdir=${x11} \
#                 --with-x11-libdir=${x11} \
#                 --with-xft-libdir=${x11} \
#                 --with-xext-libdir=${x11}

      echo
      echo "INFO: Making:"
      echo
      sleep 1
      make -j$MAUS_NUM_THREADS LDFLAGS="-Wl,--no-as-needed" || { echo "FAIL: Failed to configure/make"; exit 1; }
      # Create a link to the ROOT directory not based on version number
      cd ${MAUS_ROOT_DIR}/third_party/build/
      ln -fs ${directory} root

      echo
      echo "INFO: The package should be locally build now in your"
      echo "INFO: third_party directory, which the rest of MAUS will"
      echo "INFO: find."

    else

      echo "FATAL: Source archive still doesn't exist.  Please file a bug report with your operating system,">&2
      echo "FATAL: distribution, and any other useful information at:" >&2
      echo "FATAL: " >&2
      echo "FATAL: http://micewww.pp.rl.ac.uk:8080/projects/maus/issues/" >&2
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

# This is the maximum length of text (ignoring whitespace), 50 chars
##################################################
# this is the maximum amount of characters (including whitespace) per line)
######################################################################
