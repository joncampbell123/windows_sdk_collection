.MODEL SMALL

.CONST
TRUE	EQU 	-1
FALSE	EQU 	0
bIRET 	EQU	0CFH		; Hex value of IRET instruction
locvar1	EQU 	WORD PTR [BP-2]

; constants used by _get_ext

CMOS_PORT	EQU	70H
CMOS_DATA	EQU	71H
CMOS_REG_D2	EQU	1AH
CMOS_EXT_STF	EQU	1718H



HercStatusPort	EQU	03BAH		; Hercules status port
idmask_112	EQU	00110000B	; ID mask for GB112
idcode_112	EQU	00010000B	; ID code for GB112

idmask_222	EQU	01110000B	; ID mask for GB222
idcode_222	EQU	01010000B	; ID code for GB222


segROM	SEGMENT AT 0F000H
segROM	ENDS

SCREEN	SEGMENT AT 0B800H
SCREEN	ENDS


;=====================================

;****** VGA I/O port definitions
pMiscEGA	equ	3C2h		; Miscellaneous output

pVGAEna 	equ	3C3h		; VGA enable register
fVGAEna 	equ	00000001b	;   Enable access to VGA

pDACWindx	equ	3C8h		; DAC Write index(R/W)
pDACRindx	equ	3C7h		; DAC Read index(RO)
pDACState	equ	pDACRindx	; DAC state
pDACData	equ	3C9h		; DAC data(3 successive accesses)
pDACMask	equ	3CAh		; DAC mask

pMiscRead	equ	3CCh		; Port to read Misc output register
pFeatRead	equ	3CAh		; Port to read Feature output register

xC_CTMiscRead	equ	0Fh		; CHIPS Misc output read index
xC_CTFeatRead	equ	0Eh		; CHIPS Feat output read index
xC_CT400	equ	0F9h		; CHIPS 400 line reg index
xG_CTCtl	equ	0F7h		; CHIPS control reg index

;=================================




;******************************************************************************
;
;   Macro to cause a delay in between I/O accesses to the same device.
;
;------------------------------------------------------------------------------

IO_Delay    macro
	jmp	$+2
	jmp	$+2
	endm

.DATA

pInt67Vector	dw	67h*4,0
VDVolName	db	'VDISK  V3.2'	; used by _IsCEMMIn routine.

HMMEntryPt	dd	?


.CODE

	PUBLIC _VideoAdapter, _EGAmemory, _CGAtype, _CTVGA
	PUBLIC _IsHimemDrvPresent, _HMAreaPresent, _HMAAllocated
	PUBLIC _Herc, _Detect8514
	PUBLIC _A386Machine, _get_ext, _CallINT15, _IsCEMMIn, _A8086or8088, _Ibm


;******************************************************************************
;   _A386machine - return type of processor (386 vs. 8088/86/286).
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
;   EXIT:   ax = 386H if 386
;	    ax = 0 otherwise
;   USED:   none
;   STACK:  6 bytes
;------------------------------------------------------------------------------
_A386machine proc
	PUSH	BP		; save old BP
	MOV	BP, SP		; set new BP
 
	pushf				; save entry flags

	xor	ax,ax			; 0000 into AX
	push	ax
	popf				; try to put that in the flags
	pushf
	pop	ax			; look at what really went into flags
	test	ax,08000h		;Q: was high bit set ?
	jnz	short IsNot386_exit	;  Y: 8086/8088
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

IsNot386_exit:
	popf				; restore flags
	xor	ax,ax			; ax <-- 0
	POP	BP		; Restore BP
	ret

I386_exit:
	popf
	mov	ax,0386H
	POP	BP		; Restore BP
	ret				; *** RETURN ***

_A386machine endp




; Determine the display adapter type
; Returns in AX:
;	1 - CGA
;	2 - MCGA
;	3 - EGA Mono
;	4 - EGA color
;	5 - VGA
;      -1 - test failed
;      -2 - unknown value in BL after the first test.
;
_VideoAdapter PROC

	PUSH	BP		 ; save old BP
	MOV	BP, SP		 ; set new BP
	SUB	SP, 2		 ; space for local
	MOV	locvar1, FALSE	 ; initialise local

	MOV	AX, 1A00h	 ; read display combination code
	INT	10h
	CMP	AL, 1Ah		 ; function supported ?
	JNE	Step2		 ; No, Try other tests.

; BL contains active display code.

	CMP	BL, 02h		 ; CGA ?
	JE	YesCGA

	CMP	BL, 04h		 ; EGA color ?
	JE	YesEGAcolor

	CMP	BL, 05h		 ; EGA monochrome ?
	JE	YesEGAmono

	CMP	BL, 07h		 ; VGA mono ?
	JE	YesVGA			
	
	CMP	BL, 08h		 ; VGA color ?
	JE	YesVGA

; WORD code : 		BL == 0Ah ==>	MCGA color
; IBM documentation: 	BL == 0Ah ==>   Reserved   , assume IBM is correct

;	CMP	BL, 0Ah		 ; MCGA color ?
;	JE	YesMCGA

	CMP	BL, 0Bh		 ; MCGA BW ?
	JE 	YesMCGA

	CMP	BL, 0Ch		 ; MCGA color ?
	JE	YesMCGA

; If we come here, we have some unknown value in BL. 
	MOV 	locvar1, -2
	JMP	VideoDone
;==========================================================================
YesCGA:
	MOV	locvar1, 1
	JMP	VideoDone

YesMCGA:
	MOV	locvar1, 2
	JMP	VideoDone

YesEGAmono:
	MOV	locvar1, 3
	JMP	VideoDone

YesEGAcolor:
	MOV	locvar1, 4
	JMP	VideoDone

YesVGA:
	MOV	locvar1, 5
	JMP	VideoDone
;==========================================================================

Step2:
	MOV	AH, 12h
	MOV	BL, 10h		 ; Get EGA info
	INT	10h
	CMP	BL, 10h
	JE	EGAabsent	 ; No EGA
	
	CMP	BH, 00h		 ; EGA color ?
	JE	YesEGAcolor

	CMP	BH, 01h		 ; EGA mono ?
	JE	YesEGAmono

; Error : invalid value in BH
	MOV	locvar1, -1	 ; indicate error in IBM algo
	JMP	VideoDone
	
EGAabsent:

; Determine if user has a CGA
; MCGA might pass this test too
; CGA memory starts at B8000h and it has 4K memory.
; Hence, memory locations 07FFEh and 03FFEh are the same if it is a CGA.
; Write 1234H at 07FFEh. Write 5678h at 03FFEh. 
; Now if 07FFEh is overwritten
; with 5678 it is a CGA.

	MOV AX, SCREEN
	MOV ES, AX
	ASSUME ES:SCREEN

; The following test MUST use word values.  Otherwise, it screws up on
; some monochrome systems.

	MOV	ES:[07FFEH],WORD PTR 1234H ; See if offsets 7FFEH and 3FFEH
	MOV	ES:[03FFEH],WORD PTR 5678H ; are the same - if so then we have
	CMP	ES:[07FFEH],WORD PTR 5678H ; a color card
	JNE	VideoDone		   ; No - skip
	JMP	YesCGA			; definitely a CGA

VideoDone:
	MOV	AX, locvar1

	MOV	SP, BP		 ; remove locals from stack
	POP	BP		 ; restore old BP.
	RET

_VideoAdapter ENDP


; Determine the amount of EGA memory.
; Call this only if you have an EGA
; Returns -1 on failure.
; on success, returns 0,1,2 or 3 for 64K, 128K, 192K and 256K memory

_EGAmemory PROC

	PUSH	BP		 ; save old BP
	MOV	BP, SP		 ; set new BP
	SUB	SP, 2		 ; space for local
	MOV	locvar1, -1	 ; initialise local

	MOV	AH, 12h		 ; Get EGA info
	MOV	BL, 10h
	INT	10h
	CMP	BL, 10h
	JE	EGAmem_Done

	MOV	AL, BL
	CBW
	MOV	locvar1, AX

EGAmem_Done:

	MOV	AX, locvar1

	MOV	SP, BP		 ; remove locals from stack
	POP	BP		 ; restore old BP.
	RET

_EGAmemory ENDP




; Marc Wilson's code in Win386 and some Win20 display driver code.
; Do this test only if we have a CGA.
; It returns a CGA type in AX,
;	0 - Couldn't determine type of CGA
;	1 - IDC	i.e. Compaq Plasma Display
;       2 - AT&T VDC 400 Monochrome adapter
;	3 - normal CGA, use cga.drv
;	4 - HP Mulitmode display, use multimod.drv

_CGAtype PROC

	PUSH	BP			; save old BP
	MOV	BP, SP			; set new BP
	SUB	SP, 2			; create space for local var
	MOV	locvar1, FALSE		; initiliase local var


; Check for HP Multimode

	mov	dx,03DFH		; Read ID port
	pushf				; must set the BUS to other than
	cli				; 41H since the prefetch queue may
	even				; cause the BUS to be in this state.
	nop				; If i/o port 3DFH does not exist as
	in	al,DX			; the case on some video cards, this
	nop				; read will return what ever is on the
	nop				; bus at the time.  The nops cause a
	nop				; 90H to be on the bus.  There are 8
	nop				; since that is the size of the 80286
	nop				; prefetch queue.
	nop
	nop
	nop
	popf
	cmp	AL,041H
	je	YesMultiMode

	mov	dx,017C6h		; IDC Environment Register Port
	in	al,dx			; Read Register
	cmp	al,11100110b		; Q: IDC signature?
	jz	YesIDC 			; Y: It is an IDC

; Check for existence of not wrapped memory at BC000
	MOV	AX, SCREEN
	MOV	ES, AX
	ASSUME	ES:SCREEN
	mov	ax, ES:0000h
	push	ax
	mov	ax, ES:4000h
	push	ax
	not	ax
	mov	ES:4000h, ax
	cmp	ax, ES:4000h		; Q: Memory exist at BC000?
	jne	CGAOnly			;   N: It must be normal CGA
	cmp	ax, ES:0000h		; Q: Wrap to B8000?
	jne	YesATTcard	 	;   N: Must be ATT card
	not	ax
	mov	ES:4000h, ax
	cmp	ax, ES:4000h		; Q: Wrap to B8000?
	jz	CGAOnly			;   Y: must be CGA card
	jmp	YesATTcard			;   N: Must be ATT card

YesIDC:
	mov	locvar1, 1		; Y: It is an IDC
	jmp	CGAtypeDone

YesATTcard:

	mov	locvar1, 2
	jmp	CGAtypeDone

CGAOnly:
	mov	locvar1, 3
	pop	ax
	mov	ES:0000h,ax
	pop	ax
	mov	ES:4000h,ax
	jmp	CGAtypeDone

YesMultiMode:
	mov	locvar1,4

CGAtypeDone:

	MOV 	AX, locvar1

	MOV	SP, BP
	POP BP	
	RET

_CGAtype ENDP


; Returns true if we have a Chips and Technologies 441 VGA.
; Before calling this routine confirm we have a VGA from VideoAdapter()
; This routine should be used only to distinguish IBM VGA from C&T VGA.
;
; returns in AX 
;      - TRUE if C & T VGA is present.
;      - FALSE otherwise

_CTVGA	PROC 
	
	PUSH	BP			; save old BP
	MOV	BP, SP			; set new BP
	SUB	SP, 2			; create space for local var
	MOV	locvar1, FALSE		; initiliase local var

    mov     dx,pMiscRead
    in	    al,dx
    push    ax
    mov     dl,0D4h
    mov     al,017h
    out     dx,al
    inc     dx
    in	    al,dx
    or	    al,10h
    out     dx,al
    dec     dx
    mov     al,0f9h
    out     dx,al
    inc     dx
    in	    al,dx
    and     al,07fh
    out     dx,al
    dec     dx
    mov     al,xC_CTMiscRead
    out     dx,al
    inc     dx
    in	    al,dx
    push    ax
    mov     dx,pMiscEGA
    mov     al,0A5h
    out     dx,al
    mov     dl,0D4h
    mov     al,xC_CTMiscRead
    out     dx,al
    inc     dx
    in	    al,dx
    cmp     al, 0A5h
    mov     dx,pMiscEGA
    jne     VDD_ibm_vga
    mov     al,055h
    out     dx,al
    mov     dl,0D4h
    mov     al,xC_CTMiscRead
    out     dx,al
    inc     dx
    in	    al,dx
    mov     dx,pMiscEGA
    cmp     al,055h
    jne     VDD_ibm_vga
    pop     ax
    out     dx, al
    add     sp,2
    cmp     ax,ax
    mov     locvar1, TRUE
    jmp     CTVGAdone

VDD_ibm_vga:
    add     sp,2
    pop     ax
    out     dx, al
    cmp     ax,ax
    clc
CTVGAdone:

	MOV	AX, locvar1
	
	MOV	SP, BP
	POP	BP
	RET

_CTVGA ENDP



;
;This is the controlling routine for the process.  If this routine returns
;a -1 return code, then the 8514 was not found.  If it returns a zero, 
;then there is an 8514 on the system.
;
;The way we do this is to first check if we have a PS/2 or compatible
;with a micro-channel BUS.  Then, we proceed to put each adaptor into 
;setup mode, one at a time until we've done all of them.  Then, we read 
;the pre-defined "adaptor ID" word.  If we find the adaptor ID, for 
;the 8514, we can conclude that it is present.
;
;This algorithm will NOT work for the experimental 8514 boards which were 
;made for "XT-BUS" machines.  Since these were never released to the general
;public, we can feel safe that it'll only be of the slightest inconvenience
;that this type of 8514 cannot be detected.
;            
;Entry:
;       CL contains a -1 (e.g. assume that there's not an 8514 present).
;Exit:
;       CL will contain a zero if the 8514 is present, otherwise, it'll
;       remain a -1.                                                   
;


_Detect8514              proc    near
	PUSH	BP		 ; save old BP
	MOV	BP, SP		 ; set new BP
	SUB	SP, 2		 ; space for local
	MOV	locvar1, FALSE	 ; initialise local

mov             ax,0c400h               ;ROM BIOS call: PROGRAMMABLE OPTION
                                        ;SELECT
int             15h                     ;
jc              Detect8514Exit    ;if carry, we're not a PS/2
mov             cx,8                    ;there are 8 possible slots
mov             bl,1                    ;start at slot 1

Detect8514Loop:
mov             ax,0c401h               ;ROM BIOS call: PROGRAMMABLE OPTION
                                        ;SETUP MODE
int             15h                     ;
jc              Detect8514Exit    ;if operation failed, get out now
mov             dx,100h                 ;this is the adaptor ID port (low byte)
in              al,dx                   ;get the low byte of the adaptor ID
mov             ah,al                   ;save it
inc             dx                      ;this is adaptor ID port (high byte)
in              al,dx                   ;get the high byte of the adaptor ID
mov             si,ax                   ;save the adaptor ID word
mov             ax,0c402h               ;ROM BIOS call: PROGRAMMABLE OPTION
                                        ;ENABLE MODE
int             15h                     ;
cmp             si,7fefh                ;is the adaptor ID == 8514/A?
je              Detect8514Present       ;yes! we've found it!
inc             bl                      ;nope, bump to the next adaptor
loop            Detect8514Loop          ;and continue the search
;
jmp short       Detect8514Exit          ;and get out
;

Detect8514Present:

;Now we must determine whether our 8514 is configured correctly (that is,
;does it have the optional 4 bit plane extension feature installed?).  We
;do this by reading the 8514's Control/Status register bit 7:

mov             dx,42e8h                ;this is the Control/Status port
in              ax,dx                   ;get the word of information
and             al,80h                  ;test the "option installed" bit
jz              Detect8514Exit    ;the option wasn't installed, return
                                        ;"8514 not found"

;The 8514's memory option was found. Return success:

mov	locvar1, TRUE                   ; return success.


Detect8514Exit:

	MOV	AX, locvar1

	MOV	SP, BP		 ; remove locals from stack
	POP	BP		 ; restore old BP.
	RET

_Detect8514              endp



; Check for a Hercules. We are checking for a Herc102, 112, 222.
; This is the best possible test since the Hercules does not have any 
; standard detection mechanism built into it.
; Taken from WORD code.
; Returns in AX
; 	0 - if it is not a Hercules.
;	1 - Herc 112
;	2 - Herc 222
;	3 - Herc 102

_Herc PROC

	PUSH	BP			; save old BP
	MOV	BP, SP			; set new BP
	SUB	SP, 2			; create space for local var
	MOV	locvar1, FALSE		; initiliase local var

	MOV	DX, HercStatusPort 	; Herc status port is 03BAh
	XOR	BX, BX			; Start with no 112 or 222 successes.
	MOV	CX, 100			; Get a majority of 100 tries
	
Check:

	IN	AL, DX
	AND	AL, idmask_222		; strip to just the 222 bits
	CMP	AL, idcode_222		; check for GB222 code
	JNE	Check_112		; No, Skip to test for 112
	INC 	BH			; Count another 222 success

Check_112:
	
	AND	AL, idmask_112		; strip further to just the 112 bits
	CMP	AL, idcode_112		; check for the GB112 id code
	JNE	CheckAgain		; No skip
	INC 	BL			; Count another 112 success

CheckAgain:
	
	LOOP	Check			; Loop back and check again

; 100 tries are over.
	CMP	BL, 50			; Did we get a majority of 112 hits ?
	JBE	CheckGB102		; If not check for a Herc GB102
	
; every time we get a 222 hit we get a 112 hit too.
; Hence it can be either a 112 or a 222 at this stage.
	MOV	locvar1, 1		; indicate it is a 112, this may be 
					; over written later
	CMP	BH, 50			; Did we get a majority for the 222 ?
	JBE	HercDone		; it is a Herc 112 only.
	MOV	locvar1, 2		; It is a Herc 222, overwrite locvar1.
	JMP	HercDone
	
CheckGB102:

; Bit 7 at port HercStatusPort changes pretty often on a Hercules. Check to see
; if it flips.

	MOV	DX, HercStatusPort		
	MOV	CX, 0FFFh		; # of times we read from port 03BA
	IN	AL, DX
	MOV	BL, AL

ReadAgain:

	IN	AL, DX			; Get current value of port 03BA
	XOR	AL, BL			; XOR to see if it changes
	OR	AL, AL			; Did it ?
	JS	Yes102			; Yes it is a Hercules
	LOOP	ReadAgain		; No, try again

	JMP	HercDone
Yes102:
	MOV	locvar1, 3		; indicate it is a Herc GB102

HercDone:

	MOV	AX, locvar1
	
	MOV	SP, BP
	POP	BP
	RET

_Herc ENDP

;
; Test for PS/2 specific mouse
; If we are on a PS2 IBM machine, use this routine to see mouse hardware
; is installed. If, it returns FALSE a serial mouse or some other mouse
; can still exist. use INT 33 to determine that. In fact call bMouse routine.
;
;
;_bMouseOnPS2 PROC
;	
;	PUSH	BP
;	MOV	BP, SP
;	SUB	SP, 2
;	MOV	locvar1, FALSE
;	
;        int     11h                 ; Get equipment info
;;        test    al, 00000100b       ; Q: Pointing device installed?
;        jz      bMouseOnPS2Done     ;    Y: OK.. we have a mouse
;                                    ;    N: Might be a serial mouse still!
;	MOV	locvar1, TRUE
;bMouseonPS2Done:
;	
;	MOV	AX, locvar1
;	MOV	SP, BP
;	POP	BP
;	RET
;
;_bMouseOnPS2 ENDP
;
;
;; returns TRUE if mouse present, FALSE otherwise.
;
;_bMouse PROC
;
;	PUSH	BP			; save old BP
;	MOV	BP, SP			; set new BP
;	SUB	SP, 2			; create space for local var
;	MOV	locvar1, FALSE		; initiliase local var
;
;	XOR 	AX, AX
;	INT	33h
;	TEST	AX, AX
;	JZ	bMouseDone
;	MOV	locvar1, TRUE
;
;bMouseDone:
;	
;	MOV	AX, locvar1
;	MOV	SP, BP
;	POP	BP
;	RET
;
;_bMouse ENDP
;



; very similar to IBM AT Technical Reference BIOS listing
; determine total extended memory by looking at the CMOS RAM.

CMOS_POPF	PROC NEAR
	IRET
CMOS_POPF	ENDP

CMOS_READ	PROC NEAR
	PUSHF
	ROL	AL, 1
	STC
	RCR	AL, 1
	CLI
	OUT	CMOS_PORT, AL
	NOP

	IN	AL, CMOS_DATA
	PUSH	AX
	MOV	AL, CMOS_REG_D2
	RCR	AL, 1
	OUT	CMOS_PORT, AL
	POP	AX
	PUSH	CS
	CALL	CMOS_POPF
	RET

CMOS_READ	ENDP

_get_ext PROC NEAR

	PUSH	BP		; save old BP
	MOV	BP, SP		; set new BP

	MOV	AX, CMOS_EXT_STF
	CALL	CMOS_READ
	XCHG	AL, AH
	CALL	CMOS_READ
  
	POP	BP		; Restore BP
	RET

_get_ext ENDP

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

; Call this routine only if HIMEM.SYS is known to be installed. 
; returns TRUE in AX if HMM Area is present, FALSE otherwise.
;

_HMAreaPresent proc
	push	bp
	mov	bp, sp

	push	si
	push	di
	push	ss
	push	ds
	
	assume ds:DGROUP
	
	mov	ax, DGROUP
	mov	ds, ax

	mov	ax, 4310h
	int	2fh
	mov	word ptr [HMMEntryPt], bx
	mov	word ptr [HMMEntryPt+2], es


	mov	ax, 0
	call	[HMMEntryPt]
	
	mov	ax, dx
	
	pop	ds
	pop	ss
	pop	di
	pop	si

	pop	bp
	ret
_HMAreaPresent endp

; HMM Area Allocated to somebody ?
; returns TRUE in AX if it is, FALSE otherwise.

_HMAAllocated proc
	push	bp
	mov	bp, sp

	push	si
	push	di
	push	ss
	push	ds
	
	assume ds:DGROUP
	
	mov	ax, DGROUP
	mov	ds, ax

	mov	ax, 4310h
	int	2fh
	mov	word ptr [HMMEntryPt], bx
	mov	word ptr [HMMEntryPt+2], es


	; Request HMM area.

	mov	ah, 1
	mov	al, 63

	call	[HMMEntryPt]

	or 	ax, ax
	jnz	HMAARelExit	; Yes: Not Allocated to anyone.
				;      Release it and exit.
	mov 	ax, 1
	jmp	short HMAAExit

HMAARelExit:
	mov	ah, 2
	call	[HMMEntryPt]

	mov	ax, 0

HMAAExit:	
	pop	ds
	pop	ss
	pop	di
	pop	si

	pop	bp
	ret
_HMAAllocated endp

; IsCEMMIn - return AX = 1 if CEMM version 3.2 is installed, 0 otherwise

VDVolLen	equ	11
VDVolOffset	equ	0C2h

_IsCEMMIn proc
	push	bp
	mov	bp, sp

	push	si
	push	di
	push	ss
	push	ds
	
	assume ds:DGROUP
	
	mov	ax, DGROUP
	mov	ds, ax

	les	di, dword ptr pInt67Vector
	; now es:di = 0000:67h*4

	mov	ax, es:[di][2]
	mov	es, ax
	mov	di, VDVolOffset

	; now es:di = [seg INT 67+C2h] address of VDISK label
	
	mov	si, offset VDVolName

	mov 	cx, VDVolLen
	rep	cmpsb
	jne	ICINoVDisk

	; Found it
	mov	ax, 1
	jmp	short ICIExit

ICINoVDisk:
	xor	ax, ax
ICIExit:
	pop	ds
	pop	ss
	pop	di
	pop	si

	pop	bp
	ret
_IsCEMMIn endp

;******************************************************************************
;   _A8086or8088
;   - return type of processor (386 vs. 8088/86/286).
;
;   RETURNS:   
;	    ax = 8086 if 8086/8088
;	    ax = 0 otherwise
;------------------------------------------------------------------------------

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
        jnz     short Its8086           ;   Y: 8086/8088
	mov	ax,0			    
	
	popf
	pop	bp
	ret

Its8086:
	popf				; restore flags
        mov     ax,8086                 ; AX = 8086
	pop	bp
	ret

_A8086or8088 endp
		
; Determine if we are on a IBM machine
; Returns in AX:
;	1 - IBM PC
;	2 - IBM XT
;	3 - IBM AT
;	4 - IBM PS/2 Model 30
;	5 - IBM PS/2 Model 50 or 60
; bug in model 50/60 detection - use the code from subs.asm in memset project
;	6 - IBM PS/2 Model 80
;	7 - IBM PC/XT Model 286
; bug in this too - use int 15/c0 to look at the sub-model byte

_Ibm	PROC

	PUSH	BP		
	MOV	BP, SP
	SUB	SP, 2				; space for local
	MOV	locvar1, FALSE	; initialise local


; Get the machine's model byte in AL

	MOV     AX,0FFFFh
	MOV     ES, AX
	assume	ES:NOTHING

	MOV     AL, BYTE PTR ES:[0Eh]

; Test the model byte. 

	CMP	AL, 0FFh 	; IBM PC ?
	JE	YesPC

	CMP	AL, 0FEh 	; IBM XT ?
	JE	YesXT

	CMP	AL, 0FBh 	; IBM XT ?
	JE	YesXT

	CMP	AL, 0FAh 	; IBM PS/2 Model 30 ?
	JE	Yes30

	CMP	AL, 0F8h 	; IBM PS/2 Model 80 ?
	JE	Yes80

; Model byte is FCh for IBM AT or PC XT Model 286 or PS/2 Model 50 or 60 
; If not any of the above skip.

	CMP	AL, 0FCh		
	JNE	IBMdone

; Model Byte is FC. Look at the submodel byte and 
; decide whether it is an AT or PC XT Model 286 or PS/2 Model 50 or 60

	MOV	AL, BYTE PTR ES:[0Fh]

	CMP	AL, 04h	; IBM PS/2 Model 50 ?
	JE	Yes50

	CMP	AL, 05h	; IBM PS/2 Model 60 ?
	JE	Yes60

	CMP	AL, 02h	; IBM PC/XT Model 286 ?
	JE	YesPCXT286

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

YesPCXT286:
	MOV	locvar1, 7	; PC/XT Model 286 

IBMdone:

	MOV	AX, locvar1

	MOV	SP, BP		; remove locals from stack.
	POP	BP		; Restore BP
	RET

_Ibm ENDP

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
	
END








