#!/bin/bash

export TERM=xterm

SCRIPT="$0"
COUNT=0
while [ -L "${SCRIPT}" ]
do
	SCRIPT=$(readlink ${SCRIPT})
	COUNT=$(expr ${COUNT} + 1)
	if [ ${COUNT} -gt 100 ]; then
		echo "Too many symbolic links"
		exit 1
	fi
done
BUILDDIR=$(dirname ${SCRIPT})

cd ${BUILDDIR}
cmake .. -DCMAKE_INSTALL_PREFIX=/usr "$@" # && make

