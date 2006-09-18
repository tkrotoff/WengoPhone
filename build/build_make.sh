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

case $1 in
	configure)
		DOT=$(which dot)
		if [ -n "${DOT}" ]; then
			cmake -DCMAKE_INSTALL_PREFIX=/usr --graphviz=${BUILDDIR}/wengophone.dot "$@" ..
			${DOT} -Tpng -o${BUILDDIR}/wengophone.png ${BUILDDIR}/wengophone.dot
			${DOT} -Tsvg -o${BUILDDIR}/wengophone.svg ${BUILDDIR}/wengophone.dot
		else
			cmake -DCMAKE_INSTALL_PREFIX=/usr "$@" ..
		fi
	;;
	make)
		cmake -DCMAKE_INSTALL_PREFIX=/usr "$@" ..
		make
	;;
	*)
		echo "Usage: $(basename $0) (configure|make|graph)"
		echo
		echo "  configure - run cmake configure"
		echo "  make - run configure and build wengophone"
		echo
	;;
esac

