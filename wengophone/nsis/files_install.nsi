Delete "$INSTDIR\qtwengophone.exe"
Delete "$INSTDIR\*.dll"
Delete "$INSTDIR\styles\*.dll"
Delete "$INSTDIR\sounds\tones\default\*"
Delete "$INSTDIR\sounds\tones\jungle\*"
Delete "$INSTDIR\sounds\tones\jungle_svg\*"
Delete "$INSTDIR\sounds\tones\*"
Delete "$INSTDIR\sounds\*"
Delete "$INSTDIR\lang\*"
Delete "$INSTDIR\webpages\*"

SetOutPath "$INSTDIR"
File "${BUILD_DIR}\qtwengophone.exe"
CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\qtwengophone.exe"
CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\qtwengophone.exe"

File "${BUILD_DIR}\COPYING"

/** .dll files. */
File "${BUILD_DIR}\*.dll"
File "dll\dbghelp.dll"

/** .exe files. */
File "${BUILD_DIR}\*.exe"

/** .pdb files. */
!ifdef WITH_PDB
	File "${BUILD_DIR}\*.pdb"
!endif

/** Gaim plugins. */
SetOutPath "$INSTDIR\plugins\"
File "${BUILD_DIR}\plugins\*.dll"

/** Sounds. */
SetOutPath "$INSTDIR\sounds\"
File "${BUILD_DIR}\sounds\*"
SetOutPath "$INSTDIR\sounds\tones\default\"
File "${BUILD_DIR}\sounds\tones\default\*"
SetOutPath "$INSTDIR\sounds\tones\jungle\"
File "${BUILD_DIR}\sounds\tones\jungle\*"
SetOutPath "$INSTDIR\sounds\tones\jungle_svg\"
File "${BUILD_DIR}\sounds\tones\jungle_svg\*"

/** Icons. */
SetOutPath "$INSTDIR\emoticons\*"
File "${BUILD_DIR}\emoticons\*"

/** Avatars. */
SetOutPath "$INSTDIR\pics\avatars\*"
File "${BUILD_DIR}\pics\avatars\*"

/* Translations */
SetOutPath "$INSTDIR\lang\"
File "${BUILD_DIR}\lang\*.qm"

/** Web pages. */
SetOutPath "$INSTDIR\webpages\windows\"
File "${BUILD_DIR}\webpages\windows\*"

/**
 * So that we can execute wengophone.exe
 */
SetOutPath "$INSTDIR"
