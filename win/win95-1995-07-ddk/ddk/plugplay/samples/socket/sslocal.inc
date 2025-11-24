Pushad_Word_Struc	STRUC
Pushad_DI	DW	?
		DW	?
Pushad_SI	DW	?
		DW	?
Pushad_BP	DW	?
		DW	?
Pushad_SP	DW	?
		DW	?
Pushad_BX	DW	?
		DW	?
Pushad_DX	DW	?
		DW	?
Pushad_CX	DW	?
		DW	?
Pushad_AX	DW	?
		DW	?
Pushad_Word_Struc	ENDS

Pushad_Byte_Struc	STRUC
		DD	?			; EDI
		DD	?			; ESI
		DD	?			; EBP
		DD	?			; ESP
Pushad_BL	DB	?
Pushad_BH	DB	?
		DW	?
Pushad_DL	DB	?
Pushad_DH	DB	?
		DW	?
Pushad_CL	DB	?
Pushad_CH	DB	?
		DW	?
Pushad_AL	DB	?
Pushad_AH	DB	?
		DW	?
Pushad_Byte_Struc	ENDS


SSP_EAX 	EQU	[ebp.Pushad_EAX]
SSP_AX		EQU	[ebp.Pushad_AX]
SSP_AH		EQU	[ebp.Pushad_AH]
SSP_AL		EQU	[ebp.Pushad_AL]
SSP_EBX 	EQU	[ebp.Pushad_EBX]
SSP_BX		EQU	[ebp.Pushad_BX]
SSP_BH		EQU	[ebp.Pushad_BH]
SSP_BL		EQU	[ebp.Pushad_BL]
SSP_ECX 	EQU	[ebp.Pushad_ECX]
SSP_CX		EQU	[ebp.Pushad_CX]
SSP_CH		EQU	[ebp.Pushad_CH]
SSP_CL		EQU	[ebp.Pushad_CL]
SSP_EDX 	EQU	[ebp.Pushad_EDX]
SSP_DX		EQU	[ebp.Pushad_DX]
SSP_DH		EQU	[ebp.Pushad_DH]
SSP_DL		EQU	[ebp.Pushad_DL]
SSP_ESI 	EQU	[ebp.Pushad_ESI]
SSP_SI		EQU	[ebp.Pushad_SI]
SSP_EDI 	EQU	[ebp.Pushad_EDI]
SSP_DI		EQU	[ebp.Pushad_DI]


MAX_SOCKETS	EQU	4

ADAPT_SIG	EQU	"MSSS"

Adapter_Handle	STRUC
AH_Devnode	dd	?			; MUST BE FIRST.  Used by CS.
AH_Signature	dd	ADAPT_SIG
AH_IOBase	dd	0
AH_StatusInt	db	0
AH_NumSockets	db	0
AH_SocketStatus db	MAX_SOCKETS dup (?)
AH_SocketIRQ	db	MAX_SOCKETS dup (?)
Adapter_Handle	ENDS

VERSION_NUM_SS	EQU	201h	    ; Version 2.01
