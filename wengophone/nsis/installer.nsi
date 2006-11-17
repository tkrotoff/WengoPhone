/**
 * Global variables, already defined.
 *
 * If you add a global variable, declare it here as commented.
 */
;!define PRODUCT_NAME "WengoPhone"
;!define PRODUCT_VERSION "0.13"
;!define DEBUG
;!define BUILD_DIR "..\..\release-symbols\"
;!define INSTALLER_NAME "WengoPhone-setup-0.13.exe"
;!define QTDIR "C:\Qt\4.1.1\"
;!define WITH_PDB


!define PRODUCT_PUBLISHER "Wengo"
!define PRODUCT_WEB_SITE "http://www.wengo.com"
!define PRODUCT_REGKEY "Software\${PRODUCT_NAME}"
!define PRODUCT_UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

; Version information for the installer executable
VIAddVersionKey ProductName "${PRODUCT_NAME}"
VIAddVersionKey ProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey Comments "WengoPhone, a voice over Internet phone"
VIAddVersionKey CompanyName "${PRODUCT_PUBLISHER}"
VIAddVersionKey LegalCopyright "Copyright (C) 2004-2006 Wengo"
VIAddVersionKey LegalTrademarks "Wengo"
VIAddVersionKey FileDescription "${PRODUCT_NAME}"
VIAddVersionKey FileVersion "${PRODUCT_VERSION}"
VIAddVersionKey InternalName "${PRODUCT_NAME}"
VIAddVersionKey OriginalFilename "${INSTALLER_NAME}"
VIProductVersion "0.0.0.0"

SetCompressor lzma

; Modern UI 1.67 compatible
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "..\src\presentation\qt\win32\wengophone.ico"
!define MUI_UNICON "..\src\presentation\qt\win32\wengophone.ico"

; Language Selection Dialog Settings
; Remember the installer language
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_REGKEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE $(license)
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\qtwengophone.exe"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Language files, first language is the default language
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

; Loads license translation
LicenseLangString license ${LANG_ENGLISH} "..\COPYING"
LicenseLangString license ${LANG_FRENCH} "..\COPYING"
LicenseData $(license)

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end


Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"

/** Installer name (e.g. WengoPhone-setup-0.13.exe). */
OutFile "${INSTALLER_NAME}"

!include "nsProcess.nsh"

!include "isUserAdmin.nsi"
!include "isSilent.nsi"
Function .onInit
	/** Kills running qtwengophone.exe */
	${nsProcess::KillProcess} "qtwengophone.exe" $R0

	!insertmacro MUI_LANGDLL_DISPLAY

	Call IsSilent
	Pop $0
	StrCmp $0 1 0 +2
		goto initDone

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

; Gets installation folder from registry if available
InstallDirRegKey HKCU "${PRODUCT_REGKEY}" ""

ShowInstDetails show
ShowUnInstDetails show

Section BaseSection
	!include "files_install.nsi"
SectionEnd

Section -AdditionalIcons
	WriteIniStr "$INSTDIR\${PRODUCT_PUBLISHER}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk" "$INSTDIR\${PRODUCT_PUBLISHER}.url"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section -Post
	; Stores installation folder
	WriteRegStr HKCU "${PRODUCT_REGKEY}" "" $INSTDIR

	; Creates uninstaller
	WriteUninstaller "$INSTDIR\uninstall.exe"

	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayIcon" "$INSTDIR\qtwengophone.exe"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
	WriteRegStr HKLM "${PRODUCT_UNINSTALL_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onInit
	!insertmacro MUI_UNGETLANGUAGE
FunctionEnd

Section Uninstall
	!include "files_uninstall.nsi"
SectionEnd
