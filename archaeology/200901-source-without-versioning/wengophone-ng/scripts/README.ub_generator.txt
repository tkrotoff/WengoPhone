ub_generator.py is a script to generate Universal Binaries of
OpenWengo products (currently CoIpManager and WengoPhone).
It takes a .ini file in entry to configure itself.

## .ini file sample for CoIpManager SDK generation
[general]
projtype: coipsdk
ftpserver: buildbotftpserver
ftplogin: buildbotftplogin
ftppassword: buildbotftppassword
ppcfile: /installer/CoIpManagerSDK/macosx/ppc/trunk/CoIpManager-12414.dmg
x86file: /installer/CoIpManagerSDK/macosx/ppc/trunk/CoIpManager-12414.dmg
revnumber: 12414
ubpath: /installer/CoIpManagerSDK/macosx/universal/trunk
workdir: /tmp
projfile: /path_to_sdk_local_repository/libs/coipmanager_sdk/package/macosx/CoIpManager-SDK-0.2.pmproj

## .ini file sample for WengoPhone
[general]
projtype: wengophone
ftpserver: buildbotftpserver
ftplogin: buildbotftplogin
ftppassword: buildbotftppassword
ppcfile: /installer/NG/macosx/ppc/2.2/WengoPhone-12433.dmg
x86file: /installer/NG/macosx/x86/2.2/WengoPhone-12433.dmg
revnumber: 12433
ubpath: /installer/NG/macosx/universal/2.2/
workdir: /tmp
