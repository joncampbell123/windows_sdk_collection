;******************************************************************************
;   _cpu_type - return type of processor (386 vs. 8088/86/286).
;	This routine relies on Intel-approved code that takes advantage
;	of the documented behavior of the high nibble of the flag word
;	in the REAL MODE of the various processors.  The MSB (bit 15)
;	is always a one on the 8086 and 8088 and a zero on the 286 and
;	386.  Bit 14 (NT flag) and bits 13/12 (IOPL bit field) are
;	always zero on the 286, but can be set on the 386.
;
;	For future compatibility of this test, it is strongly recommended
;	that this specific instruction sequence be used.  The exit codes
;	can of course be changed to fit a particular need.
;
;	CALLABLE FROM REAL MODE ONLY - near ROUTINE
;
;	this routine was stolen from EMM/386 source code
;
;   ENTRY:  (none)
;   EXIT:   ax = 386 if 386
;	    ax = 8086 if 8086/8088
;	    ax = 0 otherwise
;   USED:   none
;   STACK:  6 bytes
;------------------------------------------------------------------------------
.model small

.CONST
FALSE	EQU	0
locvar1	EQU 	WORD PTR [BP-2]

.DATA

HMMEntryPt	dd	?


.code

        public  _cpu_type, _Ibm, _IsHimemDrvPresent, _QueryHimemExtMem
	public  _CallINT15, _A8086or8088

_cpu_type	proc	near
	pushf				; save entry flags
;
	xor	ax,ax			; 0000 into AX
	push	ax
	popf				; try to put that in the flags
	pushf
	pop	ax			; look at what really went into flags
	test	ax,08000h		;Q: was high bit set ?
        jnz     short Its8086           ;  Y: 8086/8088
	mov	ax,07000h		;  N: try to set the NT/IOPL bits
	push	ax
	popf				;      ... in the flags
	sti				; (for VDMM/IOPL0)
	pushf
	pop	ax			; look at actual flags
	test	ax,07000h		; Q: any high bits set ?
	jz	short IsNot386_exit	;   N: 80286
					;   Y: 80386
_cpu_type_exit:
	jmp	short I386_exit 	; and leave

Its8086:
	popf				; restore flags
        mov     ax,8086                 ; ax <-- 8086
	ret

IsNot386_exit:
	popf				; restore flags
	xor	ax,ax			; ax <-- 0
	ret

I386_exit:
	popf
        mov     ax,386
	ret				

_cpu_type	endp


; Determine if we are on a IBM machine
; Returns in AX:
;	1 - IBM PC
;	2 - IBM XT
;	3 - IBM AT
;	4 - IBM PS/2 Model 30
;	5 - IBM PS/2 Model 50 or 60
;	6 - IBM PS/2 Model 80
;	7 - IBM XT or AT

_Ibm	PROC

	PUSH	BP		; save old BP
	MOV	BP, SP		; set new BP
	SUB	SP, 2		; space for local
	MOV	locvar1, FALSE	; initialise local

	MOV	AH, 0C0h
	INT	15h       
	JC	IBMstep2 	; PC or XT or AT depending AH value
	ASSUME	ES:NOTHING

; Test the model byte. 
	CMP	BYTE PTR ES:[BX+2], 0FFh 	; IBM PC ?
	JE		YesPC
	CMP	BYTE PTR ES:[BX+2], 0FEh 	; IBM XT ?
	JE		YesXT
	CMP	BYTE PTR ES:[BX+2], 0FBh 	; IBM XT ?
	JE		YesXT
	CMP	BYTE PTR ES:[BX+2], 0FAh 	; IBM PS/2 Model 30 ?
	JE		Yes30
	CMP	BYTE PTR ES:[BX+2], 0F8h 	; IBM PS/2 Model 80 ?
	JE		Yes80

; Model byte is FCh for IBM AT or PC XT Model 286 or PS/2 Model 50 or 60 
; If not any of the above skip.

	CMP	BYTE PTR ES:[BX+2], 0FCh		
	JNE		IBMdone

; Model Byte is FC. Look at the submodel byte and 
; decide whether it is an AT( or PC XT Model 286) or PS/2 Model 50 or 60

	CMP	BYTE PTR ES:[BX+3], 04h	; IBM PS/2 Model 50 ?
	JE		Yes50
	CMP	BYTE PTR ES:[BX+3], 05h	; IBM PS/2 Model 60 ?
	JE		Yes60

; It is an AT.
	MOV	locvar1, 3	; indicate it is an AT.
	JMP	IBMdone		

YesPC:
	MOV	locvar1, 1 	; indicate it is a PC.
	JMP	IBMdone		

YesXT:
	MOV	locvar1, 2 	; indicate it is a XT.
	JMP	IBMdone		

Yes30:
	MOV	locvar1, 4 	; indicate it is a PS/2 Model 30
	JMP	IBMdone		

Yes50:
Yes60:
	MOV	locvar1, 5	; indicate it is a PS/2 Model 50 or 60.
	JMP	IBMdone		

Yes80:
	MOV	locvar1, 6	; indicate it is a PS/2 Model 80.
	JMP	IBMdone		

IBMstep2:

; depending on AH value decide whether it is a PC or XT or AT
	CMP	AH, 80h
	JE	YesPC		; can be a PCjr or PC.

	CMP	AH, 86h
	JNE	IBMdone
	MOV	locvar1, 7	; either an XT or AT

IBMdone:

	MOV	AX, locvar1

	MOV	SP, BP		; remove locals from stack.
	POP	BP		; Restore BP
	RET

_Ibm ENDP



; returns 1 in AX if HIMEM.SYS is already installed, 0 otherwise.

_IsHimemDrvPresent PROC
	PUSH	BP		; save old BP
	MOV	BP, SP		; set new BP

	mov	ax, 4300h
	int	2fh
	or	al, al 		; Is INT 2f hooked ?
	jz	NoHimem	

	xor	bx, bx
	mov	ax, 4310h
	int 	2fh
	or 	bx, bx		; bx guaranteed non-zero if HIMEM is already installed.
	jz	NoHimem

	mov	ax, 1
	pop 	bp
	ret
NoHimem:
	xor	ax, ax
	pop	bp
	ret
_IsHimemDrvPresent ENDP

_QueryHimemExtMem proc
	push	bp
	mov	bp, sp

	push	si
	push	di
	push	ss
	push	ds
	
	assume ds:DGROUP
	
	mov	ax, DGROUP
	mov	ds, ax

	xor 	bx, bx
	mov	ax, 4310h
	int	2fh
	mov	word ptr [HMMEntryPt], bx
	mov	word ptr [HMMEntryPt+2], es

	mov	ah, 8
	xor 	al, al
	call	[HMMEntryPt]

	; ax = # of K of available extended memory.
	
	pop	ds
	pop	ss
	pop	di
	pop	si

	pop	bp
	ret
_QueryHimemExtMem endp


; Does an INT 15, function 88
; returns in AX,
;		-1 if call was invalid
;		extended memory in K if call was valid
;

_CallINT15 proc

	push	bp
	mov	bp, sp

	push	si
	push	di
	push	ss
	push	ds

	clc 
	mov	ah, 88h
	int	15h
	jnc	CICarryNotSet
	
; INT 15 set carry flag.
	mov	ax, -1

CICarryNotSet:

	pop	ds
	pop	ss
	pop	di
	pop	si

	pop	bp
	ret

_CallINT15 endp
   
;   _A8086or8088
;   - return type of processor (386 vs. 8088/86/286).
;
;   RETURNS:   
;	    ax = 8086 if 8086/8088
;	    ax = 0 otherwise

_A8086or8088 proc near
	push	bp
	mov	bp, sp

	pushf				; save entry flags

	xor	ax,ax			; AX = 0
	push	ax
	popf				; try to 0 in the flags
	pushf
	pop	ax			; look at what really went into flags
	test	ax,08000h		; Q: was high bit set ?
        jnz     short A8086Done           ;   Y: 8086/8088
	mov	ax,0			    
	
	popf
	pop	bp
	ret

A8086Done:
	popf				; restore flags
        mov     ax,8086                 ; AX = 8086
	pop	bp
	ret

_A8086or8088 endp

end
