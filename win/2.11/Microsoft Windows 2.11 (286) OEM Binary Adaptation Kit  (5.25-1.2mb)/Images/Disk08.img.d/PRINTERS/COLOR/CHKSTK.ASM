%out	dmCheckStack

;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

title	Check Stack
page	,132


	.xlist
	include cmacros.inc
	.list


	ifdef	debug
	externFP FatalExit
	endif


pStackTop	equ	000Ah		;Topmost location of stack
pStackBot	equ	000Eh		;Bottom  location of stack
pStackMin	equ	000Ch		;Smaller top value


sBegin	code
assumes cs,code


;	dmCheckStack - check stack for room
;
;	Our own stack probe which will allow us to gracefully
;	abort out of the display driver routines if there isn't
;	enough stack space.  Routines like STRBLT take a lot
;	of space, and should return an error if not enough room
;	to perform the operation instead of trahsing or ripping.
;
;	Entry:	ax = # bytes needed
;
;	Exit:	'C' clear if room, space allocated
;		'C' set if no room
;		  dx:ax = 80000000h
;	Uses:	ax,bx,flags

	public	dmCheckStack
dmCheckStack:
	pop	bx			;Save return address
	sub	ax,sp			;See if room
	jnc	NoRoom			;No room, return error
	neg	ax			;Make it positive for other checks
	cmp	ss:[pStackTop],ax	;Used up too much stack?
	ja	NoRoom			;  Yes, return error
	cmp	ss:[pStackMin],ax	;Lowest we've gotton?
	jbe	NotSmaller		;  No
	mov	ss:[pStackMin],ax	;  Yes, set new minimum

NotSmaller:
	mov	sp,ax			;Set new stack
	clc				;Clear 'C' to show room
	jmp	bx			;To caller

NoRoom:
	mov	ax,ss:[pStackTop]	;Show user that all of the
	mov	ss:[pStackMin],ax	;  stack has been used

	ifdef	debug
	mov	ax,-1			;Stack overflow
	push	bx			;Save return address
	cCall	FatalExit,<ax>
	pop	bx
	endif				;Returning to caller

	xor	ax,ax			;Set error code(s)
	mov	dx,8000h
	stc				;Show no room
	jmp	bx

sEnd	code
end
