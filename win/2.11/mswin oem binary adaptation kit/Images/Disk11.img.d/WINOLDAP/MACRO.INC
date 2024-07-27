;----------------------------------------------------------
;	Macro Memory Manager constants and data structure
;----------------------------------------------------------

ifndef	NOMACROINFO		;needed to prevent conflicts
MacroInfo	STRUC
	MacroID 	dw	?
	MacroSize	dw	?
	MacroDirty	dw	?
	MacroAddress	dw	?
MacroInfo	ENDS

MacroInfoSize	equ	SIZE MacroInfo
endif


LowMacroID	equ	1024
HighMacroID	equ	2047

Winoldap	equ	1

Clean		equ	0
Dirty		equ	1

LowOp		equ	0
HighOp		equ	2

MarkID		equ	1024
CopyID		equ	1025
PasteID 	equ	1026
ScrollID	equ	1027
SwitchID	equ	1028
CloseID 	equ	1029
NullID		equ	1030
SwitchOutID	equ	1031
ExitID		equ	1032
ExecuteID	equ	1033

