;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989-1990
;
;   Title:	vkdpaste.inc -
;
;   Version:	1.00
;
;   Date:	20-Mar-1989
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   20-Mar-1989 RAP
;   23-Mar-1989 RAP added PM API equates
;
;==============================================================================

;
; structure of entries in paste buffer
;
Paste_Rec STRUC
Paste_Char	    db	?   ; OEM char value
Paste_ScanCode	    db	?   ; keyboard scan code
Paste_ShiftState    dw	?   ; shift state
Paste_Rec ENDS

Paste_Key   equ word ptr Paste_Char


;BeginDoc
;------------------------------------------------------------------------------
;
; API services:
;
VKD_API_Version     equ 0
	;   ENTER:  EAX = 0
	;   EXIT:   AH = major version #
	;	    AL = minor version #
	;	    Carry clear

VKD_Current_Version equ 200h

VKD_Force_Key	    equ 1
	;   ENTER:  EAX = 1
	;	    EBX = VM handle  (0 for current)
	;	    CH = scan code
	;	    CL = repeat count
	;	    EDX = shift state  (-1 means no change)
	;   EXIT:   Carry set, if failed

;
; shift state equates
;
FKSS_Insert_Down    equ 1000000000000000b
FKSS_CapLck_Down    equ 0100000000000000b
FKSS_NumLck_Down    equ 0010000000000000b
FKSS_ScrLck_Down    equ 0001000000000000b
FKSS_Insert_On	    equ 0000000010000000b
FKSS_CapLck_On	    equ 0000000001000000b
FKSS_NumLck_On	    equ 0000000000100000b
FKSS_ScrLck_On	    equ 0000000000010000b
FKSS_Alt_Down	    equ 0000000000001000b
FKSS_Ctrl_Down	    equ 0000000000000100b
FKSS_Shift_Down     equ 0000000000000010b

;EndDoc
