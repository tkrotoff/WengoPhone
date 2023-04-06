
echo "copy the static lib to firefox env..."
cp "..\build\coipxpcom\coipxpcom.lib" "C:\dev\firefox-2.0-source\mozilla\extensions\xulconnection\src"

echo "copy all the libs..."
cp ../build/debug/*.dll /cygdrive/c/dev/wax/trunk/libs/
