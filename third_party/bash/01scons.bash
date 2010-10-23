#!/bin/bash
# ROOT

directory=scons-2.0.1
filename=${directory}.tar.gz
url=http://prdownloads.sourceforge.net/scons/${filename}

my_prefix=/install
my_destdir=${MAUS_ROOT_DIR}/third_party

if [ -n "${MAUS_ROOT_DIR+x}" ]; then

    if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
    then
	echo "Source archive exists.  Unpacking:"
	rm -Rf ${MAUS_ROOT_DIR}/third_party/build/${directory}
	sleep 1
	tar xvfz ${MAUS_ROOT_DIR}/third_party/source/${filename} -C ${MAUS_ROOT_DIR}/third_party/build
	cd ${MAUS_ROOT_DIR}/third_party/build/${directory}
	echo "Building:"
	sleep 1
	python setup.py build
	echo "Installing within MAUS's third party directory:"
	sleep 1
	python setup.py install --prefix=${my_prefix} --root=${my_destdir}
	echo "${filename} should be locally build now in your third_party directory, which the rest of MAUS will find."
    else
	echo "Source archive doesn't exist.  Downloading..."

	wget --directory-prefix=${MAUS_ROOT_DIR}/third_party/source ${url}

	echo "These next two strings should agree (otherwise the file didn't download properly):"
	md5sum ${MAUS_ROOT_DIR}/third_party/source/${filename}
	cat ${MAUS_ROOT_DIR}/third_party/md5/${filename}.md5 
	
	echo
	echo "but if they don't agree, then please run:"
	echo
	echo "rm ${MAUS_ROOT_DIR}/third_party/source/${filename}"
	echo 
	echo "then download the file manually and put it in the 'source' directory"
	
	if [ -e "${MAUS_ROOT_DIR}/third_party/source/${filename}" ]
	then
	    echo "Source archive exists now, please rerun this command.  Cheers."
	else
	    echo "Source archive still doesn't exist.  Please file a bug report with your operating system, distribution, and any other useful information at:"
	    echo
	    echo "http://micewww.pp.rl.ac.uk:8080/projects/maus/issues/"
	    echo
	    echo "Giving up, sorry..."
	fi
  
fi

else
echo "MAUS_ROOT_DIR is not set, which is required to know where to install this package.  You have two options:"
echo
echo "1. Set the MAUS_ROOT_DIR from the command line by (if XXX is the directory where MAUS is installed):"
echo "        MAUS_ROOT_DIR=XXX ${0}"
echo
echo "2. Run the './configure' script in the MAUS ROOT directory, run 'source env.sh', then rerun this command"
echo 
fi