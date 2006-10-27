#!/bin/sh
#
# WengoPhone, a voice over Internet phone
# Copyright (C) 2004-2005  Wengo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Finalize the .app folder for WengoPhone on MacOS X.
# This script copies every dependent files and change their install name.
#
# @author Philippe Bernery
# @author Emmanuel Buu

usage()
{
	echo "usage: finalize [NG source path] [mode]"
	echo "mode can be 'debug' or 'release'"
}

##
# Check command line parameters.
##
if [ ! $# -eq 2 ]; then
	usage
	exit -1
fi

if [ $2 = "debug" ]; then
	MODE="debug"
	LIBQTDESIGNER="libQtDesigner_debug.4.dylib"
elif [ $2 = "release" ]; then
	MODE="release-symbols"
	LIBQTDESIGNER="libQtDesigner.4.dylib"
else
	usage
	exit -1
fi

##
# Set needed variables
##
WENGO_BUILD_PATH=$1/$MODE
WENGO_PATH=$WENGO_BUILD_PATH/wengophone/src/presentation/qt
WENGO_APP_PATH=$WENGO_PATH/WengoPhone.app
WENGO_RESOURCES_PATH=$WENGO_APP_PATH/Contents/Resources
WENGO_FRAMEWORK_PATH=$WENGO_APP_PATH/Contents/Frameworks
WENGO_EXE=$WENGO_APP_PATH/Contents/MacOS/WengoPhone
WENGO_FRAMEWORK_PREFIX=@executable_path/../Frameworks

EXTERN_LIB_PATH=/opt/local/lib

##
# $1: full path to the lib
##
changeWengoPhoneInstallName()
{
	if [ $# -eq 1 ]; then
		install_name_tool -change $1 $WENGO_FRAMEWORK_PREFIX/`basename $1` $WENGO_EXE
	fi

	if [ $# -eq 2 ]; then
		install_name_tool -change $1 $WENGO_FRAMEWORK_PREFIX/$2 $WENGO_EXE
	fi
}

changeInstallName()
{
	install_name_tool -change $1 $WENGO_FRAMEWORK_PREFIX/`basename $1` $2
}

testAndMkdir()
{

	if [ ! -d $1 ]; then
		mkdir -p $1
	fi
}

# Copy external libs in framework. As their their location may vary 
# depending on the installation tool used, look in the following locations 
# /sw/lib (fink) 
# /opt/local/lib (darwinports) 
# /usr/local/lib (manual compilation) 
# /usr/bin (included in the system) 

EXTLIB_RETURN=""

findExtLib()
{
	if [ -r /opt/local/lib/$1 ] ; then
		#echo found $1 in /opt/local/lib
		EXTLIB_RETURN=/opt/local/lib/$1
	elif [ -r /sw/lib/$1 ] ; then
		#echo found $1 in /sw/lib
		EXTLIB_RETURN=/sw/lib/$1
	elif [ -r /usr/local/lib/$1 ] ; then
		#echo found $1 in /usr/local/lib
		EXTLIB_RETURN=/opt/local/lib/$1
	elif [ -r /usr/lib/$1 ] ; then
		#echo found $1 in /usr/bin. Included in system?
		EXTLIB_RETURN=/usr/lib/$1
	else
		echo ERROR: lib $1 could not be found.
		return 10
	fi
}

##
# Copy the bundle into the desired one
##
echo -n "Copying bundle... "

testAndMkdir $WENGO_APP_PATH
cp -rf $WENGO_PATH/qtwengophone.app/* $WENGO_APP_PATH
mv $WENGO_APP_PATH/Contents/MacOS/qtwengophone $WENGO_EXE

echo " done"

##
# Creates the Info.plist file
##
echo -n "Generating Info.plist... "

#TODO: should get the version number from somewhere
`dirname $0`/generate_info_plist_file.py $WENGO_APP_PATH/Contents "2.0" > /dev/null

echo " done"

##
# Check the MacOS X version
##
OSVERSION=$(uname -r)
if [ $OSVERSION = "7.9.0" ]; then
	# MacOS X 10.3.9
	INTLLIB='libintl.1.dylib'
else
	INTLLIB='libintl.3.dylib'
fi

##
##
# Creates the needed directories if they do not exist
##
echo -n "Creating needed directories... "
testAndMkdir $WENGO_FRAMEWORK_PATH
testAndMkdir $WENGO_FRAMEWORK_PATH/QtCore.framework/Versions/4.0
testAndMkdir $WENGO_FRAMEWORK_PATH/QtGui.framework/Versions/4.0
testAndMkdir $WENGO_FRAMEWORK_PATH/QtXml.framework/Versions/4.0
testAndMkdir $WENGO_FRAMEWORK_PATH/QtSvg.framework/Versions/4.0
testAndMkdir $WENGO_FRAMEWORK_PATH/qt-plugins/imageformats

testAndMkdir $WENGO_FRAMEWORK_PATH/phapi-plugins

testAndMkdir $WENGO_RESOURCES_PATH

echo " done"

##
# Copy needed frameworks
##
echo -n "Copying needed frameworks... "
cp $QTDIR/lib/QtCore.framework/Versions/4.0/QtCore $WENGO_FRAMEWORK_PATH/QtCore.framework/Versions/4.0/QtCore
cp $QTDIR/lib/QtGui.framework/Versions/4.0/QtGui $WENGO_FRAMEWORK_PATH/QtGui.framework/Versions/4.0/QtGui
cp $QTDIR/lib/QtXml.framework/Versions/4.0/QtXml $WENGO_FRAMEWORK_PATH/QtXml.framework/Versions/4.0/QtXml
cp $QTDIR/lib/QtSvg.framework/Versions/4.0/QtSvg $WENGO_FRAMEWORK_PATH/QtSvg.framework/Versions/4.0/QtSvg

cp $QTDIR/lib/$LIBQTDESIGNER $WENGO_FRAMEWORK_PATH/$LIBQTDESIGNER

CRYPT_LIBS="libgnutls.12.dylib libgcrypt.11.dylib libtasn1.2.dylib libpth.14.dylib libgpg-error.0.dylib"
GLIB_LIBS="libgmodule-2.0.0.dylib $INTLLIB libintl.1.dylib libiconv.2.dylib libglib-2.0.0.dylib libgthread-2.0.0.dylib"
for f in $CRYPT_LIBS $GLIB_LIBS
do
	findExtLib $f
	cp $EXTLIB_RETURN $WENGO_FRAMEWORK_PATH
done

echo " done"

##
# Change install name of WengoPhone
##
echo -n "Changing install names..." 
EXTLIBS_DEP="libglib-2.0.0.dylib $INTLLIB libiconv.2.dylib libgthread-2.0.0.dylib libgmodule-2.0.0.dylib libgnutls.12.dylib"
for f in $EXTLIBS_DEP
do
	findExtLib $f
	changeWengoPhoneInstallName	$EXTLIB_RETURN
done

changeWengoPhoneInstallName "$QTDIR/$LIBQTDESIGNER"
changeWengoPhoneInstallName "$QTDIR/lib/QtCore.framework/Versions/4.0/QtCore" "QtCore.framework/Versions/4.0/QtCore"
changeWengoPhoneInstallName "$QTDIR/lib/QtGui.framework/Versions/4.0/QtGui" "QtGui.framework/Versions/4.0/QtGui"
changeWengoPhoneInstallName "$QTDIR/lib/QtXml.framework/Versions/4.0/QtXml" "QtXml.framework/Versions/4.0/QtXml"
changeWengoPhoneInstallName "$QTDIR/lib/QtSvg.framework/Versions/4.0/QtSvg" "QtSvg.framework/Versions/4.0/QtSvg"

##
# Change install name of WengoPhone. Libraries that could be installed by fink but that are available by
# default on Mac OS X.
##
changeWengoPhoneInstallName "/sw/lib/libssl.0.9.7.dylib" "/usr/lib/libssl.dylib"
changeWengoPhoneInstallName "/sw/lib/libcrypto.0.9.7.dylib" "/usr/lib/libcrypto.dylib"
changeWengoPhoneInstallName "/sw/lib/libxml2.2.dylib" "/usr/lib/libxml2.dylib"

##
# Change install name of copied libraries
##
install_name_tool -change "$QTDIR/lib/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PREFIX/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PATH/QtGui.framework/Versions/4.0/QtGui"
install_name_tool -change "$QTDIR/lib/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PREFIX/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PATH/QtXml.framework/Versions/4.0/QtXml"
install_name_tool -change "$QTDIR/lib/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PREFIX/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PATH/QtSvg.framework/Versions/4.0/QtSvg"
install_name_tool -change "$QTDIR/lib/QtXml.framework/Versions/4.0/QtXml" "$WENGO_FRAMEWORK_PREFIX/QtXml.framework/Versions/4.0/QtXml" "$WENGO_FRAMEWORK_PATH/QtSvg.framework/Versions/4.0/QtSvg"
install_name_tool -change "$QTDIR/lib/QtGui.framework/Versions/4.0/QtGui" "$WENGO_FRAMEWORK_PREFIX/QtGui.framework/Versions/4.0/QtGui" "$WENGO_FRAMEWORK_PATH/QtSvg.framework/Versions/4.0/QtSvg"
install_name_tool -change "$QTDIR/lib/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PREFIX/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PATH/$LIBQTDESIGNER"
install_name_tool -change "$QTDIR/lib/QtXml.framework/Versions/4.0/QtXml" "$WENGO_FRAMEWORK_PREFIX/QtXml.framework/Versions/4.0/QtXml" "$WENGO_FRAMEWORK_PATH/$LIBQTDESIGNER"
install_name_tool -change "$QTDIR/lib/QtGui.framework/Versions/4.0/QtGui" "$WENGO_FRAMEWORK_PREFIX/QtGui.framework/Versions/4.0/QtGui" "$WENGO_FRAMEWORK_PATH/$LIBQTDESIGNER"

findExtLib libglib-2.0.0.dylib
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgmodule-2.0.0.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgthread-2.0.0.dylib"

findExtLib libiconv.2.dylib
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgmodule-2.0.0.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgthread-2.0.0.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libglib-2.0.0.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/$INTLLIB"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libintl.1.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgpg-error.0.dylib"

findExtLib $INTLLIB
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgmodule-2.0.0.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgthread-2.0.0.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libglib-2.0.0.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgpg-error.0.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"

findExtLib libintl.1.dylib
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgpg-error.0.dylib"

findExtLib libpth.14.dylib
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"

findExtLib libgpg-error.0.dylib
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"

findExtLib libtasn1.2.dylib
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"

findExtLib libgcrypt.11.dylib
changeInstallName $EXTLIB_RETURN "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"

echo " done"

##
# Copy resources files
##
echo -n "Copying resources files... "

cp -r $WENGO_BUILD_PATH/sounds $WENGO_RESOURCES_PATH/
cp -r $WENGO_BUILD_PATH/emoticons $WENGO_RESOURCES_PATH/
cp -r $WENGO_BUILD_PATH/pics $WENGO_RESOURCES_PATH/

echo " done"

##
# Copy QT Plugins
##
echo -n "Copying Qt plugins... "

cp -r $QTDIR/plugins/imageformats/libqmng.dylib $WENGO_FRAMEWORK_PATH/qt-plugins/imageformats/libqmng.dylib
install_name_tool -change "$QTDIR/lib/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PREFIX/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PATH/qt-plugins/imageformats/libqmng.dylib"
install_name_tool -change "$QTDIR/lib/QtGui.framework/Versions/4.0/QtGui" "$WENGO_FRAMEWORK_PREFIX/QtGui.framework/Versions/4.0/QtGui" "$WENGO_FRAMEWORK_PATH/qt-plugins/imageformats/libqmng.dylib"

echo " done"

##
# Copy phApi plugins
##
echo -n "Copying PhApi plugins... "

cp $WENGO_BUILD_PATH/phspeexplugin.dylib $WENGO_BUILD_PATH/phamrplugin.dylib $WENGO_FRAMEWORK_PATH/phapi-plugins

echo " done"

##
# Languages
##
echo -n "Creating language directories... "

cp -r $WENGO_PATH/lang $WENGO_RESOURCES_PATH/
for f in $(ls $WENGO_RESOURCES_PATH/lang/*.qm | sed "s/.*_\([^\.]*\)\.qm/\1/");
do
	DIR=$WENGO_RESOURCES_PATH/$f.lproj
	if [ ! -d $DIR ]; then
		mkdir $DIR
	fi
done

echo " done"

echo "Finalize script finished"
