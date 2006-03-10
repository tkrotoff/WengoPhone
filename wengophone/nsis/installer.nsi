/**
 * Global variables, already defined.
 *
 * If you add a global variable, declare it here as commented.
 */
!define PRODUCT_VERSION "0.11"
;!define DEBUG
!define BUILD_DIR "..\..\release-symbols\"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "WengoPhone"

!define PRODUCT_PUBLISHER ${PRODUCT_NAME}
!define PRODUCT_WEB_SITE "http://www.wengo.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\qtwengophone.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "..\src\presentation\qt\win32\wengophone.ico"
!define MUI_UNICON "..\src\presentation\qt\win32\wengophone.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE $(license)

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\qtwengophone.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

; Load license translation
LicenseLangString license ${LANG_ENGLISH} "..\COPYING"
LicenseLangString license ${LANG_FRENCH} "..\COPYING"
LicenseData $(license)

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"

/** Changes the name of the setup depending if we are in debug or release mode. */
!ifdef DEBUG
	OutFile "${PRODUCT_NAME}-setup-dbg-${PRODUCT_VERSION}.exe"
!else
	OutFile "${PRODUCT_NAME}-setup-${PRODUCT_VERSION}.exe"
!endif

!include "isUserAdmin.nsi"
!include "writeToFile.nsi"
Function .onInit
	/** Kills running qtwengophone.exe */
	nsProcess::KillProcess "qtwengophone.exe" .R0

	!insertmacro MUI_LANGDLL_DISPLAY

	Call isUserAdmin
	Pop $R0
	StrCmp $R0 "true" isAdmin
		; Not an admin
		StrCpy $INSTDIR "$DOCUMENTS\${PRODUCT_NAME}"
		goto initDone
	isAdmin:
		; User is admin
		StrCpy $INSTDIR "$PROGRAMFILES\${PRODUCT_NAME}"
	initDone:
FunctionEnd

InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "BaseSection" BaseSection
	!include "files_install.nsi"
SectionEnd

Section -AdditionalIcons
	WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
	WriteUninstaller "$INSTDIR\uninst.exe"
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\qtwengophone.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\qtwengophone.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
	"Are you sure you want to completely remove ${PRODUCT_NAME} and all of its components?" IDYES +2
	Abort
FunctionEnd

Function un.onUninstSuccess
	HideWindow
	MessageBox MB_ICONINFORMATION|MB_OK "${PRODUCT_NAME} was successfully removed from your computer."
FunctionEnd

Section Uninstall
	!include "files_uninstall.nsi"
SectionEnd
