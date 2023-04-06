Delete "$INSTDIR\softphone.exe"
Delete "$INSTDIR\wengophone.exe"
Delete "$INSTDIR\playsound.exe"
Delete "$INSTDIR\download.exe"
Delete "$INSTDIR\memorydump.exe"
Delete "$INSTDIR\*.dll"
Delete "$INSTDIR\styles\*.dll"
Delete "$INSTDIR\sounds\*"
Delete "$INSTDIR\sounds\dtmf\*"
Delete "$INSTDIR\sounds\dtmf\jungle\*"
Delete "$INSTDIR\lang\*"

/**
 * Debug or release mode.
 */
!ifdef DEBUG
	!include "files_install_debug.nsi"
!else
	!include "files_install_release.nsi"
!endif

SetOutPath "$INSTDIR"
File "${BUILD_DIR}\softphone\gui\wengophone.exe"
CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\wengophone.exe"
CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\wengophone.exe"
File "${BUILD_DIR}\softphone\runtime\COPYING"
File "${BUILD_DIR}\softphone\runtime\licence-fr.txt"
File "${BUILD_DIR}\softphone\runtime\readme-fr.txt"
File "${BUILD_DIR}\softphone\runtime\index-fr.html"
File "${BUILD_DIR}\softphone\runtime\YLTELBOX.dll"
File "${BUILD_DIR}\softphone\runtime\wengocurl.dll"
File "${BUILD_DIR}\softphone\runtime\mgwz.dll"
File "${BUILD_DIR}\softphone\runtime\avcodec.dll"
File "${BUILD_DIR}\softphone\runtime\portaudio.dll"
File "${BUILD_DIR}\libs\wengokeramik\kdefx\kdefx.dll"
File "${BUILD_DIR}\softphone\download\download.exe"
File "${BUILD_DIR}\wifo\exosip\phapi\phapi.dll"
File "${BUILD_DIR}\libs\webcam\webcam.dll"

SetOutPath "$INSTDIR\sounds\"
File "${BUILD_DIR}\softphone\runtime\sounds\*"

SetOutPath "$INSTDIR\sounds\dtmf\"
File "${BUILD_DIR}\softphone\runtime\sounds\dtmf\*"

SetOutPath "$INSTDIR\sounds\dtmf\jungle\"
File "${BUILD_DIR}\softphone\runtime\sounds\dtmf\jungle\*"

SetOutPath "$INSTDIR\emoticons\default\"
File "${BUILD_DIR}\softphone\runtime\emoticons\default\*"

SetOutPath "$INSTDIR\lang\"
File "${BUILD_DIR}\softphone\gui\lang\wengo_fr.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_en.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_es.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_de.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_fi.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_pl.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_it.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_ja.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_zh.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_cs.qm"
File "${BUILD_DIR}\softphone\gui\lang\wengo_nl.qm"
File "${BUILD_DIR}\softphone\gui\lang\*.ts"

SetOutPath "$INSTDIR\styles\"
File "${BUILD_DIR}\libs\wengokeramik\thinkeramik\wengokeramikstyle2.dll"

SetOutPath "$INSTDIR\icons\"
File "${BUILD_DIR}\softphone\runtime\icons\homepage.png"

SetOutPath "$INSTDIR\connecting\"
File "${BUILD_DIR}\softphone\runtime\connecting\*"

/**
 * Copies the default configuration of the softphone
 * into the home directory of the user.
 * Does not overwrite the files from this directory.
 */
!define WENGODIR "$PROFILE\wengo"
SetOutPath "${WENGODIR}"
SetOverwrite off
CreateDirectory "$PROFILE"

; Tests if the file exists because SetOverwrite does
; not work with FileWrite from the function writeToFile
IfFileExists "wengo.config" 0 createFile
	Goto done
createFile:
	StrCmp $LANGUAGE ${LANG_FRENCH} french default
	french:
		Push "<Wengo><style>WengoKeramik2</style><audio><input><name></name></input><output><name></name></output><ringing><name></name></ringing></audio><lang>wengo_fr.qm</lang></Wengo>"
		Goto writeFile
	default: ; Englis
		Push "<Wengo><style>WengoKeramik2</style><audio><input><name></name></input><output><name></name></output><ringing><name></name></ringing></audio><lang>wengo_en.qm</lang></Wengo>"
		Goto writeFile

writeFile:
	Push "wengo.config"
	Call writeToFile
done:
SetOverwrite on

/**
 * So that we can execute wengophone.exe
 */
SetOutPath "$INSTDIR"
