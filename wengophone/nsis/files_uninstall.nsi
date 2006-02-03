nsProcess::KillProcess "wengophone.exe" .R0
Delete "$INSTDIR\styles\*"
RMDir "$INSTDIR\styles\"
Delete "$INSTDIR\designer\*"
RMDir "$INSTDIR\designer\"
Delete "$INSTDIR\bin\*"
RMDir "$INSTDIR\bin\"
Delete "$INSTDIR\icons\*"
RMDir "$INSTDIR\icons\"
Delete "$INSTDIR\lang\*"
RMDir "$INSTDIR\lang\"
Delete "$INSTDIR\sounds\dtmf\jungle\*"
RMDir "$INSTDIR\sounds\dtmf\jungle\"
Delete "$INSTDIR\sounds\dtmf\*"
RMDir "$INSTDIR\sounds\dtmf\"
Delete "$INSTDIR\sounds\*"
RMDir "$INSTDIR\sounds\"
Delete "$INSTDIR\connecting\*"
RMDir "$INSTDIR\connecting\"
Delete "$INSTDIR\*"
Delete "$INSTDIR\emoticons\default\*"
RMDir "$INSTDIR\emoticons\default\"
RMDir "$INSTDIR\emoticons\"
RMDir "$INSTDIR"

Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
Delete "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk"
Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
RMDir "$SMPROGRAMS\${PRODUCT_NAME}"

/**
 * Removes the configuration file + the contacts.
 * Only if the user asks for it.
 */
/*Delete "${WENGODIR}\contacts\*"
RMDir "${WENGODIR}\contacts\"
Delete "${WENGODIR}\*"
RMDir "${WENGODIR}"*/

DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
SetAutoClose true
