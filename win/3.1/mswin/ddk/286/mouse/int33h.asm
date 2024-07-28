	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	INT33H.ASM
;
; Windows mouse driver data and initialization routines for using the
; INT 33h mouse driver for Windows
;
; Created: 21-Aug-1987
; Author:  Mr. Mouse [mickeym], Walt Moore [waltm]
;
; Copyright (c) 1987  Microsoft Corporation
;
; Exported Functions:
;	None
; Public Functions:
;	I33_enable
;	I33_disable
;	I33_init
; Public Data:
;	None
; General Description:
;	This module contains the functions to find, enable, disable,
;	and process interrupts for the INT 33h mouse handler.
;-----------------------------------------------------------------------;


	title	Microsoft OS/2 INT 33h Dependent Code

	.xlist
	include cmacros.inc
	include mouse.inc
	.list


sBegin	Data

externD event_proc			;Mouse event procedure when enabled
externB device_int			;Start of mouse interrupt handler
externB mouse_flags			;Various flags
externW enable_proc			;Address of routine to	enable mouse
externW disable_proc			;Address of routine to disable mouse

sEnd	Data
page

sBegin	Code
assumes cs,Code

;	This is the start of the data which will be copied into
;	the device_int area reserved in the data segment.  References
;	to defined memory locations must be made relative to device_int

	even

I33_START	equ	this word


;--------------------------Interrupt-Routine----------------------------;
; I33_proc - Mouse Interrupt Handler
;
; This is the handler for the interrupt generated by the INT 33h
; mouse driver.  It will reside in the Data segment.
;
; Entry:
;	BL = current button status
;	  D0 = 1 if button 1 is down
;	  D1 = 1 if button 2 is down
;	SI = delta X
;	DI = delta Y
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	event_proc if valid mouse event
; History:
;	Fri 21-Aug-1987 11:43:42 -by-  Walt Moore [waltm] & Mr. Mouse
;	Initial version
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes cs,Code
	assumes ds,nothing
	assumes es,nothing
	assumes ss,nothing


I33_PROC_START	equ	$-I33_START	;Delta to this procedure

I33_proc	proc	far

	mov	ax,_DATA		 ;Need to gain access to our
	mov	ds,ax			;  data segment
	assumes ds,Data

	and	bx,00000011b			;Isolate buttons, set BH = 0
	mov	dh,bl				;Get and update mouse state
	xchg	dh,bptr device_int[OLD_STATE]	;1 indicates button down
	shl	dh,1				;Shift old state
	shl	dh,1
	or	bl,dh				;Combine states into index

;	mov	bl,deltas_states[bx]	;Get button delta/states, BH = 0
	mov	bl,bptr device_int[STATE_XLATE][bx]

	mov	cx,di			;CX = delta Y
	or	cx,si			;Show motion if any occured
	neg	cx			;'C' clear only if motion (DI ne 0)
	adc	bx,bx
	jz	I33_nothing_happened
	.errnz	SF_MOVEMENT-00000001b

	mov	ax,CLEAR_COUNTERS	;Clear dX and dY counters			   ;clear dX & dY counters
	int	MOUSE_SYS_VEC		;BX,SI,DI are preserved

	mov	cx,di			;CX = delta Y
	xchg	ax,si			;AX = delta X
	xchg	ax,bx			;AX = status, BX = delta X
	mov	dx,NUMBER_BUTTONS
        xor     si,si			;Zero out MessageExtraInfo for 3.1
        xor     di,di
	call	event_proc

I33_nothing_happened:

	ret

I33_proc	endp
page

;	old_int_33h areas contain the information required to
;	restore the previous INT 33h event handler when we
;	finally disable ourselves.

OLD_INT_33H_MASK equ	$-I33_START	;Delta to this byte
		dw	0		;Old INT 33h procedure mask

OLD_INT_33H_PROC equ	$-I33_START	;Delta to this byte
		dd	0		;Old INT 33h procedure address


;	old_status contains the state of the mouse the last time we
;	were called to process an interrupt.

OLD_STATE	equ	$-I33_START	;Delta to this byte
		db	0		;Old status will go here
page

;-----------------------------------------------------------------------;
; state_xlate
;
;	state_xlate is used to translate the current and previous
;	button state information into the values required by
;	Windows.  It is indexed as follows:
;
;	    pB2 pB1 cB2 cB1
;
;	     |	 |   |	 |
;	     |	 |   |	  --- 1 if button 1 is	down, 0 if button 1 is	up
;	     |	 |   |
;	     |	 |    ------- 1 if button 2 is	down, 0 if button 2 is	up
;	     |	 |
;	     |	  ----------- 1 if button 1 was down, 0 if button 1 was up
;	     |
;	      --------------- 1 if button 2 was down, 0 if button 2 was up
;
;	This table must be copied to the data segment along with the
;	interrupt handler.
;
;-----------------------------------------------------------------------;

STATE_XLATE	equ	$-I33_START	;delta to this table

	db	0			shr 1
	db	(SF_B1_DOWN)		shr 1
	db	(SF_B2_DOWN)		shr 1
	db	(SF_B1_DOWN+SF_B2_DOWN) shr 1

	db	(SF_B1_UP)		shr 1
	db	0			shr 1
	db	(SF_B1_UP+SF_B2_DOWN)	shr 1
	db	(SF_B2_DOWN)		shr 1

	db	(SF_B2_UP)		shr 1
	db	(SF_B1_DOWN+SF_B2_UP)	shr 1
	db	0			shr 1
	db	(SF_B1_DOWN)		shr 1

	db	(SF_B1_UP+SF_B2_UP)	shr 1
	db	(SF_B2_UP)		shr 1
	db	(SF_B1_UP)		shr 1
	db	0			shr 1

	.errnz	NUMBER_BUTTONS-2	;Won't work unless a two button mouse
page

I33_INT_LENGTH	= $-I33_START		;Length of code to copy
	.errnz	I33_INT_LENGTH gt MAX_INT_SIZE
page

;---------------------------Public-Routine------------------------------;
; I33_enable - Enable the INT 33h mouse handler
;
; The INT 33h mouse code will be requested to call our handler for
; all mouse events.  The previous event mask and event procedure will
; be saved for the restore code.
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	INT 33h
; History:
;	Fri 21-Aug-1987 11:43:42 -by-  Walt Moore [waltm] & Mr. Mouse
;	Initial version
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes cs,Code
	assumes ds,Data

I33_enable	proc	near

	mov	bptr device_int[OLD_STATE],0

	push	ds			;ES:DX --> interrupt procedure
	pop	es
	assumes es,Data

	lea	dx,device_int[I33_PROC_START]
	mov	cx,CALL_ON_ANY_INT	;Call us on any event
	mov	ax,SWAP_INT_PROC	;Swap interrupt routine
	int	MOUSE_SYS_VEC

	mov	wptr device_int[OLD_INT_33H_MASK],cx
	mov	wptr device_int[OLD_INT_33H_PROC][0],dx
	mov	wptr device_int[OLD_INT_33H_PROC][2],es
	ret

I33_enable	endp
page

;---------------------------Public-Routine------------------------------;
; I33_disable - Disable the INT 33h mouse handler
;
; The INT 33h mouse handler will be disabled from calling us.  The
; previous event mask and event procedure will be restored.
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	INT 33h
; History:
;	Fri 21-Aug-1987 11:43:42 -by-  Walt Moore [waltm] & Mr. Mouse
;	Initial version
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes cs,Code
	assumes ds,Data

I33_disable	proc	near

	mov	ax,BASH_INT_PROC
	mov	cx,wptr device_int[OLD_INT_33H_MASK]
	les	dx,dptr device_int[OLD_INT_33H_PROC]
	assumes es,nothing
	int	MOUSE_SYS_VEC

	ret

I33_disable	endp



;---------------------------Public-Routine------------------------------;
; I33_init - Initialize the INT 33h mouse handler
;
; Store the enable and disable procedure addresses where the interrupt
; procedure can find them.  Return the information needed for the caller
; to copy the interrupt procedure to the data segment.
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	INT 33h
; History:
;	Thu 17-Sep-1987 22:19:00 -by-  Bob Grudem [bobgru]
;	Coded final pieces.
;	Fri 21-Aug-1987 11:43:42 -by-  Walt Moore [waltm] & Mr. Mouse
;	Initial version
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes cs,Code
	assumes ds,Data

	public	I33_init
I33_init	proc	near

	mov	enable_proc, CodeOFFSET I33_enable
	mov	disable_proc,CodeOFFSET I33_disable
	mov	si,CodeOFFSET I33_START
	mov	cx,I33_INT_LENGTH
	ret
I33_init	endp

sEnd	Code
end