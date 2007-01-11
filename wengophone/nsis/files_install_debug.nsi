/** DLL in debug mode. */

SetOutPath "$INSTDIR"
File "${BUILD_DIR}\QtCored4.dll"
File "${BUILD_DIR}\QtGuid4.dll"
File "${BUILD_DIR}\QtXmld4.dll"
File "${BUILD_DIR}\QtSvgd4.dll"

File "${BUILD_DIR}\boost_signals-vc71-mt-gd-1_33_1.dll"
File "${BUILD_DIR}\boost_serialization-vc71-mt-gd-1_33_1.dll"
File "${BUILD_DIR}\boost_program_options-vc71-mt-gd-1_33_1.dll"
File "${BUILD_DIR}\boost_regex-vc71-mt-gd-1_33_1.dll"

File "C:\WINDOWS\system32\msvcr71d.dll"
File "C:\WINDOWS\system32\msvcp71d.dll"

SetOutPath "$INSTDIR\imageformats"
File "${QTDIR}\plugins\imageformats\qmngd1.dll"
