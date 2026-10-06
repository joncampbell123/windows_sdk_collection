;	SCCSID = @(#)atint13.asm	5.9 87/03/09
	page	,132
	title	ATINT13 - MS OS/2 INT 13H interface
	name	ATINT13
	comment !
********************* Start of Specifications *************************
*								      *
*  Source File Name: ATINT13.ASM				      *
*								      *
*  Descriptive Name: INT 13H emulation				      *
*								      *
*  Copyright:							      *
*								      *
*  Status: Version 1.1						      *
*								      *
*  Function: Provides real mode INT 13H emulation for floppy disk     *
*	     I/O on the AT					      *
*								      *
*	     This file contains Int 13H handlers that fix the	      *
*	     following known bugs present in the IBM ROM:	      *
*	     - DMA violation errors - In this case, the operation is  *
*	     retried by breaking it up into up to three parts (the    *
*	     part before the 64K wrap, the actual 64K wrap, and the   *
*	     part after).					      *
*	     - ECC error handling - These only apply to fixed disks.  *
*	     The read operation is retried one sector at a time until *
*	     the request is complete.				      *
*	     - Fixed disk READ - There is a bug in the ROM that, under*
*	     certain situations, causes it to "miss" some sectors from*
*	     the controller.  The fixed disk controller apparently    *
*	     resets the bit indicating that it has more sectors to    *
*	     read if it receives an interrupt.	The fix is to count   *
*	     the number of sectors that have been transferred instead *
*	     of testing for this bit.				      *
*								      *
*								      *
*  Notes:							      *
*    Dependencies:						      *
*    Restrictions:						      *
*								      *
*  Entry Points:  Int13Handler					      *
*								      *
*  External References:  See EXTRN statements below		      *
*								      *
********************** End of Specifications ************************** !

	.286p

	include absegs.inc	; code and data segment definitions

	BREAK <External code entry points>
Code	segment

; Routines located in ABDSK1.ASM

; Routines located in ATDSK2.ASM
	extrn	Reset_Floppy:near
	extrn	Put765:near

; Routines located in ABDSK4.ASM
	extrn	SetTimer2:near

; Routines located in ABSUBR.ASM
	extrn	Get_BDS_Addr:near
	extrn	Set_Changed:near
	extrn	SWait:near
	extrn	SWClr:near
	extrn	SSig:near

Code	ends

	BREAK <External data references>
BioData segment

; Data located in ABDSK1.ASM
	extrn	Start_BDS:word
	extrn	DevHlpFunc:dword
	extrn	ScratchBufSem:byte
	extrn	ScratchBuffer:word

; Data located in ABDSK2.ASM
	extrn	Drive:byte

; Data located in ABDSK3.ASM
	extrn	Fixed:byte

; Data located in ABINIT.ASM
	extrn  fHaveDRV2:byte

BioData ends

	.xlist				;
	include struc.inc		; structured macros
if1
	include abmac.inc		; private macros
endif
	include abdskequ.inc		; general equates
	include abddrv.inc		; IOStruc
	include devhlp.inc		; device helper equates
	include abint13.inc		; ROM equates
	include ibmrom.inc		; AT low core ROM data area
	include abdebug.inc		; debug and listing control
	include dosmac.inc		; Dos macros(<Break>, Ljc>)
	include struc.inc		; Structure macros

	ListLevel %ablistlevel,%abdebug

	BREAK	<data Segment>
BioData segment

	even
	 public  RealInt13Vector
RealInt13Vector dd	0		; Used to make int 13 requests.
;	 public  Int13_Crit_Sem
;Int13_Crit_Sem  dw	 3 DUP (0)	 ; Ram critical section semaphore.

UseROMFlag	db	2		; Can we use ROM? check date to tell
					; (Init value of 2 forces date check)
Bios_Date	db	'01/10/84',0    ; This is the magic date to check for
OldTrack	db	?		; Current track for Ext 3.5" diskette

;**	SemInt13 - Protect Application INT13 Requests
;
;	Some applications issue direct disk requests via INT13.  We can't
;	let these routines enter the ROM when we're also calling the ROM.
;	Likewise, we can't call the ROM when an application might be in it.
;	(We'll context switch out of the ROM if it was entered in user mode
;	by an application.)
;
;	We don't want to have a simple "disk hardware/ROM is in use"
;	interlock because a future implimentation will service requests from
;	the work queue at interrupt time.  A flurry of DOS requests could
;	thus lock out an INT13 user indefinitely.
;
;	The mechanism we use is as follows:
;
;	Enter INT13 handler in BIOS:
;		set/block on "want to do INT13" semaphore.  This also
;			serializes INT13 requests
;
;		set/block on IOActive.	This blocks us until the work queue
;			is empty
;
;		Enter ROM
;
;		Clear IOActive
;		Clear SemInt13
;
;	The DOS-request routines do:
;
;		If SemInt13 is set
;			block on it
;		queue request
;
;	The result is that when an application issues a direct INT13 we
;	block any further queue requests, wait till the queue is empty,
;	do our work, and release any backed up requests.

	public	SemInt13
SemInt13    db	    0,0 		; Semaphore controlling INT 13's

	public	Block_Int13	    ;					 @@2846
Block_Int13 db	    0		    ; if not zero, Int 13 process	 @@2846
				    ; is being blocked because current	 @@2846
				    ; floppy or Fixed i/o request is	 @@2846
				    ; completed yet.			 @@2846
				    ; This flag is checked at interrupt  @@2846
				    ; routine. If it is set, call state  @@2846
				    ; machine to complete current i/o	 @@2846
				    ; work.				 @@2846

oldax	    dw	    0

ratetbl     db	    RATE500		; 96tpi medium in 96 tpi drive
	    db	    RATE300		; 48 tpi medium in 96 tpi drive
	    db	    RATE250		; 48 tpi medium in 48 tpi drive
	    db	    RATE250		; 720KB medium in 3.5" drive

BioData ends

	BREAK	<Code Segment>
Code	segment

	assume	cs:BiosSeg,ds:BiosGroup,es:nothing

;*  RealModeDS - SetROMVector stores our real-mode DS here in the code
;		 segment so that we can get access to our DS at INT 13h-
;		 time.

	public	RealModeDS
RealModeDS	dw	0

	BREAK <Int13Handler - Main Line Interrupt Handler>
	comment !
************************ Start of Specifications *********************
*								     *
* Subroutine Name: Int13Handler 				     *
*								     *
* Descriptine Name: Emulate ROM Int 13 handler			     *
*								     *
* Function:							     *
*	    Int13Handler interlocks user INT 13 requests to allow    *
*	their use with the interrupt-driven disk drivers.  The	     *
*	requests are made single-threaded with a critical region     *
*	semaphore, then the process will wait till the disk is	     *
*	quiescent before being allowed to proceed.  While the	     *
*	INT 13 is going on, all other disk I/O is suspended.  When   *
*	the request is completed, any waiting I/O requests and	     *
*	INT 13s will be allowed to proceed.			     *
*	It is necessary to suspend all other I/O since if we do      *
*	a disk reset through the ROM, it resets 'everything'.        *
*	    The DevHelp BiosCritEnter is used to prevent any	     *
*	process switching from this one (e.g. switching pages)	     *
*	since the ROM cannot handle re-entry and would get totally   *
*	confused.						     *
*								     *
* Notes:							     *
*	    - This code will NOT retry failed operations. It is up   *
*	to the application to reset the drive and to retry the	     *
*	operation.						     *
*	    - This routine is a FAR routine since the actual routine *
*	that intercepts the Int 13 is in the kernel. So, we do a     *
*	FAR RET instead of an IRET on exit.			     *
*								     *
*								     *
* Entry Point: Int13Handler					     *
*								     *
* Linkage:							     *
*	    DS	  - BiosGroup					     *
*								     *
* Input:	ROM Int 13 parameters				     *
*								     *
* Exit-Normal:	ROM Int 13 outputs				     *
*								     *
* Exit-Error:	'C' set if error                                     *
*								     *
* Effects:							     *
*								     *
* Internal Refereences: 					     *
*								     *
* External References:						     *
*								     *
************************* End of Specifications ********************** !
ERROR_INT13_DRIVE_NOT_READY	EQU	80h

	Public Int13Handler
Int13Handler	PROC	FAR

;*  Get our DS from the code segment.  It was put there by
;   DevHlp_SetROMVector, which we called at init time.

	SaveReg <ds, di, si>
	mov	ds,cs:[RealModeDS]	; (DS) = BiosGroup
ASSUME DS:BiosGroup
	;;DBPRT   12,80H,<Direct INT 13, AX:$x BX:$x CX:$x DX:$x ES:$x	 >,<AX,BX,CX,DX,ES>

	call	Get_Phys_BDS
	jc	I13H0			; Jump if there is no BDS
	test	[di].DrvTypFlag, fNon_Removable
	jz	I13H0_1 		; Jump if drive has removeable media

I13H0:			;A few requests are not allowed with fixed disk.
	Cmp AH,3				;Check if WRITE request.
	JZ  I13h0_0					   ;If so, nogo.
	Cmp AH,5			       ;Check if FORMAT request.
	JZ  I13h0_0					   ;If so, nogo.
	Cmp AH,9	   ;Check if INIT DRIVE CHARACTERISTICS request.
	JZ  I13h0_0					   ;If so, nogo.
	Cmp AH,0Bh			   ;Check if LONG WRITE request.
	JNZ  I13h0_1					  ;If not, okay.

I13h0_0:				       ;Else, return with error.
	stc
	mov	ah,ERROR_INT13_DRIVE_NOT_READY
	RestoreReg <si, di, ds>
	ret	2			; Return to user program with our flags

I13h0_1:
	SaveReg <bx,cx>
	push	ax
	push	dx
	mov	al,1
	mov	dl,DevHlp_ROMCritSection
	call	dword ptr [DevHlpFunc]	   ; Enter ROM critical section
	pop	dx
	pop	ax
	mov	bx,DataOffset SemInt13
	call	SWait			; indicate want to go into INT13

;	We have a direct disk access request, and no one else is waiting for
;	an INT 13.  Check an make sure that no one is using either the floppy
;	or the fixed driver, and if he is, wait on BOTH of them to prevent
;	someone else from grabbing them.  Since we are in USER mode, someone
;	else can enter the disk I/O code, so we must be careful.

I13h1:
	DISABLE 			; disable interrupts for check.
	mov	Block_Int13,1		; assume that this int 13 request @@2846
					; process will be blocked.	  @@2846
	mov	bl,Drive.Active
	or	bl,Fixed.Active
	jz	i13h1_5 		; both semaphores are clear.
	mov	bx,DataOffset Drive.Active
	call	SWClr			; wait for it to clear
	mov	bx,DataOffset Fixed.Active
	call	SWClr			; wait for fixed drive sem too
	jmp	i13h1
;
;	Ok, we know that both DriveActive and FixedActive are clear.
;	Go and grab both of them.
;
;	Interrupts ON - SWClr enables interrupts!
;	    But it is OK since we have already set SemInt13.
;
;	Now, this Int 13 process is not being blocked,			  @@2846
;	so clear Block_Int13 flag.					  @@2846

i13h1_5:
	mov	Block_Int13,0		; current int 13 current not	  @@2846
					;  being blocked.		  @@2846
	mov	bx,DataOffset Fixed.Active
	call	SWait
	mov	bx,DataOffset Drive.Active
	call	SWait
I13h2:
	RestoreReg <cx,bx>
	call	INT13		; Call routine perform the actual INT 13 request
	;;DBPRT 12,80H,<Done   >,<>

	SaveReg <ax,bx,cx,dx,di>
	pushf
	jnc	I13h4
;
;	 There has been a disk error.  If we had a drive not ready
;	error on the floppy drive, reset the floppy disk controller.
;
	test	dl,80H			; Fixed disk request?
	jne	i13h4			;  Yes. Don't reset floppy
	cmp	ah,80H			; Drive not ready error?
	jne	i13h4			;  No, don't reset floppy.
	;;DBPRT 12,80H,<Floppy DNR, resetting floppy controller   >,<>
	call	Reset_Floppy		; Reset the floppy controller.
i13h4:
	push	ds
	call	DDS
    ASSUME   DS:RomData
	mov	al,[Motor_Status]	; get current drive motor state
	pop	ds
    ASSUME   DS:BiosGroup
	;;DBPRT 12,4,<New motor status $b   >,<AX>

;
; We update our data structures for the various disk drives to reflect the
; status of the ROMs data, in particular the drives that are still supposed
; to have their motors on. This is important since we do not want to turn
; them off while the ROM is still expecting them to be on!!
;
	mov	cx,4		; Controller can only support 4 floppy drives
	xor	dx,dx
i13h5:
	call	Get_Phys_BDS
	jc	i13h7			; No BDS for drive. Skip over set up.
	mov	bx,di			; DS:BX -> BDS for drive
	test	[bx].DrvTypFlag,fNon_Removable
	jnz	i13h6			; Don't set motor on for a fixed disk
	test	al,1
	jz	i13h6			; Drive motor off
i13h5_5:
	or	[bx].DrvFlag,Fmotoron
	call	SetTimer2		; set motor off delay
	mov	bx,[bx].Link
	or	bx,bx
	js	i13h6
	cmp	[bx].DriveNum,dl
	jz	i13h5_5 		; Set delay for all logical drives
i13h6:
;
; We need to update our idea of the location of the head on the medium
;
	cmp	dx,1
	jz	i13h7			; No updates for drives > 1
	push	ax
	push	bx
	mov	bx,dx			; BX contains physical drive number
	mov	al,es:DSK_TRK[bx]
	pop	bx			; DS:BX -> BDS for drive
	xor	ah,ah
	mov	[bx].CurCyl,AX		; Update our idea of head position
	pop	ax
i13h7:
	ror	al,1
	inc	dx
	loop	i13h5
	and	al,0F0h 		; mask motor on bits
	and	Drive.DOR,0Fh
	or	Drive.DOR,al		; move in new bits

	mov	bx,DataOffset Drive.Active
	call	SSig			; clear both floppy and
	mov	bx,DataOffset Fixed.Active
	call	SSig			; fixed active flags.
	mov	bx,DataOffset SemInt13
	call	SSig			; allow other INT 13s to proceed

	xor	al,al
	mov	dl,DevHlp_ROMCritSection
	call	dword ptr [DevHlpFunc]	   ; Exit ROM critical section
	ENABLE				   ; enable interrupt
	popf				; Set up flags for user
	RestoreReg <di,dx,cx,bx,ax>
	;;DBPRT 12,80H,<Done, returning AX:$x BX:$x CX:$x DX:$x  >,<AX,BX,CX,DX>
	RestoreReg <si, di, ds>
    ASSUME   DS:NOTHING
	ret	2			; Return to user program with our flags
Int13Handler	ENDP

	Break <INT13 - Perform call to IBM_DISK_IO>
;**	INT13 - Perform call to IBM_DISK_IO.
;
;	The INT13/Rom returns a special error code when the door has
;	been opened (if this hardware supports door flags).  We don't
;	want direct INT13 users to "suck up" this indication because
;	we'd never find out about it.  Instead, we watch all INT13
;	requests and see if they're returning a door flag error.
;
;	If they are, we do the user a favor and (after noting the door
;	flag event) we retry the operation for him.
;	We also handle DMA errors by splitting up the request into parts
;	and performing the I/O for each of the parts separately.
;	ECC errors are also handled here, in which case the request
;	is broken up into single-sector requests.
;
;	NOTE: This should be the ONLY routine that calls IBM_DISK_IO
;	      directly.
;
    ASSUME	CS:BiosSeg, DS:BiosGroup, ES:NOTHING

Int13Frame	STRUC
Int13BP DW	?		; Value of BP on stack frame
Int13AX DW	?		; Value of AX on stack frame
Int13BX DW	?		; Value of BX on stack frame
Int13CX DW	?		;  ""   ""    ""   "  "  "
Int13DX DW	?		;  ""   ""    ""   "  "  "
Int13LRet DW	?		; Local (in BIOS) return address.
Int13DS DW	?		; INT 13 caller's DS
Int13Ret DD	?		; Return address of INT 13 call
Int13F	DW	?		; Flags from call.
Int13Frame	ENDS

	PUBLIC	INT13
INT13	PROC
	;;DBPRT   02CH,0C0H,<Calling INT 13, AX:$x BX:$x CX:$x DX:$x ES:$x   >,<AX,BX,CX,DX,ES>

	MOV	[OLDAX],AX
	CALL	IBM_DISK_IO
	RETNC					; Just return if no error.

;
;	We had an error.  Check for door-flag error while preserving condition
;	codes in case it wasn't a door error
;
;	(AH) = Error code returned from ROM
;
;	Stack clear.
;
;	We jam interrupts ON to allow us to use the POPF instruction
;	(due to the POPF bug).	We cannot be hurt by taking an
;	interrupt in this code.
;
	ENABLE				; Force interrupts on.
	;;DBPRT   02CH,0C0H,<\bINT 13 Error. $x   >,<AX>
	PUSHF				; Save return flags.
	CMP	AH,6H
	JNZ	i130
	CALL	Set_Changed		; Note the fact that medium may have changed
	POPF
	RET

;
; Check for an ECC error on the disk. When an ECC error occurs, the disk
; controller reads all the sectors up to, AND INCLUDING, the one where the
; error occurred. We handle this error by reading the sectors one by one. In
; this case, we can be sure that the controller will read the sector even if
; another ECC error occurs.
;
i130:
	CMP	AH,11H
	JNE	i132			; Error not an ECC correction error
	POP	AX			; Remove flags from the stack
	MOV	AX,[OLDAX]
	CMP	AL,1
	JNZ	i131_1
	XOR	AH,AH			; Assume no error if request for
	RET				; one sector.

;
;	NOTE:	The order that variables are pushed on the
;		stack is EXTREMLY IMPORTANT - We expect the following:
;
;	TOS:	BP
;		AX
;		BX
;		CX
;		DX
;		WORD  Return Address (within BIOS)
;		DWORD Return Address (to program)
;		WORD  Program flags.
;
;	ALSO NOTE:	THE STACK MUST BE CLEAN AT THIS POINT,
;			We refer directly to the return address on the
;			stack via the Int13Frame structure.
;
i131_1:
	SaveReg <DX,CX,BX,AX,BP>
	MOV	BP,SP			; Note comments above about this frame
	SaveReg <BX,CX>
i131_2:
	PUSH	AX
	MOV	AL,1			; Request for one sector
	CALL	DMA_Do_Int		; Ignore all errors.
	POP	AX
	DEC	AL
	JZ	i131_3			; We are done
	INC	CL			; Move to next sector
	PUSH	BX
	MOV	BX,ES
	ADD	BX,512/16		; Move address by 200H bytes (= 32 paragraphs)
	MOV	ES,BX
	POP	BX
	jmp	short i131_2

i131_3:
	RestoreReg <CX,BX>
	CLC
	JMP	i13_Exit


;*	Check for a DMA violation. If there was one, we fix it.
;
;	Depending on the function, we do one of the following things:
;
;	    If command Read/Write, we break the request into (up to)
;	    three pieces and move the middle one into ScratchBuffer
;
;	    If the command is Format, we move the format table
;	    into ScratchBuffer
;
;	    If the command is Verify, we point (ES:BX) to ScratchBuffer
;
i132:	CMP	AH,09H
	JE	i1335			; Error returned not a DMA violation - just return
	POPF
	RET


i1335:
	POP	AX			; Blow away flags.
	MOV	AX,[OldAX]
	;;DBPRT   0CH,90h,<DMA Boundry error $x:$x NSec $b   >,<ES,BX,AX>
	CMP	AH,04H			; ROM VERIFY command?
	LJE	DMAEVerify		;  Yes, retry.
	CMP	AH,05H			; ROM FORMAT command?
	LJE	DMAEFormat		;  Yes, retry
;
;	The command was a Read/Write command.
;
;	(AX)	= Command/#Sectors to transfer
;
;	NOTE:	The order that variables are pushed on the
;		stack is EXTREMLY IMPORTANT - We expect the following:
;
;	TOS:	BP
;		AX
;		BX
;		CX
;		DX
;		WORD  Return Address (within BIOS)
;		DWORD Return Address (to program)
;		WORD  Program flags.
;
;	ALSO NOTE:	THE STACK MUST BE CLEAN AT THIS POINT,
;			We refer directly to the return address on the
;			stack via the Int13Frame structure.
;
	;;DBPRT   0CH,10h,<DMARead/Write   >,<>
	SaveReg <DX,CX,BX,AX,BP>
	MOV	BP,SP			; Note comments above about this frame

	CALL	DMAEGet_PhyAddr 	; (DX) = Physical address of start.
	ADD	DX,511			; We must transfer at least one sector

;*
;	(DX) = Low 16 bits of physical address of transfer (plus 1 sector)
;
;	'C' set if we must transfer immediatly through ScratchBuffer.
;
	;;DBPRT   0CH,10h,<Transfer through? $x   >,<DX>
	DLJC	DMAEBuffer		; Transfer 1 sector through
					; ScratchBuffer directly
;*	Compute the number of sectors we can transfer before the
;	DMA Boundry.
;
;	We divide the physical address by 512 which will give
;	us the number of sectors in the 64K segment up to the request
;	address.  Then subtract that from the number of sectors in
;	64K (128).  The result is the number of sectors we can transfer.
;
	SHR	DH,1			; Divide by 512 quickly.
	MOV	AH,128			; (AH) = # of sectors in 64K
	SUB	AH,DH			; (AH) = # of sectors left in 64K!
	;;DBPRT   0cH,10h,<NSectors left $x   >,<AX>
;
;	(AL) = # of sectors requested.
;	(AH) = # of sectors left in 64K.
;
;	Check to guarantee that this number is less than the number of
;	sectors requested.  If not, something went wrong!
;
	CMP	AH,AL			; Can we fit request in?
	JL	DMAEBreakUp		; No, break it up.
	CALL	DMA_Do_Int		; Do the bogus DMA request.
	JMP	i13_Exit

;	We can't fit the request into the entire block.  Perform the
;	operation on the chunk up to the 64K boundry.
;
DMAEBreakUp:
	PUSH	AX
	XCHG	AL,AH			; (AL) = # of sectors up to 64K
	CALL	DMA_Do_Int		; Perform the command
	POP	AX
	LJC	i13_Exit		 ; Return if error
	SUB	BYTE PTR [BP].Int13AX,AH; Decrement by successful operation
	ADD	CL,AH			; Advance sector number
	PUSH	AX
	PUSH	CX
	MOV	AH,0
	SHL	AX,(9-4)		; log2sectorsize - log2paragraphsize
	MOV	CX,ES
	ADD	CX,AX			; Advance DMA address
	MOV	ES,CX
	POP	CX
	POP	AX
	CALL	DMAEGet_PhyAddr 	; (DX) = Physical address, 'Z' set if
	JNZ	DMAEBuffer		; the DMA wrap was exact.
	MOV	AX,[BP].Int13AX 	; Set AX from caller
	CALL	DMA_Do_Int		; Issue the INT 13, and return, we are
	JMP	i13_Exit		; done.
;
;	The next request will wrap the 64K boundary.  If we are writing
;	copy the offending sector into our space.
;
;		  (ES:BX)    = the address of sector to transfer.
;		    (CX)     = the correct track/sector info.
;		(BP).Int13AX = correct function code.
;
DMAEBuffer:
	;;DBPRT   0ch,10h,<Transfer through sbuffer $x:$x   >,<ES,BX>
	PUSH	BX
	MOV	BX,DataOffset ScratchBufSem
	CALL	SWait		    ; Grab scratchbuffer semaphore
	POP	BX
	CMP	BYTE PTR [BP].Int13AX+1,03	; Write command?
	JNZ	DoRead		;  No, must be read.
;
;	Write command.	Copy the sector into ScratchBuffer.
;
	;;DBPRT 0ch,10h,<Write Cmd   >,<>
	SaveReg <ES,SI,DI,BX>
	SaveReg <DS>
ASSUME	DS:NOTHING, ES:NOTHING
	PUSH	DS			; Swap DS and ES
	PUSH	ES			;
	POP	DS			;
	POP	ES			;
ASSUME	ES:BiosGroup
	MOV	DI, es:ScratchBuffer	; ES:DI -> ScratchBuffer
ASSUME	ES:NOTHING
	PUSH	DI			; Save for later
	XCHG	SI,BX			; DS:SI -> Data buffer
	CALL	Move
	POP	BX			; (BX) = ScratchBuffer
	MOV	AL,1			; Move only one sector
	RestoreReg <DS>

;	(AH)	= Function (Write)
;	(AL)	= 1 (Write one sector)
;	(ES:BX) = ScratchBuffer
;	(CX)	= Track/sector number
;	(DX)	= Head/drive number
;	(SI),(DI) unchanged

	CALL	DMA_Do_Int
	RestoreReg <BX,DI,SI,ES>
ASSUME	DS:BiosGroup
	DLJC	i13_Exit		; Error, tell the guy he messed up.
	JMP	DMAETail		; Do rest of request.
;
;	We are reading a sector.  Read it into ScratchBuffer,
;	and transfer into users data space.
;	DS = BiosGroup
;
DoRead:
	;;DBPRT   0ch,10h,<Read Cmd to $x:$x   >,<ES,BX>
	SaveReg <BX,ES>
	PUSH	DS		; ES -> Scratch buffer
	POP	ES		;
ASSUME	ES:NOTHING
	MOV	BX, ds:ScratchBuffer
	MOV	AL,1		; Read only one sector.

;	(AH)	= Function (Read)
;	(AL)	= # of sectors to read (1)
;	(ES:BX) = ScratchBuffer
;	(CX)	= Track/sector number

	CALL	DMA_Do_Int
	RestoreReg <ES,BX>	; (ES:BX) = Original sector
	JC	i13_Exit	; Error - clean up and return
; copy data from scratch buffer back to caller's buffer
	SaveReg <DS,DI,SI,BX>
	XCHG	DI,BX		; ES:DI -> data sector
	MOV	SI, ds:ScratchBuffer ; DS:SI -> Scratch buffer
	CALL	Move
	Restorereg <BX,SI,DI,DS>

;*	Portion of INT 13 that went through ScratchBuffer has
;	been taken care of and we are ready to get the rest
;	of the INT 13 (we have done the first two parts of the
;	broken up request and we must finish the last one).

DMAETail:
	PUSH	BX			; Save buffer address
	MOV	BX,DataOffset ScratchBufSem
	CALL	SSig			    ; Release scratchbuffer semaphore
	MOV	BX,ES
	ADD	BX,512/16		; Add number of paragraphs
	MOV	ES,BX			; Advance sector address
	POP	BX			; Get back sector address
	INC	CX			; Advance start sector pointer
	MOV	AL,BYTE PTR [BP].Int13AX	; Get # sectors left
	CLC
	DEC	AL			; Fix up number of sectors to do
	JZ	i13_Exit		;  All done? Just return
	CALL	DMA_Do_Int

;	We are done.
;		(AX) = Final return code, throw away entry AX
;		'C' Set if error.
;
;	Pop off the frame on the stack and return to our caller.
;
;
i13_Exit:
	MOV	SP,BP
	POP	BP
	POP	BX
	POP	BX
	POP	CX
	POP	DX
	RET				; All done, return.

;*	The command was a VERIFY command.
;
;	Simply point ES:BX to ScratchBuffer.
;	 (AX)	= Command/#Sectors to format
;
;	Stack must be clean, we return directly.
;
DMAEVerify:
	SaveReg <BX,ES>
	PUSH	DS		  ; ES -> Scratch buffer
	POP	ES
ASSUME	ES:NOTHING
	MOV	BX,DataOffset ScratchBufSem
	CALL	SWait
	MOV	BX, es:ScratchBuffer		; (ES:BX) = Scratch Buffer
	CALL	IBM_DISK_IO
	ENABLE				    ; Force interrupts on
	PUSHF
	MOV	BX,DataOffset ScratchBufSem
	CALL	SSig
	POPF
	RestoreReg <ES,BX>
	RET


;	The command was a FORMAT command.
;
;	(ES:BX) = Address of FORMAT buffer
;	 (AX)	= Command/#Sectors to format
;
;	Stack must be clean, we return directly.
;
DMAEFormat:
	SaveReg <ES,SI,DI,BX,DS>
	PUSH	ES
	PUSH	DS		  ; ES -> Scratch buffer
	POP	ES
	MOV	SI,BX
	MOV	BX,DataOffset ScratchBufSem
	CALL	SWait			; Grab the ScratchBuffer semaphore
	POP	DS			; DS:SI -> user buffer
    ASSUME	DS:Nothing
	MOV	DI, es:ScratchBuffer	; ES:DI -> new address
	push	di	; mov address of scratchbuffer to bx see 2 lines from here
	CALL	MOVE
	pop	bx	; scratchbuffer to BX
	PUSH	ES			; Restore DS
	POP	DS
	CALL	IBM_DISK_IO		; Re-issue the request.
	ENABLE				; Force interrupts on to avoid
	PUSHF				; POPF bug on 286
	MOV	BX,DataOffset ScratchBufSem
	CALL	SSig			; Release semaphore we're done with it
	POPF
	RestoreReg <DS,BX,DI,SI,ES>
	RET

ASSUME	DS:NOTHING, ES:NOTHING


;*	DMA_Do_Int - Do DMA ROM Disk request.
;
;	This routine will read/write/verify/format a sector off of the
;	disk.
;
;	ENTRY	(SS:BP) = INT13Frame pointer
;		 (AH)	= # of sectors before boundary
;		 (AL)	= # of sectors to read/write/...
;		 (CH)	= Cylinder number
;		 (CL)	= Sector number within cylinder
;			  NB:	Top two bits of (CL) are the high
;				two bits of the cylinder
;		(ES:BX) = DTA (Disk Transfer Address)
;
;	EXIT	'C' Set if error (AH) = Error code
;		'C' Clear if no error.
;
;
;	NOTE:	The user flags are pushed from inside the Int13Frame
;		structure directly onto the stack to simulate an IRET
;		frame
;
    ASSUME	DS:BiosGroup
DMA_Do_Int	PROC
	CMP	AL,0
	RETZ
	MOV	DX,[BP].Int13DX
	CALL	Check_Int13_Wrap		; See if we need to wrap around
						; a head or cylinder.
	MOV	AH,BYTE PTR [BP].Int13AX+1	; (AH) = Command
	CALL	IBM_DISK_IO
	PUSHF
	POP	[BP].Int13F
	RET
DMA_Do_Int	ENDP

;*	DMAEGet_PhyAddr - Return the physical address of the destination addr
;
;	ENTRY	(ES:BX) = Address of a INT 13 Request packet
;
;	EXIT	(DX)	= Low 16 bits of physical address
;		(Flags) = Set as appropriate -
;			  'C' set if request address wrapped 64K bound
;			  'Z' set if wrap was an even wrap.
;
DMAEGet_PhyAddr PROC
	MOV	DX,ES
	SHL	DX,4			; (DX) = Phy address of segment
	ADD	DX,BX			; (DX) = Phy address of buffer
	RET
DMAEGet_PhyAddr ENDP

;*	Move a block of memory
;
;	ENTRY	(ES:DI) = Destination buffer
;		(DS:SI) = Source buffer
;
;	EXIT	NONE
;
;	USES	DI,SI,Flags
;
MOVE	PROC	NEAR
	;;DBPRT   0ch,10h,<Move $x:$x to $x:$x	 >,<DS,SI,ES,DI>
	PUSH	CX
	MOV	CX,512/2
	CLD
    REP MOVSW
	POP	CX
	RET
MOVE	ENDP

INT13	ENDP


;*****	Check_Int13_Wrap - Adjust the parameters for Int 13 entry
;
;	If we get a DMA violation or an ECC error and have tp split up the
;	Int 13 request into parts, we may end up with starting sector numbers
;	that are bigger than the number of sectors on a track. The ROM is
;	unable to handle such cases and will return an error.
;	We therefore try and map the request on to the correct sector/head/
;	cylinder before calling the ROM. We effectively wrap around the head
;	and track boundaries.
;	The fixed disk controller is able to step heads, so there is a
;	possibility for someone to write a program that issues an Int 13
;	Read or Write of a lot of sectors (In fact, there are applications
;	that do that!!). Therefore, we need to be able to handle this
;	for fixed disks.
;
;	ENTRY	(SS:BP) = INT13Frame pointer
;		(AH)	= # of sectors before boundary
;		(AL)	= # of sectors to read/write/...
;		(CH)	= Cylinder number
;		(CL)	= Sector number within cylinder
;			  NB:	Top two bits of (CL) are the high
;				two bits of the cylinder
;		 (DH)	= Head number
;		 (DL)	= Physical drive number
;
;	EXIT	(CH)	= Cylinder number after wrapping
;		(CL)	= Sector number after wrapping
;			  NB:	Top two bits of (CL) are the high
;				two bits of the cylinder
;		(DH)	= Head number after wrapping
;
;		ALL other registers preserved except Flags.
;
;	NOTE	There may have been no wrapping to perform.
;		We only perform the wrapping for fixed disks.
;
Check_Int13_Wrap PROC
	SaveReg <DI,DS,BX,AX>
	CALL	Get_Phys_BDS		; Get BDS using physical drive # in DL
	TEST	WORD PTR [DI].DrvTypFlag,fNon_Removable
	JZ	ci13w3			; No wraps handled for floppies

	MOV	AX,CX
	AND	AL,03FH 		; Extract sector #
	XOR	AH,AH			; AX = Sector number
	CMP	AX,[DI].BPBtrksiz
	JBE	ci13w3			; No wrap around head

	MOV	BX,[DI].BPBtrksiz
	DIV	BL			; AH = New Sector #, AL = # of head wraps
; If new sector number is 0, we are on a track boundary. So, set AH = BPBtrksiz
	OR	AH,AH
	JNZ	ci13w1
	MOV	AH,BL
	DEC	AL			; Decrement # of head wraps
; AH = New sector #, AL = # of head wraps, BL = BPBtrksiz
; CL = OLD sector #, CH = Cylinder #, DH = head #
ci13w1:
	AND	CL,0C0H 		; Remove sector #
	OR	CL,AH			; OR in new sector #
	ADD	AL,DH
	XOR	AH,AH
	CMP	AX,[DI].BPBnhead
	JB	ci13w2

	MOV	BX,[DI].BPBnhead
	DIV	BL			; AH = New head #, AL = # of cylinder wraps
	PUSH	DX
	MOV	DX,CX
	MOV	BX,CX
	SHR	BL,6			; Cylinder high bits into l.s. end
	ADD	BH,AL			; Adjust for new cylinder number
	ADC	BL,0
	SHL	BL,6			; Cylinder high bits into m.s. end
	MOV	CX,BX
	AND	CL,0C0H
	AND	DL,03FH
	OR	CL,DL			; OR in sector #
	POP	DX
	XCHG	AH,AL			; AL = New head

; AL = new head #, CH = cylinder #, CL = sector #
ci13w2:
	MOV	DH,AL

ci13w3:
	RestoreReg <AX,BX,DS,DI>
	RET

Check_Int13_Wrap ENDP



ASSUME	DS:BiosGroup

;**	Get_Phys_BDS - Return pointer to BDS for physical drive
;
;	Get_Phys_BDS returns a pointer to the BDS for the physical
;	drive in DL.  The BDS contains various drive parameters
;	such as physical disk dimensions and the size of FATs and the
;	root directory.
;
;	ENTRY	(DL) = Physical Drive number
;		DS = BiosGroup
;
;	EXIT	DI = Pointer to BDS for the drive
;		'C' set if no BDS found for this drive
;
;	USES	DI, Flags
;
PUBLIC	Get_Phys_BDS
Get_Phys_BDS	PROC
; Scan through the BDS chain and look for the correct logical drive
	MOV	DI,WORD PTR [Start_BDS]
gpb0:	CMP	DL,BYTE PTR [DI].DriveNum
	JZ	gpb2
	CMP	WORD PTR [DI].Link,-1
	JZ	gpb1
	MOV	DI,WORD PTR [DI].Link
	JMP	SHORT gpb0
gpb2:	RET

gpb1:	STC
	RET			    ; Return carry if no BDS for drive
Get_Phys_BDS	ENDP


BREAK < Handler for hard disk Read operations >

;
; The following code handles READ and LONG READ oparations to the fixed
; disks. It is necessary to handle these specially because there is a
; bug in the IBM ROM that causes some sectors to not be read under certain
; circumstances. The reas on is that the coed in the ROM asks the controller
; if it is busy, and assumes that all the sectors have been transferred if
; the controller says it is done. Instead of this, it should really keep
; a count of sectors transferred, and stop only when all have been read
; in.
;
; NOTES: This routine should be the last one in the Int 13 chain before
;	 calling the ROM.
;	 This should be the ONLY routine in the disk driver that actually
;	 calls RealInt13Vector, all INT 13 requests should go through
;	 this routine.
;

;**
; Macro to call the routine in the ROM code segment
;
fcall	macro	dest
	push	cs
	call	dest
	endm
.LIST

;--------------------------------------------------------
;  Absolute pointers to ROS  1/10/84
;--------------------------------------------------------
far_ret 	equ	0ff65h
disk_setup	equ	028dah
ROMCode 	segment at 0f000h
ASSUME	CS:ROMCode
		org	disk_setup+197h
f_disk_io	label	far
		org	disk_setup+544h
f_command	label	far
		org	disk_setup+5a5h
f_wait		label	far
		org	disk_setup+608h
f_wait_drq	label	far
		org	disk_setup+61eh
f_check_status	label	far
		org	disk_setup+68fh
f_check_dma	label	far
		org	disk_setup+6b4h
f_get_vec	label	far
ROMCode 	ends

ASSUME	CS:BiosSeg
;*****	IBM_DISK_IO - Handle the fixed disk READ operations locally
;
;	IBM_DISK_IO handles the READ operations to fixed disks here
;	instead of passing it to the ROM.
;
;	ENTRY	As for Int 13
;
;	EXIT	As for Int 13
;
;	ALGORITHM:
;		      If this is a RESET command
;			  Handle it here
;		      If request is for a fixed disk
;			  If request is a READ or LONG READ
;				  Handle it here
;			  Else
;				  Pass it on to the ROM
;		      Else
;			  If the request is for an external drive (Drive 2)
;				  Pass it to the 3.5" drive handler
;			  Else
;				  Pass on to the ROM
;		      Endif
;
	ASSUME	DS:BiosGroup,ES:NOTHING

IBM_DISK_IO	PROC
	OR	AH,AH
	JZ	Int13ResetDisk		; Reset command - Do not pass to ROM

	CMP	DL,80H
	DLJAE	HARD_DISK		; Request for fixed disk. Handle here.

	CMP	DL,2
	JNE	go_rom			; Command not for Drive 2 - Pass to ROM
idi1:
	CALL	Drv2Int13		; Handle this specially since ROM cannot
	RET

Int13ResetDisk:
	MOV	DS,CS:[RealModeDS]	; Set up DS = BiosGroup
	CALL	Reset_Floppy
	RET

go_rom:
	;;DBPRT   02CH,0C0H,<Real INT 13, AX:$x BX:$x CX:$x DX:$x ES:$x   >,<AX,BX,CX,DX,ES>
	PUSHF
	CALL	DWORD PTR [RealInt13Vector] ; let the ROM's routine handle it
	RET

;
; We have a request for the hard disk. See if it is a read request.
;
HARD_DISK:
	cmp	ah,02h
	je	A3
	cmp	ah,0ah
	jne	go_rom			; Pass to the ROM if this is not a READ
A3:
	cmp	[UseROMFlag],0		; Check if we can jump into ROM directly
	dljz	go_rom			; Jump if we can
	cmp	UseROMFlag,1		; Check if set to not be used
	je	short A4		; Jump if can't
; Must set up UseROMFlag the first time
	mov	UseROMFlag,0		; Assume its OK to use
	SaveReg <es>
	mov	si,0f000h
	mov	es,si
	mov	di,0fff5h		; ROM bios string is at F000:FFF5
	mov	si,OFFSET BiosGroup:Bios_Date
	cld
CmpByte:
	cmpsb				; Compare ROM date string
	jnz	A3_1
	cmp	byte ptr [si - 1],0
	jnz	CmpByte
	mov	UseROMflag,1		; make non-zero to use ROM directly
A3_1:	RestoreReg <es>
	jmp	A3			; Try checks again
; Don't use ROM
A4:
	ENABLE				; Enable Interrupts
	PUSHA				; Save all registers during operation
	PUSH	DS
	PUSH	ES
	CALL	DISK_IO_CONT		; Perform the READ operation
	CALL	DDS
     ASSUME  DS:RomData
	MOV	AH,DISK_STATUS1 	; Get status from operation
	CMP	AH,1
	CMC				; Set 'C' to indicate success or failure
	POP	ES
	POP	DS
	POPA
	RET
IBM_DISK_IO	ENDP


;*****	DISK_IO_CONT - Perform the actual READ operation on the fixed disk
;
;	DISK_IO_CONT performs the actual READ operation on the fixed disk
;	by calling some routines in the ROM code segment, and using other
;	routines in this segment.
;
;	ENTRY	As for Int 13
;
;	EXIT	NONE
;
;	USES	ALL - No attempt is made to preserve registers
;
DISK_IO_CONT	PROC
	CALL	DDS			; DS = RomData
     ASSUME  DS:RomData
	MOV	DISK_STATUS1,0		; RESET THE STATUS INDICATOR
	PUSH	BX			; SAVE DATA ADDRESS
	MOV	BL,HF_NUM		; GET NUMBER OF DRIVES
	PUSH	AX
	AND	DL,7FH			; GET DRIVE AS 0 OR 1
	CMP	BL,DL
	JBE	BAD_COMMAND_POP 	; INVALID DRIVE
	PUSH	ES
	fcall	GET_VEC 		; GET DISK PARMS
	MOV	AX,WORD PTR ES:[BX][5]	; GET WRITE PRE-COMP CYL
	SHR	AX,2
	MOV	CMD_BLOCK,AL
	MOV	AL,BYTE PTR ES:[BX][8]	; GET CONTROL BYTE MODIFIER
	PUSH	DX
	MOV	DX,FX_RFDR
	OUT	DX,AL			; SET EXTRA HEAD OPTION
	POP	DX
	POP	ES
	MOV	AH,CONTROL_BYTE 	; SET EXTRA HEAD OPTION IN
	AND	AH,0C0H 		; CONTROL BYTE
	OR	AH,AL
	MOV	CONTROL_BYTE,AH
	POP	AX
	MOV	CMD_BLOCK+1,AL		; SECTOR COUNT
	PUSH	AX
	MOV	AL,CL			; GET SECTOR NUMBER
	AND	AL,3FH
	MOV	CMD_BLOCK+2,AL
	MOV	CMD_BLOCK+3,CH		; GET CYLINDER NUMBER
	MOV	AL,CL
	SHR	AL,6
	MOV	CMD_BLOCK+4,AL		; CYLINDER HIGH ORDER 2 BITS
	MOV	AL,DL			; DRIVE NUMBER
	SHL	AL,4
	AND	DH,0FH			; HEAD NUMBER
	OR	AL,DH
	OR	AL,80H OR 20H		; ECC AND 512 BYTE SECTORS
	MOV	CMD_BLOCK+5,AL		; ECC/SIZE/DRIVE/HEAD
	POP	AX
	POP	BX			; AND DATA ADDRESS
	PUSH	CX
	PUSH	AX			; ADJUST ES:BX
	MOV	CX,BX			; GET 3 HIGH ORDER NYBBLES OF BX
	SHR	CX,4
	MOV	AX,ES
	ADD	AX,CX
	MOV	ES,AX
	AND	BX,000FH		; ES:BX CHANGED TO ES:000X
	POP	AX
	POP	CX
	MOV	CMD_BLOCK+6,FX_CREAD
	cmp	ah,02h
	je	COMMANDI
	MOV	CMD_BLOCK+6,FX_CREAD OR FX_ECC
	jmp	short COMMANDI

BAD_COMMAND_POP:
	POP	AX
	POP	BX
BAD_COMMAND:
	MOV	DISK_STATUS1,BAD_CMD	 ; COMMAND ERROR
	MOV	AL,0
	RET

;*****	COMMANDI - Input data from the controller until done
;
;	COMMANDI repeatedly inputs data from the fixed disk controller until
;	NSECTOR is zero.
;
;	ENTRY	CMD_BLOCK is set up for request to controller
;
;	EXIT	NONE
;
COMMANDI:
	fcall	CHECK_DMA		; CHECK 64K BOUNDARY ERROR
	JC	CMD_ABORT
	MOV	DI,BX
	fcall	COMMAND 		; OUTPUT COMMAND
	JNZ	CMD_ABORT
CMD_I1:
	fcall	WAIT_			; WAIT FOR DATA REQUEST INT
	JNZ	TM_OUT			; TIME OUT
	MOV	CX,256D 		; SECTOR SIZE IN WORDS
	MOV	DX,FX_PDAT
	DISABLE
	CLD
	REP	INSW			; GET THE SECTOR
	ENABLE
	TEST	CMD_BLOCK+6,FX_ECC    ; CHECK FOR NORMAL INPUT
	JZ	CMD_I3
	fcall	WAIT_DRQ		; WAIT FOR DATA REQUEST
	JC	TM_OUT
	MOV	DX,FX_PDAT
	MOV	CX,4			; GET ECC BYTES
CMD_I2: IN	AL,DX
	MOV	ES:BYTE PTR [DI],AL	; GO SLOW FOR BOARD
	INC	DI
	LOOP	CMD_I2
CMD_I3: fcall	CHECK_STATUS
	JNZ	CMD_ABORT		; ERROR RETURNED
	DEC	CMD_BLOCK+1		; CHECK FOR MORE
	JNZ	SHORT CMD_I1
CMD_ABORT:
TM_OUT:
	RET

DISK_IO_CONT	ENDP


;*****	COMMAND - Output the command block to the controller
;
;	COMMAND sends out the command block in CMD_BLOCK to the fixed
;	disk controller by using the routine in the ROM code segment.
;
;	ENTRY	NONE
;
;	EXIT	BL  Status
;		BH  Error Register
;
COMMAND PROC
	PUSH	far_ret
	jmp	f_command
COMMAND ENDP


;*****	WAIT_ - Wait for an interrupt from the fixed disk controller
;
;	WAIT_ uses the routine in the ROM code segment.
;
;	ENTRY	NONE
;
;	EXIT	NONE
;
WAIT_	PROC
	PUSH	far_ret
	jmp	f_wait
WAIT_	ENDP


;*****	WAIT_DRQ - Wait for data request from the controller
;
;	WAIT_DRQ uses the routine in the ROM code segment.
;
;	ENTRY	NONE
;
;	EXIT	NONE
;
WAIT_DRQ	PROC
	PUSH	far_ret
	jmp	f_wait_drq
WAIT_DRQ	ENDP


;*****	CHECK_STATUS - Check the fixed disk status
;
;	CHECK_STATUS uses the routine in the ROM code segment.
;
;	ENTRY	NONE
;
;	EXIT	NONE
;
CHECK_STATUS	PROC
	PUSH	far_ret
	jmp	f_check_status
CHECK_STATUS	ENDP


;*****	CHECK_DMA - Check to make sure we don't cross 64K boundary
;
;	CHECK_DMA whecks to make sure that the request will not cross
;	over a 64K boundary.
;
;	ENTRY	ES:BX -> Data transfer area
;
;	EXIT	'C' clear if # SECTORS < 80H (7FH if LONG READ OR WRITE)
;		'C' clear IF # SECTORS = 80H (7FH) and BX <= 00H (04H)
;		'C' set if we will cross a 64K boundary otherwise.
;
CHECK_DMA	PROC
	PUSH	far_ret
	jmp	f_check_dma
CHECK_DMA	ENDP


;***** GET_VEC - Set up disk parameters
;
;	ENTRY	NONE
;
;	EXIT	ES:BX -> Disk parameters
;
GET_VEC PROC
	PUSH	far_ret
	jmp	f_get_vec
GET_VEC ENDP


BREAK <Support for External 3.5" diskette drives>

;
; We need to handle external drives specially because the ROM will only allow
; us to access physical drives 0 and 1 (floppy drives). The external drive,
; however, is physical drive 2 to the controller.
; The following code is the support for the external 3.5" drive - it assumes
; that if there is an external drive it is a 3.5" (720 KByte) drive.
;
; The code talks directly to the controller to perform the operations on
; the external drive.
;

;
; EQUATES & LABELS
;
DLY125MS1	EQU		0	; 16 BIT RAM VERSION DELAY COUNTS
DLY125MS2	EQU		27910
DLYNECOUT1	EQU		2
DLYNECOUT2	EQU		0
DLY1MS		EQU		800
DLYINT1 	EQU		4
DLYINT2 	EQU		0
NECDLY		EQU		20
;
;-------------- DISKETTE EQUATES
DMA		EQU	0
INT_FLAG	EQU	080H		; INTERRUPT OCCURRENCE FLAG
MOTOR_WAIT	EQU	37		; 2 SECS OF COUNTS FOR MOTOR TURN OFF
TIME_OUT	EQU	80H		; ATTACHMENT FAILED TO RESPOND
BAD_SEEK	EQU	40H		; SEEK OPERATION FAILED
BAD_NEC 	EQU	20H		; NEC CONTROLLER HAS FAILED
BAD_CRC 	EQU	10H		; BAD CRC ON DISKETTE READ
DMA_BOUNDARY	EQU	09H		; ATTEMPT TO DMA ACROSS 64K BOUNDARY
BAD_DMA 	EQU	08H		; DMA OVERRUN ON OPERATION
MEDIA_CHANGE	EQU	06H		; MEDIA REMOVED ON DUAL ATTACH CARD
RECORD_NOT_FND	EQU	04H		; REQUESTED SECTOR NOT FOUND
WRITE_PROTECT	EQU	03H		; WRITE ATTEMPTED ON WRITE PROT DISK
BAD_ADDR_MARK	EQU	02H		; ADDRESS MARK NOT FOUND
BAD_CMD 	EQU	01H		; BAD COMMAND PASSED TO DISK I/O
;
XRATE		EQU	02H		; 250KBS DATA TRANSFER RATE
;
HD720_SETTLE	EQU	020D		; 720 K HEAD SETTLE TIME
WRITE_OP	EQU	080H		; WRITE OPERATION FLAG
;
;------ DISK CHANGE LINE EQUATES
DSK_CHG 	EQU	080H		; DISKETTE CHANGE FLAG MASK


;*****	Drv2Int13 - Handle Int 13 requests for physical drive 2
;
;	Drv2Int13 executes the Int 13 requests for the external drive.
;	This is because the ROM only handles physical drives 0 and 1.
;
ASSUME DS:BiosGroup
Drv2Int13   PROC
	MOV	DS,CS:[RealModeDS]
	TEST	BYTE PTR [fHaveDRV2],0FFH
	JZ	d2i31			; Physical drive 2 is not present
	CALL	DISKETTE_IO
	RET

d2i31:
	MOV	AH,BAD_CMD
	STC
	RET

Drv2Int13   ENDP


;*****	DDS - Set up DS to point to RomData
;
;	ENTRY	NONE
;
;	EXIT	DS  RomData
;
;	USES	DS
;
DDS PROC NEAR
	PUSH	AX
	MOV	AX,RomData
	MOV	DS,AX
	POP	AX
	RET
DDS ENDP

CODE	ENDS

BiosExtDrv  SEGMENT

;*****	Diskette_IO - Provide Int 13 support for external 3.5" drive
;
;	Diskette_IO provides access to a 3 1/2", 720 KB, 80 track, 250 Kb/s,
;	9 sec/trk, changeline diskette drive located at physical drive 2.
;
;	ENTRY	As for Int 13
;
;	EXIT	As for Int 13
;
;	NOTE: Not all functions are supported. The ones supported are
;	      Reset, Read, Write, Verify, Format, Set DASD.
;	      The others return a 'bad command' error. This is done
;	      entirely to save space, since mose functions are not
;	      really that important for the external drive.
;	      The code is essentially that obtained from the DOS 3.2
;	      version of EXDSKBIO.DRV. Modifications
;	      have been made where it was thought necessary to do so.
;
;------------------------------------------------------------------------
;

ASSUME CS:BiosSeg, DS:BiosGroup, ES:NOTHING
	public ExtDrv2
ExtDrv2    label near
DISKETTE_IO	PROC	NEAR
	ENABLE				; INTERRUPTS BACK ON
	PUSHA
	PUSH	DS			; SAVE SEGMENT REGISTER VALUE
	MOV	BP,SP			; Set up pointer to the following:
BP_HEAD equ 0Dh 			; offset into stack to head
BP_SECT equ 0Eh 			; offset into stack to sector
BP_CYL	equ 0Fh 			; offset into stack to cylinder
BP_AH	equ 011h			; offset into stack to AH

	CALL	DDS
ASSUME	DS:RomData
	CALL	J1			; CALL THE REST TO ENSURE DS RESTORED
	XCHG	CX,AX			; SAVE NUMBER SECTORS XFERED IN AL
;
	MOV	BX,4			; GET THE MOTOR WAIT PARAMETER
	CALL	GET_PARM
	MOV	MOTOR_COUNT,AH		; SET THE TIMER COUNT FOR THE MOTOR
	XCHG	AX,CX			; RESTORE NUMBER OF SECTORS XFERED IN AL
	MOV	AH,DISKETTE_STATUS	; GET STATUS OF OPERATION
	CMP	AH,1			; SET THE CARRY FLAG TO INDICATE
	CMC				;  SUCCESS OR FAILURE
	MOV	[BP+BP_AH],AH		; RETURN STATUS
 DRET1:
	POP	DS
	POPA
	RET
DISKETTE_IO	ENDP
;
;
;------------------------------------------------------------------------------
;
J1	PROC	NEAR
	MOV	DH,AL			; SAVE # SECTORS IN DH
	AND	MOTOR_STATUS,07FH	; INDICATE A READ OPERATION
;
RJ2:	MOV	DISKETTE_STATUS,0	; RESET THE STATUS INDICATOR
	CMP	AH,16H			; READ DISK CHANGE LINE STATUS ?
	JNE	RJ4			; JUMP IF NOT
	JMP	MEDIACHNG
RJ4:	CMP	AH,17H			; SET DASD TYPE COMMAND ?
	JNE	RJ5
	JMP	SETDASD
;
RJ5:	CALL	RDCHGLN 		; IS DISKETTE CHANGE LINE ACTIVE ?
	JZ	RC1			; JUMP IF NOT
	JMP	RESETCHNGLN
;
RC1:	PUSH	AX			;SAVE
	PUSH	DX
	OR	LASTRATE,0C0H		;SET ILLEGAL RATE BITS
	MOV	AL,XRATE		;250KB/S TRANSFER RATE
	MOV	DX,3F7H 		;DATA RATE OUTPUT REGISTER
	OUT	DX,AL			;SET DATA TRANSFER RATE
	POP	DX			;RESTORE
	POP	AX
;
	CMP	AH,2			; AH=2? (READ COMMAND)
	JNE	RJ6
	JMP	DISK_READ
RJ6:	CMP	AH,3			; AH=3? (WRITE COMMAND)
	JNE	J2			; TEST_DISK_VERF
	JMP	DISK_WRITE
;					; TEST_DISK_VERF
J2:	CMP	AH,4			; AH=4? (VERIFY COMMAND)
	JZ	DISK_VERF
	CMP	AH,5			; AH=5? (FORMAT COMMAND)
	JZ	DISK_FORMAT
;
J3:	MOV	DISKETTE_STATUS,BAD_CMD ; ERROR CODE, NO SECTORS TRANSFERRED
	RET				; UNDEFINED OPERATION
;
; ATTEMPT TO RESET DISKETTE MEDIA CHANGE LINE
;
RESETCHNGLN: CALL    Reset_Floppy	;RESET DISKETTE SYSTEM
	     CALL    OUTENSEL		   ;SET DRIVE 2 MOTOR EN & DRV SEL
	     ENABLE			   ;INTERRUPTS BACK ON
	     MOV     DX,2		   ;DRIVE 2
	     MOV     CH,1		   ;TRACK 1
	     CALL    I13Seek		   ;SEEK TO TRACK 1
	     MOV     DX,2		   ;DRIVE 2
	     XOR     CH,CH		   ;TRACK 0
	     CALL    I13Seek		   ;SEEK TO TRACK 0
	     MOV     DISKETTE_STATUS,MEDIA_CHANGE ; SET MEDIA CHANGE STATUS
;
	     CALL    RDCHGLN		   ;DID CHG LINE RESET ?
	     JZ      RSTCL1		   ;JUMP IF YES
	     MOV     DISKETTE_STATUS,TIME_OUT ;SET TIME OUT STATUS
RSTCL1:      RET
;
J1	ENDP


;*****	SETDASD - Set the DASD type of the drive for FORMAT
;
;	ENTRY	NONE
;
;	EXIT	NONE
;
SETDASD      PROC    NEAR
	     CMP     AL,4		   ;720KB IN 720KB REQUESTED ?
	     JNE     J3 		   ;ILLEGAL COMMAND IF NOT
	     CALL    RDCHGLN		   ;READ CHANGE LINE
	     JZ      SDRET1		   ;JUMP IF NOT ACTIVE
	     JMP     RESETCHNGLN	   ;GO ATTEMPT CHANGE LINE RESET
SDRET1:      RET
SETDASD      ENDP


;*****	DISK_READ - Perform the read operation on the disk
;
;	ENTRY	As for Int 13
;
;	EXIT	As for Int 13
;
DISK_READ	PROC	NEAR
	MOV	AL,046H 		; READ COMMAND FOR DMA
J9:					; DISK_READ_CONT
	CALL	DMA_SETUP		; SET UP THE DMA
	MOV	AH,0E6H 		; SET UP RD COMMAND FOR NEC CONTROLLER
	JMP	SHORT RW_OPN		; GO DO THE OPERATION
DISK_READ	ENDP


;*****	DISK_VERF - Perform the verify operation on the disk
;
;	ENTRY	As for Int 13
;
;	EXIT	As for Int 13
;
DISK_VERF	PROC	NEAR
	MOV	AL,042H 		; VERIFY COMMAND FOR DMA
	JMP	J9			; DO AS IF DISK READ
DISK_VERF	ENDP


;*****	DISK_FORMAT - Perform the format operation on the disk
;
;	ENTRY	As for Int 13
;
;	EXIT	As for Int 13
;
DISK_FORMAT	PROC	NEAR
	OR	MOTOR_STATUS,80H	; INDICATE WRITE OPERATION
	MOV	AL,04AH 		; WILL WRITE TO THE DISKETTE
	CALL	DMA_SETUP		; SET UP THE DMA
	MOV	AH,04DH 		; ESTABLISH THE FORMAT COMMAND
	JMP	SHORT RW_OPN		; DO THE OPERATION
J10:					; CONTINUATION OF RW_OPN FOR FMT
	MOV	BX,7			; GET THE
	CALL	GET_PARM		;  BYTES/SECTOR VALUE TO NEC
	MOV	BX,9			; GET THE
	CALL	GET_PARM		;  SECTORS/TRACK VALUE TO NEC
	MOV	BX,15			; GET THE
	CALL	GET_PARM		;  GAP LENGTH VALUE TO NEC
	MOV	BX,17			; GET THE FILLER BYTE
	JMP SHORT J16			;  TO THE CONTROLLER
DISK_FORMAT	ENDP


;*****	MEDIACHNG - Read the changeline status
;
;	ENTRY	As for Int 13
;
;	EXIT	As for Int 13
;
MEDIACHNG    PROC    NEAR
	     CALL    RDCHGLN		   ;READ CURRENT CHANGE LINE STATUS
	     JZ      MC1		   ;JUMP IF NOT ACTIVE
	     MOV     DISKETTE_STATUS,MEDIA_CHANGE ;SET MEDIA CHANGE STATUS
MC1:	     RET
MEDIACHNG    ENDP


;*****	DISK_WRITE - Perform the write operation on the disk
;
;	ENTRY	As for Int 13
;
;	EXIT	As for Int 13
;
DISK_WRITE	PROC	NEAR
	OR	MOTOR_STATUS,80H	; INDICATE WRITE OPERATION
	MOV	AL,04AH 		; DMA WRITE COMMAND
	CALL	DMA_SETUP
	MOV	AH,0C5H 		; NEC COMMAND TO WRITE TO DISKETTE
IF2
    ERRNZ   <$-RW_OPN>
ENDIF
;
; * *  ALLOW WRITE ROUTINE TO FALL INTO RW_OPN * *
;
DISK_WRITE	ENDP


;*****	RW_OPN - Performs tha actual READ/WRITE/VERIFY operaion
;
;	ENTRY	AL  CODE for operation
;
;	EXIT	As for Int 13
;
RW_OPN	PROC	NEAR
	JNC	J11			; TEST FOR DMA ERROR
	MOV	DISKETTE_STATUS,DMA_BOUNDARY	; SET ERROR
	MOV	AL,0			; NO SECTORS TRANSFERRED
	RET				; RETURN TO MAIN ROUTINE
J11:					; DO_RW_OPN
	PUSH	AX			; SAVE THE COMMAND
	CALL	MOTOR_ON		; SELECT DRIVE & TURN ON MOTOR
;
;----- DO THE SEEK OPERATION
;
	CALL	I13Seek 		; MOVE TO CORRECT TRACK
	POP	AX			; RECOVER COMMAND
	MOV	BH,AH			; SAVE COMMAND IN BH
	MOV	DH,0			; SET NO SECTORS READ IN CASE OF ERROR
	JC	J17			; IF ERROR, THEN EXIT AFTER MOTOR OFF
	MOV	SI,OFFSET J17		; DUMMY RETURN ON STACK FOR NEC_OUTPUT
	PUSH	SI			;  SO THAT IT WILL RETURN TO MOTOR OFF
;					;  LOCATION
;
;----- SEND OUT THE PARAMETERS TO THE CONTROLLER

	CALL	NEC_OUTPUT		; OUTPUT THE OPERATION COMMAND
	MOV	AH,[BP+BP_HEAD] 	; GET THE CURRENT HEAD NUMBER
	SAL	AH,2			; MOVE IT TO BIT 2
	AND	AH,4			; ISOLATE THAT BIT
	OR	AH,2			; OR IN THE DRIVE NUMBER (2)
	CALL	NEC_OUTPUT
;
;----- TEST FOR FORMAT COMMAND
;
	CMP	BH,04DH 		; IS THIS A FORMAT OPERATION
	JNE	J15			; NO. CONTINUE WITH R/W/V
	JMP	J10			; IF SO, HANDLE SPECIAL
;
J15:	MOV	AH,CH			; GET CYLINDER NUMBER
	CALL	NEC_OUTPUT
	MOV	AH,[BP+BP_HEAD] 	; HEAD NUMBER OFF STACK
	CALL	NEC_OUTPUT
	MOV	AH,CL			; SECTOR NUMBER
	CALL	NEC_OUTPUT
	MOV	BX,7			; BYTES/SECTOR PARM FROM BLOCK
	CALL	GET_PARM		;  TO THE NEC
	MOV	BX,9			; EOT PARM FROM BLOCK
	CALL	GET_PARM		;  TO THE NEC
;
	MOV	AH,02AH 		;SET 720KB GAP LENGTH
	CALL	NEC_OUTPUT
;
	MOV	BX,13			; DTL PARM FROM BLOCK
;					; RW_OPN_FINISH
J16:	CALL	GET_PARM		;  TO THE NEC
	POP	SI			; CAN NOW DISCARD THAT DUMMY
;					;  RETURN ADDRESS
;
;----- LET THE OPERATION HAPPEN
;
	CALL	WAIT_INT		; WAIT FOR THE INTERRUPT
;					; MOTOR_OFF
J17:	JC	J21			; LOOK FOR ERROR
	CALL	RESULTS 		; GET THE NEC STATUS
	JC	J20			; LOOK FOR ERROR
;
;----- CHECK THE RESULTS RETURNED BY THE CONTROLLER
;
	CLD				; SET THE CORRECT DIRECTION
	MOV	SI,OFFSET NEC_STATUS	; POINT TO STATUS FIELD
	LODS	NEC_STATUS		; GET ST0
	AND	AL,0C0H 		; TEST FOR NORMAL TERMINATION
	JZ	J22			; OPN_OK
	CMP	AL,040H 		; TEST FOR ABNORMAL TERMINATION
	JNZ	J18			; NOT ABNORMAL, BAD NEC
;
;----- ABNORMAL TERMINATION, FIND OUT WHY
;
	LODS	NEC_STATUS		; GET ST1
	SAL	AL,1			; TEST FOR EOT FOUND
	MOV	AH,RECORD_NOT_FND
	JC	J19			; RW_FAIL
	SAL	AL,2			; TEST FOR CRC ERROR
	MOV	AH,BAD_CRC
	JC	J19			; RW_FAIL
	SAL	AL,1			; TEST FOR DMA OVERRUN
	MOV	AH,BAD_DMA
	JC	J19			; RW_FAIL
	SAL	AL,2			; TEST FOR RECORD NOT FOUND
	MOV	AH,RECORD_NOT_FND
	JC	J19			; RW_FAIL
	SAL	AL,1
	MOV	AH,WRITE_PROTECT	; TEST FOR WRITE_PROTECT
	JC	J19			; RW_FAIL
	SAL	AL,1			; TEST MISSING ADDRESS MARK
	MOV	AH,BAD_ADDR_MARK
	JC	J19			; RW_FAIL
;
;----- NEC MUST HAVE FAILED
;
;					; RW-NEC-FAIL
J18:	MOV	AH,BAD_NEC
;					; RW-FAIL
J19:	OR	DISKETTE_STATUS,AH
	CALL	NUM_TRANS		; HOW MANY WERE REALLY TRANSFERRED
;					; RW_ERR
J20:	RET				; RETURN TO CALLER
;					; RW_ERR_RES
J21:	CALL	RESULTS 		; FLUSH THE RESULTS BUFFER
	RET
;
;----- OPERATION WAS SUCCESSFUL
;					; OPN_OK
J22:	CALL	NUM_TRANS		; HOW MANY GOT MOVED
	XOR	AH,AH			; NO ERRORS
	RET
RW_OPN	ENDP


;*****	DMA_SETUP Set up the DMA chip for the I/O operation
;
;	ENTRY	(AL) = MODE BYTE FOR THE DMA
;		(ES:BX) - ADDRESS TO READ/WRITE THE DATA
;
;	EXIT	'C' set if transfer will cross 64K boundary
;
;	USES	AX
;
DMA_SETUP	PROC	NEAR
	PUSH	CX			; SAVE THE REGISTER
	DISABLE 			; NO MORE INTERRUPTS
	OUT	DMA+12,AL		; SET THE FIRST/LAST F/F
	JMP	$+2			; DELAY
	OUT	DMA+11,AL		; OUTPUT THE MODE BYTE
	MOV	AX,ES			; GET THE ES VALUE
	ROL	AX,4			; ROTATE LEFT
	MOV	CH,AL			; GET HIGHEST NYBLE OF ES TO CH
	AND	AL,0F0H 		; ZERO THE LOW NYBBLE FROM SEGMENT
	ADD	AX,BX			; TEST FOR CARRY FROM ADDITION
	JNC	J33
	INC	CH			; CARRY MEANS HIGH 4 BITS MUST BE INC
J33:
	PUSH	AX			; SAVE START ADDRESS
	OUT	DMA+4,AL		; OUTPUT LOW ADDRESS
	JMP	$+2
	MOV	AL,AH
	OUT	DMA+4,AL		; OUTPUT HIGH ADDRESS
	MOV	AL,CH			; GET HIGH 4 BITS
	JMP	$+2
	AND	AL,0FH
	OUT	081H,AL 		; OUTPUT THE HIGH 4 BITS TO
					;  THE PAGE REGISTER
;
;----- DETERMINE COUNT
;
	MOV	AH,DH			; NUMBER OF SECTORS
	SUB	AL,AL			;  TIMES 256 INTO AX
	SHR	AX,1			; SECTORS * 128 INTO AX
	PUSH	AX
	MOV	BX,6			; GET THE BYTES/SECTOR PARM
	CALL	GET_PARM
	MOV	CL,AH			; USE AS SHIFT COUNT (0=128, 1=256 ETC)
	POP	AX
	SHL	AX,CL			; MULTIPLY BY CORRECT AMOUNT
	DEC	AX			; -1 FOR DMA VALUE
	PUSH	AX			; SAVE COUNT VALUE
	OUT	DMA+5,AL		; LOW BYTE OF COUNT
	JMP	$+2
	MOV	AL,AH
	OUT	DMA+5,AL		; HIGH BYTE OF COUNT
	ENABLE				; INTERRUPTS BACK ON
	POP	CX			; RECOVER COUNT VALUE
	POP	AX			; RECOVER ADDRESS VALUE
	ADD	AX,CX			; ADD, TEST FOR 64K OVERFLOW
	POP	CX			; RECOVER REGISTER
	MOV	AL,FD_DMA		; MODE FOR 8237
	OUT	DMA+10,AL		; INITIALIZE THE DISKETTE CHANNEL
	RET
;					;  'C' SET BY ABOVE IF ERROR
DMA_SETUP	ENDP


;*****	MOTOR_ON - Turn on the motor for the external drive
;
;	MOTOR_ON outputs motor enable & drive select for drive 2, sets motor
;	on and drive select bits in motor status, & waits for motor start
;	if the motor bit in motor status was not already set. A motor wait
;	delay loop is executed.
;
;	NOTE: This is the only routine that sets the motor on bit in
;	      [MOTOR_STATUS].
;
;	ENTRY	NONE
;
;	EXIT	NONE
;
MOTOR_ON     PROC    NEAR
	     PUSH    AX 		   ;SAVE USED REGISTERS
	     PUSH    BX
	     PUSH    CX
	     CALL    OUTENSEL		   ;SET DRIVE 2 SELECT & MOTOR EN BITS
	     TEST    MOTOR_STATUS,4	   ;WAS DRIVE 2 MOTOR BIT ALREADY SET ?
	     PUSHF			   ;SAVE RESULT
	     AND     MOTOR_STATUS,0CFH	   ;RESET DRIVE SELECT BITS
	     OR      MOTOR_STATUS,24H	   ;SET DRIVE 2 MOTOR & SELECT BITS
	     POPF			   ;RESTORE
	     ENABLE			   ;INTERRUPTS BACK ON
	     JNZ     NOWAIT		   ;JUMP IF MOTOR BIT ALREADY ON
;
	     MOV     BX,20		   ;GET MOTOR WAIT PARAMETER
	     CALL    GET_PARM		   ;  INTO AH
	     OR      AH,AH		   ;ANY MOTOR WAIT SPECIFIED ?
	     JZ      NOWAIT		   ;JUMP IF NOT (NO WAIT)
;
MLP1:	     MOV     CX,DLY125MS1	   ;SET 1/8 SECOND DELAY LOOP
MLP2:	     LOOP    MLP2		   ;DELAY
	     MOV     CX,DLY125MS2	   ; 2ND HALF
MLP3:	     LOOP    MLP3		   ;DELAY
	     DEC     AH 		   ;COUNT EXPIRED ?
	     JNZ     MLP1
;
NOWAIT:      POP     CX 		   ;RESTORE USED REGISTERS
	     POP     BX
	     POP     AX
	     RET
MOTOR_ON     ENDP


;*****	WAIT_INT - Wait for a floppy interrupt
;
;	WAIT_INT waits for a floppy interrupt to occur. There is also
;	a time out routine so that an error may be returned if the drive
;	is not ready.
;
;	ENTRY	NONE
;
;	EXIT	'C' clear if no time out
;		'C' set if failure; [DISKETTE_STATUS] set accordingly
;
;	USES	AX
;
WAIT_INT    PROC    NEAR
	ENABLE			       ; TURN ON INTERRUPTS, JUST IN CASE
	PUSH	AX			; SAVE REGISTERS
	PUSH	BX			; *
	PUSH	CX			; *
	CLC				; CLEAR TIMEOUT INDICATOR
;
	MOV	BL,DLYINT1		; CLEAR THE COUNTERS
W1:	MOV	CX,DLYINT2		; FOR 2 SECOND WAIT
W2:	TEST	SEEK_STATUS,INT_FLAG	; TEST FOR INTERRUPT OCCURRING
	JNZ	W3
	LOOP	W2			; COUNT DOWN WHILE WAITING
	DEC	BL			; SECOND LEVEL COUNTER
	JNZ	W1
;
WERR:	OR	DISKETTE_STATUS,TIME_OUT ; NOTHING HAPPENED
	STC				; ERROR RETURN
;
W3:	PUSHF				; SAVE CURRENT CARRY
	AND	SEEK_STATUS,NOT INT_FLAG ; TURN OFF INTERRUPT FLAG
	POPF				; RECOVER CARRY
	POP	CX			; RECOVER REGISTERS
	POP	BX			; *
	POP	AX			; *
	RET				; GOOD RETURN CODE COMES FROM TEST INST
WAIT_INT    ENDP


;*****	I13Seek - Seek to the desired cylinder
;
;	I13Seek moves the head of drive 2 to the desired cylinder. If the
;	drive has not been accessed since the last RESET command was issued,
;	the drive will first be Recalibrated. One additional Recalibrate is
;	issued if the first cails to accomodate drives with more than 77
;	tracks.
;
;	ENTRY	CH  Desired cylinder to seek to
;
;	EXIT	'C' set if seek failure; [DISKETTE_STATUS] set accordingly
;		'C' clear if successful operation
;
I13Seek      PROC    NEAR
	     MOV     AL,4		   ;RECAL MASK FOR DRIVE 2
	     TEST    SEEK_STATUS,AL	   ;RECAL BIT SET FOR DRIVE 2 ?
	     JNZ     SK2		   ;JUMP IF NO RECAL REQUIRED
	     PUSH   DS
	     MOV    DS,CS:[RealModeDS]
ASSUME DS:BiosGroup
	     MOV    DS:OldTrack,0ffH	  ;RESET CURRENT TRACK POSITION
	     POP    DS
ASSUME DS:RomData
	     OR      SEEK_STATUS,AL	   ;RESET RECAL BIT FOR DRIVE 2
	     CALL    I13Recal		   ;ISSUE RECALIBRATE
	     JNC     SK2_a		   ;JUMP IF NO ERROR
;
; ISSUE 2ND RECALIBRATE
	     MOV     DISKETTE_STATUS,0	   ;CLEAR OUT INVALID STATUS
	     CALL    I13Recal		   ;ISSUE RECALIBRATE
	     JC      SKERROR		   ;JUMP IF ERROR
;
; SPECIFY
;
SK2_a:
	     MOV     AH,03H		   ;NEC SPECIFY COMMAND
	     CALL    NEC_OUTPUT 	   ;ISSUE
	     MOV     AH,0DFh		   ;DiskSPECIFY_1
	     CALL    NEC_OUTPUT 	   ;ISSUE
	     MOV     AH,2		   ;DiskSPECIFY_2
	     CALL    NEC_OUTPUT 	   ;ISSUE TO NEC
;
; SEEK TO DESIRED TRACK
;
SK2:
	     PUSH   DS
	     MOV    DS,CS:[RealModeDS]
ASSUME DS:BiosGroup
	     CMP    CH,DS:OldTrack	  ;DESIRED TRACK=CURRENT TRACK ?
	     POP    DS
ASSUME DS:RomData
	     JE      NOSEEKREQ		   ;NO SEEK REQUIRED IF YES (& CY SET=0)
	     PUSH   DS
	     MOV    DS,CS:[RealModeDS]
ASSUME DS:BiosGroup
	     MOV    DS:OldTrack,CH	  ;SAVE DESIRED TRACK AS CURRENT
	     POP    DS
ASSUME DS:RomData
;
	     MOV     AH,0FH		   ;NEC SEEK COMMAND
	     CALL    NEC_OUTPUT 	   ;ISSUE
	     MOV     AH,2		   ;DRIVE NUMBER
	     CALL    NEC_OUTPUT 	   ;ISSUE
	     MOV     AH,CH		   ;GET DESIRED TRACK
	     CALL    NEC_OUTPUT 	   ;ISSUE TO NEC
	     CALL    CHK_STAT_2 	   ;CHECK RESULTING STATUS
	     PUSHF			   ; & SAVE
;
	     MOV     BX,18		   ;GET HEAD SETTLE PARAMETER
	     CALL    GET_PARM		   ; INTO AH
	     OR      AH,AH		   ;TEST FOR NO HEAD SETTLE DELAY
	     JZ      SEEKDONE		   ;SEEK COMPLETE IF ZERO
;
	     PUSH    CX
SK4:	     MOV     CX,DLY1MS		   ;1 MS DELAY
SK3:	     LOOP    SK3		   ;DELAY FOR 1 MS
	     DEC     AH 		   ;DECREMENT COUNT
	     JNZ     SK4		   ;JUMP IF NOT DONE
	     POP     CX
;
SEEKDONE:    POPF			   ;RESTORE STATUS FROM NEC SEEK
NOSEEKREQ:
SKERROR:     RET
I13Seek      ENDP


;*****	I13Recal - Recalibrate drive 2
;
;	ENTRY	NONE
;
;	EXIT	'C' set if recalibrate fails
;		'C' clear if successful operation
;
I13Recal     PROC    NEAR
	     MOV     AH,7
	     CALL    NEC_OUTPUT 	   ;ISSUE RECALIBRATE
	     MOV     AH,2		   ; (RETURN TO CALLER IF NECOUT FAILS)
	     CALL    NEC_OUTPUT 	   ;DRIVE NUMBER
	     CALL    CHK_STAT_2
	     RET
I13Recal     ENDP


;*****	GET_PARM - Get the floppy parameter and maybe issue it to the NEC
;
;	GET_PARM fetches the indexed pointer from the Disk_Base pointed to
;	by DSKADR. The byte indexed by BX into this table is moved into AH.
;	If the low bit of BX is on, the byte is immediately output to the
;	NEC controller.
;
;	ENTRY	BX  Index of byte to be fetched * 2
;
;	EXIT	AH  Byte from the DPT
;
GET_PARM	PROC	NEAR
	PUSH	DS			; SAVE SEGMENT
	PUSH	SI			; SAVE
	SUB	AX,AX			; ZERO TO AX
	MOV	DS,AX
ASSUME	DS:ABS0
	LDS	SI,DS:[DSKADR]		; POINT TO BLOCK
	SHR	BX,1			; DIVIDE BX BY 2, AND SET FLAG FOR EXIT
	MOV	AH,[SI+BX]		; GET THE WORD
	POP	SI			; RESTORE
	POP	DS			; RESTORE SEGMENT
ASSUME	DS:RomData
	PUSHF				; SAVE RESULTS FOR EXIT
	CMP	BX,10			; LOOK FOR MOTOR STARTUP DELAY PARM
	JNE	GP0			; BYPASS IF NOT PARM LOOKING FOR
	TEST	MOTOR_STATUS,WRITE_OP	; IS THIS A WRITE
	JZ	GP1			; NO, ENFORCE MINIMUM READ WAIT
	CMP	AH,8			; SEE IF AT LEAST A SECOND IS SPECIFIED
	JAE	GP2			; IF YES, CONTINUE
	MOV	AH,8			; FORCE A SECOND WAIT FOR MOTOR START
	JMP	SHORT GP2		; CONTINUE
;
GP1:	CMP	AH,5			; SEE IF A 625 MS WAIT ON READ
	JAE	GP2			; IF THERE GO CONTINUE
	MOV	AH,5			; ENFORCE A 625 MS WAIT
	JMP	SHORT GP2		; CONTINUE
;
GP0:	CMP	BX,9			; IS THIS HEAD SETTLE PARM
	JNE	GP2			; BYPASS IF NOT HEAD SETTLE
	TEST	MOTOR_STATUS,WRITE_OP	; SEE IF A WRITE OPERATION
	JZ	GP2			; IF NOT, DONT ENFORCE ANY VALUES
	OR	AH,AH			; CHECK FOR ANY WAIT?
	JNZ	GP2			; IF THERE DONT ENFORCE
	MOV	AH,HD720_SETTLE 	; FORCE SPEC'ED 720KB HEAD SETTLE
;
GP2:	POPF				; RESTORE EXIT RESULTS
	JC	NEC_OUTPUT		; IF FLAG SET, OUTPUT TO CONTROLLER
	RET				; RETURN TO CALLER
GET_PARM	ENDP


;*****	NEC_OUTPUT - Send a byte to the NEC controller
;
;	ENTRY	(AH)	BYTE TO BE OUTPUT
;
;	EXIT	NONE
;
;	USES	AX
;
NEC_OUTPUT	PROC	NEAR
	PUSH	DX
	XCHG	AH,AL		; Put765 expects byte in AL
	CALL	Put765
	POP	DX
	RET
NEC_OUTPUT	ENDP


;*****	RESULTS - Read all the data output by the NEC controller
;
;	RESULTS will read anything that the NEC controller outputs following
;	an interrupt.
;
;	ENTRY	NONE
;
;	EXIT	'C' clear if no error returned
;		'C' set if time out waiting for status
;		[NEC_STATUS] has status byte in it
;
;	USES	AH
;
RESULTS PROC	NEAR
	CLD
	MOV	DI,OFFSET NEC_STATUS	; POINTER TO DATA AREA
	PUSH	CX			; SAVE COUNTER
	PUSH	DX
	PUSH	BX
	MOV	BL,7			; MAX STATUS BYTES
;
;------ WAIT FOR REQUEST FOR MASTER
;
RSLT1:	MOV	BH,DLYNECOUT1		; HIGH ORDER COUNTER
RSLT2:	MOV	CX,DLYNECOUT2		; COUNTER
	MOV	DX,03F4H		; STATUS PORT
;
RSLT3:	IN	AL,DX			; GET STATUS
	TEST	AL,080H 		; MASTER READY
	JNZ	RSLT5			; TEST_DIR
	LOOP	RSLT3			; WAIT_MASTER
	DEC	BH			; DECREMENT HIGH ORDER COUNTER
	JNZ	RSLT2			; REPEAT TIL DELAY DONE
;
; RESULTS ERROR
;
	OR	DISKETTE_STATUS,TIME_OUT
RSLT4:	STC				; SET ERROR RETURN
	POP	BX
	POP	DX
	POP	CX
	RET
;
;------ TEST THE DIRECTION BIT
;
RSLT5:	IN	AL,DX		; GET STATUS REG AGAIN
	TEST	AL,040H 	; TEST DIRECTION BIT
	JNZ	RSLT7		; OK TO READ STATUS
;
; NEC_FAIL
;
RSLT6:	OR	DISKETTE_STATUS,BAD_NEC
	JMP	RSLT4		; RESULTS_ERROR
;
;------ READ IN THE STATUS
;
RSLT7:	INC	DX		; POINT AT DATA PORT
	IN	AL,DX			; GET THE DATA
	MOV	[DI],AL 		; STORE THE BYTE
	INC	DI			; INCREMENT THE POINTER
	MOV	CX,NECDLY		; LOOP TO KILL TIME FOR NEC
RSLT8:	LOOP	RSLT8
	DEC	DX			; POINT AT STATUS PORT
	IN	AL,DX			; GET STATUS
	TEST	AL,010H 		; TEST FOR NEC STILL BUSY
	JZ	RSLT9			; RESULTS DONE
	DEC	BL			; DECREMENT THE STATUS COUNTER
	JNZ	RSLT1		       ; GO BACK FOR MORE
	JMP	RSLT6			; CHIP HAS FAILED
;
;------ RESULT OPERATION IS DONE
;
RSLT9:	POP	BX
	POP	DX
	POP	CX			; RECOVER REGISTERS
	RET				; GOOD RETURN CODE FROM TEST INST
RESULTS ENDP


;*****	RDCHGLN - Read the change line status
;
;	RDCHGLN does the following actions:
;	    - Sets max motor count to eliminate timeout interrupt motor off
;	    - Outputs drive 2 select & motor enable to drive ctrl register
;	    - Reads state of diskette change line
;	    - Turns off drv2 motor enable bit in motor status if change
;	      line set
;
;	ENTRY	NONE
;
;	EXIT	'Z' set if changeline not active
;		'Z' clear (NZ) if changeline active
;
RDCHGLN      PROC    NEAR
	     CALL    OUTENSEL		   ;DRIVE 2 SELECT & MOTOR ENABLE
;
	     PUSH    AX 		   ;SAVE USED REGISTERS
	     PUSH    DX
	     MOV     DX,3F7H		   ;DSK CHNG INPUT REGISTER ADDR
	     JMP     $+2		   ;DELAY
	     IN      AL,DX		   ;READ CHANGE LINE BIT
	     TEST    AL,DSK_CHG 	   ;CHANGE LINE BIT SET ?
	     PUSHF			   ;SAVE
	     JZ      RCLN1		   ;JUMP IF NO CHANGE LINE
	     AND     MOTOR_STATUS,0FBH	   ;MOTOR BIT OFF IF CHANGE LINE SET
RCLN1:	     POPF			   ;RESTORE CHANGE LINE STATE
	     ENABLE			   ;INTERRUPTS BACK ON
	     POP     DX 		   ;RESTORE USED REGISTERS
	     POP     AX
	     RET
RDCHGLN      ENDP


;*****	OUTENSEL - Output drive 2 motor enable & drive select
;
;	OUTENSEL does the following actions:
;	    - Disables interrupts & sets max time-out motor count
;	    - Copy existing motor status
;	    - Set drive 2 motor enable & selects bits
;	    - Adjust to drive control output register format
;	    - Output to register
;
;	ENTRY	NONE
;
;	EXIT	Interrupts turned OFF
;
OUTENSEL    PROC    NEAR
	    SaveReg <AX,CX,DX>
	    MOV     DX,3F2H		;SET DRIVE CONTROL REG ADDR
	    DISABLE			;NO INTERRUPTS
	    MOV     MOTOR_COUNT,0FFH	;SET MAX TIME-OUT COUNT
	    MOV     AL,MOTOR_STATUS	;GET CURRENT MOTOR STATUS
	    AND     AL,0FH		;CLEAR DRIVE SELECT & CTRL BITS
	    OR	    AL,24H		;SET DRIVE 2 SELECT & MOTOR BITS
	    ROL     AL,4		;SWAP NIBBLES
	    OR	    AL,0CH		;SET DMA & NO RESET BITS
	    OUT     DX,AL		;OUTPUT TO REGISTER
	    OR	    MOTOR_STATUS,04h	;Set motor on bit
	    RestoreReg <DX,CX,AX>
	    RET
OUTENSEL    ENDP


;*****	CHK_STAT_2 - Sense interrupt status
;
;	CHK_STAT_2 handles the interrupt received after a Recalibrate,
;	Seek or Reset operation. It waits for the interrupt, senses
;	the interrupt status, and returns the result to the caller.
;
;	ENTRY	NONE
;
;	EXIT	'C' clear of success
;		'C' set if failure - error is in [DISKETTE_STATUS]
;
;	USES	AX
;
CHK_STAT_2	PROC	NEAR
	CALL	WAIT_INT		; WAIT FOR THE INTERRUPT
	JC	J34			; IF ERROR, RETURN IT
	MOV	AH,08H			; SENSE INTERRUPT STATUS COMMAND
	CALL	NEC_OUTPUT
	CALL	RESULTS 		; READ IN THE RESULTS
	JC	J34			; CHK2_RETURN
	MOV	AL,NEC_STATUS		; GET THE FIRST STATUS BYTE
	AND	AL,060H 		; ISOLATE THE BITS
	CMP	AL,060H 		; TEST FOR CORRECT VALUE
	JZ	J35			; IF ERROR, GO MARK IT
	CLC				; GOOD RETURN
J34:
	RET				; RETURN TO CALLER
J35:					; CHK2_ERROR
	OR	DISKETTE_STATUS,BAD_SEEK
	STC				; ERROR RETURN CODE
	RET
CHK_STAT_2	ENDP


;*****	NUM_TRANS - Calculate the number of sectors transferred
;
;	ENTRY	[BP+BP_CYL]  Start cylinder of operation
;		[BP+BP_SECT] Start sector of operation
;		[BP+BP_HEAD] Start head
;
;	EXIT	AL	    Number transferred
;
;	USES	AX
;
NUM_TRANS    PROC    NEAR
	     XOR     AL,AL		   ;CLEAR FOR ERROR
	     CMP     DISKETTE_STATUS,0	   ;ERROR ?
	     JNZ     NTRET		   ;NON XFERRED IF ERROR
	     MOV     BX,8
	     CALL    GET_PARM		   ;GET SECTORS/TRACK INTO AH
	     MOV     AL,NEC_STATUS+5	   ;GET ENDING SECTOR
	     MOV     CH,NEC_STATUS+3	   ;GET ENDING TRACK
	     CMP     CH,[BP+BP_CYL]	   ;SAME AS STARTING TRACK ?
	     JE      SAME_TRK		   ;JUMP IF SAME
	     ADD     AL,AH		   ;ADD IN 2*SECTORS/TRACK
	     ADD     AL,AH
	     JMP SHORT GETSTRT
;
SAME_TRK:    CMP     BYTE PTR NEC_STATUS+4,1 ;ENDING HEAD=1 ?
	     JNE     GETSTRT
	     ADD     AL,AH		   ;ADD IN SEC/TRK OFFSET FOR HEAD 1
;
GETSTRT:     MOV     BL,[BP+BP_SECT]	   ;GET STARTING SECTOR
	     CMP     BYTE PTR[BP+BP_HEAD],1  ;STARTING HEAD = 1 ?
	     JNE     CALCNUM		   ;JUMP IF NOT
	     ADD     BL,AH		   ;ADD IN SEC/TRK OFFSET
CALCNUM:     SUB     AL,BL
;
NTRET:	     RET
NUM_TRANS    ENDP


BiosExtDrv  ENDS

CodeInit    SEGMENT


	Break <Int13Init - Int 13 Initialization>
;**	INT13 - Int 13 Initialization
;
;	Called from ABINIT during INIT command processing. Set up what
;	ever we can.

	PUBLIC	Int13Init
Int13Init PROC
	ASSUME	CS:BiosSeg, DS:BiosGroup, ES:NOTHING
; Install int 13 handler and save the old value of the vector
	pusha
	mov	si,OFFSET BiosSeg:RealModeDS
	mov	bx,013H
	mov	ax,OFFSET BiosSeg:Int13Handler
	mov	dl,DevHlp_SetROMVector
	call	dword ptr [DevHlpFunc]
; Returns the previous handler in AX:DX
	mov	WORD PTR [RealInt13Vector],dx
	mov	WORD PTR [RealInt13Vector+2],ax
; Check if we have an external 3.5" diskette attached
	call	Drv2Pres	; Set up fhaveDrv2; 0 = not there
	popa
	ret
Int13Init ENDP

	Break <Drv2Pres - See if we have an external drive>
;*****	Drv2Pres - See if we have an external drive in the system
;
;	Drv2Pres determines the presence of an external drive. If
;	present, it is assumed to be a 3.5" drive
;
;	ENTRY	DS  BiosGroup
;
;	EXIT	[fHaveDRV2] = 0 if not present
;			      non-zero if drive present
;
;	USES	Flags
;
ASSUME DS:BiosGroup, CS:BiosSeg
	public Drv2Pres
Drv2Pres     PROC
	SaveReg <AX,CX,DX>
	PUSH	DS
	CALL	DDS			; DS = RomData
ASSUME DS:RomData
	DISABLE
	MOV	MOTOR_COUNT,0FFH	; SET MAXIMUM COUNT
	MOV	MOTOR_STATUS,24H	; SELECT DRIVE 2
	MOV	AL,04EH 		; DRIVE 2 MOTOR ON AND SELECT
	MOV	DX,FD_PSEL
	OUT	DX,AL			; OUTPUT DRV2 SEL & MOTOR ENABLE
	ENABLE
	MOV	CX,3
drp1:	CALL	I13Recal
	LOOP	DRP1
	MOV	MOTOR_STATUS,0		; CLEAR MOTOR_STATUS
	POP	DS
ASSUME DS:BiosGroup
	JC	DRP2			; DRIVE ABSENT IF RECAL RETURNS ERROR
	INC	BYTE PTR [fhaveDRV2]	; SET FLAG TO SAY WE HAVE PHYSICAL DRIVE 2
drp2:
	MOV	AL,0CH			; TURN OFF THE MOTOR
	OUT	DX,AL			; SET ORIGINAL MOTOR STATE
	RestoreReg <DX,CX,AX>
	RET
Drv2Pres     ENDP

CodeInit	ENDS
END
