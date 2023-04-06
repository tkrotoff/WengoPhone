/** DLL in release mode. */

SetOutPath "$INSTDIR"
File "${QTDIR}\bin\QtCore4.dll"
File "${QTDIR}\bin\QtGui4.dll"
File "${QTDIR}\bin\QtXml4.dll"
File "${QTDIR}\bin\QtSvg4.dll"

File "C:\WINDOWS\system32\msvcr71.dll"
File "C:\WINDOWS\system32\msvcp71.dll"

SetOutPath "$INSTDIR\imageformats"
File "${QTDIR}\plugins\imageformats\qmng1.dll"
File "${QTDIR}\plugins\imageformats\qjpeg1.dll"
File "${QTDIR}\plugins\imageformats\qgif1.dll"
