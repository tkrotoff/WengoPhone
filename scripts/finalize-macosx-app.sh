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

##
# Copy the bundle into the desired one
##

testAndMkdir $WENGO_APP_PATH
cp -rf $WENGO_PATH/qtwengophone.app/* $WENGO_APP_PATH
mv $WENGO_APP_PATH/Contents/MacOS/qtwengophone $WENGO_EXE

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
testAndMkdir $WENGO_FRAMEWORK_PATH
testAndMkdir $WENGO_FRAMEWORK_PATH/QtCore.framework/Versions/4.0
testAndMkdir $WENGO_FRAMEWORK_PATH/QtGui.framework/Versions/4.0
testAndMkdir $WENGO_FRAMEWORK_PATH/QtXml.framework/Versions/4.0
testAndMkdir $WENGO_FRAMEWORK_PATH/QtSvg.framework/Versions/4.0
testAndMkdir $WENGO_FRAMEWORK_PATH/qt-plugins/imageformats

testAndMkdir $WENGO_FRAMEWORK_PATH/phapi-plugins

testAndMkdir $WENGO_RESOURCES_PATH

##
# Copy needed frameworks
##
cp $QTDIR/lib/QtCore.framework/Versions/4.0/QtCore $WENGO_FRAMEWORK_PATH/QtCore.framework/Versions/4.0/QtCore
cp $QTDIR/lib/QtGui.framework/Versions/4.0/QtGui $WENGO_FRAMEWORK_PATH/QtGui.framework/Versions/4.0/QtGui
cp $QTDIR/lib/QtXml.framework/Versions/4.0/QtXml $WENGO_FRAMEWORK_PATH/QtXml.framework/Versions/4.0/QtXml
cp $QTDIR/lib/QtSvg.framework/Versions/4.0/QtSvg $WENGO_FRAMEWORK_PATH/QtSvg.framework/Versions/4.0/QtSvg

cp $QTDIR/lib/$LIBQTDESIGNER $WENGO_FRAMEWORK_PATH/$LIBQTDESIGNER
cp /sw/lib/libgnutls.12.dylib /sw/lib/libgcrypt.11.dylib /sw/lib/libtasn1.2.dylib /sw/lib/libpth.14.dylib /sw/lib/libgpg-error.0.dylib $WENGO_FRAMEWORK_PATH
cp /sw/lib/libgmodule-2.0.0.dylib /sw/lib/libintl.1.dylib /sw/lib/libintl.3.dylib /sw/lib/libiconv.2.dylib /sw/lib/libglib-2.0.0.dylib /sw/lib/libgthread-2.0.0.dylib $WENGO_FRAMEWORK_PATH

cp $WENGO_BUILD_PATH/phspeexplugin.dylib $WENGO_BUILD_PATH/libspeex.1.dylib $WENGO_FRAMEWORK_PATH/phapi-plugins

##
# Change install name of qtwengophone
##
changeWengoPhoneInstallName "/sw/lib/libglib-2.0.0.dylib"
changeWengoPhoneInstallName "/sw/lib/$INTLLIB"
changeWengoPhoneInstallName "/sw/lib/libintl.1.dylib"
changeWengoPhoneInstallName "/sw/lib/libiconv.2.dylib"
changeWengoPhoneInstallName "/sw/lib/libgthread-2.0.0.dylib"
changeWengoPhoneInstallName "/sw/lib/libgmodule-2.0.0.dylib"
changeWengoPhoneInstallName "/sw/lib/libgnutls.12.dylib"
changeWengoPhoneInstallName "$QTDIR/$LIBQTDESIGNER"
changeWengoPhoneInstallName "$QTDIR/lib/QtCore.framework/Versions/4.0/QtCore" "QtCore.framework/Versions/4.0/QtCore"
changeWengoPhoneInstallName "$QTDIR/lib/QtGui.framework/Versions/4.0/QtGui" "QtGui.framework/Versions/4.0/QtGui"
changeWengoPhoneInstallName "$QTDIR/lib/QtXml.framework/Versions/4.0/QtXml" "QtXml.framework/Versions/4.0/QtXml"
changeWengoPhoneInstallName "$QTDIR/lib/QtSvg.framework/Versions/4.0/QtSvg" "QtSvg.framework/Versions/4.0/QtSvg"

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

changeInstallName "/sw/lib/libglib-2.0.0.dylib" "$WENGO_FRAMEWORK_PATH/libgmodule-2.0.0.dylib"
changeInstallName "/sw/lib/libiconv.2.dylib" "$WENGO_FRAMEWORK_PATH/libgmodule-2.0.0.dylib"
changeInstallName "/sw/lib/libintl.3.dylib" "$WENGO_FRAMEWORK_PATH/libgmodule-2.0.0.dylib"

changeInstallName "/sw/lib/libglib-2.0.0.dylib" "$WENGO_FRAMEWORK_PATH/libgthread-2.0.0.dylib"
changeInstallName "/sw/lib/libiconv.2.dylib" "$WENGO_FRAMEWORK_PATH/libgthread-2.0.0.dylib"
changeInstallName "/sw/lib/$INTLLIB" "$WENGO_FRAMEWORK_PATH/libgthread-2.0.0.dylib"

changeInstallName "/sw/lib/libiconv.2.dylib" "$WENGO_FRAMEWORK_PATH/libglib-2.0.0.dylib"
changeInstallName "/sw/lib/$INTLLIB" "$WENGO_FRAMEWORK_PATH/libglib-2.0.0.dylib"

changeInstallName "/sw/lib/libiconv.2.dylib" "$WENGO_FRAMEWORK_PATH/$INTLLIB"

changeInstallName "/sw/lib/libiconv.2.dylib" "$WENGO_FRAMEWORK_PATH/libintl.1.dylib"

changeInstallName "/sw/lib/libintl.1.dylib" "$WENGO_FRAMEWORK_PATH/libgpg-error.0.dylib"
changeInstallName "/sw/lib/libiconv.2.dylib" "$WENGO_FRAMEWORK_PATH/libgpg-error.0.dylib"

changeInstallName "/sw/lib/libtasn1.2.dylib" "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"
changeInstallName "/sw/lib/libgcrypt.11.dylib" "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"
changeInstallName "/sw/lib/libpth.14.dylib" "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"
changeInstallName "/sw/lib/$INTLLIB" "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"
changeInstallName "/sw/lib/libiconv.2.dylib" "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"
changeInstallName "/sw/lib/libgpg-error.0.dylib" "$WENGO_FRAMEWORK_PATH/libgnutls.12.dylib"

changeInstallName "/sw/lib/libpth.14.dylib" "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"
changeInstallName "/sw/lib/libgpg-error.0.dylib" "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"
changeInstallName "/sw/lib/libintl.1.dylib" "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"
changeInstallName "/sw/lib/libiconv.2.dylib" "$WENGO_FRAMEWORK_PATH/libgcrypt.11.dylib"

install_name_tool -change "/usr/local/lib/libspeex.1.dylib" "libspeex.1.dylib" "$WENGO_FRAMEWORK_PATH/phapi-plugins/phspeexplugin.dylib"

##
# Copy resources files
##
cp -r $WENGO_BUILD_PATH/sounds $WENGO_RESOURCES_PATH/
cp -r $WENGO_BUILD_PATH/emoticons $WENGO_RESOURCES_PATH/
cp -r $WENGO_BUILD_PATH/pics $WENGO_RESOURCES_PATH/

##
# Copy QT Plugins
##
cp -r $QTDIR/plugins/imageformats/libqmng.dylib $WENGO_FRAMEWORK_PATH/qt-plugins/imageformats/libqmng.dylib
install_name_tool -change "$QTDIR/lib/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PREFIX/QtCore.framework/Versions/4.0/QtCore" "$WENGO_FRAMEWORK_PATH/qt-plugins/imageformats/libqmng.dylib"
install_name_tool -change "$QTDIR/lib/QtGui.framework/Versions/4.0/QtGui" "$WENGO_FRAMEWORK_PREFIX/QtGui.framework/Versions/4.0/QtGui" "$WENGO_FRAMEWORK_PATH/qt-plugins/imageformats/libqmng.dylib"

##
# Languages
##
cp -r $WENGO_PATH/lang $WENGO_RESOURCES_PATH/
for f in $(ls $WENGO_RESOURCES_PATH/lang/*.qm | sed "s/.*_\([^\.]*\)\.qm/\1/");
do
	DIR=$WENGO_RESOURCES_PATH/$f.lproj
	if [ ! -d $DIR ]; then
		mkdir $DIR
	fi
done

