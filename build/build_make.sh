#!/bin/bash

export TERM=xterm

function configure() {
	cmake "$@" .. || exit 1
}

function compile() {
	CPUCOUNT=$(grep -c processor /proc/cpuinfo)
	if [ "${CPUCOUNT}" -gt "1" ]; then
		make -j${CPUCOUNT} $1
	else
		make $1
	fi
}


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

OPTIONS="-DCMAKE_INSTALL_PREFIX=/usr -DPORTAUDIO_INTERNAL:BOOL=OFF -DFFMPEG_INTERNAL:BOOL=OFF -DSPEEX_INTERNAL:BOOL=OFF --graphviz=${BUILDDIR}/wengophone.dot"

if [ "$(uname -m)" == "x86_64" ]; then
	OPTIONS="${OPTIONS} -DLIB_SUFFIX=64"
fi

case $1 in
	configure)
		shift
		configure ${OPTIONS} "$@"
	;;
	final)
		shift
		OPTIONS="${OPTIONS} -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=0"
		configure ${OPTIONS} "$@"
		compile
	;;
	release)
		shift
		OPTIONS="${OPTIONS} -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=0"
		configure ${OPTIONS} "$@"
		compile
	;;
	debug)
		shift
		OPTIONS="${OPTIONS} -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=0"
		configure ${OPTIONS} "$@"
		compile
	;;
	verbose)
		shift
		OPTIONS="${OPTIONS} -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=1"
		configure ${OPTIONS} "$@"
		compile VERBOSE=1
	;;
	*)
	echo "Usage: $(basename $0) (configure|final|release|debug|verbose)"
		echo
		echo "  configure - run cmake configure"
		echo "  final - run configure and build wengophone in release mode"
		echo "  release - run configure and build wengophone in release with some debug info mode"
		echo "  debug - run configure and build wengophone in debug mode"
		echo "  verbose - run configure and make in verbose mode and build wengophone in debug mode"
		echo
		exit 0
	;;
esac

DOT=$(which dot 2>/dev/null)
if [ -n "${DOT}" ]; then
	${DOT} -Tpng -o${BUILDDIR}/wengophone.png ${BUILDDIR}/wengophone.dot
	${DOT} -Tsvg -o${BUILDDIR}/wengophone.svg ${BUILDDIR}/wengophone.dot
fi

