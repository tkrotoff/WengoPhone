/**
 * DLL in debug mode.
 */

SetOutPath "$INSTDIR"
File "dll\debug\*.dll"
File "dll\msvcr71.dll"
File "${BUILD_DIR}\softphone\memorydump\memorydump.exe"
File "${BUILD_DIR}\softphone\gui\wengophone.pdb"
File "${BUILD_DIR}\wifo\exosip\phapi\phapi.pdb"

SetOutPath "$INSTDIR\styles"
File "dll\styles\*.dll"
