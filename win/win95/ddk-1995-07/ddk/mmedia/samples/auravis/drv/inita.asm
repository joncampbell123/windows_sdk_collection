	TITLE INITA.ASM
	page 60,132

;-----------------------------------------------------------------------;
;	INITA.ASM							;
;									;
;	These are the routines to Enable and Disable Interrupts.	;
;									;
;	For the AuraVision video capture driver AVCAPT.DRV.		;
;									;
;-----------------------------------------------------------------------;
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************


PMODE = 1

.xlist
include cmacros.inc		
include windows.inc
include mmsystem.inc
include vcap.inc
.list
.286

?PLM=1		; Pascal calling convention
?WIN=0		; NO! Windows prolog/epilog code


;---------------------------------------;
;	Extrn Declarations		;
;---------------------------------------;

	extrn   HW_ISR:far 
	extrn 	HW_IRQEnable:far 
	extrn 	HW_IRQDisable:far
	extrn 	HW_GetIRQUsed:far


;---------------------------------------;
;	Segment Declarations		;
;---------------------------------------;

IFNDEF SEGNAME
	SEGNAME equ <_TEXT>
ENDIF

createSeg %SEGNAME, CodeSeg, word, public, CODE


;---------------------------------------;
;	Data Segment			;
;---------------------------------------;

sBegin Data

	externB gbInt,		-1
	externW gwBaseReg,	-1

	globalD gdwOldISR,	0
	globalB gfEnabled,	0

sEnd Data


sBegin DATA

sEnd DATA


;---------------------------------------;
;	Code Segment			;
;---------------------------------------;

sBegin CodeSeg

	assumes cs, CodeSeg
	assumes ds, Data

;-----------------------------------------------------------------------;
;	IRQEnable()							;
;									;
;	This function hooks interrupts when the driver is enabled.	;
;									;
;	The return value is zero if the call is successful, otherwise	;
;	an error code is returned.					;
;-----------------------------------------------------------------------;

cProc	IRQEnable <FAR, PASCAL, PUBLIC, WIN> <si, di>
cBegin
	mov	al,[gfEnabled]		; Already enabled?
	or	al,al
	jnz	IRQEnableExit

	cCall   HW_GetIRQUsed		; Which interrupt to use?
	mov	[gbInt], al

	mov	dx, seg HW_ISR
	mov	ax, offset HW_ISR
	mov	bl, [gbInt]

	cCall   InitSetInterruptVector, <bx,dx,ax>

	mov	[gdwOldISR].off, ax
	mov	[gdwOldISR].sel, dx

	; Enable interrupts.
	cCall   HW_IRQEnable

	; Enable interrrupts at the PIC.
	mov	bl, [gbInt]
	cCall   InitSetIntMask, <bx, 0>

	inc	[gfEnabled]		; Mark as being enabled.
	xor	ax,ax			; Show success.

IRQEnableExit:
cEnd


;-----------------------------------------------------------------------;
;	IRQDisable()							;
;									;
;	This function disables the driver.  It disables the		;
;	hardware, unhooks interrupts and frees any memory allocated.	;
;									;
;	The return value is zero if the call is successful otherwise	;
;	an error code is returned.					;
;-----------------------------------------------------------------------;

cProc IRQDisable <FAR, PASCAL, PUBLIC, WIN> <si, di>

cBegin
	mov	al,[gfEnabled]
	or	al,al
	jz	IRQDisableExit

	; Disable interrupts on the frame grabber.
	cCall   HW_IRQDisable

	; Disable interrupts at the PIC.
	mov	bl, [gbInt]
	cCall   InitSetIntMask, <bx, 1>

	mov	ax,[gdwOldISR].off
	mov	dx,[gdwOldISR].sel
	mov	bl,[gbInt]

	cCall   InitSetInterruptVector, <bx,dx,ax>

	dec	[gfEnabled]

IRQDisableExit:
	
	xor	ax,ax
cEnd


;-----------------------------------------------------------------------;
;	BOOL NEAR PASCAL InitSetIntMask(bIRQ, bMask)			;
;									;
;	This function sets or unsets the interrupt vector mask.		;
;									;
;	ENTRY:								;
;	ParmB   bIRQ	:   The IRQ (0 - 15) to mask/unmask		;
;	ParmB   bMask	:   The mask					;
;									;
;	EXIT:								;
;	AX	:   The return value is the previous interrupt mask.	;
;-----------------------------------------------------------------------;

	assumes ds, Data
	assumes es, nothing

cProc	InitSetIntMask <NEAR, PUBLIC> <>
	ParmB   bIRQ
	ParmB   bMask
cBegin

;	See if we need to talk to the slave or master PIC
	mov	cl, bIRQ
	mov	dx, PIC_IMR_MASTER
	cmp	cl, 8
	jb	SetIntMask_Master
	and	cl, 07h
	mov	dx, PIC_IMR_SLAVE

SetIntMask_Master:

;	Compute the interupt mask.
;	DX = slave or master mask register
;	CL = 0-7 bit to set/clear

	mov	ch, 1			; CH = 1
	shl	ch, cl			; CH = int mask

	mov	cl, bMask		; Get mask.
	or	cl, cl
	jz	SetIntMask_UnMask
	mov	cl,ch

SetIntMask_UnMask:

;	CH  = PIC mask = (1 << (bInt&7))
;	CL  = wanted mask = bMask ? ch : 0

	not	ch			; we need inverse of mask

	EnterCrit			; !!! Trashes BX !!!
	in	al, dx			; grab current mask
	mov	ah, al			; save it
	and	al, ch			; clear bit
	or	al, cl			; clear or set based on bMask
	cmp	al, ah			; don't set the same state again!
	je	SetIntMask_Same
	out	dx, al			; enable/disable ints...
SetIntMask_Same:
	LeaveCrit			; !!! Trashes BX !!!

;	We have set/cleared the PIC, now return the old state.

	not	ch			; return previous mask state
	mov	al, ah
	and	al, ch
	xor	ah, ah
cEnd


;-----------------------------------------------------------------------;
;	LPFUNC NEAR PASCAL InitSetInterruptVector(bIRQ, lpNewISR)	;
;									;
;	This function takes the IRQ and sets the appropriate interrupt	;
;	vector, and returns the pointer to the previous handler of the	;
;	IRQ.								;
;									;
;	ENTRY:								;
;	ParmB   bIRQ       :   The IRQ (0 - 15) to install handler for.	;
;	ParmD   lpNewISR   :   The handler.				;
;									;
;   EXIT:								;
;	DX:AX   :   The return value is the previous interrupt handler.	;
;-----------------------------------------------------------------------;

	assumes ds, Data
	assumes es, nothing

cProc	InitSetInterruptVector <NEAR, PUBLIC> <>
	ParmB   bIRQ
	ParmD   lpNewISR
cBegin

;	Convert IRQ to interrupt vector.
	mov	al, bIRQ
	mov	ah, 08h
	cmp	al, ah		; Q: slave or master IRQ?
	jl	isv_Continue

	mov	ah, (70h - 08h)	;   slave

isv_Continue:
	add	al, ah		; AL = interrupt vector

;	Get old interrupt vector (AL == interrupt vector).

	mov	ah, 35h
	int	21h			; get the old vector in es:bx

	push	es			; save for a bit
	push	bx

;	Set new interrupt vector (AL == interrupt vector).

	mov	ah, 25h
	push	ds
	lds	dx, lpNewISR
	assumes ds, nothing
	int	21h			; set the new vector
	pop	ds

	pop	ax			; restore old ISR for return value
	pop	dx			; ... DX:AX is old handler
cEnd

sEnd CodeSeg

	end
