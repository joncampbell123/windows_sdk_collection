;===============================================================================
;		File	MSDOS.INC
;===============================================================================

MSDOS				equ	21h

DosCreateHandle 		equ	3ch
DosCloseHandle			equ	3eh

DosGetMachineName		equ	5e00h
DosGetAssignListEntry		equ	5f02h
DosMakeAssignListEntry		equ	5f03h
DosCancelAssignListEntry	equ	5f04h
DosGetAssignListEntry2		equ	5f05h	; !!! undocumented !!!

DosSerialDevice 		equ	03h	; Returned by DosGetAssignListEntry
DosBlockDevice			equ	04h

AssignListEntry struc
		aleLocalName	db	16 dup ( ? )
		aleRemoteName	db	128 dup ( ? )
AssignListEntry ends
