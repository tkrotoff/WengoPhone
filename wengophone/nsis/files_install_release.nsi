/** DLL in release mode. */

SetOutPath "$INSTDIR"
File "${BUILD_DIR}\QtCore4.dll"
File "${BUILD_DIR}\QtGui4.dll"
File "${BUILD_DIR}\QtXml4.dll"
File "${BUILD_DIR}\QtSvg4.dll"

File "${BUILD_DIR}\boost_signals-vc71-mt-1_33_1.dll"
File "${BUILD_DIR}\boost_serialization-vc71-mt-1_33_1.dll"
File "${BUILD_DIR}\boost_program_options-vc71-mt-1_33_1.dll"
File "${BUILD_DIR}\boost_regex-vc71-mt-1_33_1.dll"

File "C:\WINDOWS\system32\msvcr71.dll"
File "C:\WINDOWS\system32\msvcp71.dll"

SetOutPath "$INSTDIR\imageformats"
File "${QTDIR}\plugins\imageformats\qmng1.dll"
