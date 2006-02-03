; Function that writes a piece of text to a file.
; Usage:
; Push "hello$\r$\n" ;text to write to file
; Push "$INSTDIR\log.txt" ;file to write to
; Call writeToFile
;
; ${writeToFile} "hello World!" "toto.txt"
Function writeToFile
	Exch $0 #file to write to
	Exch
	Exch $1 #text to write

	FileOpen $0 $0 w #open file
		FileWrite $0 $1 #write to file
	FileClose $0

	Pop $1
	Pop $0
FunctionEnd

!macro writeToFile String File
	Push "${String}"
	Push "${File}"
		Call writeToFile
!macroend
!define writeToFile "!insertmacro writeToFile"
