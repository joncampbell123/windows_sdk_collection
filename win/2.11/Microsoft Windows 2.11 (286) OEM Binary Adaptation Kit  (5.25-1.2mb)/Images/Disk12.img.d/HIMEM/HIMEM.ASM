;*****************************************************************************
;*									     *
;*  HIMEM.ASM -						    Chip Anderson    *
;*									     *
;*	Extended Memory Specification Driver -				     *
;*	    Copyright 1988, Microsoft Corporation			     *
;*									     *
;*	The HMA was originally envisioned by Ralph Lipe.		     *
;*	Original XMS spec designed by Aaron Reynolds, Chip Anderson, and     *
;*	Tony Gosling.  Additional spec suggestions by Phil Barrett and David *
;*	Weise of Microsoft, Ed McNierney of Lotus and Bob Smith of Qualitas. *
;*									     *
;*	MoveExtMemory function written by Tony Gosling.			     *
;*									     *
;*	AT&T 6300 support added by Bill Hall, Olivetti ATC, Cupertino, CA    *
;*	HP Vectra support added by Ralph Peterson, Hewlett Packard Corp.     *
;*									     *
;*****************************************************************************

XMSVersion	equ	0200h
HimemVersion	equ	0204h

; Version -
;
;   1.00	- Written					4/17/88
;   1.01	- Added Global/Temporary Enable of A20		4/18/88
;   1.02	- Don't use DOS to change interrupts		4/19/88
;		- Return 1 for success in Disable cases		4/19/88
;		- Prevent disables if A20 enabled at start	4/19/88
;		- Added INT 2F handler check			4/19/88
;		- Used smarter hooking routine			4/19/88
;   1.03	- Optimized some code				4/20/88
;		- Temporarily Disable properly			4/20/88
;		- Disable INT's while changing vars		4/20/88
;   1.04	- Check to see if A20 works during init		4/23/88
;		- Fixed PS/2 support				4/23/88
;   1.05	- Added QueryExtMemory and AllocExtMemory	4/25/88
;   1.06	- Never remove the INT 15h hook once it is in	4/27/88
;		- Changed ExtMemory calls to provide "full" MM	4/27/88
;		- Force A20 line off on PS2's at init time	4/27/88
;   1.07	- Added Multiple A20 Handler support		5/01/88
;		- Added popff macro				5/01/88
;		- REP MOVSW words not bytes			5/04/88
;   1.08	- Made popff relocateable			5/04/88
;		- Added support for the AT&T 6300 Plus		5/05/88
;		- Added support for the HP Vectra		5/06/88
;   1.09	- Added CEMM recognition code			5/16/88
;		- Don't automatically trash BX in HMMControl	5/17/88
;   1.10	- Change PS/2 ID routine to use Watchdog timer	6/02/88
;		- Changed CEMM recognition string to just VDISK 6/07/88
;		- Fixed 2 instance bug				6/14/88
;   1.11	- Changed INT 2F multiplex number to 43h	6/23/88
;		- Fixed HP Vectra support for older Vectras	6/23/88
;		- Fixed Block Move register trashing bug	6/25/88
;   2.00	- Updated to XMS v2.00				7/10/88
;		- Reworked initialization code			7/12/88
;		- Reworked reentrancy issues			7/13/88
;		- Finialized A20 handling			7/14/88
;		- Added parameter support			7/14/88
;   2.01	- Official version with Vecta and 6300 support	7/15/88
;   2.02	- Removed INT 1 from MoveExtMemory		7/19/88
;		- Fixed minor problems in QueryExtMemory	7/19/88
;   2.03	- Added 286 LoadAll and 386 Big Mode MoveBlock	8/04/88
;		- Added Compaq "Built-In" Memory support	8/05/88
;		- Fixed 64K-free Installation bug		8/05/88
;		- Change PS/2 detection code yet again		8/08/88
;		- Fixed "A20 On" Message bug (ugh)		8/09/88
;		- Fixed A20 Init testing code			8/09/88
;		- Removed 286 LoadAll for publication		8/10/88
;   2.04	- Restored 286 LoadAll				8/11/88
;		- Fixed AT&T 6300 Plus A20 Handler		8/16/88
;		- Aligned A20 handlers on word boundaries	8/17/88

	    name    Himem
	    title   'HIMEM.SYS - Microsoft XMS Device Driver'

;*--------------------------------------------------------------------------*
;*	EQUATES								    *
;*--------------------------------------------------------------------------*

DEFHANDLES		equ	32		; Default # of EMB handles
MAXHANDLES		equ	128		; Max # of EMB handles

cXMSFunctions		equ	12h		; = # of last function+1!

FREEFLAG		equ	00000001b	; EMB Flags
USEDFLAG		equ	00000010b
UNUSEDFLAG		equ	00000100b

; XMS Error Codes.
ERR_NOTIMPLEMENTED	equ	080h
ERR_VDISKFOUND		equ	081h
ERR_A20			equ	082h
ERR_GENERAL		equ	08Eh
ERR_UNRECOVERABLE	equ	08Fh

ERR_HMANOTEXIST		equ	090h
ERR_HMAINUSE		equ	091h
ERR_HMAMINSIZE		equ	092h
ERR_HMANOTALLOCED	equ	093h

ERR_OUTOMEMORY		equ	0A0h
ERR_OUTOHANDLES		equ	0A1h
ERR_INVALIDHANDLE	equ	0A2h
ERR_SHINVALID		equ	0A3h
ERR_SOINVALID		equ	0A4h
ERR_DHINVALID		equ	0A5h
ERR_DOINVALID		equ	0A6h
ERR_LENINVALID		equ	0A7h
ERR_OVERLAP		equ	0A8h
ERR_PARITY		equ	0A9h
ERR_EMBUNLOCKED		equ	0AAh
ERR_EMBLOCKED		equ	0ABh
ERR_LOCKOVERFLOW	equ	0ACh
ERR_LOCKFAIL		equ	0ADh

ERR_UMBSIZETOOBIG	equ	0B0h
ERR_NOUMBS		equ	0B1h
ERR_INVALIDUMB		equ	0B2h

.386p

; In order to address memory above 1 MB on the AT&T 6300 PLUS, it is
; necessary to use the special OS-MERGE hardware to activate lines
; A20 to A23.  However, these lines can be disabled only by resetting
; the processor.  The address to which to return after reset are placed
; at 40:A2, noted here as RealLoc1.

BiosSeg SEGMENT USE16 AT 40h	      ; Used to locate 6300 PLUS reset address

		org	00A2h
RealLoc1	dd	0

BiosSeg ends


; Macro to avoid the 286 POPF bug.  Performs a harmless IRET to simulate a
;   popf.  Some 286s allow interrupts to sneak in during a real popf.

popff	    macro
	    push    cs
	    call    pPPFIRet	; Defined as the offset of any IRET
	    endm

;*--------------------------------------------------------------------------*
;*	    SEGMENT DEFINITION						    *
;*--------------------------------------------------------------------------*

CODE	    SEGMENT PARA PUBLIC USE16 'CODE'

	    assume  cs:code,ds:code,es:code

	    org	    0

; The Driver Header definition.
Header		dd	-1	    ; Link to next driver, -1 = end of list
		dw	1010000000000000b
				    ; Device attributes, Non-IBM bit set
		dw	Strategy    ; "Stategy" entry point
		dw	Interrupt   ; "Interrupt" entry point
		db	'XMSXXXX0'  ; Device name


;****************************************************************************
;*									    *
;*  Data Structures and Global Variables -				    *
;*									    *
;****************************************************************************

; The driver Request Header structure definition.
ReqHdr struc
    ReqLen	db	?
    Unit	db	?
    Command	db	?
    Status	dw	?
    Reserved	db	8 dup (?)
    Units	db	?
    Address	dd	?
    pCmdLine	dd	?
ReqHdr ends

; An EMB Handle structure definition.
Handle struc			; Handle Table Entry
    Flags	db	?	; Unused/InUse/Free
    cLock	db	?	; Lock count
    Base	dw	?	; 32-bit base address in K
    Len		dw	?	; 32-bit length in K
Handle ends

; Extended Memory Move Block structure definition.
MoveExtendedStruc struc
    bCount	    dd	?	; Length of block to move
    SourceHandle    dw	?	; Handle for souce
    SourceOffset    dd	?	; Offset into source
    DestHandle	    dw	?	; Handle for destination
    DestOffset	    dd	?	; Offset into destination
MoveExtendedStruc ends

; The Global variables.
pPPFIRet	dw	PPFIRet ; The offset of an IRET for the POPFF macro
pReqHdr		dd	?	; Pointer to MSDOS Request Header structure
pInt15Vector	dw	15h*4,0 ; Pointer to the INT 15 Vector
PrevInt15	dd	0	; Original INT 15 Vector
PrevInt2F	dd	0	; Original INT 2F Vector
fHMAInUse	db	0	; High Memory Control Flag, != 0 -> In Use
fCanChangeA20	db	0	; A20 Enabled at start?
fHMAMayExist	db	0	; True if the HMA could exist at init time
fHMAExists	db	0	; True if the HMA exists
fVDISK		db	0	; True if a VDISK device was found
EnableCount	dw	0	; A20 Enable/Disable counter
fGlobalEnable	dw	0	; Global A20 Enable/Disable flag
KiddValley	dw	0	; The address of the handle table 
KiddValleyTop	dw	0	; Points to the end of the handle table
MinHMASize	dw	0	    ; /HMAMIN= parameter value
cHandles	dw	DEFHANDLES  ; # of handles to allocate

cImplementedFuncs db	cXMSFunctions-3	    ; Omit the UMB functions
					    ; and ReallocEMB

A20Handler	dw	0	; Offset of the A20 Handler

BIMBase 	dw	0	; Base address and Lenght of remaining Compaq
BIMLength	dw	0	;   Built-In Memory (set at Init time)

MemCorr		dw	0	; KB of memory at FA0000 on AT&T 6300 Plus.
				;      This is used to correct INT 15h,
				;      Function 88h return value.
OldStackSeg	dw	0	; Stack segment save area for 6300 Plus.
				;      Needed during processor reset.

;*--------------------------------------------------------------------------*
;*									    *
;*  Strategy -								    *
;*									    *
;*	Called by MS-DOS when ever the driver is accessed.		    *
;*									    *
;*  ARGS:   ES:BX = Address of Request Header				    *
;*  RETS:   Nothing							    *
;*  REGS:   Preserved							    *
;*									    *
;*--------------------------------------------------------------------------*

Strategy    proc    far

	    ; Save the address of the request header.
	    mov	    word ptr cs:[pReqHdr],bx
	    mov	    word ptr cs:[pReqHdr][2],es
	    ret

Strategy    endp


;*--------------------------------------------------------------------------*
;*									    *
;*  Interrupt -								    *
;*									    *
;*	Called by MS-DOS immediately after Strategy routine		    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   Return code in Request Header's Status field		    *
;*  REGS:   Preserved							    *
;*									    *
;*--------------------------------------------------------------------------*

Interrupt   proc    far

	    ; Save the registers including flags.
	    push    ax		    ; We cannot use pusha\popa because
	    push    bx		    ;	we could be on an 8086 at this point
	    push    cx
	    push    dx
	    push    ds
	    push    es
	    push    di
	    push    si
	    push    bp
	    pushf

	    ; Set DS=CS for access to global variables.
	    push    cs
	    pop	    ds

	    les	    di,[pReqHdr]	; ES:DI = Request Header

	    mov     bl,es:[di.Command]	; Get Function code in BL

	    or	    bl,bl		; Only Function 00h (Init) is legal
	    jz	    short IInit

	    cmp     bl,16		; Test for "legal" DOS functions
	    jle     short IOtherFunc

IBogusFunc: mov     ax,8003h		; Return "Unknown Command"
	    jmp     short IExit

IOtherFunc: xor     ax,ax		; Return zero for unsupported functions
	    jmp     short IExit

	    ; Initialize the driver.
IInit:	    call    InitDriver

	    les	    di,[pReqHdr]	; Restore ES:DI = Request Header

IExit:	    or	    ax,0100h		; Turn on the "Done" bit
	    mov	    es:[di.Status],ax	; Store return code

	    ; Restore the registers.
	    popff
	    pop	    bp
	    pop	    si
	    pop	    di
	    pop	    es
	    pop	    ds
	    pop	    dx
	    pop	    cx
	    pop	    bx
	    pop	    ax
	    ret

Interrupt   endp


;*--------------------------------------------------------------------------*
;*									    *
;*  Int2FHandler -							    *
;*									    *
;*	Hooks Function 43h, Subfunction 10h to return the		    *
;*	address of the High Memory Manager Control function.		    *
;*	Also returns 80h if Function 43h, Subfunction 0h is requested.	    *
;*									    *
;*  ARGS:   AH = Function, AL = Subfunction				    *
;*  RETS:   ES:BX = Address of XMMControl function (if AX=4310h)	    *
;*	    AL = 80h (if AX=4300)					    *
;*  REGS:   Preserved except for ES:BX (if AX=4310h)			    *
;*	    Preserved except for AL    (if AX=4300h)			    *
;*									    *
;*--------------------------------------------------------------------------*

Int2FHandler proc   far

	    sti				    ; Flush any queued interrupts

	    cmp	    ah,43h		    ; Function 43h?
	    jne     short I2FNextInt
	    or	    al,al		    ; Subfunction 0?
	    jne     short I2FNextSub	    ; No, continue

	    ; Indicate that an XMS driver is installed.
	    mov	    al,80h		    ; Return 80h in AL
PPFIRet:    iret			    ; Label sets up the POPFF macro

I2FNextSub: cmp	    al,10h		    ; Subfunction 10?
	    jne     short I2FNextInt	    ; No, goto next handler

	    ; Return the address of the XMS Control function in ES:BX.
	    push    cs
	    pop	    es
	    mov	    bx,offset XMMControl
	    iret

	    ; Continue down the Int 2F chain.
I2FNextInt: cli				    ; Disable interrupts again
	    jmp	    cs:[PrevInt2F]

Int2FHandler endp


;*--------------------------------------------------------------------------*
;*									    *
;*  ControlJumpTable -							    *
;*									    *
;*	Contains the address for each of the XMS Functions.		    *
;*									    *
;*--------------------------------------------------------------------------*

ControlJumpTable label word
		dw	Version			; Function 00h
		dw	RequestHMA		; Function 01h
		dw	ReleaseHMA		; Function 02h
		dw	GlobalEnableA20		; Function 03h
		dw	GlobalDisableA20	; Function 04h
		dw	LocalEnableA20		; Function 05h
		dw	LocalDisableA20		; Function 06h
		dw	IsA20On			; Function 07h
		dw	QueryExtMemory		; Function 08h
		dw	AllocExtMemory		; Function 09h
		dw	FreeExtMemory		; Function 0Ah
		dw	MoveBlock		; Function 0Bh
		dw	LockExtMemory		; Function 0Ch
		dw	UnlockExtMemory		; Function 0Dh
		dw	GetExtMemoryInfo	; Function 0Eh

		; We don't implement Realloc in this version.
;		dw	ReallocExtMemory	; Function 0Fh
	 
		; We don't implement the UMB functions.
;		dw	RequestUMB		; Function 14
;		dw	ReleaseUMB		; Function 15


;*--------------------------------------------------------------------------*
;*									    *
;*  XMMControl -							    *
;*									    *
;*	Main Entry point for the Extended Memory Manager		    *
;*									    *
;*  ARGS:   AH = Function, AL = Optional parm				    *
;*  RETS:   AX = Function Success Code, BL = Optional Error Code	    *
;*  REGS:   AX, BX, DX and ES may not be preserved depending on function    *
;*									    *
;*  INTERNALLY REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

XMMControl  proc   far

	    jmp	    short XCControlEntry	; For "hookability"
	    nop					; NOTE: The jump must be a 
	    nop					;  short jump to indicate
	    nop					;  the end of any hook chain.
						;  The nop's allow a far jump
						;  to be patched in.
XCControlEntry:
	    ; Preserve the following registers.
	    push    cx
	    push    si
	    push    di
	    push    ds
	    push    es
	    pushf

	    ; Save DS in ES.
	    push    ds
	    pop	    es		    ; NOTE: ES cannot be used for parms!
	    
	    ; Set DS equal to CS.
	    push    cs
	    pop	    ds
	    
	    ; Preserve the current function number.
	    push    ax

	    ; Is this a call to "Get XMS Version"?
	    or	    ah,ah
	    jz	    short XCCallFunc	  ; Yes, don't hook INT 15h yet
	    
	    ; Is this a valid function number?
	    cmp	    ah,[cImplementedFuncs]
	    jb	    short XCCheckHook
	    pop	    ax		    ; No, Un-preserve AX and return an error
	    xor	    ax,ax
	    mov	    bl,ERR_NOTIMPLEMENTED
	    jmp	    short XCExit

	    ; Is INT 15h already hooked?
XCCheckHook:pushf
	    cli			    ; This is a critical section
	    
	    cmp	    word ptr [PrevInt15][2],0	; Is the segment non-zero?
	    jne     short XCCheckVD

	    ; Try to claim all remaining extended memory.
	    call    HookInt15

	    ; Was a VDISK device found?
XCCheckVD:  popff		    ; End of critical section
	    cmp	    [fVDISK],0
	    je	    short XCCallFunc
	    pop	    ax		    ; Yes, Un-preserve AX and return an error
	    xor	    ax,ax
	    mov	    bl,ERR_VDISKFOUND
	    xor	    dx,dx
	    jmp	    short XCExit

	    ; Call the appropriate API function.	    
XCCallFunc: pop	    ax		    ; Restore AX
	    mov	    al,ah
	    xor	    ah,ah
	    shl	    ax,1
	    mov	    di,ax	    ; NOTE: DI cannot be used for parms!
	    
	    call    word ptr [ControlJumpTable+di]

	    ; Restore the preserved registers.
XCExit:	    popff		    ; NOTE: Flags must be restored immediately
	    pop	    es		    ;	after the call to the API functions.
	    pop	    ds
	    pop	    di
	    pop	    si
	    pop	    cx
	    ret

XMMControl  endp


;*--------------------------------------------------------------------------*
;*									    *
;*  Get XMS Version Number -				    FUNCTION 00h    *
;*									    *
;*	Returns the XMS version number					    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = XMS Version Number					    *
;*	    BX = Internal Driver Version Number				    *
;*	    DX = 1 if HMA exists, 0 if it doesn't			    *
;*  REGS:   AX, BX and DX are clobbered					    *
;*									    *
;*  INTERNALLY REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

Version	    proc    near

	    mov	    ax,XMSVersion
	    mov	    bx,HimemVersion
	    xor	    dh,dh

	    ; Is Int 15h hooked?
	    cmp	    word ptr [PrevInt15][2],0	; Is the segment non-zero?
	    jne     short VHooked
	    mov	    dl,[fHMAMayExist]		; No, return the status at
	    ret					;  init time.

VHooked:    mov	    dl,[fHMAExists]		; Yes, return the real status
	    ret

Version	    endp


;*--------------------------------------------------------------------------*
;*									    *
;*  HookInt15 -								    *
;*									    *
;*	Insert the INT 15 hook						    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None							    *
;*  REGS:   AX, BX, CS, DI, SI, and Flags are clobbered			    *
;*									    *
;*  EXTERNALLY NON-REENTRANT						    *
;*	Interrupts must be disabled before calling this function.	    *
;*									    *
;*--------------------------------------------------------------------------*

HookInt15   proc    near

	    push    es
    
	    ; Has a VDISK device been installed?	    
	    call    IsVDISKIn
	    cmp	    [fVDISK],0
	    je	    short HINoVD    ; No, continue
	    pop	    es		    ; Yes, return without hooking
	    ret
	    
HINoVD:	    mov	    ah,88h	    ; Is 64K of Extended memory around?
	    int	    15h
	    sub	    ax,[MemCorr]    ; 6300 Plus may have memory at FA0000h
	    cmp	    ax,64
	    jb	    short HIInitMemory	; Less than 64K free?  Then no HMA.
	    mov	    [fHMAExists],1

HIInitMemory:
	    ; Init the first handle to be one huge free block.
	    mov	    bx,[KiddValley]
	    mov	    [bx.Flags],FREEFLAG
	    mov	    [bx.Len],ax
	    mov	    [bx.Base],1024
	    cmp	    [fHMAExists],0
	    je	    short HICont
	    add     [bx.Base],64

	    ; See if any Compaq "Built In Memory" exists.
HICont:     mov     ax,[BIMBase]
	    or	    ax,ax
	    jz	    short HIHookEmHorns

	    mov     cx,[BIMLength]

	    ; Fill out the next handle entry.
	    add     bx,size Handle
	    mov	    [bx.Flags],FREEFLAG
	    mov     [bx.Len],cx
	    mov     [bx.Base],ax

	    ; Save the current INT 15 vector.
HIHookEmHorns:
	    les     si,dword ptr pInt15Vector

	    ; Exchange the old vector with the new one.
	    mov	    ax,offset Int15Handler
	    xchg    ax,es:[si][0]
	    mov	    word ptr [PrevInt15][0],ax
	    mov	    ax,cs
	    xchg    ax,es:[si][2]
	    mov	    word ptr [PrevInt15][2],ax
	    
	    pop	    es
	    ret

HookInt15   endp


;*--------------------------------------------------------------------------*
;*									    *
;*  IsVDISKIn -								    *
;*									    *
;*	Looks for drivers which use the IBM VDISK method of allocating	    *
;*  Extended Memory.  XMS is incompatible with the VDISK method.	    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None.  Sets "fVDISK" accordingly				    *
;*  REGS:   AX, CX, SI, DI and Flags are clobbered			    *
;*									    *
;*  INTERNALLY REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

pVDISK	    label   dword
	    dw	    00013h
	    dw	    0FFFFh
	    
szVDISK	    db	    'VDISK'	       

IsVDISKIn   proc    near

	    ; Look for "VDISK" starting at the 4th byte of extended memory.
	    call    LocalEnableA20		; Turn on A20

	    push    ds
	    push    es

	    ; Set up the comparison.
	    lds	    si,cs:pVDISK
	    push    cs
	    pop	    es
	    mov	    di,offset szVDISK
	    mov	    cx,5
	    cld
	    rep     cmpsb			; Do the comparison

	    pop     es				; Restore ES and DS
	    pop     ds

	    jz	    short IVIFoundIt
	    mov     [fVDISK],0			; No VDISK device found
	    jmp	    short IVIExit
	    
	    ; "VDISK" was found.
IVIFoundIt: mov	    [fVDISK],1

IVIExit:    call    LocalDisableA20
	    ret 				; Turn off A20

IsVDISKIn   endp


;*--------------------------------------------------------------------------*
;*									    *
;*  Int15Handler -							    *
;*									    *
;*	Hooks Function 88h to return zero as the amount of extended	    *
;*	memory available in the system.					    *
;*									    *
;*	Hooks Function 87h and preserves the state of A20 across the	    *
;*	block move.							    *
;*									    *
;*  ARGS:   AH = Function, AL = Subfunction				    *
;*  RETS:   AX = 0 (if AH == 88h)					    *
;*  REGS:   AX is clobbered						    *
;*									    *
;*--------------------------------------------------------------------------*

I15RegSave  dw	    ?
 
Int15Handler proc   far

	    ; Is this a request for the amount of free extended memory?
	    cmp	    ah,88h
	    jne     short I15HCont	; No, continue

	    xor     ax,ax		; Yes, return zero
	    iret

	    ; Is it a Block Move?
I15HCont:   cmp	    ah,87h
	    jne     short I15HNext	; No, continue

	    ; Int 15h Block Move:
	    cli 			; Make sure interrupts are off

	    ; Store the A20 line's state.
	    pusha			; Preserve the registers
	    call    IsA20On
	    mov	    cs:[I15RegSave],ax
	    popa			; Restore the registers

	    ; Call the previous Int 15h handler.
	    pushf			; Simualate an interrupt
	    call    cs:[PrevInt15]
	    pusha			; Preserve previous handler's return

	    ; Was A20 on before?
	    cmp	    cs:[I15RegSave],0
	    je	    short I15HExit	; No, continue
	    mov     ax,1
	    call    cs:[A20Handler]	; Yes, turn A20 back on

I15HExit:   popa			; Restore the previous handler's return
	    iret

	    ; Continue down the Int 15h chain.
I15HNext:   jmp     cs:[PrevInt15]

Int15Handler endp


;*--------------------------------------------------------------------------*
;*									    *
;*  RequestHMA -					    FUNCTION 01h    *
;*									    *
;*	Give caller control of the High Memory Area if it is available.	    *
;*									    *
;*  ARGS:   DX = HMA space requested in bytes				    *
;*  RETS:   AX = 1 if the HMA was reserved, 0 otherwise.  BL = Error Code   *
;*  REGS:   AX, BX and Flags clobbered					    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

RequestHMA  proc   near

	    cli			    ; This is a non-reentrant function.
				    ;	Flags are restored after the return.	    
	    mov	    bl,ERR_HMAINUSE
	    cmp	    [fHMAInUse],1   ; Is the HMA already allocated?
	    je	    short RHRetErr

	    mov	    bl,ERR_HMANOTEXIST
	    cmp	    [fHMAExists],0  ; Is the HMA available?
	    je	    short RHRetErr

	    mov	    bl,ERR_HMAMINSIZE
	    cmp	    dx,[MinHMASize] ; Is this guy allowed in?
	    jb	    short RHRetErr

	    mov	    ax,1
	    mov	    [fHMAInUse],al  ; Reserve the High Memory Area
	    xor     bl,bl	    ; Clear the error code
	    ret

RHRetErr:   xor     ax,ax	    ; Return failure with error code in BL
	    ret

RequestHMA  endp


;*--------------------------------------------------------------------------*
;*									    *
;*  ReleaseHMA -					    FUNCTION 02h    *
;*									    *
;*	Caller is releasing control of the High Memory area		    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if control is released, 0 otherwise.	 BL = Error Code    *
;*  REGS:   AX, BX and Flags clobbered					    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

ReleaseHMA  proc   near

	    cli			    ; This is a non-reentrant function
	    
	    ; Is the HMA currently in use?
	    mov	    al,[fHMAInUse]
	    or	    al,al
	    jz	    short RLHRetErr ; No, return error
	    
	    ; Release the HMA and return success.
	    mov	    [fHMAInUse],0
	    mov	    ax,1
	    xor	    bl,bl
	    ret
	    
RLHRetErr:  xor	    ax,ax
	    mov	    bl,ERR_HMANOTALLOCED
	    ret		   

ReleaseHMA  endp


;*--------------------------------------------------------------------------*
;*									    *
;*  GlobalEnableA20 -					    FUNCTION 03h    *
;*									    *
;*	Globally enable the A20 line					    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if the A20 line is enabled, 0 otherwise.  BL = Error	    *
;*  REGS:   AX, BX and Flags clobbered					    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

GlobalEnableA20 proc near

	    cli				; This is a non-reentrant function

	    ; Is A20 already globally enabled?
	    cmp	    [fGlobalEnable],1
	    je	    short GEARet

	    ; Attempt to enable the A20 line.
	    call    LocalEnableA20
	    or	    ax,ax
	    jz	    short GEAA20Err

	    ; Mark A20 as globally enabled.
	    mov	    [fGlobalEnable],1

	    ; Return success.
GEARet:	    mov	    ax,1
	    xor	    bl,bl
	    ret

	    ; Some A20 error occured.
GEAA20Err:  mov	    bl,ERR_A20
	    xor	    ax,ax
	    ret

GlobalEnableA20 endp


;*--------------------------------------------------------------------------*
;*									    *
;*  GlobalDisableA20 -					    FUNCTION 04h    *
;*									    *
;*	Globally disable the A20 line					    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if the A20 line is disabled, 0 otherwise.  BL = Error    *
;*  REGS:   AX, BX and Flags are clobbered				    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

GlobalDisableA20 proc near

	    cli				; This is a non-reentrant function

	    ; Is A20 already globally disabled?
	    cmp	    [fGlobalEnable],0
	    je	    short GDARet

	    ; Attempt to disable the A20 line.
	    call    LocalDisableA20
	    or	    ax,ax
	    jz	    short GDAA20Err

	    ; Mark A20 as globally disabled.
	    mov	    [fGlobalEnable],0

	    ; Return success.
GDARet:	    mov	    ax,1
	    xor	    bl,bl
	    ret

	    ; Some A20 error occured.
GDAA20Err:  mov	    bl,ERR_A20
	    xor	    ax,ax
	    ret

GlobalDisableA20 endp


;*--------------------------------------------------------------------------*
;*									    *
;*  LocalEnableA20 -					    FUNCTION 05h    *
;*									    *
;*	Locally enable the A20 line					    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if the A20 line is enabled, 0 otherwise.  BL = Error	    *
;*  REGS:   AX, BX and Flags clobbered					    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

LocalEnableA20 proc near

	    cli				; This is a non-reentrant function
	    
	    cmp	    [fCanChangeA20],1	; Can we change A20?
	    jne     short LEARet	; No, don't touch A20

	    ; Only actually enable A20 if the count is zero.
	    cmp	    [EnableCount],0
	    jne     short LEAIncIt

	    ; Attempt to enable the A20 line.
	    mov	    ax,1
	    call    [A20Handler]	; Call machine-specific A20 handler
	    or	    ax,ax
	    jz	    short LEAA20Err

LEAIncIt:   inc	    [EnableCount]

	    ; Return success.
LEARet:	    mov	    ax,1
	    xor	    bl,bl
	    ret

	    ; Some A20 error occurred.
LEAA20Err:  mov	    bl,ERR_A20
	    xor	    ax,ax
	    ret
	    
LocalEnableA20 endp


;*--------------------------------------------------------------------------*
;*									    *
;*  LocalDisableA20 -					    FUNCTION 06h    *
;*									    *
;*	Locally disable the A20 line					    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if the A20 line is disabled, 0 otherwise.  BL = Error    *
;*  REGS:   AX, BX and Flags are clobbered				    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

LocalDisableA20 proc near

	    cli				; This is a non-reentrant function
	    
	    cmp	    [fCanChangeA20],0	; Can we change A20?
	    je	    short LDARet	; No, don't touch A20

	    ; Make sure the count's not zero.
	    cmp	    [EnableCount],0
	    je	    short LDAA20Err

	    ; Only actually disable if the count is one.
	    cmp	    [EnableCount],1
	    jne     short LDADecIt

	    xor	    ax,ax
	    call    [A20Handler]	; Call machine-specific A20 handler
	    or	    ax,ax
	    jz	    short LDAA20Err

LDADecIt:   dec	    [EnableCount]

	    ; Return success.
LDARet:	    mov	    ax,1
	    xor	    bl,bl
	    ret

	    ; Some A20 error occurred.
LDAA20Err:  mov     bl,ERR_A20
	    xor     ax,ax
	    ret
			
LocalDisableA20 endp


;*--------------------------------------------------------------------------*
;*									    *
;*  IsA20On -						    FUNCTION 07h    *
;*									    *
;*	Returns the state of the A20 line				    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if the A20 line is enabled, 0 otherwise		    *
;*  REGS:   AX, CX, DI, SI and Flags clobbered				    *
;*									    *
;*  INTERNALLY REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

LowMemory   label   dword	    ; Set equal to 0000:0080
	    dw	    00080h
	    dw	    00000h

HighMemory  label   dword
	    dw	    00090h	    ; Set equal to FFFF:0090
	    dw	    0FFFFh

IsA20On	    proc    near

	    push    ds
	    push    es

	    lds	    si,cs:LowMemory	; Compare the four words at 0000:0080
	    les	    di,cs:HighMemory	;   with the four at FFFF:0090
	    mov	    cx,4
	    cld
	    repe    cmpsw

	    pop	    es
	    pop	    ds
	    xor	    ax,ax

	    jcxz    short IAONoWrap ; Are the two areas the same?
	    inc	    ax		    ; No, return A20 Enabled

IAONoWrap:  xor     bl,bl
	    ret			    ; Yes, return A20 Disabled

IsA20On	    endp


;*--------------------------------------------------------------------------*
;*									    *
;*  QueryExtMemory -					    FUNCTION 08h    *
;*									    *
;*	Returns the size of the largest free extended memory block in K	    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = Size of largest free block in K.  BL = Error code	    *
;*	    DX = Total amount of free extended memory in K		    *
;*  REGS:   AX, BX, DX, DI, SI and Flags clobbered			    *
;*									    *
;*  INTERNALLY REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

QueryExtMemory proc near

	    ; Init the error code in DL.
	    mov	    dl,ERR_OUTOMEMORY
    
	    ; Scan for the largest block marked FREE.
	    xor	    di,di		    ; DI = Max. size found so far
	    xor	    si,si		    ; SI = Total amount of free memory
	    mov	    bx,KiddValley
	    mov     cx,[cHandles]	    ; Loop through the handle table
QEMLoop:    cmp	    [bx.Flags],FREEFLAG
	    jne     short QEMBottom

	    ; Add this free block to the total.
	    add     si,[bx.Len] 	    ; CHANGED 7/19/88

	    ; Is this block larger?
	    mov	    ax,[bx.Len]
	    cmp	    di,ax
	    jae     short QEMBottom	    ; CHANGED 7/19/88
	    
	    ; Yes, save it away.
	    mov	    di,ax
	    xor     dl,dl		    ; We ain't Out o' Memory
	    
QEMBottom:  add	    bx,SIZE Handle
	    loop    QEMLoop

	    ; Setup the return.
	    mov	    ax,di
	    mov	    bl,dl		    ; Retrieve the error code
	    mov     dx,si		    ; CHANGED 7/19/88
	    ret

QueryExtMemory endp


;*--------------------------------------------------------------------------*
;*									    *
;*  AllocExtMemory -					    FUNCTION 09h    *
;*									    *
;*	Reserve a block of extended memory				    *
;*									    *
;*  ARGS:   DX = Amount of K being requested				    *
;*  RETS:   AX = 1 of successful, 0 otherwise.	BL = Error Code		    *
;*	    DX = 16-bit handle to the allocated block			    *
;*  REGS:   AX, BX, DX and Flags clobbered				    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

; Algorithm -
;
;   Scan the Handle Table looking for BOTH an unused handle and
;	a free block which is large enough:
;
;   1.	If both are found -
;	    Mark the free block as used and adjust its size.
;	    Make the unused handle a free block containing the remaining
;		unallocated portion of the original free block.
;
;   2.	If only an unused handle is found -
;	    We're out of memory.
;
;   3.	If only a properly sized free block is found -
;	    We only have one handle left.
;	    Mark the free block as used.  The requester gets all of the
;		block's memory.
;
;   4.	If neither are found -
;	    We're out of memory.

hFreeBlock	dw  ?
hUnusedBlock	dw  ?

AllocExtMemory proc near

	    cli 			; This is a non-reentrant function
	    
	    ; Scan the handle table looking for BOTH an unused handle and 
	    ;	a free block which is large enough.
	    xor	    ax,ax
	    mov	    [hFreeBlock],ax
	    mov	    [hUnusedBlock],ax
	    mov	    bx,KiddValley
	    mov     cx,[cHandles]	; Loop through the handle table

	    ; Have we already found a free block which is large enough?
AEMhLoop:   cmp     [hFreeBlock],0
	    jne     short AEMUnused	; Yes, see if this one is unused
	    
	    ; Is this block free?
	    cmp     [bx.Flags],FREEFLAG
	    jne     short AEMUnused	; No, see if it is unused
	    
	    ; Is it large enough?
	    cmp	    dx,[bx.Len]
	    ja	    short AEMNexth	; No, get the next handle

	    ; Save this guy away.
	    mov	    [hFreeBlock],bx
	    jmp	    short AEMBottom
	    
AEMUnused:  ; Have we already found an unused handle?
	    cmp     [hUnusedBlock],0
	    jne     short AEMNexth	; Yes, get the next handle

	    ; Is this block unused?
	    cmp     [bx.Flags],UNUSEDFLAG
	    jne     short AEMNexth	; No, get the next handle
	    
	    ; Save this guy away.
	    mov	    [hUnusedBlock],bx
			
	    ; Have we found what we are looking for?
	    cmp	    [hFreeBlock],0
	    je	    short AEMNexth
AEMBottom:  cmp	    [hUnusedBlock],0
	    jne     short AEMGotBoth	; Yes, continue
	    
AEMNexth:   ; Get the next handle
	    add	    bx,SIZE Handle
	    loop    AEMhLoop
	    
	    ; We are at the end of the handle table and we didn't find both
	    ; things we were looking for.  Did we find a free block?
	    mov     si,[hFreeBlock]
	    or	    si,si
	    jnz     short AEMRetSuc	; Yes, Case 3 - Alloc the entire block
	    
	    ; Did we find an unused handle?
	    cmp	    [hUnusedBlock],0
	    je	    short AEMOOHandles	; No, Case 4 - We're out of handles
	    
	    ; Case 2 - Is this a request for a zero-length handle?
	    or	    dx,dx
	    jnz     short AEMOOMemory	; No, we're out of memory
	    
	    ; Reserve the zero-length handle.
	    mov	    si,[hUnusedBlock]
	    mov	    [hFreeBlock],si
	    jmp	    short AEMRetSuc
	    
AEMGotBoth: ; We're at Case 1 above.
	    ;	Mark the free block as used (done below) and adjust its size.
	    ;	Make the unused handle a free block containing the remaining
	    ;	   unallocated portion of the original free block.
	    mov	    si,[hFreeBlock]
	    mov	    di,[hUnusedBlock]
	    
	    ; Unused.Base = Old.Base + request
	    mov	    ax,[si].Base
	    add	    ax,dx
	    mov	    [di].Base,ax
	    
	    ; New.Len = request
	    mov	    ax,dx
	    xchg    [si].Len,ax
	    
	    ; Unused.Len = Old.Len - request
	    sub	    ax,dx
	    mov	    [di].Len,ax
	    mov	    [di].Flags,FREEFLAG	    ; Unused.Flags = FREE
	    
AEMRetSuc:  ; Complete the allocation.
	    mov	    [si].Flags,USEDFLAG	    ; New.Flags = USED
	    mov	    dx,[hFreeBlock]
	    mov	    ax,1
	    xor	    bl,bl
	    ret

AEMOOMemory:mov	    bl,ERR_OUTOMEMORY
	    jmp	    short AEMErrRet
	    
AEMOOHandles:
	    mov	    bl,ERR_OUTOHANDLES
AEMErrRet:  xor	    ax,ax		    ; Return failure
	    mov	    dx,ax
	    ret		       
	    
AllocExtMemory endp


;*--------------------------------------------------------------------------*
;*									    *
;*  ValidateHandle -							    *
;*									    *
;*	Validates an extended memory block handle			    *
;*									    *
;*  ARGS:   DX = 16-bit handle to the extended memory block		    *
;*  RETS:   Carry is set if the handle is valid				    *
;*  REGS:   Preserved except the carry flag				    *
;*									    *
;*--------------------------------------------------------------------------*

ValidateHandle proc near

	    pusha			    ; Save everything
	    mov	    bx,dx		    ; Move the handle into BX

	    ; The handle must be equal to or above "KiddValley".
	    cmp	    bx,[KiddValley]
	    jb	    short VHOne
	    
	    ; The handle must not be above "KiddValleyTop".
	    cmp	    bx,[KiddValleyTop]
	    ja	    short VHOne
	    
	    ; (The handle-"KiddValley") must be a multiple of a handle's size.
	    sub	    dx,[KiddValley]
	    mov	    ax,dx
	    xor	    dx,dx
	    mov	    cx,SIZE Handle
	    div	    cx			    
	    or	    dx,dx		    ; Any remainder?		
	    jnz     short VHOne 	    ; Yup, it's bad

	    ; Does the handle point to a currently USED block?
	    cmp	    [bx.Flags],USEDFLAG
	    jne     short VHOne 	    ; This handle is not being used.
	    
	    ; The handle looks good to me...
	    popa			    ; Restore everything
	    stc 			    ; Return success
	    ret

VHOne:	    ; It's really bad.
	    popa			    ; Restore everything
	    clc 			    ; Return failure
	    ret
	    
ValidateHandle endp


;*--------------------------------------------------------------------------*
;*									    *
;*  FreeExtMemory -					    FUNCTION 0Ah    *
;*									    *
;*	Frees a block of extended memory which was allocated via AllocExt   *
;*									    *
;*  ARGS:   DX = 16-bit handle to the extended memory block		    *
;*  RETS:   AX = 1 if successful, 0 otherwise.	BL = Error code		    *
;*  REGS:   AX, BX, CX, DX, SI, DI and Flags clobbered			    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

FreeExtMemory proc near

	    cli 			    ; This is a non-reentrant function
	    
	    ; Make sure that the handle is valid.
	    call    ValidateHandle
	    jnc     short FEMBadh
	    mov	    si,dx		    ; Move the handle into SI
	    
	    ; Make sure that the handle points to a FREE block.
	    cmp	    [si].cLock,0
	    jne     short FEMLockedh
	    
	    ; Mark the block as FREE and cancel any locks.
	    mov	    [si].Flags,FREEFLAG
	    mov	    [si].cLock,0
	    
	    ; Is an adjacent block also free?
FEMScanIt:  mov	    bx,[si].Base	    ; BX = Bottom of block
	    mov	    ax,bx
	    add	    ax,[si].Len		    ; AX = Top of block
	    
	    ; Search for an adjacent FREE block.
	    mov	    di,[KiddValley]	    ; DI = Handle being scanned
	    
	    mov     cx,cHandles 	    ; Loop through the handle table
FEMLoopTop: cmp	    [di].Flags,FREEFLAG	    ; Is this block free?
	    jne     short FEMNexth	    ; No, continue

	    ; Is this block just above the one we are freeing?
	    mov	    dx,[di].Base
	    cmp     dx,ax
	    je	    short FEMBlockAbove     ; Yes, coaless upwards

	    ; Is it just below?
	    add	    dx,[di].Len
	    cmp     dx,bx
	    je	    short FEMBlockBelow     ; Yes, coaless downwards
	    
FEMNexth:   add	    di,SIZE Handle
	    loop    FEMLoopTop

	    ; No adjacent blocks to coaless.
	    mov     ax,1		    ; Return success
	    xor	    bl,bl
	    ret

	    ; Exchange the pointer to the "upper" and "lower" blocks.
FEMBlockBelow:
	    xchg    si,di

	    ; Move the free block above into the current handle.
FEMBlockAbove:
	    mov	    dx,[si].Len
	    add     dx,[di].Len 	    ; Combine the lengths
	    mov	    [si].Len,dx
	    mov     [di].Flags,UNUSEDFLAG   ; Mark old block's handle as UNUSED
	    jmp	    short FEMScanIt	    ; Rescan the list

FEMBadh:    mov	    bl,ERR_INVALIDHANDLE
	    jmp	    short FEMErrExit
	    
FEMLockedh: mov	    bl,ERR_EMBLOCKED
FEMErrExit: xor     ax,ax		    ; Return failure
	    ret
	    
FreeExtMemory endp	      


;*--------------------------------------------------------------------------*
;*									    *
;*  LockExtMemory -					    FUNCTION 0Ch    *
;*									    *
;*	Locks a block of extended memory				    *
;*									    *
;*  ARGS:   DX = 16-bit handle to the extended memory block		    *
;*  RETS:   AX = 1 of successful, 0 otherwise.	BL = Error code		    *
;*	    DX:BX = 32-bit linear address of the base of the memory block   *
;*  REGS:   AX, BX, DX and Flags clobbered				    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

LockExtMemory proc near

	    cli 			    ; This is a non-reentrant function
	    
	    ; Is the handle valid?
	    call    ValidateHandle
	    jnc     short LEMBadh
	    mov     bx,dx		    ; Move the handle into BX

	    ; Are we at some preposterously large limit?
	    cmp	    [bx.cLock],0FFh
	    je	    short LEMOverflow

	    ; Lock the block.
	    inc	    [bx.cLock]
	    
	    ; Return the 32-bit address of its base.
	    mov	    dx,[bx.Base]
	    mov	    bx,dx
	    shr	    dx,6
	    shl	    bx,10

	    ; Return success.
	    mov	    ax,1
	    ret
	    
LEMBadh:    mov	    bl,ERR_INVALIDHANDLE
	    jmp	    short LEMErrExit

LEMOverflow:mov	    bl,ERR_LOCKOVERFLOW
LEMErrExit: xor     ax,ax		    ; Return failure
	    mov	    dx,ax
	    ret 
	    
LockExtMemory endp


;*--------------------------------------------------------------------------*
;*									    *
;*  UnlockExtMemory -					    FUNCTION 0Dh    *
;*									    *
;*	Unlocks a block of extended memory				    *
;*									    *
;*  ARGS:   DX = 16-bit handle to the extended memory block		    *
;*  RETS:   AX = 1 if successful, 0 otherwise.	BL = Error code		    *
;*  REGS:   AX, BX and Flags clobbered					    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

UnlockExtMemory proc near

	    cli				; This is a non-reentrant function
    
	    ; Is the handle valid?
	    call    ValidateHandle
	    jnc     short UEMBadh
	    mov	    bx,dx		; Move the handle into BX

	    ; Does the handle point to a locked block?
	    cmp	    [bx.cLock],0
	    je	    short UEMUnlocked	; No, return error

	    ; Unlock the block.
	    dec	    [bx.cLock]

	    mov     ax,1		; Return success
	    xor     bl,bl
	    ret
	    
UEMUnlocked:mov	    bl,ERR_EMBUNLOCKED
	    jmp	    short UEMErrExit

UEMBadh:    mov	    bl,ERR_INVALIDHANDLE
UEMErrExit: xor	    ax,ax
	    ret		   

UnlockExtMemory endp


;*--------------------------------------------------------------------------*
;*									    *
;*  GetExtMemoryInfo -					    FUNCTION 0Eh    *
;*									    *
;*	Gets other information about an extended memory block		    *
;*									    *
;*  ARGS:   DX = 16-bit handle to the extended memory block		    *
;*  RETS:   AX = 1 if successful, 0 otherwise.	BL = Error code		    *
;*	    BH = EMB's lock count					    *
;*	    BL = Total number of unused handles in the system		    *
;*	    DX = EMB's length						    *
;*  REGS:   AX, BX, CX, DX and Flags clobbered				    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

GetExtMemoryInfo proc	near

	    cli 			    ; This is a non-reentrant function
	    
	    ; Is the handle valid?
	    call    ValidateHandle
	    jnc     short GEMBadh
	    mov     si,dx		    ; Move the handle into SI

	    ; Count the number of handles which are not currently being used.
	    xor     al,al
	    mov     bx,[KiddValley]
	    mov     cx,[cHandles]	    ; Loop through the handle table
GEMLoop:    cmp     [bx.Flags],USEDFLAG     ; Is this handle in use?
	    je	    short GEMNexth		  ; Yes, continue
	    inc     al			    ; No, increment the count
GEMNexth:   add     bx,SIZE Handle
	    loop    GEMLoop

	    ; Setup the return.
	    mov     dx,[si.Len] 	    ; Length in DX
	    mov     bh,[si.cLock]	    ; Lock count in BH
	    mov     bl,al
	    mov	    ax,1
	    ret
	    
GEMBadh:    mov	    bl,ERR_INVALIDHANDLE
	    xor	    ax,ax
	    ret

GetExtMemoryInfo endp
	    

;*--------------------------------------------------------------------------*
;*									    *
;*  ReallocExtMemory -					    FUNCTION 0Fh    *
;*									    *
;*	Reallocates a block of extended memory				    *
;*									    *
;*  ARGS:   DX = 16-bit handle to the extended memory block		    *
;*  RETS:   AX = 1 if successful, 0 otherwise.	BL = Error code		    *
;*  REGS:								    *
;*									    *
;*  INTERNALLY NON-REENTRANT						    *
;*									    *
;*--------------------------------------------------------------------------*

; FUNCTION NOT YET IMPLEMENTED


;*--------------------------------------------------------------------------*
;*									    *
;*  NOTE: RequestUMB and ReleaseUMB will not be implemented by HIMEM.	    *
;*									    *
;*--------------------------------------------------------------------------*


;*--------------------------------------------------------------------------*
;*									    *
;*  MoveExtMemory -					    FUNCTION 0Bh    *
;*									    *
;*	Copys a block of memory from anywhere to anywhere		    *
;*									    *
;*  ARGS:   ES:SI = Pointer to an Extended Memory Block Move Structure	    *
;*		(NOTE: Originally passed in as DS:SI)			    *
;*  RETS:   AX = 1 of successful, 0 otherwise.	BL = Error code.	    *
;*  REGS:   Everybody clobbered						    *
;*									    *
;*  INTERNALLY REENTRANT (believe it or not)				    *
;*									    *
;*--------------------------------------------------------------------------*

EVEN				; Must be word aligned.

MoveBlock:

MEM3_Data	label	byte	; Start of MoveBlock data
MoveBlock286:

include xm286.asm

EndMoveBlock286:

;*--------------------------------------------------------------------------*

MoveBlock386:

include xm386.asm

EndMoveBlock386:


;****************************************************************************
;*									    *
;* A20 Handler Section: 						    *
;*									    *
;* The Init code copies the proper A20 Handler in place just below the	    *
;*   proper MoveBlock routine.						    *
;*									    *
;* NOTE: A20 HANDLERS MUST ONLY HAVE RELATIVE JUMPS!  HOWEVER ANY CALLS TO  *
;*	 FUNCTIONS OUTSIDE OF THE HANDLER MUST BE NON-RELATIVE!	 The best   *
;*	 method is to call thru a variable such as ControlJumpTable[n].	    *
;*									    *
;****************************************************************************

;*--------------------------------------------------------------------------*
;*									    *
;*  AT_A20Handler -					    HARDWARE DEP.   *
;*									    *
;*	Enable/Disable the A20 line on non-PS/2 machines		    *
;*									    *
;*  ARGS:   AX = 0 for Disable, 1 for Enable				    *
;*  RETS:   AX = 1 for success, 0 otherwise				    *
;*  REGS:   AX, CX and Flags clobbered					    *
;*									    *
;*--------------------------------------------------------------------------*

EVEN
AT_A20Handler proc  near

	    or	    ax,ax
	    jz	    short AAHDisable

AAHEnable:  call    Sync8042	; Make sure the Keyboard Controller is Ready
	    jnz     short AAHErr

	    mov	    al,0D1h	; Send D1h
	    out	    64h,al
	    call    Sync8042
	    jnz     short AAHErr

	    mov     al,0DFh	; Send DFh
	    out	    60h,al
	    call    Sync8042
	    jnz     short AAHErr

	    ; Wait for the A20 line to settle down (up to 20usecs)
	    mov     al,0FFh	; Send FFh (Pulse Output Port NULL)
	    out	    64h,al
	    call    Sync8042
	    jnz     short AAHErr
	    jmp	    short AAHExit

AAHDisable: call    Sync8042	; Make sure the Keyboard Controller is Ready
	    jnz     short AAHErr

	    mov	    al,0D1h	; Send D1h
	    out	    64h,al
	    call    Sync8042
	    jnz     short AAHErr

	    mov	    al,0DDh	; Send DDh
	    out	    60h,al
	    call    Sync8042
	    jnz     short AAHErr

	    ; Wait for the A20 line to settle down (up to 20usecs)
	    mov     al,0FFh	; Send FFh (Pulse Output Port NULL)
	    out	    64h,al
	    call    Sync8042
	    
AAHExit:    mov     ax,1
	    ret
	    
AAHErr:     xor     ax,ax
	    ret
	    
AT_A20Handler endp


;*--------------------------------------------------------------------------*

Sync8042    proc    near

	    xor	    cx,cx
S8InSync:   in	    al,64h
	    and	    al,2
	    loopnz  S8InSync
	    ret

Sync8042    endp

EndAT_A20Handler:


;*--------------------------------------------------------------------------*
;*									    *
;*  PS2_A20Handler -					    HARDWARE DEP.   *
;*									    *
;*	Enable/Disable the A20 line on PS/2 machines			    *
;*									    *
;*  ARGS:   AX = 0 for Disable, 1 for Enable				    *
;*  RETS:   AX = 1 for success, 0 otherwise				    *
;*  REGS:   AX, CX and Flags clobbered					    *
;*									    *
;*--------------------------------------------------------------------------*

PS2_PORTA   equ	    0092h
PS2_A20BIT  equ	    00000010b

EVEN
PS2_A20Handler proc   near

	    or	    ax,ax
	    jz	    short PAHDisable

PAHEnable:  in	    al,PS2_PORTA    ; Get the current A20 state
	    test    al,PS2_A20BIT   ; Is A20 already on?
	    jnz     short PAHErr

	    or	    al,PS2_A20BIT   ; Turn on the A20 line
	    out	    PS2_PORTA,al

	    xor	    cx,cx	    ; Make sure we loop for awhile
PAHIsItOn:  in	    al,PS2_PORTA    ; Loop until the A20 line comes on
	    test    al,PS2_A20BIT
	    loopz   PAHIsItOn
	    jz	    short PAHErr    ; Unable to turn on the A20 line
	    jmp	    short PAHExit

PAHDisable: in	    al,PS2_PORTA    ; Get the current A20 state
	    and	    al,NOT PS2_A20BIT	; Turn off the A20 line
	    out	    PS2_PORTA,al

	    xor	    cx,cx	    ; Make sure we loop for awhile
PAHIsItOff: in	    al,PS2_PORTA    ; Loop until the A20 line goes off
	    test    al,PS2_A20BIT
	    loopnz  PAHIsItOff
	    jnz     short PAHErr    ; Unable to turn off the A20 line

PAHExit:    mov	    ax,1
	    ret
	    
PAHErr:	    xor	    ax,ax
	    ret		   

PS2_A20Handler endp

EndPS2_A20Handler:


;*--------------------------------------------------------------------------*
;*									    *
;*  $6300Plus_A20Handler -				    HARDWARE DEP.   *
;*									    *
;*     Enable/Disable address lines A20-A23 on AT&T 6300 Plus		    *
;*									    *
;*  ARGS:   AX = 0 for Disable, 1 for Enable				    *
;*  RETS:   AX = 1 for success, 0 otherwise				    *
;*  REGS:   AX, BX, and Flags clobbered					    *
;*									    *
;*  Note:   Don't want to do two back to back disables on PLUS,		    *
;*	    so we call IsA20On to see if it is necessary.		    *
;*  Warning:  The calcuation of the ReturnBack label depends on the	    *
;*	      expectation that this routine is being moved at init time.    *
;*									    *
;*--------------------------------------------------------------------------*

PLUS_PORT   equ	    03F20h
PLUS_STATUS equ     03FA0h
PLUS_SET    equ     10000000b	; Turn on A20-A23
PLUS_RESET  equ     00010000b	; Turn off A20-A23 and point to our routine

EVEN
$6300PLUS_A20Handler proc   near
            mov     bx,ax
            push    dx
            mov     dx,PLUS_STATUS
            in      al,dx
            pop     dx
            and     ax,1
            cmp     ax,bx
            jne     short $6AHEnable
	    mov     ax,1
            ret                             ; No, just return

$6AHEnable: 
            pushf
            sti
            mov     al,PLUS_SET
            or      bx,bx                   ; Zero if disable
            jnz     short $6AHNext
            mov     al,PLUS_RESET

$6AHNext:   push    dx                      ; Set/reset the port
            mov     dx,PLUS_PORT
            out     dx,al
            pop     dx
            or      bx,bx
            jnz     short $6AHNext1
            call    $6300Reset              ; Reset the processor
$6AHNext1:
            popff
            mov ax,1
            ret

$6300Plus_A20Handler endp


;*--------------------------------------------------------------------------*
;*									    *
;* $6300Reset -						   HARDWARE DEP.    *
;*									    *
;* Reset the 80286 in order to turn off the address lines on the 6300 PLUS. *
;* This is the only way to do this on the current hardware.		    *
;* The processor itself is reset by reading or writing port 03F00h.	    *
;*									    *
;*  Uses flags.								    *
;*									    *
;*--------------------------------------------------------------------------*

$6300Reset  proc    near

	    pusha			    ; Save world
	    push    ds			    ; Save segments
	    push    es
	    mov	    ax,BiosSeg		    ; Point to the BIOS segment
	    mov	    ds,ax		    ; ds -> 40h

	    ; Setup the reset return address.
assume ds:nothing
	    push    word ptr ds:[RealLoc1]  ; Save what might have been here
	    push    word ptr ds:[RealLoc1+2]

            ; Load our return address, remembering that we will be relocated
            ;   at init time.
            mov     word ptr ds:[RealLoc1+2],cs
            mov     ax,cs:[A20Handler]
            add     ax,offset ReturnBack-offset $6300Plus_A20Handler
            mov     word ptr ds:[RealLoc1],ax 
            mov     cs:[OldStackSeg],ss     ; Save the stack segment, too

	    ; Reset the processor - turning off A20 in the process.
	    mov	    dx,03F00h
	    in	    ax,dx

	    ; We shouldn't get here.  Halt the machine if we do.
	    nop
	    nop
	    nop
	    nop
	    cli
	    hlt

ReturnBack: mov	    ss,cs:[OldStackSeg]		; Start the recovery
	    pop	    word ptr ds:[RealLoc1+2]	; ROM code has set ds->40h
	    pop	    word ptr ds:[RealLoc1]
	    pop	    es
	    pop	    ds

assume ds:code
            xor     al,al
            mov     dx,PLUS_PORT
            out     dx,al
            popa
            ret

$6300Reset endp

End6300Plus_Handler:


;*--------------------------------------------------------------------------*
;*									    *
;*  HP_A20Handler -					    HARDWARE DEP.   *
;*									    *
;*	Enable/Disable the A20 line on HP Vectra machines		    *
;*									    *
;*  ARGS:   AX = 0 for Disable, 1 for Enable				    *
;*  RETS:   AX = 1 for success, 0 otherwise				    *
;*  REGS:   AX, CX and Flags clobbered					    *
;*									    *
;*--------------------------------------------------------------------------*

EVEN
HP_A20Handler proc   near

	    or	    ax,ax
	    jz	    short HAHDisable

HAHEnable:  call    HPSync8042	; Make sure the Keyboard Controller is ready
	    jnz     short HAHErr

	    mov	    al,0DFh	; Send DFh
	    out	    64h,al
	    call    HPSync8042
	    jnz     short HAHErr

	    mov	    al,0DFh	; Send second DFh
	    out	    64h,al
	    call    HPSync8042
	    jnz     short HAHErr
	    jmp	    short HAHExit

HAHDisable: call    HPSync8042	  ; Make sure the Keyboard Controller is Ready
	    jnz     short HAHErr

	    mov	    al,0DDh	; Send DDh
	    out	    64h,al
	    call    HPSync8042
	    jnz     short HAHErr

	    mov	    al,0DDh	; Send second DDh
	    out	    64h,al
	    call    HPSync8042
	    jnz     short HAHErr

HAHExit:    mov	    ax,1
	    ret

HAHErr:	    xor	    ax,ax
	    ret
	    
HP_A20Handler endp


;*--------------------------------------------------------------------------*

HPSync8042  proc    near

	    xor	    cx,cx
H8InSync:   in	    al,64h
	    and	    al,2
	    loopnz  H8InSync
	    ret

HPSync8042  endp

EndHP_A20Handler:


;*--------------------------------------------------------------------------*
;*									    *
;*  Extended Memory Handle Table -					    *
;*									    *
;*--------------------------------------------------------------------------*

; Actually, the handle table will be located just after the end of whichever
; A20 Handler is installed and will overwrite the others.  In large cases
; however, it could extend beyond all of these guys and crunch the Init code.
; Hence this buffer.  If the driver ever gets over 64K (heaven forbid) this
; scheme should be reworked.

; Guarantee space for all handles.  Kinda overkill but any extra will be 
; discarded with the Init code.

HandleFiller	    dw	    MAXHANDLES * SIZE Handle DUP(?)


;****************************************************************************
;*									    *
;*  NOTE: Code below here will be discarded after driver initialization.    *
;*									    *
;****************************************************************************

;*--------------------------------------------------------------------------*
;*									    *
;*  InitDriver -							    *
;*									    *
;*	Called when driver is Initialized.				    *
;*									    *
;*  ARGS:   ES:DI = Address of the Request Header			    *
;*  RETS:   AX = 0, pHdr.Address = Bottom of resident driver code	    *
;*  REGS:   AX, CX and Flags are clobbered				    *
;*									    *
;*--------------------------------------------------------------------------*

InitLine    dw	    MoveBlock	    ; The line which defines what will be dis-
				    ;	carded after the driver is initialized
InitDriver  proc    near

	    ; Display the sign on message.
	    mov	    ah,9h
	    mov	    dx,offset SignOnMsg
	    int	    21h

	    ; Is this DOS 3.00 or higher?
	    mov	    ah,30h
	    int	    21h			; Get DOS versions number
	    cmp	    al,3
	    jae     short IDCheckXMS
	    
	    mov	    dx,offset BadDOSMsg
	    jmp     short IDFlushMe2
			
	    ; Is another XMS driver already installed?
IDCheckXMS: mov     ax,4300h
	    int	    2Fh
	    cmp	    al,80h		; Is INT 2F hooked?
	    jne     short IDNotInYet
	    mov	    dx,offset NowInMsg
	    jmp     short IDFlushMe2

	    ; What kind of processor are we on?
IDNotInYet: call    MachineCheck
	    cmp	    ax,1		; Is it an 8086/8088?
	    jne     short IDProcOK
	    mov	    dx,offset On8086Msg
	    jmp     short IDFlushMe

	    ; Is there any Compaq BIM?
IDProcOK:
	    call    GetBIMMemory
	    or	    ax,ax
	    jz	    short IDNoBIM

	    ; How much extended memory is installed?
	    call    GetInt15Memory
	    cmp     ax,64		; Is there >= 64K of extended?
	    jb	    short IDNoHMA	; Nope - No HMA
	    jmp     short IDHMAOK	; Yup - continue

	    ; How much extended memory is installed?
IDNoBIM:    call    GetInt15Memory
	    cmp     ax,64		; Is there >= 64K of extended?
	    jae     short IDHMAOK	; Yup - continue

	    or	    ax,ax		; Is there any extended?
	    jnz     short IDNoHMA	; Yup - No HMA

	    ; We can't find any memory to manage.
	    mov	    dx,offset NoExtMemMsg
IDFlushMe2: jmp     short IDFlushMe
	    
	    ; There isn't enough for the HMA, but there is some extended
	    ; memory which we can manage.
IDNoHMA:    mov     dx,offset NoHMAMsg
	    mov	    ah,9h
	    int	    21h
	    jmp     short IDFinalInit

IDHMAOK:    mov     [fHMAMayExist],1

	    ; Install the proper MoveBlock function.
IDFinalInit:call    InstallMoveBlock

	    ; Install the proper A20 handler.
	    call    InstallA20
	    
	    ; Was A20 on already?
	    call    IsA20On
	    xor	    ax,1
	    mov	    [fCanChangeA20],al	; Yes, don't ever disable it
	    cmp     al,0
	    jne     short IDDiddler
	    
	    ; Display the "A20 Already On" message.
	    mov	    dx,offset A20OnMsg
	    mov     ah,9h		; CHANGED - 8/9/88
	    int	    21h
	    
	    ; Can we successfully diddle A20?	CHANGED - 8/9/88
IDDiddler:  call    LocalEnableA20	; Try to enable A20
	    or	    ax,ax		; Did we do it?
	    jz	    short IDBadA20	; Nope - phoosh ourselves

	    call    IsA20On		; Is A20 on?
	    or	    ax,ax
	    jz	    short IDBadA20	; Nope - phoosh ourselves

	    call    LocalDisableA20	; Try to disable A20
	    or	    ax,ax		; Did we do it?
	    jnz     short IDA20Cont	; Yup - continue

	    cmp     [fCanChangeA20],1	; Has A20 been permenantly enabled?
	    je	    short IDGetParms	; Yup - continue
	    jmp     short IDBadA20	; Nope - phoosh ourselves
	    
IDA20Cont:  call    IsA20On		; Is A20 off?
	    or	    ax,ax
	    jz	    short IDGetParms	; Yup - continue

	    ; A20 ain't doing it.
IDBadA20:   mov	    dx,offset BadA20Msg

	    ; Display the message in DX followed by the "Flush" message.
IDFlushMe:  mov	    ah,9h
	    int	    21h
	    mov	    dx,offset FlushMsg
	    mov	    ah,9h
	    int	    21h
	    
	    ; Discard the entire driver.
	    xor	    ax,ax
	    mov	    [InitLine],ax
	    les	    di,[pReqHdr]
	    mov	    es:[di.Units],al
	    jmp	    short IDReturn
	    
	    ; Process the command line parameters.
IDGetParms: call    GetParms
	    
	    ; Initialize the Handle Table.
	    call    InitHandles

	    ; Display the success messages.
	    cmp	    [fHMAMayExist],0
	    je	    short IDIgnition
	    mov	    dx,offset HMAOKMsg
	    mov	    ah,9h
	    int	    21h

	    ; "Turn On" the driver.
IDIgnition: call    HookInt2F

	    ; Discard the initialization code.
IDReturn:   les	    di,[pReqHdr]
	    mov	    ax,[InitLine]
	    mov	    word ptr es:[di.Address][0],ax
	    mov	    word ptr es:[di.Address][2],cs

	    ; Return success.
	    xor	    ax,ax
	    ret

InitDriver  endp

	    
;*--------------------------------------------------------------------------*
;*									    *
;*  HookInt2F -								    *
;*									    *
;*	Insert the INT 2F hook						    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None							    *
;*  REGS:   AX, SI, ES and Flags are clobbered				    *
;*									    *
;*  EXTERNALLY NON-REENTRANT						    *
;*	Interrupts must be disabled before calling this function.	    *
;*									    *
;*--------------------------------------------------------------------------*

HookInt2F   proc    near

	    ; Save the current INT 2F vector
	    cli
	    xor	    ax,ax
	    mov	    es,ax
	    mov	    si,2Fh * 4		    ; ES:SI = Address of 2F vector

	    ; Exchange the old vector with the new one
	    mov	    ax,offset Int2FHandler
	    xchg    ax,es:[si][0]
	    mov	    word ptr [PrevInt2F][0],ax
	    mov	    ax,cs
	    xchg    ax,es:[si][2]
	    mov	    word ptr [PrevInt2F][2],ax
	    sti
	    ret

HookInt2F   endp


;*--------------------------------------------------------------------------*
;*									    *
;*  MachineCheck -							    *
;*									    *
;*	Determines the CPU type.					    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if we're on an 8086/8088 or an 80186, 0 otherwise	    *
;*  REGS:   AX and Flags are clobbered					    *
;*									    *
;*--------------------------------------------------------------------------*

; NOTE: This is the "official" Intel method for determining CPU type.

MachineCheck proc   near

	    xor     ax,ax		; Move 0 into the Flags register
	    push    ax
	    popf
	    pushf			; Try and get it back out
	    pop     ax
	    and     ax,0F000h		; If the top four bits are set...
	    cmp     ax,0F000h
	    je	    short MC_8086	; ...it's an 8086 machine

	    mov     ax,0F000h		; Move F000h into the Flags register
	    push    ax
	    popf
	    pushf			; Try and get it back out
	    pop     ax
	    and     ax,0F000h		; If the top four bits aren't set...
	    jz	    short MC_80286	; ...it's an 80286 machine

	    ; We're on an 80386 machine
	    mov     ax,3
	    ret

MC_80286:   ; We're on an 80286 machine
	    mov     ax,2
	    ret

MC_8086:    ; We're on an 8086 machine
	    mov     ax,1
	    ret
	    
MachineCheck endp

	    
;*--------------------------------------------------------------------------*
;*									    *
;*  GetBIMMemory -							    *
;*									    *
;*	Look for Compaq 'Built In Memory' and add it to the pool of	    *
;*  available memory							    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = Amount of BIM memory found				    *
;*  REGS:   AX, BX, CX, and Flags are clobbered				    *
;*									    *
;*--------------------------------------------------------------------------*

; "Built In Memory" (BIM) starts at FE00:0000h and grows downward.  It is
; controlled by a data structure at F000:FFE0h.  Changing the data structure
; involves un-write-protecting the ROMs (!) by flipping bit 1 of 80C00000.

pBIMSTRUCT  equ     0FFE0h
AVAILABLE   equ     0		; Set to -1 if BIM isn't around
TOTALBIM    equ     2		; Total amount of BIM in the system
AVAILBIM    equ     4		; Amount of BIM available in paragraphs
LASTUSED    equ     6		; Paragraph address of last (lowest) used
				;  paragraph of BIM

pCOMPAQ     label   dword
	    dw	    0FFE8h
	    dw	    0F000h
	    
szCOMPAQ    db	    '03COMPAQ'

pRAMRELOC   label   dword
	    dw	    00000h
	    dw	    080C0h

GDT_TYPE    struc
	    dw	    0,0,0,0
	    dw	    0,0,0,0

S_LIMIT     dw	    1
S_BASE_LOW  dw	    0
S_BASE_HI   db	    0
S_RIGHTS    db	    93h
S_RESERVED  dw	    0

D_LIMIT     dw	    1
D_BASE_LOW  dw	    0000h
D_BASE_HI   db	    0C0h
D_RIGHTS    db	    93h
D_RES386    db	    0
D_BASE_XHI  db	    080h

	    dw	    0,0,0,0
	    dw	    0,0,0,0
GDT_TYPE    ends

BIMGDT	    GDT_TYPE <>

BIMBuffer   dw	    ?


GetBIMMemory proc near

	    xor     ax,ax

	    ; Are we on a Compaq 386 machine?
	    push    es

	    ; Set up the comparison.
	    les     di,cs:pCOMPAQ
	    mov     si,offset szCOMPAQ
	    mov     cx,8
	    cld
	    rep     cmpsb		    ; Do the comparison

	    jne     short FCMNoMem2	    ; Nope, return

	    ; Is there a 32-bit memory board installed?
	    mov     bx,pBIMSTRUCT
	    mov     bx,es:[bx]		    ; De-reference the pointer
	    mov     dx,es:[bx+AVAILABLE]    ; -1 means no board is installed
	    inc     dx
	    jz	    short FCMNoMem2	    ; Nope, return

	    ; How much memory is available and where does it start?
	    mov     dx,es:[bx+AVAILBIM]     ; Size in paragraphs
	    or	    dx,dx		    ; Any left?
	    jz	    short FCMNoMem
	    mov     cx,dx		    ; CX = Size in paragraphs
	    mov     ax,es:[bx+LASTUSED]
	    sub     ax,cx		    ; AX = Starting location - F0000h
					    ;	     in paragraphs
	    push    es			    ; Save for a rainy day...
	    push    bx
	    push    ax

	    ; Change AX to the starting location in K.
	    shr     ax,4
	    add     ax,0F000h
	    shr     ax,2

	    ; Change CX to the size in K.
	    shr     cx,6

	    ; Store away for use by HookInt15().
	    mov     [BIMBase],ax
	    mov     [BIMLength],cx

	    ; Un-WriteProtect the ROMs.
	    mov     si,offset BIMGDT		    ; Set up the BlockMove GDT
	    mov     ax,cs
	    mov     es,ax
	    mov     cx,16
	    mul     cx
	    add     ax,offset BIMBuffer
	    adc     dl,0
	    mov     [si.S_BASE_LOW],ax
	    mov     [si.S_BASE_HI],dl
	    mov     cx,1

	    mov     word ptr [BIMBuffer],0FEFEh     ; FEh unlocks the ROMs

	    mov     ah,87h			    ; Do the BlockMove
	    int     15h
	    or	    ah,ah			    ; Was there an error?
	    jz	    short FCMReserve		    ; Nope - continue

	    ; Return error.
	    pop     ax				    ; Clean up
	    pop     bx
	    pop     es
	    xor     ax,ax
	    mov     [BIMBase],ax
	    mov     [BIMLength],ax
FCMNoMem2:  jmp     short FCMNoMem

	    ; Change the ROM values to reserve the BIM stuff.
FCMReserve: pop     ax
	    pop     bx
	    pop     es
	    mov     word ptr es:[bx+AVAILBIM],0      ; Reserve all remaining BIM
	    mov     word ptr es:[bx+LASTUSED],ax

	    ; Re-WriteProtect the ROMs.
	    push    cs
	    pop     es
	    mov     si,offset BIMGDT		    ; Set up the BlockMove GDT

	    mov     word ptr [BIMBuffer],0FCFCh     ; FCh unlocks the ROMs

	    mov     ah,87h			    ; Do the BlockMove
	    int     15h

	    mov     ax,1			    ; Return success

FCMNoMem:   pop     es
	    ret

GetBIMMemory endp


;*--------------------------------------------------------------------------*
;*									    *
;*  GetInt15Memory -							    *
;*									    *
;*	Returns the amount of memory INT 15h, Function 88h says is free	    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = Amount of free extended memory in K-bytes		    *
;*  REGS:   AX and Flags are clobbered					    *
;*									    *
;*--------------------------------------------------------------------------*
	    
GetInt15Memory proc near

	    ; Check for 6300 Plus (to set "MemCorr").
	    call    Is6300Plus

	    ; Get the amount of extended memory Int 15h says is around.
	    mov	    ah,88h
	    clc
	    int	    15h		    ; Is Function 88h around?
	    jnc     short GIM100
	    xor	    ax,ax	    ; No, return 0
	    ret
	    
GIM100:	    sub	    ax,[MemCorr]    ; Compensate for 6300 Plus machines
	    ret

GetInt15Memory endp			   
	    
	    
;*--------------------------------------------------------------------------*
;*									    *
;*  InstallMoveBlock -					    HARDWARE DEP.   *
;*									    *
;*	Attempt to install the proper MoveBlock function.		    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None							    *
;*  REGS:   AX, CX, DI, SI, ES and Flags are clobbered			    *
;*									    *
;*--------------------------------------------------------------------------*

InstallMoveBlock proc near

	    ; Are we on a 386 machine?
	    call    MachineCheck
	    cmp     ax,3
	    je	    short IMBOn386	    ; Yes, install the 386 routine

	    ; Install the 286 MoveBlock routine.
	    mov     [InitLine],offset EndMoveBlock286
	    call    InitMoveBlock286
	    ret

	    ; Install the 386 MoveBlock routine.
IMBOn386:   mov     si,offset MoveBlock386
	    mov     cx,(offset EndMoveBlock386 - offset MoveBlock386)

	    ; REP MOV the routine into position.
	    cld
	    push    cs
	    pop	    es
	    mov     di,offset MoveBlock
	    add     [InitLine],cx
	    rep movsb
	    call    InitMoveBlock386
	    ret

InstallMoveBlock endp


;*--------------------------------------------------------------------------*
;*									    *
;*  InitMoveBlock286 -					    HARDWARE DEP.   *
;*									    *
;*	Initializes the 286 MoveBlock routine				    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None							    *
;*  REGS:   AX, DX and Flags are clobbered				    *
;*									    *
;*--------------------------------------------------------------------------*

InitMoveBlock286 proc near

	    mov     ax, cs
	    mov     [cs0],ax		; Patch far jumps to restore
	    mov     [cs1],ax		; Patch far jumps to restore
	    inc     ax			;* Trick 3
	    mov     [LCSS],ax
	    dec     ax
	    mov     dx,16
	    mul     dx	   ; Believe or not, this is faster than a 32 bit SHIFT
	    mov     [CSDES].seg_base_lo,ax
	    mov     [CSDES].seg_base_hi,dl
	    ret

InitMoveBlock286 endp


;*--------------------------------------------------------------------------*
;*									    *
;*  InitMoveBlock386 -					    HARDWARE DEP.   *
;*									    *
;*	Initializes the 386 MoveBlock routine				    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None							    *
;*  REGS:   AX, CX, DX and Flags are clobbered				    *
;*									    *
;*--------------------------------------------------------------------------*

InitMoveBlock386 proc near

	    mov     ax,cs			; dx has CS of codeg
	    mov     [patch3],ax 		; Patch code
	    mov     cx,16
	    mul     cx
	    mov     [descCS].LO_apDesc386,ax	; Set up selector for our CS
	    mov     [descCS].MID_apDesc386,dl
	    add     ax,offset code:OurGDT	; Calculate Base of GDT
	    adc     dx,0
	    mov     [GDTPtr.LO_apBaseGdt],ax
	    mov     [GDTPtr.HI_apBaseGdt],dx
	    mov     ax,offset code:MoveExtended386+MEM3_Offset
	    mov     ControlJumpTable[0Bh*2],ax
	    ret

InitMoveBlock386 endp


;*--------------------------------------------------------------------------*
;*									    *
;*  InstallA20 -					    HARDWARE DEP.   *
;*									    *
;*	Attempt to install the proper A20 Handler			    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None							    *
;*  REGS:   AX, CX, DI, SI, ES and Flags are clobbered			    *
;*									    *
;*--------------------------------------------------------------------------*

InstallA20  proc near

	    ; Are we on a 6300 Plus?
	    call    Is6300Plus
	    or	    ax,ax
	    jz	    short IAChkPS2

	    ; Yes, relocate 6300 Plus A20 handler.
	    mov	    si,offset $6300PLUS_A20Handler
	    mov	    cx,(offset End6300PLUS_Handler - offset $6300PLUS_A20Handler)
	    jmp	    short IAMoveIt

	    ; Are we on a PS/2?
IAChkPS2:   call    IsPS2Machine
	    cmp	    ax,1
	    jne     short IACheckHP

	    ; Yes, relocate the PS/2 A20 handler.
	    mov	    si,offset PS2_A20Handler
	    mov	    cx,(offset EndPS2_A20Handler - offset PS2_A20Handler)
	    jmp	    short IAMoveIt

	    ; Are we on a HP Vectra?
IACheckHP:  call    IsHPMachine
	    cmp	    ax,1
	    jne     short IAOnAT

	    ; Yes, relocate the HP A20 handler.
	    mov	    si,offset HP_A20Handler
	    mov     cx,(offset EndHP_A20Handler - offset HP_A20Handler)
	    jmp     short IAMoveIt

IAOnAT:     mov     si,offset AT_A20Handler
	    mov     cx,(offset EndAT_A20Handler - offset AT_A20Handler)

	    ; REP MOV the proper handler into position.
IAMoveIt:   cld
	    push    cs
	    pop	    es
	    mov     di,[InitLine]
	    mov     [A20Handler],di
	    add	    [InitLine],cx
	    rep movsb
	    ret

InstallA20  endp


;*--------------------------------------------------------------------------*
;*									    *
;*  GetParms -								    *
;*									    *
;*	Get any parameters off of the HIMEM command line		    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None							    *
;*  REGS:   AX, BX, CX, DX, DI, SI, ES and Flags clobbered		    *
;*									    *
;*  Side Effects:   cHandles and MinHMASize may be changed		    *
;*									    *
;*--------------------------------------------------------------------------*

GPRegSave   dw	    ?

GetParms    proc    near

	    push    ds

	    cld
	    les	    di,[pReqHdr]
	    lds	    si,es:[di.pCmdLine]	    ; DS:SI points to first char
					    ; after "DEVICE="
	    ; Scan until we see a frontslash or the end of line.
GPBadParm:	      
GPNextChar: lodsb
	    cmp	    al,'/'
	    je	    short GPGotOne
	    cmp	    al,13
	    jne     short GPNextChar
GPDatsAll:  pop	    ds
	    ret

	    ; Save what we found and get the number after it.
GPGotOne:   lodsb
	    mov	    cs:[GPRegSave],ax
	    
	    ; Scan past the rest of the parm for a number, EOL, or a space.
GPNeedNum:  lodsb
	    cmp	    al,' '
	    je	    short GPBadParm
	    cmp	    al,13
	    je	    short GPBadParm
	    cmp	    al,'0'
	    jb	    short GPNeedNum
	    cmp	    al,'9'
	    ja	    short GPNeedNum
	    
	    ; Read the number at DS:SI into DX.
	    xor	    dx,dx
GPNumLoop:  sub	    al,'0'
	    cbw
	    add	    dx,ax
	    lodsb
	    cmp	    al,' '
	    je	    short GPNumDone
	    cmp	    al,13
	    je	    short GPNumDone
	    cmp	    al,'0'
	    jb	    short GPBadParm
	    cmp	    al,'9'
	    ja	    short GPBadParm
	    shl	    dx,1		    ; Stupid multiply DX by 10
	    mov	    bx,dx
	    shl	    dx,1
	    shl	    dx,1
	    add	    dx,bx
	    jmp	    short GPNumLoop
	    
	    ; Which parameter are we dealing with here?
GPNumDone:  xchg    ax,cs:[GPRegSave]
	    cmp	    al,'H'		    ; HMAMIN= parameter?
	    je	    short GPGotMin
	    cmp	    al,'N'		    ; NUMHANDLES= parameter?
	    jne     short GPBadParm

	    ; Process /NUMHANDLES= parameter.
GPGotHands: cmp	    dx,MAXHANDLES
	    ja	    short GPBadParm

	    mov	    cs:[cHandles],dx

	    ; Print descriptive message.
	    mov	    dx,offset StartMsg
	    call    GPPrintIt
	    mov	    ax,cs:[cHandles]
	    call    GPPrintAX
	    mov	    dx,offset HandlesMsg
	    call    GPPrintIt
	    jmp	    short GPNextParm

	    ; Process /HMAMIN= parameter.
GPGotMin:   cmp	    dx,64
	    ja	    short GPBadParm

	    push    dx
	    mov	    cs:[MinHMASize],dx

	    ; Print a descriptive message.
	    mov	    dx,offset HMAMINMsg
	    call    GPPrintIt
	    mov	    ax,cs:[MinHMASize]
	    call    GPPrintAX
	    mov	    dx,offset KMsg
	    call    GPPrintIt

	    pop	    dx
	    mov	    cl,10		    ; Convert from K to bytes
	    shl	    dx,cl
	    mov	    cs:[MinHMASize],dx

	    ; Were we at the end of the line?
GPNextParm: mov	    ax,cs:[GPRegSave]
	    cmp	    al,13
	    je	    short GPExit
	    jmp     GPNextChar

GPExit:	    pop	    ds
	    ret
	    
GetParms    endp

;*--------------------------------------------------------------------------*

GPPrintIt   proc    near

	    push    ds			    ; Save current DS
	    push    cs			    ; Set DS=CS
	    pop	    ds
	    mov	    ah,9h
	    int	    21h
	    pop	    ds			    ; Restore DS
	    ret

GPPrintIt   endp

;*--------------------------------------------------------------------------*

GPPrintAX   proc    near

	    mov	    cx,10
	    xor	    dx,dx
	    div	    cx
	    or	    ax,ax
	    jz	    short GPAPrint
	    push    dx
	    call    GPPrintAX
	    pop	    dx
GPAPrint:   add	    dl,'0'
	    mov	    ah,2h
	    int	    21h
	    ret

GPPrintAX   endp


;*--------------------------------------------------------------------------*
;*									    *
;*  InitHandles -							    *
;*									    *
;*	Initialize the Extended Memory Handle Table			    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   None							    *
;*  REGS:   AX, BX, CX, and Flags are clobbered				    *
;*									    *
;*--------------------------------------------------------------------------*

InitHandles proc    near

	    ; Allocate room for the Handle table at the end.
	    mov	    ax,[InitLine]
	    mov	    [KiddValley],ax
	    mov	    ax,SIZE Handle
	    mov	    cx,[cHandles]
	    mul	    cx
	    add	    [InitLine],ax
	    
	    ; Init the Handle table.
	    mov	    bx,KiddValley
	    xor	    ax,ax
	    mov	    cx,[cHandles]
IHTabLoop:  mov	    [bx.Flags],UNUSEDFLAG
	    mov	    [bx.cLock],al
	    mov	    [bx.Base],ax
	    mov	    [bx.Len],ax
	    add	    bx,SIZE Handle
	    loop    IHTabLoop

	    ; Store away the top of the table for handle validation.
	    mov	    [KiddValleyTop],bx
	    ret
	    
InitHandles endp


;*--------------------------------------------------------------------------*
;*									    *
;*  Is6300Plus						   HARDWARE DEP.    *
;*									    *
;*	Check for AT&T 6300 Plus					    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if we're on an AT&T 6300 Plus, 0 otherwise		    *
;*  REGS:   AX, flags used.						    *
;*									    *
;*  Side Effects:   MemCorr value updated to 384 if necessary.		    *
;*									    *
;*--------------------------------------------------------------------------*

Is6300Plus  proc near

	    xor	    ax,ax
	    push    bx
	    mov	    bx,0FC00h		    ; Look for 'OL' at FC00:50
	    mov	    es,bx
	    cmp	    es:[0050h],'LO'
	    jne     short I6PNotPlus	    ; Not found
	    mov	    bx,0F000h
	    mov	    es,bx
	    cmp	    word ptr es:[0FFFDh],0FC00h	    ; Look for 6300 PLUS
	    jne     short I6PNotPlus

	    in	    al,66h		    ; Look for upper extended memory
	    and	    al,00001111b
	    cmp	    al,00001011b
	    jne     short I6PNoMem
	    mov	    [MemCorr],384	    ; Save value

I6PNoMem:   mov	    ax,1		    ; We found a PLUS
I6PNotPlus: pop	    bx
	    ret

Is6300Plus  endp


;*--------------------------------------------------------------------------*
;*									    *
;*  IsPS2Machine					    HARDWARE DEP.   *
;*									    *
;*	Check for PS/2 machine						    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if we're on a valid PS/2 machine, 0 otherwise	    *
;*  REGS:   AX	and Flags clobbered					    *
;*									    *
;*--------------------------------------------------------------------------*

IsPS2Machine proc   near

	    mov     ah,0C0h	    ; Get System Description Vector
	    stc
	    int	    15h
	    jc	    short IPMNoPS2  ; Error?  Not a PS/2.

	    ; Are we on a PS/2 Model 35?
	    cmp     es:[bx+2],09FCh
	    je	    short IPMFoundIt	    ; Yup, use the PS/2 method

	    ; Do we have a "Micro Channel" computer?
	    mov     al,byte ptr es:[bx+5]   ; Get "Feature Information Byte 1"
	    test    al,00000010b    ; Test the "Micro Channel Implemented" bit
	    jz	    short IPMNoPS2

IPMFoundIt: xor	    ax,ax	    ; Disable A20. Fixes PS2 Ctl-Alt-Del bug
	    call    PS2_A20Handler
	    mov	    ax,1
	    ret

IPMNoPS2:   xor	    ax,ax
	    ret

IsPS2Machine endp


;*--------------------------------------------------------------------------*
;*									    *
;*  IsHPMachine						    HARDWARE DEP.   *
;*									    *
;*	Check for HP Vectra Machine					    *
;*									    *
;*  ARGS:   None							    *
;*  RETS:   AX = 1 if we're on a HP Vectra machine, 0 otherwise		    *
;*  REGS:   AX, ES and Flags clobbered					    *
;*									    *
;*--------------------------------------------------------------------------*

IsHPMachine proc   near

	    mov	    ax,0F000h
	    mov	    es,ax
	    mov	    ax,word ptr es:[0F8h]
	    cmp	    ax,'PH'
	    je	    short IHMFoundIt
	    xor	    ax,ax
	    ret

IHMFoundIt: mov	    ax,1
	    ret

IsHPMachine endp


;*--------------------------------------------------------------------------*
;*	DRIVER MESSAGES							    *
;*--------------------------------------------------------------------------*		 

SignOnMsg   db	13,10,'HIMEM: DOS XMS Driver, Version 2.04 - 8/17/88'
	    db	13,10,'Copyright 1988, Microsoft Corp.'
	    db	13,10,'$'

BadDOSMsg   db	13,10,'ERROR: HIMEM.SYS requires DOS 3.00 or higher.$'
NowInMsg    db	13,10,'ERROR: An Extended Memory Manager is already installed.$'
On8086Msg   db	13,10,'ERROR: HIMEM.SYS requires an 80x86-based machine.$'
NoExtMemMsg db	13,10,'ERROR: No available extended memory was found.$'
BadA20Msg   db	13,10,'ERROR: Unrecognized A20 hardware.$'
FlushMsg    db	13,10,'	      XMS Driver not installed.',13,10,13,10,'$'

StartMsg    db	13,10,'$'
HandlesMsg  db	' extended memory handles available.$'
HMAMINMsg   db	13,10,'Minimum HMA size set to $'
KMsg	    db	'K.$'

NoHMAMsg    db	13,10,'WARNING: The High Memory Area is unavailable.',13,10,'$'
A20OnMsg    db	13,10,'WARNING: The A20 Line was already enabled.',13,10,'$'

HMAOKMsg    db	13,10,'64K High Memory Area is available.',13,10,13,10,'$'

	    db	'This program is the property of Microsoft Corporation.'
	    
	    db	64 dup(0)			; For internationalization

;*==========================================================================*

code	    ends

	    end
