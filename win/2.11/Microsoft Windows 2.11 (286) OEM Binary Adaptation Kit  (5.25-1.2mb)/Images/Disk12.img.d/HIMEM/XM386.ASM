;**************************************************************************
;*	* * * 386 PROTECT info * * *
;**************************************************************************

;*	* 386 Descriptor template
DESC386	STRUC
limDesc386	dw	0		; limit bits (0..15)
LO_apDesc386	dw	0		; base bits (0..15)
MID_apDesc386	db	0		; base bits (16..23)
accessDesc386	db	0		; accessDesc386 byte
granDesc386	db	0		; granularity byte
HI_apDesc386	db	0		; base bits (24..31)
DESC386	ENDS

GDT386	STRUC
limitGdt	dw	?
LO_apBaseGdt	dw	?
HI_apBaseGdt	dw	?
GDT386	ENDS

Zero segment use32 at 0
	org	13*4
Int13Vector	label	dword
Zero	ends

;MEM3_Code segment	 use16 para public 'CODE'
;MEM3_Code ends

;codeg	 group	 code, MEM3_Code

;MEM3_Code segment
	assume	cs:code, ds:code, es:nothing

MEM3_Start	label	byte	; Start of relocatable code
;MEM3_Data	 label	 byte	 ; Start of our data

MEM3_Offset	=	offset code:MEM3_Data-offset code:MEM3_Start

OurGDT	equ	byte ptr ($+MEM3_Offset)		; Simple GDT
		DESC386	<>
descCS	equ	byte ptr ($+MEM3_Offset)
		DESC386	<0FFFFh,0,0,09Fh,0,0>		; Conforming CS
descRealBig	equ	byte ptr ($+MEM3_Offset)
		DESC386	<0FFFFh,0,0,093h,0cfh,0>	; Page Granularity
							; 4Gb Limit
GDTLen	EQU ($+MEM3_Offset-OurGDT)

GDTPtr	equ	qword ptr ($+MEM3_Offset)
		GDT386 <GDTLen,0,0>

OldInt13	equ	dword ptr ($+MEM3_Offset)
		dd	0			; Old contents of int 13 vector

GDTMoveBlock	equ byte ptr ($+MEM3_Offset)
						; 386 Template OK for Move Block
		DESC386	<>			; Nul Descriptor
		DESC386	<>			; GDT Descriptor
descSource	equ byte ptr ($+MEM3_Offset)
		DESC386 <0FFFFh,0,0,93h,0,0>	; Source Segment Descriptor
descDest	equ byte ptr ($+MEM3_Offset)
		DESC386 <0FFFFh,0,0,93h,0,0>	; Destination Segment Descriptor
		DESC386	<>			; BIOS use
		DESC386	<>			; BIOS use

;*******************************************************************************
;
; MoveExtended386
;	XMM Move Extended Memory Block for the 80386
;
; Entry:
;	ES:BX	Points to structure containing:
;		bCount		dd	?	; Length of block to move
;		SourceHandle	dw	?	; Handle for souce
;		SourceOffset	dd	?	; Offset into source
;		DestHandle	dw	?	; Handle for destination
;		DestOffset	dd	?	; Offset into destination
;
; Return:
;	AX = 1	Success
;	AX = 0	Failure
;		Error code in BL
;
; Registers Destroyed:
;	Flags
;
;-------------------------------------------------------------------------------
MoveExtended386	proc	near

	sti					; Be nice
	push	bp				; Set up stack frame so we
	mov	bp, sp				; can have local variables
	sub	sp, 18
Count	= -4					; Local DWORD for byte count
Return	= -6					; Local WORD for return code
SrcHandle = -8
DstHandle = -10
SrcLinear = -14
DstLinear = -18
	push	eax				; Save upper word of registers
	push	ecx
	push	esi
	push	edi
	push	bx

	xor	ax, ax
	mov	[bp.Return], ax			; Assume success
	mov	[bp.SrcHandle], ax
	mov	[bp.DstHandle], ax
	mov	ecx, es:[si.bCount]
	mov	[bp.Count], ecx

	shr	dword ptr [bp.Count], 1		; No odd byte counts
	jc	MEM3_InvCount
	jz	MEM3_Exit			; Exit immediately if zero

	lea	bx, [si.SourceHandle]		; Normalize Source
	call	GetLinear386			; Linear address in edi
	jc	MEM3_SrcError			; Have Dest Error Code
	xchg	esi, edi			; Save source address in ESI
	mov	[bp.SrcHandle], bx		; Save Handle for Unlock

	lea	bx, [di.DestHandle]
	call	GetLinear386			; Normalize Destination
	jc	MEM3_Error
	mov	[bp.DstHandle], bx		; Save Handle for Unlock

	smsw	ax
	shr	ax, 1				; Protected mode?
	jc	MEM3_MoveBlock			;   if so, use int 15h
						; Must preserve DS

	call	word ptr ControlJumpTable[5*2]	; Call LocalEnableA20()
	cmp	ax, 1
	jne	MEM3_Error

	xor	cx, cx
	mov	es, cx
assume	es:Zero
	mov	ax, cs
	shl	eax, 16
	mov	ax, offset code:Int13Handler+MEM3_Offset
	cli
	push	[OldInt13]			; For reentrancy
	xchg	eax, [Int13Vector]		; Install our int 13 handler
	mov	[OldInt13], eax
	sti

	push	ds
	mov	ds, cx
assume	ds:Zero
	mov	ecx, [bp.Count]
	shr	ecx, 1				; Now DWORD count
						; Odd word count in carry
		; Now we have:
		;	ESI = 32 bit Source Linear Address
		;	EDI = 32 bit Destination Linear Address
		;	DS = ES = 0
		; If the limit of DS or ES is still the Real Mode
		; default of 64k and ESI or EDI is greater than 64k,
		; these instructions will fault with an int 13.
		; In this case, our int 13 handler will set up
		; the descriptors to have 4Gb limits (real big mode)
		; and will iret to the faulting instruction.

		; The following persuades masm to output
		; both a 66h and 67h prefix
Fault0:
	rep movs dword ptr [esi], dword ptr [edi]	; DWORDS first
			; THE NEXT INSTRUCTION MUST HAVE ADDRESS SIZE OVERRIDE
	db	67h		; CHIP BUG - DO NOT REMOVE
	nop			; CHIP BUG - DO NOT REMOVE

	rcl	ecx, 1
Fault1:
	rep movs word ptr [esi], word ptr [edi]		; Now the odd word
			; THE NEXT INSTRUCTION MUST HAVE ADDRESS SIZE OVERRIDE
	db	67h		; CHIP BUG - DO NOT REMOVE
	nop			; CHIP BUG - DO NOT REMOVE

	pop	ds
assume	ds:code
	pop	eax				; saved [OldInt13]
	cli					; NECESSARY
	xchg	eax, [OldInt13]	; OldInt13 potentially INVALID
	mov	[Int13Vector], eax		; Deinstall our handler
	sti

	call	word ptr ControlJumpTable[6*2]	; Call LocalDisableA20()
	cmp	ax, 1
	jne	short MEM3_Error
	
MEM3_Exit:
	mov	bx, [bp.SrcHandle]		; Unlock Handles if necessary
	or	bx, bx
	jz	short MEM3_NoSrcHandle
	dec	[bx.cLock]			; Unlock Source
MEM3_NoSrcHandle:
	mov	bx, [bp.DstHandle]
	or	bx, bx
	jz	short MEM3_NoDstHandle
	dec	[bx.cLock]			; Unlock Destination
MEM3_NoDstHandle:
	pop	bx				; Restore original registers
	pop	edi
	pop	esi
	pop	ecx
	pop	eax
	mov	ax, 1
	cmp	word ptr [bp.Return], 0
	je	short MEM3_Success
	dec	ax				; AX = 0 for error
	mov	bl, byte ptr [bp.Return]
MEM3_Success:
	mov	sp, bp				; Unwind stack
	pop	bp
	ret

MEM3_InvCount:
	mov	bl, ERR_LENINVALID
	jmp	short MEM3_Error
MEM3_SrcError:
	cmp	bl, ERR_LENINVALID		; Invalid count
	je	short MEM3_Error		;   yes, no fiddle
	sub	bl, 2				; Convert to Source error code
MEM3_Error:
	mov	[bp.Return], bl
	jmp	short MEM3_Exit

;*******************************************************************************
;
; GetLinear386
;	Convert Handle and Offset (or 0 and SEG:OFFSET) into Linear address
;	Locks Handle if necessary
;	Nested with MoveExtended386 to access local variables
;
; Entry:
;	ES:BX	Points to structure containing:
;		Handle	dw
;		Offset	dd
;	ECX	Count of bytes to move
;
; Return:
;	BX	Handle of block (0 if conventional)
;	EDI	Linear address
;	CARRY	=> Error
;
; Registers Destroyed:
;	EAX
;
;-------------------------------------------------------------------------------

GetLinear386	proc	near
	cli					; NO INTERRUPTS
	mov	edi, dword ptr es:[bx+2]	; Offset from start of handle
	mov	bx, word ptr es:[bx]		; Handle in bx
	or	bx, bx
	jz	short GL3_Conventional

	cmp	[bx.Flags], USEDFLAG		; Valid Handle?
	jne	short GL3_InvHandle

	movzx	eax, [bx.Len]			; Length of Block
	shl	eax, 10				; now in bytes
	sub	eax, edi			; EAX = max possible count
	jb	short GL3_InvOffset		; Base past end of block
	cmp	eax, ecx
	jb	short GL3_InvCount		; Count too big

	inc	[bx.cLock]			; Lock Handle
	movzx	eax, [bx.Base]
	shl	eax, 10				; Base byte address
	add	edi, eax			; Linear address

GL3_OKExit:
	clc
	sti
	ret

GL3_Conventional:
	movzx	eax, di				; Offset in EAX
	shr	edi, 16
	shl	edi, 4				; Segment*16 in EDI
	add	edi, eax			; Linear address in EDI
	mov	eax, edi
	add	eax, ecx
	cmp	eax, 10FFF0h			; Max addressable inc. HMA
	jbe	short GL3_OKExit
GL3_InvCount:
	mov	bl, ERR_LENINVALID
	jmp	short GL3_Error
GL3_InvHandle:
	mov	bl, ERR_DHINVALID		; Dest handle invalid
	jmp	short GL3_Error
GL3_InvOffset:
	mov	bl, ERR_DOINVALID		; Dest Offset invalid
GL3_Error:
	stc
	sti
	ret
	
GetLinear386	endp

;*******************************************************************************
;
; Int13Handler
;	Handler for int 13 during our rep moves
;	If it is a real interrupt, jump to the old handler
;	If it is a fault, set Real Big Mode and return
;
; Entry:
;
; Return:
;
; Registers Destroyed:
;	BX, DS, ES if fault from one of our instructions, otherwise
;	NONE
;
;-------------------------------------------------------------------------------

Int13Handler	proc	far
assume	cs:code, ds:nothing, es:nothing
	push	bp
	mov	bp, sp			; Base to look at faulting address
	push	ax

	mov	al, 0Bh			; Party on PIC to see if interrupt
	out	20h, al
	in	al, 20h			; ISR
	test	al, 20h			; IRQ5, int 13
	jnz	short NotOurInt13

	mov	ax, cs
	cmp	[bp+4], ax		; Fault from our cs?
	jne	short NotOurInt13	;   no, SOMETHING IS FUNNY!
	cmp	word ptr [bp+2], offset code:Fault0+MEM3_Offset
	je	short LoadDescriptorCache
	cmp	word ptr [bp+2], offset code:Fault1+MEM3_Offset
	jne	short NotOurInt13	; Not one of our instructions ????

LoadDescriptorCache:
	mov	bx, descRealBig - OurGDT	; Special 4Gb selector
	lgdt	qword ptr cs:[GDTPtr]

	mov	eax, cr0
	or	al,1
	mov	cr0, eax			; Go into Protected Mode
						; NOTE: NMIs will kill us!!!

	db	0eah				; jmp far flush_prot
	dw	offset code:flush_prot+MEM3_Offset     ; Clears the prefetch
	dw	descCS - OurGDT	

flush_prot:
	mov	es, bx				; Set up the segments we want
	mov	ds, bx

	and	al, 0FEh
	mov	cr0, eax			; Return to Real Mode

	db	0EAH			; jmp far flush_real
	dw	offset code:flush_real+MEM3_Offset
patch3	equ word ptr ($+MEM3_Offset)
	dw	0

flush_real:
	xor	ax, ax
	mov	ds, ax
	mov	es, ax

	pop	ax
	pop	bp
	iret					; Back to faulting instruction

NotOurInt13:
	pop	ax
	pop	bp
	jmp	cs:[OldInt13]

Int13Handler	endp

;*******************************************************************************
;
; MEM3_MoveBlock
;	Set up GDT and call int 15h Move Block
;	Nested within MoveExtended386
;	See 80286 programmer's reference manual for GDT entry format
;	See Int 15h documentation for Move Block function
;
; Entry:
;	[BP.Count]	Word count for move
;	ESI		Linear address of the source
;	EDI		Linear address of the destination
;
;	Interrupts are ON
;
; Return:
;	CARRY	=> Error
;		Error code in BL
;
; Registers Destroyed:
;	Flags, EAX, ECX, ESI, EDI, ES
;
;-------------------------------------------------------------------------------
MEM3_MoveBlock:
assume	ds:code
	mov	[bp.SrcLinear], esi
	mov	[bp.DstLinear], edi
	mov	ax, ds
	mov	es, ax
assume	es:code

DMB_loop:
	mov	ecx, 512			; Do max of # words left or
	cmp	ecx, [bp.Count]			; or max Move Block allows
	jbe	short DMB0
	mov	ecx, [bp.Count]
DMB0:
	push	ecx
	lea	si, [GDTMoveBlock]	; Pointer to GDT for Block Move

	lea	di, [descSource.LO_apDesc386]	; Source Descriptor
	mov	eax, dword ptr [bp.SrcLinear]

	CLI					; No interrupts until int 15h
						; Allows reentrancy
	stosw
	shr	eax, 16
	stosb					; Source Descriptor done

	lea	di, [descDest.LO_apDesc386]	; Destination Descriptor
	mov	eax, dword ptr [bp.DstLinear]
	stosw
	shr	eax, 16
	stosb					; Destination Descriptor done

	clc				; MUST DO THIS, int 15h doesn't bother
	mov	ah, 87h			; Block Move - Assumes protect
	int	15h			; mode code will allow interrupts

	STI
	pop	ecx
	jc	short DMB_Error

	sub	[bp.Count], ecx
	jz	MEM3_Exit			; All done
	shl	ecx, 1				; Back to byte count
	add	[bp.SrcLinear], ecx		; Update source for next chunk
	add	[bp.DstLinear], ecx		; Update destination
	jmp	short DMB_loop

DMB_Error:
	xor	bh, bh
	mov	bl, al
	mov	bl, cs:[Int15Err][bx]		; Pick up correct error code
	jmp	MEM3_Error

Int15Err	equ	byte ptr ($+MEM3_Offset)
		db	0, ERR_PARITY, ERR_LENINVALID, ERR_A20

MoveExtended386	endp

MEM3_End	label	byte		; End of relocatable code
