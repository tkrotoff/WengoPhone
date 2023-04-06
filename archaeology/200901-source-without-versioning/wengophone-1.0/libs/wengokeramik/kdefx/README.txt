*****KDEFX Library for Win32*****

KDEFX Library for QT 3.3.X under Win32.

The purpose of this was to get the nice looking KDE style under Windows.
This has nothing to do with the KDE/Win32 porting.

Very little changes was done to the original Linux sources except:

-Wherever the original source had '#include "XXXX.moc"' this has been replace into 
'#include "moc_XXXX.cpp"'. This is because the qmake tool automatically generate moc
file using that naming convention and AFAIK there's nothing I can do about this.


I'd like to thanks Jaroslaw Staniek from the Kexi project who gave me very good tips to 
achieve this. You can see the status of the KDE/Win32 porting at this address:

http://wiki.kde.org//tiki-index.php?page=KDElibs+for+win32 

Comments to gael@kelta.net