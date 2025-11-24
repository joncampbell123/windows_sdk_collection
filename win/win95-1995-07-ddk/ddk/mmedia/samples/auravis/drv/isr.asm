	TITLE ISR.ASM
	page 60,132

;-----------------------------------------------------------------------;
;	ISR.ASM								;
;									;
;	Interrupt service routine entry point.				;
;									;
;	For the AuraVision video capture driver AVCAPT.DRV.		;
;									;
;	(C) Copyright AuraVision Corp. 1992-1993.  All rights reserved.	;
;	(C) Copyright Microsoft Corp. 1992.  All rights reserved.	;
;-----------------------------------------------------------------------;


?PLM=1		; PASCAL Calling convention is DEFAULT
?WIN=0		; Windows calling convention

.286
.xlist
include cmacros.inc
include vcap.inc
.list

externNP	InStreamISR		; cap.c
externFP 	HW_IRQClear


; Manually perform "pushad" instruction to remove warning
FULLREGPUSH macro
	db	66h
	pusha
endm

; Manually perform "popad" instruction to remove warning
FULLREGPOP macro
	db	66h
	popa
endm

;---------------------------------------;
;	Data Segment			;
;---------------------------------------;

ifndef SEGNAME
	SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

	externW _wIRQStatus
	externW _wCapStatus
	externD	_dwVideoClock
	externB	gbInt,		-1
	externW	gwBaseReg,	-1

sBegin Data
	wIntCount	dw	0000h
sEnd Data


;---------------------------------------;
;	Code Segment			;
;---------------------------------------;

sBegin CodeSeg
	assumes cs,CodeSeg
	assumes ds,Data
	assumes es,nothing


;-----------------------------------------------------------------------;
;	HW_ISR()							;
;									;
;	This is the interrupt service routine that handles vertical	;
;	sync hardware interrupts.  This is the main control routine for	;
;	streaming capture.						;
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

	public HW_ISR
.386
HW_ISR	proc	far

	cld

	.386
	FULLREGPUSH
	push	ds
	push	es
	push	fs

	; set up local DS

	mov	ax, DataBASE
	mov	ds, ax
	assumes ds,Data

	add	_dwVideoClock.lo,1      ; vertical sync interrupt counter
	adc	_dwVideoClock.hi,0      ;   frame syncronization depends
                                        ;   on accurate count of vsync's

;	Set it up so other interrupts can come through.
;	Send EOI to PIC.

	mov	al, 20H			; non-specific EOI

	cmp	[gbInt], 8              ; the interrupt in use
	jb	short @f
	out	0A0h, al		; slave EOI
@@:	out	020h, al		; master EOI

	cCall   HW_IRQClear		; re-enable device interrupts

	cmp	[wIntCount], 0          ; already processing an interrupt?
	jnz	short isrExit           ;   y, don't re-enter frame processing

;	Check the timing and perform the frame capture if needed.

	inc	[wIntCount]             ; bump re-entrancy lockout
	sti                             ; 
	cCall	InStreamISR             ; Get a frame!
	dec	[wIntCount]             ; decrement lockout

isrExit:
	pop	fs
	pop	es
	pop	ds
	assumes ds,nothing
	FULLREGPOP
	.286
	iret

HW_ISR endp



;-----------------------------------------------------------------------;
;	HW_CheckAcquire()						;
;									;
;	Checks current value of VxP-500 Interrupt Status register 61h	;
;	and returns whether or not a frame is currently being acquired	;
;	into the video frame buffer.					;
;									;
;	Entry:	No parameters.						;
;									;
;	Exit:	AX = 0 if FALSE						;
;		AX = 1 if TRUE						;
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

cProc	HW_CheckAcquire,<FAR, PASCAL, PUBLIC>,<ds>
cBegin
	mov	ax, DataBASE		; Set up local DS.
	mov	ds, ax
	assumes ds,Data

	cli				; Make sure no changes here.
	mov	ax, _wIrqStatus		; Get current IRQ Status value.
	and	ax, 40h			; Check frame acquire bit.
	jz	@f			; Return AX = 0 if FALSE.
	mov	ax, 1			; Return AX = 1 if TRUE.
@@:
	and	_wIrqStatus, 0BFh	; Clear IRQ Status acquire bit.
	sti
cEnd




;-----------------------------------------------------------------------;
;	HW_CheckCapture()						;
;									;
;	Checks current value of VxP-500 Field Status register 64h and	;
;	returns whether or not a frame is ready to be captured from	;
;	the video frame buffer.						;
;									;
;	Entry:	No parameters.						;
;									;
;	Exit:	AX = 0 if FALSE						;
;		AX = 1 if TRUE						;
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

cProc	HW_CheckCapture,<FAR, PASCAL, PUBLIC>,<ds>
cBegin

	mov	ax, DataBASE		; Set up local DS.
	mov	ds, ax
	assumes ds,Data

	cli				; Make sure no changes here.
	mov	ax, _wCapStatus		; Get current Field Status value.
	and	ax, 10h			; Check capture data available bit.
	jz	@f			; Return AX = 0 if FALSE.
	mov	ax, 1			; Return AX = 1 if TRUE.
@@:
	and	_wCapStatus, 0EFh	; Clear Field Status acquire bit.
	sti
cEnd



sEnd
end
