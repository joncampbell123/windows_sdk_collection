page	,132
name	CHECKCUR
title	Check Cursor Routine


.xlist
include CMACROS.INC
include WINDEFS.INC		;see new definition of Enter/LeaveCrit
.list


sBegin	Data
externB DRAW_BUSY_FLAG		;FF HEX = DON'T YOU DARE TURN CURSOR ON!!!
sEnd	Data

externA CURSOR_ON_VACATION	;in SET.ASM

sBegin	Code
assumes cs,Code
assumes ds,Data


externFP    CursorUnExclude		;in ROUTINES.ASM


cProc	CheckCursor, <FAR,PUBLIC>, <si,di>

cBegin   
	EnterCrit a			;disallow mouse interrupts
	mov	cl,0ffh 		;set the draw busy flag and
	xchg	cl,DRAW_BUSY_FLAG	;get the state of the flag
					;upon entry to this routine
	LeaveCrit a			;reallow mouse interrupts
	test	cl, CURSOR_ON_VACATION	;are we doing something else?
	jnz	CC_EXIT 		;yes, leave now!
	cCall	CursorUnExclude 	;do the same code as over there

CC_EXIT:
	EnterCrit a			;disallow interrupts
	xchg	cl,DRAW_BUSY_FLAG	;get back the original state of
					;the draw busy flag
	LeaveCrit a			;reallow interrupts
cEnd

sEnd	code
end
