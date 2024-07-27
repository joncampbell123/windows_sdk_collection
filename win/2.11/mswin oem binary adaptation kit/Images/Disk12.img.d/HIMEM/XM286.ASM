;**************************************************************************
;*	* * * 286 LOADALL information * * *
;**************************************************************************

;*	* Segment Descriptor Access Bytes
DEF_ACCESS	EQU	92H

DEF_LIMIT	EQU	0FFFFH

SEGREG_DESCRIPTOR STRUC
SEG_BASE_LO	DW	0
SEG_BASE_HI	DB	0
SEG_ACCESS	DB	DEF_ACCESS
SEG_LIMIT	DW	DEF_LIMIT
SEGREG_DESCRIPTOR ENDS

DTR_DESCRIPTOR STRUC
DTR_BASE	DW	?
		DB	?
		DB	0
DTR_LIMIT	DW	?
DTR_DESCRIPTOR ENDS

LODAL286	MACRO	
	DW	050Fh
ENDM

;*******************************************************************************
;
; MoveExtended286
;	XMM Move Extended Memory Block for the 80286
;
;	Copyright (c) 1988, Microsoft Corporation
;
; Entry:
;	ES:BX	Points to a MoveExtendedStruc
;
; Return:
;	AX = 1	Success
;	AX = 0	Failure
;		Error Code in BL
;
; Registers Destroyed:
;	Flags, CX, SI, DI, ES
;
;			WARNING
;			=======
;
; This routine enables interrupts and can be re-entered
;
; Notes:
;	The case of copying from conventional to conventional memory
;	is not treated specially in this example.
;
; History:
;	Wed Jul 13 - AWG - Original version
;-------------------------------------------------------------------------------
ifndef	XM286INCLUDED
public	MoveExtended286
endif
MoveExtended286 proc    near

assume	cs:code, ds:code, es:nothing

	sti					; Be nice
	push	bp				; Set up stack frame so we
	mov	bp, sp				; can have local variables
	sub	sp, 18				; Space for local variables
Count	  = -4					; Local DWORD for byte count
MEReturn  = -6					; Local WORD for return code
SrcHandle = -8
DstHandle = -10
SrcLinear = -14
DstLinear = -18
	push	bx
	push	dx

	xor	ax, ax
	mov	[bp.MEReturn], ax			; Assume success
	mov	[bp.SrcHandle], ax
	mov	[bp.DstHandle], ax
	mov	ax, word ptr es:[si.bCount]	; Pick up length specified
	mov	word ptr [bp.Count], ax
	mov	cx, word ptr es:[si.bCount+2]
	mov	word ptr [bp.Count+2], cx
	or	cx, ax
	jcxz	MEM2_Exit			; Exit immediately if zero

	lea	bx, [si.SourceHandle]		; Normalize Source
	call	GetLinear286			; Linear address in DX:AX
	jc	short MEM2_SrcError		      ; Have Dest Error Code
	mov	word ptr [bp.SrcLinear], ax	; Save Linear address
	mov	word ptr [bp.SrcLinear+2], dx
	mov	[bp.SrcHandle], bx		; Save Handle for Unlock

	lea	bx, [si.DestHandle]		; Normalize Destination
	call	GetLinear286
	jc	short MEM2_Error
	mov	word ptr [bp.DstLinear], ax	; Save Linear address
	mov	word ptr [bp.DstLinear+2], dx
	mov	[bp.DstHandle], bx		; Save Handle for Unlock

	shr	word ptr [bp.Count+2], 1	; Make word count
	rcr	word ptr [bp.Count], 1
	jc	short MEM2_InvCount		      ; Odd count not allowed

	call	LocalEnableA20			; Enable A20 for LoadAll code
	cmp	ax, 1
	jne	short MEM2_Error

	call	DoLoadAll

	call	LocalDisableA20
	cmp	ax, 1
	jne	short MEM2_Error

MEM2_Exit:
	mov	bx, [bp.SrcHandle]		; Unlock Handles if necessary
	or	bx, bx
	jz	short NoSrcHandle
	dec	[bx.cLock]			; Unlock Source
NoSrcHandle:
	mov	bx, [bp.DstHandle]
	or	bx, bx
	jz	short NoDstHandle
	dec	[bx.cLock]			; Unlock Destination
NoDstHandle:
	pop	dx
	pop	bx
	mov	ax, 1
	cmp	word ptr [bp.MEReturn], 0
	jz	short MEM2_Success
	dec	ax
	mov	bl, byte ptr [bp.MEReturn]
MEM2_Success:
	mov	sp, bp				; Unwind stack
	pop	bp
	ret

MEM2_SrcError:
	cmp	bl, ERR_LENINVALID		; Invalid count
	je	short MEM2_Error		      ;   yes, no fiddle
	sub	bl, 2				; Convert to Source error code
	jmp	short MEM2_Error
MEM2_InvCount:
	mov	bl, ERR_LENINVALID
MEM2_Error:
	mov	byte ptr [bp.MEReturn], bl	; Pass error code through
	jmp	short MEM2_Exit

;*******************************************************************************
;
; GetLinear286
;	Convert Handle and Offset (or 0 and SEG:OFFSET) into Linear address
;	Locks Handle if necessary
;	Nested with MoveExtended286 to access local variables
;
; Entry:
;	ES:BX	Points to structure containing:
;			Handle	dw
;			Offset	dd
;	[BP.Count]	Count of bytes to move
;
; Return:
;	BX	Handle of block (0 if conventional)
;	AX:DX	Linear address
;	CARRY	=> Error
;		Error code in BL
;
; Registers Destroyed:
;	Flags, CX, DI
;
;-------------------------------------------------------------------------------

GetLinear286	proc	near
	push	si
	cli					; NO INTERRUPTS
	mov	si, word ptr es:[bx+2]		; Offset from start of handle
	mov	di, word ptr es:[bx+4]		; in DI:SI
	mov	bx, word ptr es:[bx]		; Handle in bx
	or	bx, bx
	jz	short GL2_Conventional

	test	[bx.Flags], USEDFLAG		; Valid Handle?
	jz	short GL2_InvHandle

	mov	ax, [bx.Len]			; Length of Block
	mov	cx, 1024
	mul	cx				; mul is faster on the 286
	sub	ax, si
	sbb	dx, di				; DX:AX = max possible count
	jc	short GL2_InvOffset		      ; Base past end of block
	sub	ax, word ptr [bp.Count]
	sbb	dx, word ptr [bp.Count+2]
	jc	short GL2_InvCount		      ; Count too big

	inc	[bx.cLock]			; Lock the Handle
	mov	ax, [bx.Base]
	mul	cx
	add	ax, si				; Linear address
	adc	dx, di				; in DX:AX

GL2_OKExit:
	clc
GL2_Exit:
	sti
	pop	si
	ret

GL2_Conventional:
	mov	ax, di				; Convert SEG:OFFSET into
	mov	dx, 16				; 24 bit address
	mul	dx
	add	ax, si
	adc	dx, 0				; DX:AX has base address
	mov	di, dx
	mov	si, ax
	add	si, word ptr [bp.Count]		; Get End of Block + 1 in DI:SI
	adc	di, word ptr [bp.Count+2]
	cmp	di, 010h			; 32-bit cmp
	ja	short GL2_InvCount
	jb	short GL2_OKExit
	cmp	si, 0FFF0h
	jbe	GL2_OKExit			; Must be < 10FFEFh + 2
GL2_InvCount:
	mov	bl, ERR_LENINVALID
	jmp	short GL2_Error
GL2_InvHandle:
	mov	bl, ERR_DHINVALID		; Dest handle invalid
	jmp	short GL2_Error
GL2_InvOffset:
	mov	bl, ERR_DOINVALID		; Dest Offset invalid
GL2_Error:
	stc
	jmp	short GL2_Exit
	
GetLinear286	endp

;*******************************************************************************
;
; DoLoadAll
;	Use 286 LoadAll for copy - see warnings below
;	Nested within MoveExtended286
;
; Entry:
;	[BP.Count]	Word count for move
;	[BP.SrcLinear]	Linear address of the source
;	[BP.DstLinear]	Linear address of the destination
;
;	Interrupts are ON
;
; Return:
;	CARRY	=> Error
;		Error code in BL
;
; Registers Destroyed:
;	Flags, AX, BX, CX, DX, SI, DI
;
;-------------------------------------------------------------------------------

;******************************************************************
;*
;*	Trick 1: using 286 LOADALL.
;*	===========================
;*
;*	To use the LOADALL instruction 102 bytes at location 80:0 must
;*	be used. We copy the contents of 80:0 into its own buffer,
;*	copies in the LOADALL info, performs the LOADALL, and then copies
;*	back the previous contents of 80:0.  This copy is performed with
;*	interrupts disabled (since we will be trashing BIOS data that
;*	interrupt routines will need).
IFDEF LATER
;*	* if running DOS 3.3 or greater, the 80:0 buffer is free (IBM freed
;*	*  it to work with VDISK) so we do not need to save and restore it.
ENDIF ;LATER
;*
;*	WARNING NUMBER ONE:
;*	    THIS CODE WILL NOT WORK ON ANY 80286 MACHINE WHERE THE NMI
;*	    INTERRUPT IS ANYTHING BUT A FATAL, SYSTEM HALTING ERROR.
;*	    (since we rely on CLI disabling all interrupts).
;*	The initialization check checks for an AT or true compatible.
;*
;*
;*	Trick 2: Addressing with 24 bits.
;*	=================================
;*
;*	In order to gain full 24 bit addressing, address line 20 must be
;*	enabled. This effects 8086 compatibility on 80286 systems.
;*	This code leaves address line 20 enabled for the ENTIRE duration of
;*	the copy.
;*
;*	WARNING NUMBER TWO.
;*	    IF AN INTERRUPT ROUTINE (OR AN APPLICATION RUN BY A MULTITASKING
;*	    PRE-EMPTIVE SCHEDULER) RELIES ON THE 1 MEG ADDRESS WRAP
;*	    PROPERTY OF THE 8086 AND 8088 DURING THE TIME THE A20 LINE IS
;*	    ENABLED, THEN THE INTERRUPT ROUTINE WILL NOT WORK (IF THE INTERRUPT
;*	    ROUTINE DISABLES THE A20 LINE, THEN THE MEMORY COPY MAY BE
;*	    CORRUPTED.
;*
;*	Trick 3: Copying with interrupts on.
;*	====================================
;*
;*	The actual data copy is performed with interrupts on.
;*	Since the LOADALL puts the CPU in a meta-stable state that can be
;*	changed by an interrupt (i.e. an interrupt may push and pop DS and ES,
;*	thereby changing the segment descriptor info from pointing to extended
;*	memory into pointing to conventional memory).  If the copy were to
;*	continue after an interrupt, then conventional memory could get
;*	This trick relies on pointing the CS segment descriptor info to the
;*	actual code doing the copy, and pointing the CS register 16 bytes past
;*	where the code is executing (i.e. paCs = SEG CODE<<4, CS = SEG CODE+1).
;*	When an interrupt occurs, it will always push CS (==SEG CODE+1) and
;*	return to that address.  This will stop the copy and jump 16 bytes
;*	past the interrupted address.  A 16 byte NOP region follows the copy
;*	which will trap any interrupts, which then jumps to the LOADALL code
;*	to reset the meta-stable copy state and resume the copy.
;*
;*	WARNING NUMBER THREE.
;*	    THIS TRICK IS NOT GUARANTEED TO WORK ON A 386 PROCESSOR WHERE
;*	    THE 286 LOADALL INTRUCTION IS EMULATED.
;*	The initialization code checks the CPU, if a 386 then a different
;*	algorithm is used (FMoveExtended386).
;*
;*
;*	METHOD:
;*	    Enable address line 20
;*	    Perform various LOADALL preparations
;*	loop:
;*	    CLI
;*		copy 80:0 to buffer
;*		copy LOADALL info to 80:0
;*		LOADALL
;*		copy buffer back to 80:0
;*	    STI - (CS is set into CS+1 mode)
;*	    while (cwMove--)				} REP MOVSW
;*		copy 1 word;				}
;*	    if (interrupted && cwMove != 0)
;*	    	goto loop;
;*	    clear CS+1 mode (disables interrupt detection)
;*	    Disable address line 20

;******************************************************************

	EVEN		;* WORD alignment for data

; Swap buffer for contents of 80:0
cwBuffer  EQU	51
rgwSwap80 DW	cwBuffer DUP (?)

; LOADALL data buffer placed at 80:0
;
LOADALL_TBL	LABEL	BYTE
	DB	6 DUP(0)
LDSW	DW	?
	DB	14 DUP (0)
TR	DW	0
LFLAGS	DW	0		; High 4 bits 0, Int off, Direction clear
				;   Trace clear. Rest don't care.
LIP	DW	OFFSET	AFTER_LOADALL
LDT	DW	0
LDSS	DW	8000H
LSSS	DW	80H
LCSS	DW	?
LESS	DW	8000H
LDI	DW	0
LSI	DW	Offset rgwSwap80
LBP	DW	0
LSP	DW	?
LBX	DW	?
LDX	DW	?
LCX	DW	cwBuffer
LAX	DW	80H
ESDES	SEGREG_DESCRIPTOR <>
CSDES	SEGREG_DESCRIPTOR <>
SSDES	SEGREG_DESCRIPTOR <800H,0>
DSDES	SEGREG_DESCRIPTOR <>
GDTDES	DTR_DESCRIPTOR <>
LDTDES	DTR_DESCRIPTOR <0D000H,0,0FFH,0088H>
IDTDES	DTR_DESCRIPTOR <>
TSSDES	DTR_DESCRIPTOR <0C000H,0,0FFH,0800H>

DescSaved	dw	-1		; Flag for reentrancy

SaveDesc	macro	reg
	push	word ptr [reg]		; Save 3 word descriptor
	push	word ptr [reg+2]	; pointed to by reg
	push	word ptr [reg+4]
	endm

RestoreDesc	macro	reg
	pop	word ptr [reg+4]	; Restore 3 word descriptor
	pop	word ptr [reg+2]	; pointed to by reg
	pop	word ptr [reg]
	endm

NOP4	macro
	sti				; Faster than nop
	sti
	sti
	sti
	endm

DoLoadAll	proc	near

assume	cs:code, ds:code, es:nothing

	cld					;* just to be sure

	mov	ax, word ptr [bp.SrcLinear]	; Create descriptors for source
	mov	dl, byte ptr [bp.SrcLinear+2]	; and destination of transfer
	mov	cx, word ptr [bp.DstLinear]
	mov	dh, byte ptr [bp.DstLinear+2]
	xchg	[DSDES].SEG_BASE_LO,ax		; Fill in table and pick up
	xchg	[DSDES].SEG_BASE_HI,dl		; old values
	xchg	[ESDES].SEG_BASE_LO,cx
	xchg	[ESDES].SEG_BASE_HI,dh

	mov	bx,Offset IDTDES
	mov	si,Offset GDTDES

	inc	[DescSaved]
	jz	short DLA_NoSave	      ; Don't save old values first time
	push	ax			; Save these so we can be re-entrant
	push	cx
	push	dx
	push	[LDSW]
	SaveDesc	bx		; Save IDTDES on stack
	SaveDesc	si		; Save GDTDES on stack
DLA_NoSave:

	smsw	[LDSW]			; Fill in MSW, GDT and IDT in buffer
    ;* NOW The damn SXXX instructions store the desriptors in a
    ;*	  different order than LOADALL wants
	sidt	qword ptr [bx]
	call	FixDescriptor
	mov	bx, si
	sgdt	qword ptr [bx]
	call	FixDescriptor

DLA_MoveLoop:				; Want byte count to fit in one word
	mov	cx, 7FFFh			; Must be < 8000h
	cmp	word ptr [bp.Count+2], 0	; Lots to do?
	ja	short DLA_DoMove
	cmp	word ptr [bp.Count], cx
	jae	short DLA_DoMove
	mov	cx, word ptr [bp.Count]		; Just what is left

DLA_DoMove:
	push	ds
	push	bp			;* gets trashed later
	mov	ax, 80H			; Set up for first copy - can do before
	mov	bx, cs			; disabling interrupts
	mov	dx, cwBuffer
	xor	si, si
	mov	es, bx
	mov	di, Offset rgwSwap80	;* ES:DI = CS:rgwSwap80 = save address

	CLI				; Un interruptable
	MOV	[LSP], sp		;* the real stack limit
	mov	[LBX], ss
	mov	[LDX], cx		;* the actual count of words to move
					;* will be in DX
	mov	ds, ax			;* DS:SI = 80:0 = source address
assume	ds:nothing
	mov	cx, dx			; cwBuffer

	rep movsw			; Save contents of 80:0
;*	* now move info from LOADALL_TBL buffer into 80:0 
	xor	di, di
	mov	es, ax			;* ES:DI = 80:0 (dest)
	mov	si, Offset LOADALL_TBL
	mov	ds, bx			;* DS:SI = CS:LOADALL_TBL (src)
	mov	cx, dx			; cwBuffer
	rep movsw			; Transfer in LOADALL info
	LODAL286			;* LOADALL INSTRUCTION (set AX = 80H)
; now set up stack for moving 80:0 info back again
; LOADALL will set the following registers:
;	AX = 80H
;	BX = old SS
;	CX = cwBufer
;	DX = word count to move
;	SI = OFFSET rgwSwap80
;	DI = 0
;	BP = 0, SS = 80H  (SS:BP => loadall area to restore)
;	DS:0 = source address (DS may be outside real mode), do not rely
;		on the DS register.
;	ES:0 = destination address (ES may be outside real mode), do not
;		rely on the ES register unless reading.
AFTER_LOADALL:
move_loop:
	lods	word ptr cs:[si]
	mov	ss:[bp],ax		;* can't override ES
	inc	bp
	inc	bp
	loop	move_loop

;*	* now actually move the data
	mov	ss, bx	 		;* restore SS
	mov	cx, dx			;* actual word count
	mov	si, di			;* source and destination 0
	STI
;* * * ** End Interrupt Off Code
;* * * ** Begin Interrupt Protected Code
	rep movsw			;* Move data

	db	0EAh				; jmp far move_stopped
	dw	Offset move_stopped		;	fixes up CS
cs0	dw	0
;* * * ** End Interrupt Protected Code
;*	* if this ever gets interrupted, we will jump 16 bytes
;*	*  CS will be bogus CS+1 value
;		CX WILL INDICATE IF THE MOVE WAS COMPLETED
	NOP4
	NOP4
	NOP4
	NOP4

	db	0EAh				; jmp far move_stopped
	dw	Offset move_stopped		;	fixes up CS
cs1	dw	0

move_stopped:					;* resume after move stops
	pop	bp
	pop	ds				; Get our DS back
assume ds:code
	mov	cx, si
	shr	cx, 1				; # words moved
	sub	word ptr [bp.Count], cx		; Subtract what we just did
	jz	short DLA_TestDone
	sbb	word ptr [bp.Count+2], 0	; Not zero, more to do
DLA_KeepGoing:
	add	[DSDES].SEG_BASE_LO, si		; Update base of segments to end
	adc	[DSDES].SEG_BASE_HI, 0		; of transfer done so far.  Our
	add	[ESDES].SEG_BASE_LO, di		; LoadAll buffer always puts 0
	adc	[ESDES].SEG_BASE_HI, 0		; in SI and DI
	jmp	DLA_MoveLoop

DLA_TestDone:
	sbb	word ptr [bp.Count+2], 0
	jnz	short DLA_KeepGoing		      ; High word not zero, more to do

DLA_done:
	cmp	[DescSaved], 0		; Did we save registers?
	jz	short DLA_NoRestore

	mov	bx, offset GDTDES
	RestoreDesc	bx
	mov	bx, offset IDTDES
	RestoreDesc	bx
	pop	[LDSW]
	pop	dx			; Restore original Segment bases
	pop	bx			; in our LoadAll buffer
	pop	ax
	mov	[DSDES].SEG_BASE_LO, ax
	mov	[DSDES].SEG_BASE_HI, dl
	mov	[ESDES].SEG_BASE_LO, bx
	mov	[ESDES].SEG_BASE_HI, dh
DLA_NoRestore:
	dec	[DescSaved]
	ret

;**	FixDescriptor - Shuffle GTD IDT descriptors
;*
;*	The segment descriptors for the IDT and GDT are stored
;*	by the SIDT instruction in a slightly different format
;*	than the LOADALL instruction wants them. This routine
;*	performs the transformation.
;*
;*	ENTRY:
;*	    DS:BX points to IDT or GDT descriptor in SIDT form
;*	EXIT:
;*	    DS:BX points to IDT or GDT descriptor in LOADALL form
;*	USES: AX,CX,DX
;*
;*	NOTE: The transformation is reversable, so this routine
;*		will also work to transform a descriptor in LOADALL
;*		format to one in SIDT format.
;*
FixDescriptor	proc	near
	mov	ax,ds:[bx+4]
	mov	cx,ds:[bx+2]
	mov	dx,ds:[bx]
	mov	ds:[bx+4],dx
	mov	ds:[bx],cx
	mov	ds:[bx+2],ax
	ret
FixDescriptor	endp

DoLoadAll	endp

MoveExtended286	endp
