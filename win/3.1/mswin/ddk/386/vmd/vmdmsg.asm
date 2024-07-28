PAGE 58,132
;******************************************************************************
TITLE vmdmsg.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp.  All Rights Reserved, 1989, 1990
;
;   Title:	vmdmsg.asm -
;
;   Version:	1.00
;
;   Date:	22-Feb-1990
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   22-Feb-1990 RAP
;
;==============================================================================

.386

	INCLUDE VMM.INC


VxD_IDATA_SEG

BeginMsg
PUBLIC I33_Soft_Init_INI
I33_Soft_Init_INI   db	'MouseSoftInit', 0
;
;   Set to FALSE, if INT 33h function 0 init calls should not be converted to
;   function 33 soft init calls.  Function 33 init calls do not reprogram the
;   hardware, so mouse apps can start in a Window and will still have a working
;   mouse when they are made fullscreen.  Users should set this switch to
;   FALSE, if they have problems with starting a mouse app and then the mouse
;   doesn't function correctly, because it doesn't know what display mode the
;   app is running.  If they set this switch to FALSE, then they should avoid
;   starting mouse apps in a window, because the mouse driver will not be able
;   to detect the mouse hardware, so the mouse won't ever work in that VM.
;   The default for this switch is FALSE.

PUBLIC	I33_Hard_Init_INI
I33_Hard_Init_Ini	db	'MouseHardResetAtInit',0
;
;    When the user has a PS-style mouse and an attempt is made to run an
;    application which does not make int33h fuction 0 init call before
;    beginning to use the mouse, the app will not be able to use the mouse.
;    To get around this problem, the user should set this variable to 0.
;
EndMsg

VxD_IDATA_ENDS

END
