PAGE 58,132
;******************************************************************************
TITLE vfdmsg.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	vfdmsg.asm -
;
;   Version:	1.00
;
;   Date:	31-Oct-1989
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   31-Oct-1989 RAP
;
;==============================================================================

.386

.xlist
	INCLUDE VMM.INC
.list

VxD_DATA_SEG

BeginMsg

; INI switch to enable/disable allowing floppy reads to addresses 0E0000h-0FFFFFh
; default is TRUE [enabled]

PUBLIC VFD_Verify_Hack_INI_Switch
VFD_Verify_Hack_INI_Switch  db 'HIGHFLOPPYREADS', 0

;
; INI switch to enable automatically setting Compaq machines into HIGH speed
; mode when they are set to AUTO mode
;
PUBLIC VFD_Enable_Compaq_Slime
VFD_Enable_Compaq_Slime     db "COMPAQFASTFLOPPY", 0

EndMsg

VxD_DATA_ENDS

END
