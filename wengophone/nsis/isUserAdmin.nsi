; Function that checks if the user is an Administrator
; Usage:
; Call isUserAdmin
; Pop $R0
; StrCmp $R0 "true" isAdmin
; 	; Not an admin
; 	StrCpy $INSTDIR "$DOCUMENTS\${PRODUCT_NAME}"
; 	SetShellVarContext current
; 	goto initDone
; isAdmin:
; 	; User is admin
; 	StrCpy $INSTDIR "$PROGRAMFILES\${PRODUCT_NAME}"
; 	SetShellVarContext all
; initDone:
Function isUserAdmin
	Push $R0
	Push $R1
	Push $R2

	ClearErrors
	UserInfo::GetName
	IfErrors Win9x
	Pop $R1
	UserInfo::GetAccountType
	Pop $R2

	StrCmp $R2 "Admin" 0 Continue
	StrCpy $R0 "true"
	Goto Done

	Continue:
		StrCmp $R2 "" Win9x
	StrCpy $R0 "false"
		Goto Done

	Win9x:
		StrCpy $R0 "true"

	Done:
	Pop $R2
	Pop $R1
	Exch $R0
FunctionEnd
