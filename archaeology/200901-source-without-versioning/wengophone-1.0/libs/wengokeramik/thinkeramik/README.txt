*****KDE Style for Win32*****

KDE Style for QT 3.3.X under Win32. KDEFX library required.

The purpose of this was to get the nice looking KDE styles under Windows.
This has nothing to do with the KDE/Win32 porting.

Very little changes was done to the original Linux sources except:

-Wherever the original source had '#include "XXXX.moc"' this has been replace into 
'#include "moc_XXXX.cpp"'. This is because the qmake tool automatically generate moc
file using that naming convention and AFAIK there's nothing I can do about this.

-I first tried to compile the styles using QT without RTTI support and I had bad crashes. 
I therefore replaced all the dynamic cast by a simple cast which is not very good but it 
worked. In order to avoid people to recompile their QT library with RTTI support to be able
to build these styles I left the modifications in the code but with conditional defines

E.g.

#ifdef QT_SUPPORT_RTTI
	const QProgressBar *pb = dynamic_cast<const QProgressBar*>(widget);
#else
	const QProgressBar *pb = (const QProgressBar*) widget;
#endif //QT_SUPPORT_RTTI

So If you have QT built with RTTI, I strongly advise you to define QT_SUPPORT_RTTI in each
style's ".pro" file.


I'd like to thanks Jaroslaw Staniek from the Kexi project who gave me very good tips to 
achieve this. You can see the status of the KDE/Win32 porting at this address:

http://wiki.kde.org//tiki-index.php?page=KDElibs+for+win32 

Comments to gael@kelta.net