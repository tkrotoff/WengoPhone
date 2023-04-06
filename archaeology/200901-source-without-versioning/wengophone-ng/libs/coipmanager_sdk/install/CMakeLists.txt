include(../CMakeLists-common.txt)

## Create SDK directories
make_directory(${INCLUDE_DIR})
make_directory(${LIB_DIR})
make_directory(${BIN_DIR})
make_directory(${COIPPLUGINS_DIR})
make_directory(${PHAPIPLUGINS_DIR})
####

ow_copy_file(${CMAKE_SOURCE_DIR}/libs/coipmanager_sdk/README ${BUNDLE_DIR})

## Copy CoIpManager SDK documentation (doc directory)
# Copy documentation available on the repository
execute_process(
	COMMAND ${SUBVERSION_SVN_EXECUTABLE} export --force ${CMAKE_SOURCE_DIR}/libs/coipmanager/doc ${BUNDLE_DIR}/doc
)

# Copy documentation generated on the machine
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/libs/coipmanager/doc/javadoc ${BUNDLE_DIR}/doc/javadoc
)
####

## Copy python test script
ow_copy_file(${CMAKE_SOURCE_DIR}/libs/coipmanager_bindings/sip/tests/test_coipmanager.py ${BIN_DIR})
####

## Copy a library in the framework/SDK directory
macro (ow_copy_lib libName)
	ow_copy_dir_recursive(${CMAKE_SOURCE_DIR}/libs/${libName}/include/${libName}/*.h ${INCLUDE_DIR}/${libName})

	if (APPLE)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/${libName}/lib${libName}.dylib ${LIB_DIR})
	endif (APPLE)
	if (WIN32)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/${libName}/${libName}.lib ${LIB_DIR})
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/${libName}/${libName}.dll ${LIB_DIR})
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/${libName}/${libName}.pdb ${LIB_DIR})
	endif (WIN32)
	if (LINUX)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/${libName}/lib${libName}.so ${LIB_DIR})
	endif (LINUX)
endmacro (ow_copy_lib)

set(LIB_LIST
	coipmanager_threaded
	coipmanager
	coipmanager_base
	ssorequest
	webservice
	sipwrapper
	networkdiscovery
	pixertool
	webcam
	sound
	qtcoreutil
	qtevent
)

foreach (lib ${LIB_LIST})
	ow_copy_lib(${lib})
endforeach (lib)
####


## Copy owutil sublibraries in the framework/SDK directory
macro (ow_copy_owutil subLibName)
	if (APPLE)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/owutil/${subLibName}/libow${subLibName}.dylib ${LIB_DIR})
	endif (APPLE)
	if (WIN32)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/owutil/${subLibName}/ow${subLibName}.lib ${LIB_DIR})
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/owutil/${subLibName}/ow${subLibName}.dll ${LIB_DIR})
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/owutil/${subLibName}/ow${subLibName}.pdb ${LIB_DIR})
	endif (WIN32)
	if (LINUX)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/owutil/${subLibName}/libow${subLibName}.so ${LIB_DIR})
	endif (LINUX)
endmacro (ow_copy_owutil)

set(OWUTIL_LIB_LIST
	cutil
	http
	serialization
	serializer
	shlibloader
	system
	settings
	util
	memorydump
)

ow_copy_dir_recursive(${CMAKE_SOURCE_DIR}/libs/owutil/*.h ${INCLUDE_DIR})

foreach (lib ${OWUTIL_LIB_LIST})
	ow_copy_owutil(${lib})
endforeach (lib)
####


## Copy the CoIpManager plugins in the framework/SDK directory
macro (ow_copy_lib2 src libName dst)
	if (UNIX)
		ow_copy_file(${src}/lib${libName}.so ${dst})
	endif (UNIX)
	if (WIN32)
		ow_copy_file(${src}/${libName}.dll ${dst})
		ow_copy_file(${src}/${libName}.pdb ${dst})
	endif (WIN32)
endmacro (ow_copy_lib2)

# CoIpManager plugins
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/purplewrapper purplewrapper ${LIB_DIR})
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/sipwrapper/src/phapi phapiwrapper ${LIB_DIR})
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/initplugin/phapi phapiinitplugin ${COIPPLUGINS_DIR}/initplugin/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/initplugin/libpurple purpleinitplugin ${COIPPLUGINS_DIR}/initplugin/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/connectmanager/sipwrapper phapiconnectmanager ${COIPPLUGINS_DIR}/connectmanager/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/connectmanager/libpurple purpleconnectmanager ${COIPPLUGINS_DIR}/connectmanager/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/callsessionmanager/sipwrapper phapicallsessionmanager ${COIPPLUGINS_DIR}/callsessionmanager/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/chatsessionmanager/phapi phapichatsessionmanager ${COIPPLUGINS_DIR}/chatsessionmanager/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/chatsessionmanager/libpurple purplechatsessionmanager ${COIPPLUGINS_DIR}/chatsessionmanager/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/filesessionmanager/phapi phapifilesessionmanager ${COIPPLUGINS_DIR}/filesessionmanager/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/presencemanager/account/libpurple purpleaccountpresencemanager ${COIPPLUGINS_DIR}/presencemanager/account/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/presencemanager/account/sipwrapper phapiaccountpresencemanager ${COIPPLUGINS_DIR}/presencemanager/account/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/presencemanager/contact/libpurple purplecontactpresencemanager ${COIPPLUGINS_DIR}/presencemanager/contact/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/presencemanager/contact/sipwrapper phapicontactpresencemanager ${COIPPLUGINS_DIR}/presencemanager/contact/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/smssessionmanager/wengo wengosmssessionmanager ${COIPPLUGINS_DIR}/smssessionmanager/.)
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/coipmanager/plugins/syncmanager/contact/libpurple purplecontactsyncmanager ${COIPPLUGINS_DIR}/syncmanager/contact/.)

# PhApi
ow_copy_lib2(${CMAKE_BINARY_DIR}/wifo/owbase owbase ${LIB_DIR})
ow_copy_lib2(${CMAKE_BINARY_DIR}/wifo/owsl owsl ${LIB_DIR})
ow_copy_lib2(${CMAKE_BINARY_DIR}/wifo/phapi phapi ${LIB_DIR})
ow_copy_lib2(${CMAKE_BINARY_DIR}/wifo/phapi-util phapiutil ${LIB_DIR})
ow_copy_lib2(${CMAKE_BINARY_DIR}/wifo/wifo-plugins/sfp-plugin sfp-plugin ${PHAPIPLUGINS_DIR})
ow_copy_lib2(${CMAKE_BINARY_DIR}/wifo/phapi/phamrplugin phamrplugin ${PHAPIPLUGINS_DIR})
ow_copy_lib2(${CMAKE_BINARY_DIR}/wifo/phapi/phspeexplugin phspeexplugin ${PHAPIPLUGINS_DIR})
####

## Other libraries
ow_copy_lib2(${CMAKE_BINARY_DIR}/libs/qtutil owqtutil ${LIB_DIR})
####

## Python binding
set(PYTHON_COMMAND "python")
if (APPLE)
	# On Mac OS X, we want to use Python coming by the system which
	# is 2.3. Some machine could have installed more recent ones.
	set(PYTHON_COMMAND "python2.3")
endif(APPLE)

execute_process(
	COMMAND
		${PYTHON_COMMAND} ${CMAKE_SOURCE_DIR}/libs/coipmanager_sdk/install/copyBindingDependencies.py ${LIB_DIR}
)
if (UNIX)
	ow_copy_file(${CMAKE_BINARY_DIR}/libs/coipmanager_bindings/sip/libPyCoIpManager.so ${LIB_DIR}/PyCoIpManager.so)
endif (UNIX)
if (WIN32)
	ow_copy_file(${CMAKE_BINARY_DIR}/libs/coipmanager_bindings/sip/PyCoIpManager.dll ${LIB_DIR}/PyCoIpManager.pyd)
	ow_copy_file(${CMAKE_BINARY_DIR}/libs/coipmanager_bindings/sip/PyCoIpManager.pdb ${LIB_DIR}/PyCoIpManager.pdb)
endif (WIN32)
####

## Qt
macro (ow_copy_qt_include libName qtLib)
	# Copy Qt into SDK
	# e.g: libName = QtCore, qtLib = /Library/Frameworks/QtCore.framework (on Mac OS X)
	if (APPLE)
		set(FRAMEWORKS_DIR "${BUNDLE_DIR}/Versions/${COIPMANAGER_VERSION}/Frameworks/")
		make_directory(${FRAMEWORKS_DIR}/${libName}.framework)
		execute_process(
			COMMAND cp -fR ${qtLib} ${FRAMEWORKS_DIR}
		)
	endif (APPLE)

	if (WIN32 OR LINUX)
		execute_process(
			COMMAND
				cmake -E copy_directory ${QT_INCLUDE_DIR}/${libName}/ ${INCLUDE_DIR}/${libName}/
		)
	endif (WIN32 OR LINUX)
endmacro (ow_copy_qt_include)

message(STATUS "** QTCORE and PATH: $ENV{QTCORE}, $ENV{PATH}")
find_package(Qt4 REQUIRED)
set(QT_CORE "")
set(QT_GUI "")
set(QT_XML "")
set(QT_SVG "")
string(REGEX REPLACE "\\\\" "/" QTDIR $ENV{QTDIR})

if (CMAKE_BUILD_TYPE MATCHES Debug)
	if (WIN32)
			ow_copy_file(${QTDIR}/bin/QtCored4.dll ${LIB_DIR})
			ow_copy_file(${QTDIR}/lib/QtCored4.lib ${LIB_DIR})
			ow_copy_file(${QTDIR}/bin/QtGuid4.dll ${LIB_DIR})
			ow_copy_file(${QTDIR}/lib/QtGuid4.lib ${LIB_DIR})
			ow_copy_file(${QTDIR}/bin/QtXmld4.dll ${LIB_DIR})
			ow_copy_file(${QTDIR}/lib/QtXmld4.lib ${LIB_DIR})
			ow_copy_file(${QTDIR}/bin/QtSvgd4.dll ${LIB_DIR})
			ow_copy_file(${QTDIR}/lib/QtSvgd4.lib ${LIB_DIR})
	endif (WIN32)

	set(QT_CORE ${QT_QTCORE_LIBRARY_DEBUG})
	set(QT_GUI ${QT_QTGUI_LIBRARY_DEBUG})
	set(QT_XML ${QT_QTXML_LIBRARY_DEBUG})
	set(QT_SVG ${QT_QTSVG_LIBRARY_DEBUG})
else (CMAKE_BUILD_TYPE MATCHES Debug)
	if (WIN32)
			ow_copy_file(${QTDIR}/bin/QtCore4.dll ${LIB_DIR})
			ow_copy_file(${QTDIR}/lib/QtCore4.lib ${LIB_DIR})
			ow_copy_file(${QTDIR}/bin/QtGui4.dll ${LIB_DIR})
			ow_copy_file(${QTDIR}/lib/QtGui4.lib ${LIB_DIR})
			ow_copy_file(${QTDIR}/bin/QtXml4.dll ${LIB_DIR})
			ow_copy_file(${QTDIR}/lib/QtXml4.lib ${LIB_DIR})
			ow_copy_file(${QTDIR}/bin/QtSvg4.dll ${LIB_DIR})
			ow_copy_file(${QTDIR}/lib/QtSvg4.lib ${LIB_DIR})
	endif (WIN32)

	set(QT_CORE ${QT_QTCORE_LIBRARY_RELEASE})
	set(QT_GUI ${QT_QTGUI_LIBRARY_RELEASE})
	set(QT_XML ${QT_QTXML_LIBRARY_RELEASE})
	set(QT_SVG ${QT_QTSVG_LIBRARY_RELEASE})
endif (CMAKE_BUILD_TYPE MATCHES Debug)

ow_copy_qt_include(QtCore ${QT_CORE})
ow_copy_qt_include(QtGui ${QT_GUI})
ow_copy_qt_include(QtXml ${QT_XML})
ow_copy_qt_include(QtSvg ${QT_SVG})
####

## Boost
# Copy headers
# Libraries are copied by copy_missing_libraries.py for Mac and Linux and
# in the Windows install script for Windows.
find_package(Boost REQUIRED)
execute_process(
	COMMAND
		cmake -E copy_directory ${BOOST_INCLUDE_DIRS}/boost/ ${INCLUDE_DIR}/boost/
)
####

## Copy other 3rd party libraries
macro (ow_copy_3rdparty subLibName)
	if (APPLE)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/3rdparty/${subLibName}/lib${subLibName}.dylib ${LIB_DIR})
	endif (APPLE)
	if (WIN32)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/3rdparty/${subLibName}/${subLibName}.lib ${LIB_DIR})
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/3rdparty/${subLibName}/${subLibName}.dll ${LIB_DIR})
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/3rdparty/${subLibName}/${subLibName}.pdb ${LIB_DIR})
	endif (WIN32)
	if (LINUX)
		ow_copy_file(${CMAKE_BINARY_DIR}/libs/3rdparty/${subLibName}/lib${subLibName}.so ${LIB_DIR})
	endif (LINUX)
endmacro (ow_copy_3rdparty)

set(3RDPARTYLIB_LIST
	portaudio
	curl
)

if (NOT WIN32)
	# Special case for windows. See CMakeLists-install-windows.txt
	set(3RDPARTYLIB_LIST
		${3RDPARTYLIB_LIST}
		libpurple
	)
endif (NOT WIN32)

foreach (lib ${3RDPARTYLIB_LIST})
	ow_copy_3rdparty(${lib})
endforeach (lib)
####

## Look for used libraries and copy missing ones in ${LIB_DIR}
if (APPLE OR LINUX)
	message(STATUS "Copying missing libraries...")

	set(SCRIPT
		${CMAKE_SOURCE_DIR}/scripts/copy_missing_libraries.py
	)

	execute_process(
		COMMAND python ${SCRIPT} "${LIB_DIR}"
	)
endif (APPLE OR LINUX)
####

## Platform specific actions
if (APPLE)
	include("CMakeLists-install-macosx.txt")
endif (APPLE)

if (WIN32)
	include("CMakeLists-install-windows.txt")
endif (WIN32)

if (LINUX)
	include("CMakeLists-install-linux.txt")
endif (LINUX)
####

# Install CoIpManager SDK
# Installation for Mac OS X is made in CMakeLists-install-macosx.txt
if (NOT APPLE)
	message(STATUS "Copy CoIpManager SDK to ${CMAKE_INSTALL_PREFIX}")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${BUNDLE_DIR} ${CMAKE_INSTALL_PREFIX}/${COIPMANAGER_BUNDLE_NAME}-${COIPMANAGER_VERSION}
	)
endif (NOT APPLE)
