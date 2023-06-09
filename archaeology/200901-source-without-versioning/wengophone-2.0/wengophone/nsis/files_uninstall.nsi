/** Kills running qtwengophone.exe */
${nsProcess::KillProcess} "qtwengophone.exe" $R0

Delete "$INSTDIR\styles\*"
RMDir "$INSTDIR\styles\"

Delete "$INSTDIR\icons\*"
RMDir "$INSTDIR\icons\"

Delete "$INSTDIR\lang\*"
RMDir "$INSTDIR\lang\"

Delete "$INSTDIR\sounds\tones\jungle\*"
RMDir "$INSTDIR\sounds\tones\jungle\"

Delete "$INSTDIR\sounds\tones\*"
RMDir "$INSTDIR\sounds\tones\"

Delete "$INSTDIR\sounds\*"
RMDir "$INSTDIR\sounds\"

Delete "$INSTDIR\emoticons\*"
RMDir "$INSTDIR\emoticons\"

Delete "$INSTDIR\plugins\*"
RMDir "$INSTDIR\plugins\"

Delete "$INSTDIR\pics\avatars\*"
RMDir "$INSTDIR\pics\avatars\"

Delete "$INSTDIR\pics\*"
RMDir "$INSTDIR\pics\"

Delete "$INSTDIR\webpages\windows\*"
RMDir "$INSTDIR\webpages\windows\"

Delete "$INSTDIR\webpages\*"
RMDir "$INSTDIR\webpages\"

Delete "$INSTDIR\imageformats\*"
RMDir "$INSTDIR\imageformats\"

Delete "$INSTDIR\*"
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

DeleteRegKey HKLM "${PRODUCT_UNINSTALL_KEY}"
DeleteRegKey HKCU "${PRODUCT_REGKEY}"
SetAutoClose true
