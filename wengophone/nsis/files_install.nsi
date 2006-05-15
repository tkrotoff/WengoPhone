Delete "$INSTDIR\qtwengophone.exe"
Delete "$INSTDIR\*.dll"
Delete "$INSTDIR\styles\*.dll"
Delete "$INSTDIR\sounds\tones\jungle\*"
Delete "$INSTDIR\sounds\tones\*"
Delete "$INSTDIR\sounds\*"
Delete "$INSTDIR\lang\*"
Delete "$INSTDIR\webpages\*"

/** Debug or release mode. */
!ifdef DEBUG
	!include "files_install_debug.nsi"
!else
	!include "files_install_release.nsi"
!endif

SetOutPath "$INSTDIR"
File "${BUILD_DIR}\qtwengophone.exe"
CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\qtwengophone.exe"
CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\qtwengophone.exe"

File "${BUILD_DIR}\..\wengophone\COPYING"

/** Gaim. */
File "${BUILD_DIR}\gaim.dll"
File "${BUILD_DIR}\iconv.dll"
File "${BUILD_DIR}\intl.dll"
File "${BUILD_DIR}\nspr4.dll"
File "${BUILD_DIR}\nss3.dll"
File "${BUILD_DIR}\nssckbi.dll"
File "${BUILD_DIR}\plc4.dll"
File "${BUILD_DIR}\plds4.dll"
File "${BUILD_DIR}\softokn3.dll"
File "${BUILD_DIR}\ssl3.dll"
File "${BUILD_DIR}\zlib1.dll"

/** GLib. */
File "${BUILD_DIR}\libglib-2.0-0.dll"
File "${BUILD_DIR}\libgmodule-2.0-0.dll"
File "${BUILD_DIR}\libgthread-2.0-0.dll"

/** PhApi. */
File "${BUILD_DIR}\phapi.dll"
File "${BUILD_DIR}\phspeexplugin.dll"
File "${BUILD_DIR}\phamrplugin.dll"
File "${BUILD_DIR}\owcurl.dll"

/** Wenbox. */
File "${BUILD_DIR}\wenboxplugin.dll"
File "${BUILD_DIR}\YLTELBOX.dll"

/** FFmpeg. */
File "${BUILD_DIR}\avcodec.dll"

/** MemoryDump. */
File "dll\dbghelp.dll"
File "${BUILD_DIR}\qtwengophone.pdb"
File "${BUILD_DIR}\phapi.pdb"
File "${BUILD_DIR}\owcrashreport.exe"

/** Gaim plugins. */
SetOutPath "$INSTDIR\plugins\"
File "${BUILD_DIR}\plugins\gaimrc.dll"
File "${BUILD_DIR}\plugins\history.dll"
File "${BUILD_DIR}\plugins\libgg.dll"
File "${BUILD_DIR}\plugins\libirc.dll"
File "${BUILD_DIR}\plugins\libjabber.dll"
File "${BUILD_DIR}\plugins\libmsn.dll"
File "${BUILD_DIR}\plugins\libnapster.dll"
File "${BUILD_DIR}\plugins\libnovell.dll"
File "${BUILD_DIR}\plugins\liboscar.dll"
File "${BUILD_DIR}\plugins\libsimple.dll"
File "${BUILD_DIR}\plugins\libyahoo.dll"
File "${BUILD_DIR}\plugins\perl.dll"
File "${BUILD_DIR}\plugins\ssl.dll"
File "${BUILD_DIR}\plugins\ssl-nss.dll"
File "${BUILD_DIR}\plugins\statenotify.dll"
File "${BUILD_DIR}\plugins\tcl.dll"

/** Sounds. */
SetOutPath "$INSTDIR\sounds\"
File "${BUILD_DIR}\sounds\*"
SetOutPath "$INSTDIR\sounds\tones\"
File "${BUILD_DIR}\sounds\tones\*"
SetOutPath "$INSTDIR\sounds\tones\jungle\"
File "${BUILD_DIR}\sounds\tones\jungle\*"

/** Icons. */
SetOutPath "$INSTDIR\emoticons\*"
File "${BUILD_DIR}\emoticons\*"

/** Avatars. */
SetOutPath "$INSTDIR\pics\avatars\*"
File "${BUILD_DIR}\pics\avatars\*"

/* Translations */
SetOutPath "$INSTDIR\lang\"
File "${BUILD_DIR}\wengophone\src\presentation\qt\lang\qtwengophone_fr.qm"
File "${BUILD_DIR}\wengophone\src\presentation\qt\lang\qtwengophone_en.qm"

/** web pages. */
SetOutPath "$INSTDIR\webpages\*"
File "${BUILD_DIR}\webpages\*"

/*
SetOutPath "$INSTDIR\styles\"
File "dll\styles\*.dll"
*/

/*
SetOutPath "$INSTDIR\styles\"
File "${BUILD_DIR}\libs\wengokeramik\thinkeramik\wengokeramikstyle2.dll"
*/

/*
SetOutPath "$INSTDIR\icons\"
File "${BUILD_DIR}\softphone\runtime\icons\homepage.png"
*/

/*
SetOutPath "$INSTDIR\connecting\"
File "${BUILD_DIR}\softphone\runtime\connecting\*"
*/

/**
 * So that we can execute wengophone.exe
 */
SetOutPath "$INSTDIR"
