page ,132
.xlist
include cmacros.inc
include pdb.inc
include tty.inc
include galloc.inc
include ems.inc 			;~~tqn 032187
include partitio.inc
;

.list
title Windows memory manager Extensions
;
;++++++++  Vectra Extensions  ++++++++++++++++++++++++++++++++++++++++++++++++++
;
;
;  $Author:   tqn  $
;  $Revision:   1.30  $
;  $Date:   15 Nov 1987 18:12:42  $
;
;
;  This module is accesed when the AllocateMemory routine in TTYEXEC fails
;  because of free memory shortage. The following operations are performed:
;
;  - Reduce the DOS partition to contain only switch state buffers.
;  - Save the Windows memory arena to swap drive.
;  - Install our DOS traps and relocate the Winoldap group to TopPDB.
;  - Exec the DOS application by using function 4BH.
;  - Upon a context switch, Save the DOS application arena, Relocate the
;    Winoldap group back to its original layout, Restore the Windows arena,
;    Install back the Windows DOS traps and give control to TTYTASK.
;  - Upon exit, Restore the Windows arena.
;
;  Author:	Tuan Nguyen
;
;  History:	06/10/86	Microsoft Version 1.03 adaptation
;		08/09/86	~~Preserve CY returned by the 4BH call
;		08/20/86	~~SwapHWBlock is increased to include vectors 0H-7H
;				~~Relocate MouseSeg if EGA
;		09/24/86	~~TN~~ Get the new TopTDB after a switch back
;				from Windows.
;		09/26/86	~~TN~~Compare end of count to 0 and -1 to
;				finish swapping in SaveWindows.
;		09/28/86	~~TN~~Install our own int0 handler
;
;		10/01/86	~~RP~~ Add localization comments, lines
;				beginning with ;L indicate the previous
;				line must be localized
;
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

;
; Define only grabber stuff we need
;
NO_GRABINFO	equ	1
NO_GRABREQUEST	equ	1
NO_GRABBUF	equ	1
NO_LC		equ	1

include grabber.inc

include mac.inc
page
include ttyext.inc
page

sBegin	DATA
	assumes DS,data

ExternW 	<BufPara, SwitchPara, HPPara, MousePara> ; (TTYTASK)
ExternW 	<BufSeg, SwitchSeg, HPSeg, MouseSeg, fEGA>
ExternB 	<fVectra>
ExternW 	<CopySeg, CopyPara>
ExternW 	<MacroSeg,MenuSeg,MacroPara,MenuPara>
ExternW 	<PifMinMem1, PifMaxMem1, Partition>
ExternB 	PifBehavior
ExternW		<wReturnCode>		; RetCode of the TopLevel App
ExternB 	fOrgPartition		;~~tqn 050587	   (TTYSWAP)
;
ExternW 	<HeadTailSize>		;~~tqn 051887
ExternW 	<EEMSSwapBlock>		;~~tqn 032187	   (TTYEEMS)
;
GlobalW 	SwapTable,0,64*SWLength ; Blocks swapped to the swapfile
					; A Zero entry marks the end of table
	dw	-1			; sentinel
GlobalW 	AppTable,0,10*SWLength	; Blocks of swapped DOS app
	dw	-1			; Sentinel

GlobalW 	SwapHWBlock,0,HWBlock_Size ; Saved hardware interrupts
;
GlobalW 	TotalSize,0		; Swapped Windows size in paras
GlobalW 	CODESize,0		; Windoldap code size in paras
DATASize	dw	 0		;	    data
PartitionSize	dw	 0		; Grabber partition size in paras
OrgPartitionSize dw	 0		; Original partition size in paras <>
					; ..PartitionSize if we are not the 
					; ..original owner.
;
GlobalW 	OldCODE,0		; Original CODE segmemt
OldDATA 	dw	0		;	   DATA
SavePartition	dw	0		;	   DOS partition
OldGrabSeg	dw	0		;	   Grabber code segmemt
;
GlobalW 	WinOwner,0		; Owner of Winoldap group
WinPara 	dw	0		; Winoldap group size
SwapNumber	dw	0		; Exclusive number for swap file
;
GlobalW 	TopTDB,0		; First Windows task block
TopSize 	dw	0		; TopPDB size in paras
OldPDB		dw	0		; DOS PDB before Windows swapped
AppPDB		dw	0		; Swapped DOS app PDB
;
AppInt20_24	dw	10 dup(0)	; DOS app private int20-int24
;
GlobalD 	SaveDMA, 0		; Current DOS oldapp DMA
OldDMA		dd	0		; Current Winoldap DMA
SaveTaskSS	dw	0		; Saved current instance stack
SaveTaskSP	dw	0
NoEOF		dw	8		; MSDOS Not EOF flag
;
GlobalW 	SFTSeg,0		; Saved SFT start segment
GlobalW 	SFTPara,0		; its size in paras
;
GlobalW 	SFTBlockSize,0		; Size of a SFT structure
WinSFTLink	dw	0		; First SFT link created by Windows
NumberSFT	dw	0		; Number of saved SFT blocks
LastSFTLink	dw	0		; Last link in SFT chain
;
;
;~~tqn 032187
;
; fEEMSWin is set by SaveWindows if allocation for the Windows save and the
; DOS app context area are allocated from EEMS.
;
; fEEMSApp is Set by SaveApp if fEEMSWin is set and allocation for the App
; swap area is gotten from EEMS
;
GlobalB 	fEEMSWin,0
fEEMSApp	db	0
;
AppMemSize	dw	0		; Size of DOS app including PDB
					; ..and environment


sEnd	DATA

page

sBegin	CODE
	assumes DS,data
	assumes CS,code

ExternFP	<GlobalHandleNoRIP>	; Undocumented Kernel call
ExternFP	<GlobalReAlloc, GetCurrentPDB, GetCurrentTask>
ExternFP	<A20Proc>
ExternNP	<Int20Handler, Int21Handler, Int24Handler>    ; (TTYDOS)
ExternNP	<EnableInt21, DisableInt21>		      ; (TTYDOS)
ExternFP	<Int23Handler> 
;
ExternNP	< GetFreeMem, MyAllocate, GetParagraphs, GetBytes>;(TTYEXEC)
ExternNP	<ShowMessage>
ExternNP	<BeginExecution, EndExecution, GetBufferSizes>; (TTYTASK)
ExternNP	<GoGrab, ShowTerminate>
ExternNP	<FindPartition, InitPartition, GetDiskFree>   ; (TTYSWAP)
ExternNP	<GetSwapfile, DeleteSwapFile, FreePartition>
ExternNP	<SaveHPSystem, RestoreHPSystem> 	      ; (TTYSYS)
;
;~~tqn 032187
;
ExternNP	<EnableGrabber, DisableGrabber> 	(MENUDISP)
;
ExternNP	<AllocEEMSPool, ReallocPID, MapBanks, RWBank> ; (TTYEEMS)
ExternNP	<SaveContext, RestoreContext, GetBankSeg>     ; (TTYEEMS)
ExternNP	<RestorePages, UnmapBanks>
;~~
;
ExternW 	TopPDB				;	(TTYWIN)
ExternW 	<GrabSeg, GrabSize, fHook>	;	(TTYTASK)
ExternW 	<COMMcount>
ExternW 	<WinOldapDS, Win_TaskSS, Win_TaskSP> ;	(TTYDOS)
ExternW 	<FileEntrySize, fInt21>
ExternB 	<DOS_version, DOS_revision>
ExternD 	<lpFileTable, lpDOSData, OrgInt21Proc>
;
ExternW 	EEMSBlock			;~~tqn 032187 (TTYEEMS)
;
		public	MoveWinGroup, CheckWinCom
		public	GrowSFT, ShrinkSFT	;!!*

;!! Debug
		public	SaveWindows,RestoreWindows
		public	PatchHooks, MoveWindows
;********

GlobalW 	fExtend,0		; = 1: In extended mode (Windows Swapped)
GlobalB 	ExtCount,0		; Number of tasks in extended mode
		db	?		; Word aligned
SaveBP		dw	0		; Save Entry BP
MouseModule	db	'MOUSE'		; Mouse module name


page
;========= DOSExec ============================================================
;
;  This module is accessed when a BAD oldapp requires more memory than the
;  Windows arena can provide. It triggers the following actions:
;
;  - Allocate a fixed block to save grabber and HPSystem buffers.
;  - Disable OEM layer and DOS traps
;  - Swap the Windows memory image starting from TopPDB to Swap drive.
;  - Move Winoldap +Grabber +Buffers to TopPDB
;  - Reallocate itself to the TopPDB and load and Execute the DOS .EXE file.
;
;  Outputs:
;    CY:	 1: Error found
;		    AX: Exec_Not_Enough_Memory
;		      : Exec_Exec_Bad_Format
;		      : Msg_Not_enough_disk
;		 0: Success
;
;  Registers:
;    all but DS, BP
;
;==============================================================================

cPROC	DOSExec, <PUBLIC, NEAR>
	parmW	PathNameSeg		; See DOS EXEC function for details
	parmW	PathNameOff
	parmW	ParmBlockSeg
	parmW	ParmBlockOff

cBegin

; We can only roll the Windows kernel out if there is no comm interrupt
; handlers installed by the COMM library or by a DOS app without notification

	call	CheckWinCom
	jnc	DOSEok
	mov	AX, msg_swap_win	; CY set => error
	call	ShowMessage
	jmp short DOSEbomb

DOSEok:
	call	AllocateBuffers 	; allocate grabber buffers
	jnz	DOSE1
DOSEBomb:
	mov	AX, Exec_Not_Enough_Memory

DOSEError:
	stc				; Flag Error
	jmp	DOSERet 		; and Exit

DOSE1:
	xor	AX, AX
	mov	AL, [PifBehavior]
	inc	AH			; Set Up AX= Pifbehavior and Exec_Func

	mov	CX, EXT_MODE		; we are in the extended case
	call	BeginExecution		; Take out Windows hooks and
					; ..install our hooks.

;  Save the original state of HPSYSTEM

	xor	DI, DI
	mov	AX, [HPSeg]		; Get pointer to HP saved area
	or	AX, AX
	jz	DOSE1IBM		; We don't have a Vectra
	mov	ES, AX
	regptr	ESDI, ES, DI
	ccall	SaveHPSystem, <ESDI>

DOSE1IBM:
	call	GetWinSizes		; Get Winoldap block sizes
	ccall	GetCurrentPDB
	mov	[OldPDB], AX		; Save current PDB
	DOSCall SET_PDB,, [TopPDB]	; Tell DOS to switch to TopPDB
	DOSCall GET_DMA 		; ES:BX= Current DOS DMA
	mov	Word Ptr [OldDMA], BX	; Save it
	mov	Word Ptr [OldDMA +2], ES
	push	DS
	mov	DX, 80H 		; Switch to TopPDB DMA
	mov	DS, CS:[TopPDB]
	DOSCall SET_DMA
	pop	DS

	xor	AX, AX			; Start up save flag
	ccall	SaveWindows, <AX>	; Save Windows arena to swapdrive
	jnc	DOSE2			; AX= error message if CY set
	push	AX			; Save Error message and flag
	pushf
	jmp	DOSE3			; Return with Error status

DOSE2:	; Generate a Far frame on stack and save stack pointers in the
;	  current Winoldap TDB. This stack space will be used inside Windows
;	  when the current instance of Winoldap is in foreground

	cCall	GetCurrentTask		; AX= Current Winoldap task
	mov	ES, AX
 	mov	AX, SP			; Save current SP
 	sub	SP, 256 		; Allocate stack room for subsequent
 					; DOS calls.
	push	CS			; Simulate a FAR call
	push	AX
;
	mov	CS:[SaveBP], BP 	; SaveEntry BP
	inc	BP			; BP= FAR frame marker
	push	BP
	mov	BP, SP
	push	SS			; Saved DATA
	sub	SP, TASKDS		; Allocate saved task registers area
	mov	ES:[TDB_taskSP], SP
	mov	CS:[Win_taskSP], SP	; mark as DOStrap stack
 	mov	SP, AX			; Get back old stack above the far frame

ifdef Kludge
;
;~~tqn 071787
;
; KLUGE!!: Kernel 2.0 installs its own int 17h handler (printer IO) which
; is not taken off by DisableOEMlayer. That's why printing a file from
; an oldapp in extended mode blowed up as well as Word 4.xx RIP!!!
;
; This ugly kludge looks at a specific offset from the kernel int 17h handler
; to retrieve a pointer to the original handler (i.e. DOS printer device) and
; installs it back.
;
	public 	tuan
tuan:
	mov	SI, 4*17h
	xor	AX, AX
	mov	DS, AX
	lES	BX, DS:[SI]
	mov	AX, ES
	cmp	AX, CS:[TopPDB]		; If handler outside Windows, OK
	jb	tuanOK
	cmp	AX, 0F000h
	jae	tuanOK
	add	BX, 22
	mov	BX, ES:[BX]
	lES	BX, ES:[BX]
	mov 	word ptr DS:[SI], BX
	mov	AX, ES
	mov	word ptr DS:[SI+2], AX
tuanOK:
	push	SS
	pop	DS
;~~
endif
	mov	AX, DOWN
	ccall	MoveWindows, <AX>	; Move Winoldap to overlay Windows

; Now, DS= Reallocated Winoldap data segment
;      ES=    ''          ''    code

	call	SaveHWInts		; Save reallocated state of hardware vectors

; Call EnableSave if SwitchSeg

	cmp	[SwitchSeg],0
	jz	DOSE2a
	mov	SI, InquireGrab
	mov	AX, GRAB_ENABLESAVE
	call	GoGrab

DOSE2a:
	xchg	BP, CS:[SaveBP] 	; BP= Entry BP
	mov	DX, DataOffset NoEOF	; Clean out MSDOS EOF
	DOSCall WRITE,,1,1

	mov	AX, DS
	mov	PathnameSeg, AX 	; Patch Parameter Block and pathname
	mov	ParmBlockSeg, AX	; ..pointers with new data segment
	mov	SI, ParmBlockOff
	mov	Word Ptr [SI+2].ComLine, AX ; Patch with new data segment
	mov	Word Ptr [SI+2].FCB1, AX
	mov	Word Ptr [SI+2].FCB2, AX

DOSENew:				; New Winoldap code accessed here
	mov	BX, CS:[TopPDB]
	dec	BX
	mov	ES, BX
	mov	AX, [TotalSize]
	xchg	AX, ES:[ga_size]
	mov	[TopSize], AX		; Save TopPDB arena size
	inc	BX

	mov	ES, BX
	mov	AX, [TotalSize] 	; Init TopPDB Block length
	add	AX, BX
	mov	ES:[PDB_Block_Len], AX


	mov	AX, ES:[PDB_environ]	; Copy the WIN.COM environment to
	mov	SI, ParmBlockOff	; ..the EXEC parameter block environment
	mov	[SI].Environment, AX	; ..entry.
	inc	CS:[ExtCount]		; One more extended task
;
;~~tqn 032187
;
; fix to add byte ExtCount instead of word ExtCount
;
	mov	BL, CS:[ExtCount]	; Make sure no extended task PDBs are
	xor	BH, BH			; identical
	add	BX, [WinPara]		; Size of Winoldap +grabber
;~~
	DOSCall SETBLOCK		; Shrink TopPDB to actual required size

	mov	ES, ParmBlockSeg	; Setup for EXEC call
	mov	BX, ParmBlockOff
	mov	DS, PathNameSeg
	mov	DX, PathNameOff
	xchg	BP, CS:[SaveBP] 	; Save Entry BP

	DOSCall EXEC, EXEC_PROG 	; Start Up the .EXE or .COM file
	pushf				;~~TN 080986 ~~
	jnc	DOSEBack
	cmp	AL, Exec_Bad_Format
	jne	DOSEBack
	or	AL, 80H 		; Put in BAD EXE format message

DOSEBack:
;~~TN 080986 ~~
	popf
	mov	BX, CS:[Win_taskSS]
	cli
	mov	SS, BX
	mov	DS, BX			; Get back our data segment
	mov	BX, CS:[Win_taskSP]
	mov	SP, BX
;~~
	sti

	push	AX
	pushf				; Save error flags

;  Get the return code of the app

	DOSCall	GET_RETURN
	mov	wReturnCode, AX

;  Call DisableSave if SwitchSeg

	cmp	[SwitchSeg], 0
	jz	DOSE3a
	mov	SI, InquireGrab
	mov	AX, GRAB_DISABLESAVE
	call	GoGrab

DOSE3a:
	mov	AX, CS:[TopPDB]
	mov	BX, AX
	dec	AX
	mov	ES, AX
	mov	AL, GA_ENDSIG
	mov	ES:[ga_sig], AL 	; Mark TOPPDB as the last DOS block

	mov	AX, [TopSize]		; Restore TopPDB arena size
	mov	ES:[ga_size], AX
	mov	ES, BX			; ES= TopPDB
	add	AX, BX
	mov	ES:[PDB_Block_len], AX

;~~TN 092886~~
	call	SwapHWInts		; Get back original hardware vectors
;~~
	mov	AX, UP
	ccall	MoveWindows, <AX>	; Move Winoldap back to the original place
	call	RestoreWindows		; Reload saved Windows arena image
;
;~~tqn 032187
;
; Free PID associated with DOS app just exited
;
	mov	DX, [EEMSSwapBlock].AppPID
	or	DX, DX			; Only free if App was allocated from EEMS
	je	DOSE3b
	EMSCall wEMM_FREE_PID
;~~
;
; Remove far frame marker from stack
;
DOSE3b:
	mov	BP, CS:[Win_taskSP]
	add	BP, TASKBP
	mov	BP, SS:[BP]
	dec	BP

DOSE3:
	push	DS
	LDS	DX, [OldDMA]		; DX:DS = Old DOS disk transfer address
	DOSCall SET_DMA 		; Restore it
	pop	DS
	DOSCall SET_PDB,, [OldPDB]	; Tell DOS to switch back to Winoldap PDB

	mov	AX, [Partition]
	add	AX, [PartitionSize]
	mov	ES, AX			; ES= End of partition

;  Restore the original state of HPSYSTEM

	xor	DI, DI
	mov	AX, [HPSeg]		; Get pointer to HP saved area
	or	AX, AX
	jz	DOSE3IBM		; We don't have a Vectra
	mov	ES, AX
	regptr	ESDI, ES, DI
	ccall	RestoreHPSystem, <ESDI> ; ES= Block we can free up

DOSE3IBM:
	push	ax
	mov	ax, 1
	cCall	A20Proc,<ax>		; Reenable A20
	pop	ax

	cCall	ShowTerminate		; If appropriate, show (Inactive ...)
	call	FreePartition
	xor	AX, AX
	mov	AL, [PifBehavior]
	inc	AH			; Set Up AX= Pifbehavior and Exec_Func

	mov	CX, EXT_MODE		; We are back from the exetended mode
	call	EndExecution		; Free hooks and enable Windows layer

	POPFF				; Get back error flags from EXEC call
	pop	AX			; AX= Error Type if CY set

DOSERet:
cEnd


page
;========  MoveWinGroup  ======================================================
;
;  This routine is called when a context switch happened.
;  It performs two tasks, RESTORE and SAVE.
;  RESTORE task:
;  -  Save the state of the current DOS oldapp.
;  -  Move Winoldap Group back to its original layout
;  -  Disable Extended mode DOS traps
;
;  SAVE task:
;  - Move Winoldap group to its Extended layout
;  - Install Extended mode DOS traps
;  - Reload the suspended DOS Oldapp
;
;  Inputs:
;    AX=	RESTORE task
;		SAVE	task
;
;  Outputs:
;    CY:	=1	Error
;		=0	Succees
;
;  Registers:
;    All but BP, DS
;
;=============================================================================

MoveWinGroup	Proc	NEAR

	cmp	AX, SAVE_W
	jne	MoveWGRestore
	jmp	MoveWGSave

MoveWGRestore:
	ccall	SaveApp 		; Save the current DOS oldapp
	jnc	MoveWGROk
	jmp	MoveWGErr		; Trouble: do not switch

MoveWGROk:
	mov	AX, CS:[Win_taskSP]
	mov	[SaveTaskSP], AX	; Save current instance stack
	mov	AX, CS:[Win_taskSS]
	mov	[SaveTaskSS], AX

	mov	AX, CS:[TopPDB]
	mov	BX, AX
	push	AX
	dec	AX
	mov	ES, AX
	mov	AL, GA_ENDSIG		; Mark TopPDB as the last DOS block as
	mov	ES:[ga_sig], AL 	; ..it should originally be.
	mov	AX, [TopSize]
	add	BX, AX			; BX= High memory limit
	xchg	AX, ES:[ga_size]	; Save current TopPDB size and restore
	mov	[TopSize], AX		; its original value
	pop	ES			; ES= TopPDB
	mov	ES:[PDB_block_len], BX

	call	SwapHWints		; Save DOS app hardware traps and
					; restore Winoldap's
	mov	AX, UP
	ccall	MoveWindows, <AX>	; Move Winoldap back to its original layout
					; Start new CODE
	DOSCall GET_DMA 		; Save Current DMA and switch to
	push	DS			; TopPDB DMA
	mov	Word Ptr [SaveDMA], BX
	mov	Word Ptr [SaveDMA +2], ES
	mov	DX, 80H
	mov	DS, CS:[TopPDB]
	DOScall SET_DMA
	pop	DS

	ccall	RestoreWindows		; Reload saved Windows arena image
	push	DS			; Switch to Winoldap DMA
	LDS	DX, [OldDMA]
	DOSCall SET_DMA

	pop	DS
	jmp	MoveWGRet

MoveWGSave:	; Get back from Windows after a context switch

	push	DS			; Switch to TopPDB DMA
	mov	DX, 80H
	mov	DS, CS:[TopPDB]
	DOScall SET_DMA
	pop	DS

;
;~~TN 092486~~
;
; The Windows TopPDB might be changed so we need to get the new TopTDB.
; SaveWindows relies on it.
;
	ccall	GetcurrentTask		; DX= first TDB in the task chain
	mov	ES, DX

MoveWGTDB:
	mov	ES, ES:[TDB_next]
	mov	AX, ES
	or	AX ,AX			; Is it the last task
	jz	MoveWGTDB1		; Yes, finish
	cmp	AX, DX			; Is the next task started first?
	ja	MoveWGTDB		; No, compare the next one
	mov	DX, ES			; Mark it as the Top task
	jmp	MoveWGTDB

MoveWGTDB1:
	mov	[TopTDB], DX
;~~
	mov	AX, EXT_MODE		; Flag as back from a switch
	ccall	SaveWindows, <AX>	; Save the Windows arena to be overlaid
	jnc	MoveWGNext
	jmp	MoveWGErr

MoveWGNext:
	mov	AX, DOWN
	ccall	MoveWindows, <AX>	; Move Winoldap group to its extended location
					; Start new code

;  restore current instance stack and patch TopPDB header

	push	DS
	LDS	DX, [SaveDMA]		; Switch back to current DOS DMA
	DOSCall SET_DMA
	pop	DS

	mov	AX, CS:[TopPDB]
	mov	BX, AX
	dec	AX
	mov	ES, AX			; ES= TopPDB header
	mov	AX, [TopSize]
	xchg	AX, ES:[ga_size]	; Save original TopPDB size and restore
	mov	[TopSize], AX		; its extended value
	mov	AL, GA_SIGNATURE	; Mark TopPDB as a middle DOS block
	mov	ES:[ga_sig], AL
	mov	ES, BX			; ES= TopPdb
	add	BX, [TotalSize]
	mov	ES:[PDB_block_len], BX

	mov	AX, [SaveTaskSP]
	mov	CS:[Win_TaskSP], AX
	mov	AX, [SaveTaskSS]
	mov	CS:[Win_TaskSS], AX
	mov	CS:[WinOldapDS], AX

	call	RestoreApp		; Restore the saved DOS app
	call	SwapHWints		; Restore DOS app hardware traps

MoveWGRet:
	clc

MoveWGErr:
	ret

MoveWinGroup	Endp


.xlist
Subttl	SaveApp: Swap the current Oldapp to disk
.list
page
;========  SaveApp  ==========================================================
;
;  - Save all DOS memory blocks owned by the current DOS Oldapp to swap disk.
;  - Update the AppTable which is used by "RestoreApp"
;
;  Inputs:
;    DS:	Winoldap data segment
;
;  Outputs:
;    CY:	1= Error: Cannot create or write to swap file
;
;  Registers:
;    All but DS, BP
;
;=============================================================================

cproc	SaveApp , <PUBLIC, NEAR>
	LocalW	ConvParas		; init to LowestBankSeg -Start of App

cbegin
;
; Make sure we still have our DOS traps

	xor	AX,AX			    ; Windows int 21 handler prevents
	push	DS
	pop	ES
	mov	DS,AX			    ; int 21 update.  We do it directly.
	mov	BX, 4 *20H
	mov	SI, BX
	lea 	DI, AppInt20_24
	cld
	mov	CX, 10			; int20-int24

;  In case the oldapp replaces int 20H,int 21h and int 24H

	rep movsw

	mov	Word Ptr [BX +16], CodeOffset Int24Handler
	mov	Word Ptr [BX +4], CodeOffset Int21Handler
	mov	Word Ptr [BX], CodeOffset Int20Handler

	mov	AX, CS			; Patch int20, int21 vector segment with the
	mov	[BX +2], AX		; current code segment
	mov	[BX +6], AX
	mov	[BX+18], AX
	push	SS
	pop	DS

	DOScall ALLOC_MEM,,-1		; Compact contiguous free blocks

	DOSCall GET_PDB 		; BX= Current PDB
	mov	[AppPDB], BX		; Save it
	push	BX
	DOSCall SET_PDB,, CS:[TopPDB]	; Switch to TOPPDB
	pop	BX
;
;~~tqn 032187
;
;
; Save the App mapping context if EMS is present
;
	cmp	CS:[EEMSBlock].fEEMS, 0
	jz	SaveAdisk
;
	ccall	SaveContext, <[EEMSSwapBlock].AppContextSeg>
;
	mov	AX, [EEMSSwapBlock].AppPID; Special EEMS processing
	or 	AL, [fEEMSWin]
	jz	SaveAdisk		  ; ..there is EEMS support
	mov	AX, CS:[TopPDB]
	mov	BX, AX
	dec	AX
	mov	ES, AX			; ES= TopPDB header
	inc	AX
	add	AX, ES:[ga_size]	; AX= Start of app environment
	cmp	AX, CS:[EEMSBlock].LowestBankSeg
	jae	SAeems1 		; >= : App fully resides in EEMS
;
; set BX= min(LowestBankSeg, TopPDB +TotalSize);
;
	add	BX, [TotalSize]
	cmp	BX, CS:[EEMSBlock].LowestBankSeg
	jbe	SAeems1a
	mov	BX, CS:[EEMSBlock].LowestBankSeg
;
SAeems1a:
	sub	BX, AX			; BX= App portion residing in conv mem
	mov	[ConvParas], BX
	ccall	AllocEEMSpool, <BX>
	jc	SaveAdisk		; if error, save to disk
	mov	[EEMSSwapBlock].SaveAppPID, AX
	inc	[fEEMSApp]		; Flag App swaps to EEMS
;
; Map new banks to LIM area for swapping
;
	mov	BX, FRAME_SIZE
	ccall	MapBanks, <CS:[EEMSBlock].Startframe, BX, AX>
	xor	BX, BX			; Mark as no swap file handle
	jmp short SaveA1a		; Start saving
;
; There is no need to allocate banks for saving the App if the App start
; segment resides in EEMS.
;
SAeems1:
	xor	SI, SI		       ; Set up regs for return
	xor	BX, BX
	jmp	SaveARet

SaveAdisk:
;~~
	call	GrowSFT 		; make sure we have at least 1 free SFT
	mov	BX, [OldDATA]
	xor	BX, [AppPDB]		; Get unique number for swapfile
	mov	[SwapNumber], BX	; Save it for later file delete

	mov	SI, CREATE_FILE
	call	GetSwapFile		; open the swap file
	jnc	SaveA1a
	jmp	SaveAErr		; BX= file handle

SaveA1a:
	mov	AX, CS:[TopPDB]
	dec	AX
	mov	ES, AX			; ES= TopPDB header
	xor	SI, SI

SaveALoop1:
	add	AX, ES:[ga_size]
	inc	AX
	mov	ES, AX			; ES= App header
	xchg	[AppTable +SI].SWAddress, AX ; Save as block to be swapped
	cmp	AX, -1			; = if sentinel
	jnz	SaveAL1
	mov	[AppTable +SI].SWAddress, AX ; Put back sentinel in App table
	jmp	SaveAClose		; Close swap file and makr error

SaveAL1:
	mov	AX, ES			; Current app header to be swapped
	xor	CX, CX			; CX= Current block size

;
;  After compaction, Two free blocks cannot be located back-to-back

SaveALoop:
	cmp	ES:[ga_owner], 0	; Is it free?
	jz	SaveAWrite		; Yes, swap current block
	add	CX, ES:[ga_size]	; Update block size
	cmp	ES:[ga_sig], GA_ENDSIG	; Is it the last block ?
	jz	SaveAWrite		; Yes, swap current block

;  Merge  more valid blocks

	inc	CX			; Include header
	add	AX, ES:[ga_size]
	inc	AX
	mov	ES, AX			; AX= ES= next block header
	jmp	SaveALoop

SaveAWrite:
	inc	CX			; Yes, Swap the current block including
	mov	[AppTable +SI].SWSize, CX ; the free block header.
	mov	DS, [AppTable +SI].SWAddress
;
;~~tqn 032187
;
; DS <> SS (winoldap Data) !!
;
	or	BX, BX			; If proceesing without a swap handle
	jne	SaveAWdisk		; then it must be using EEMS
;
; It is assumed that the fWRITE function is 0 so we can conveniently use BX
; for it in RWBank.
;
	mov	AX, [ConvParas]
	cmp	AX, CX
	jbe	SaveAE1 		; jump to write last block of conv mem
	sub	[ConvParas], CX 	; update remaining paras to save
	jmp short SaveAE2

SaveAE1: ; Last save
;
	mov	SS:[AppTable +SI].SWSize, AX ; Stuff the new size
	mov	CX, AX
	xor	AX, AX			; Mark as last save
	add	SI, SWLength		; Update table pointer
SaveAE2:
	save	<AX, BX>
	ccall	RWBank, <BX, DS, CX, SS:[EEMSSwapBlock].SaveAppPID, SS>
;
	push	SS
	pop	DS
;
	or	AX, AX			; End if last save
	je	SAveARet

	jmp short SaveA1

SaveAWdisk:
;~~
	push	SI			; Save current table index
	push	BX			; Save handle
	mov	BX, CX			; # of paras to write
	call	GetBytes		; DI,SI= Block size in bytes
	pop	BX			; BX= Swap handle
	mov	AH, WRITE
	call	RWBlock 		; Swap block to disk
	pop	SI			; Get back table index
	jnc	SaveA1

SaveAClose:
	DOSCall CLOSE
	mov	BX, [SwapNumber]
	call	DeleteSwapFile		; Delete file if error
	jmp	SaveAErr

SaveA1:
	add	SI, SWLength		; Next table entry
	cmp	ES:[ga_sig], GA_ENDSIG	; last block
	je	SaveARet		; Yes, we're done
	mov	AX, ES			; AX= Current block header
	jmp	SaveALoop1		; ..which has just been swapped

SaveARet:				; BX= Swap file handle
	xor	AX, AX
	xchg	[AppTable +SI], AX	; Mark last entry
	cmp	AX, -1			; Sentinel ?
	jnz	SaveARet1		; No, return as usual
	mov	[AppTable +SI], AX	; Don't have to change the sentinel
SaveARet1:
	or	BX, BX			;~~tqn 032187 Don't close if no handle
	je	SaveARet2		;~~tqn 032187

	DOSCall CLOSE
	call	ShrinkSFT
	clc
	jmp short SaveARet2

SaveAErr:
	call	shrinkSFT
	call	EnableApp		; Restore DOS app PDB and int20, int21
	stc				; flag failure

SaveAret2:

cEnd	; SaveApp


.xlist
Subttl	RestoreApp:  Swap back DOS App arena
.list
page
;========  RestoreApp  ========================================================
;
;  - Restore the DOS app arena image from the swapdisk by looking up the
;    [AppTable].
;
;  Registers:
;    All but DS, BP
;
;==============================================================================

RestoreApp	Proc	NEAR

;
;~~tqn 032187
;
; Assume we have EEMS, so set BX(=SwapHandle) to 0 (invalid)

	cmp	[fEEMSApp], 0		; Do not open swapfile if EEMS supported
	je	RestoreA1
;
; Reset LIM frame pointers to Start and map LIM frame to SaveAppPID
;
	mov	[EEMSSwapBlock].CurrentPage, 0
	mov	AX, FRAME_SIZE
	ccall	MapBanks, <CS:[EEMSBlock].StartFrame, AX, [EEMSSwapBlock].SaveAppPID>
	xor	BX, BX			; mark as invalid handle
	jmp short RestoreA2
;
RestoreA1:
;~~
	call	GrowSFT 		; Expand SFT
	mov	AX, [OldDATA]
	mov	BX, [AppPDB]		; Set up to open the DOS App swapfile
	xor	BX, AX
	mov	[SwapNumber], BX

	mov	SI, OPEN_FILE
	call	GetSwapFile		; BX= Swapfile handle

RestoreA2:
	xor	SI, SI			; Index to start of Swaptable

RestoreALoop:
;
; Setup:
;	BX= Swap Handle (0 if EEMS)
;	SI= Swap index

	mov	AX, [AppTable +SI].SWAddress ; Get block info to load
	or	AX, AX			; Is it the end ?
	jz	RestoreAEnd		; Yes
	cmp	AX, -1			; Or is it the end of table
	jz	RestoreAEnd		; Yes

	push	SI			; Save swap index
	push	BX			; Save handle
	mov	BX, [AppTable +SI].SWSize
	push	BX			;~~tqn 032187 Save size in paras
	call	GetBytes		; DI,SI= Block size in bytes
	pop	DX			;~~tqn 032187 DX= Size in paras
	pop	BX			; BX= Swap handle
	push	DS
	mov	DS, AX			; DS= Swap Address
;
;~~tqn 032187
;
; DS <> SS (Winoldap DATA)
;
	or	BX, BX			; If 0 then it must be EEMS
	jne	RAppDisk
	mov	CX, BANK_READ		; Flag RWBank as read function
	save	<BX>
	ccall	RWBank, <CX, AX, DX, SS:[EEMSSwapBlock].SaveAppPID, SS>
	jmp short RestA1

RAppDisk:
;~~
	mov	AH, READ
	call	RWBlock 		; Retrieve Windows arena block
	jnc	RestA1
	call	PrintErr
	hlt
RestA1:
	pop	DS
	pop	SI
	add	SI, SWLength		; Bump to next swap index
	jmp	RestoreALoop

RestoreAEnd:
;
;~~tqn 032187
;
	or	BX, BX			; if 0 then it must be EEMS
	jne	RAppEnd
;
; Release the SaveAppPID
;
	EMSCall wEMM_FREE_PID,,,[EEMSSwapBlock].SaveAppPID
	mov	[fEEMSApp], 0		; Reset EEMS flag
	jmp short RAppEnd1

RAppEnd:
;~~
	DOSCall ClOSE
	mov	BX, [SwapNumber]
	call	DeleteSwapFile		; We're done, so delete the temp file
	call	ShrinkSFT		; Put SFT back to original layout

RAppEnd1:				;~~tqn 032187
;
; Restore the App mapping context if App owns banks
;
	cmp	CS:[EEMSBlock].fEEMS, 0
	jz	RAppEnd2
	ccall	RestoreContext, <[EEMSSwapBlock].AppContextSeg>
RAppEnd2:
;~~
	call	EnableApp		; Restore DOS app PDB and int20, int21

	ret

RestoreApp	Endp


page
;	GrowSFT
;
;  Connect a free SFT entry to the current SFT chain so that we can assure
;  a successful open of the Application swap file.
;
;  Inputs:
;    DS:	Winoldap Data
;
;  Regs:
;   AX, DI, ES
;
GrowSFT 	Proc	Near

	cmp	[SFTBlockSize], 0	; Do we have saved links ?
	jnz	GrowSFT1		; Yes
	LES	DI, CS:[lpFileTable]	; No, So grow DOS SFT
	lES	DI, ES:[DI +sftlink]	; ES:DI -> last DOS SFT
	xor	AX, AX
	mov	Word Ptr ES:[DI +sftlink], AX ; Connect free SFT entry
	mov	AX, [SFTSeg]
	mov	Word Ptr ES:[DI +sftlink+2], AX
	mov	[LastSFTlink], ES	; Mark current SFT as last
	jmp	GrowSFTRet		; Finish

GrowSFT1:
	mov	AX, [SFTSeg]		; Find last link
	sub	AX, [SFTBlockSize]

GrowSFTLoop:
	add	AX, [SFTBlockSize]
	mov	ES, AX
	cmp	Word Ptr ES:[sftlink], -1
	jnz	GrowSFTLoop

	mov	[LastSFTLink], ES	; Save last for "ShrinkSFT"
	add	AX, [SFTBlockSize]	; AX= Free SFT entry
	mov	Word ptr ES:[sftlink +2], AX		; Connect it
	xor	AX, AX
	mov	ES:[0], AX

GrowSFTRet:
	ret

GrowSFT 	endp


page
;	ShrinkSFT
;
;  Put the current SFT modified by GrowSFT back to its original layout
;
;  Inputs:
;    DS:	Winoldap Data
;
;  Regs:
;    AX, ES
;
ShrinkSFT	Proc	Near

	mov	ES, [LastSFTLink]	; mark as the last SFT
	mov	AX, -1
	mov	Word Ptr ES:[sftlink], AX
	inc	AX
	mov	Word Ptr ES:[sftlink +2], AX
	ret

ShrinkSFT	endp


page
;
;	EnableApp
;
;  Restore DOS app int20, int21, int24
;  Restore current PDB
;
;  Inputs:
;    DS:	Winoldap DATA
;
;  Registers:
;    AX,CX,SI, DI, ES
;

EnableApp	Proc	Near

	push	DS
	DOSCall SET_PDB,, [AppPDB]	; Restore App PDB

	xor	AX, AX
	mov	ES, AX
	mov	DI, 4 *20H
	lea	SI, AppInt20_24
	cld
	mov	CX, 10
	rep movsw 			; Restore DOS app int20,int21,int24
	pop	DS
	ret

EnableApp	Endp


.xlist
subttl	SaveHWints:  Save hardware vectors
.list
page
;========  SaveHWints  ========================================================
;
;  ****  OEM DEPENDENT !!!  ****
;
;  This routine saves the state of the hardware vectors in [SwapHWBlock]
;  Vectors 00H-1F and 70-77 inclusive are saved.
;
;  Inputs:
;    DS:	Winoldap DATA
;
;  Registers:
;    CX, SI, DI, ES
;
;==============================================================================

SaveHWints	Proc	NEAR

	push	DS			; Save DATA

	push	DS
	pop	ES			; Set up for string move
	xor	SI, SI
	mov	DS, SI
	mov	SI, Int00_IP		; DS:SI -> int08 vector
	mov	DI, DataOffset SwapHWBlock
	mov	CX, Int00_1F_Size	; block size in words
	cld
	cli
	REP movsw
	mov	CX, Int70_77_Size
	mov	SI, Int70_IP		; DS:SI -> Int70 vector
	REP movsw
	sti



;~~TN 092886~~
;
; The generic Winoldap does not save/restore the int0 vector. Since we
; overlay the Windows kernel in extended mode, we will also have to install
; our own int0 hanlder.
;
	xor	DI, DI
	mov	ES, DI
	lea	AX, DivideZero
	stosw				; Install offest
	mov	AX, CS
	stosw				; Install Segment
;~~
	pop	DS			; Restore DATA
	ret

;~~TN 092886~~
;
DivideZero:
	sti
	mov	AX, 4C00H		; Exit code
	int	21H
;~~
SaveHWints	EndP


page
;
;	 SwapHWints
;
;  ****  OEM-DEPENDENT !!!!  ****
;
;  Swap [SwapHWBlock] with current state in the interrupt vector table
;
;  Inputs:
;    DS:	Winoldap DATA
;
;  Registers:
;    AX, CX, SI, DI, ES
;

SwapHWints	Proc	NEAR

	xor	DI, DI
	mov	ES, DI
	mov	DI, Int00_IP		; ES:DI -> vector int00 ~~TN 082086 ~~
	mov	CX, Int00_1F_Size	; Swap block size in words
	mov	SI, DataOffset SwapHWBlock
	cld				; DS:SI -> SwapHWBlock
	cli
SwapHWLoop1:
	mov	AX, [SI]		; Swap vectors
	xchg	AX, ES:[DI]
	mov	[SI], AX
	inc	SI			; Bump Pointers
	inc	SI
	inc	DI
	inc	DI
	loop	SwapHWLoop1

	mov	CX, Int70_77_Size
	mov	DI, Int70_IP		; ES:DI -> vector int70

SwapHWLoop2:
	mov	AX, [SI]		; Swap vectors
	xchg	AX, ES:[DI]
	mov	[SI], AX
	inc	SI			; Bump Pointers
	inc	SI
	inc	DI
	inc	DI
	loop	SwapHWLoop2
	sti
	ret

SwapHWints	EndP


.xlist
subttl	CheckWinCom:  Check COMM hanlder presence
.list
page
;========  CheckWinCom	=======================================================
;
; ****	OEM-DEPENDENT !!!!  ****
;
;  Check if the COMM library has installed any of its hardware comm interrupt
;  handlers or if a DOS app running without notification has grabbed the COMM
;  ports.
;
;  Inputs:
;    DS:	Winoldap DATA
;
;  Outputs:
;    CY:	1:	Error: COMM handler installed
;
;  Regs:
;   AX, ES
;
;==============================================================================

CheckWinCom	Proc	Near


;  Assume that the COMM library has a fixed code segment.

	xor	AX, AX
	mov	ES, AX			; ES -> Vector table

	mov	AX, word ptr ES:[INT_COM1_CS]
	call	IsCom 
	or	AX, AX
	jnz	WinComFound

CheckNextCom:
	mov	AX, word ptr ES:[INT_COM2_CS] 
	call	IsCom
	or	AX, AX
	jnz	WinComFound

CheckDOSCom:
     	cmp	CS:[COMMcount], 0	; Any DOS COMM app running without
	jnz	WinComFound		; notification? Yes, so error
	clc
	ret				; Return with success

WinComFound:
 	stc				; Return error
	ret

CheckWinCom	Endp



;**  	IsCom: 	Check to see if the Windows COMM handler is active
;	      
; Inputs:
;  AX:	Segment of Comm Handler
;
; Outputs:
;  AX:		<> 0: Com found
;
; Uses:
;  AX,CX,DX,DI,SI
;

IsCom	Proc	Near

	push	ES
	push	DS
	ccall	GlobalHandleNoRIP, <AX>
	or 	AX, DX
	jz 	IsComEnd
	dec	AX
	mov	ES, AX 
	mov	AX, ES:[ga_owner]
	mov	ES, AX
;  
; Find out which module owns the Comm handler code.
; if it is the mouse, ignore it, else flag that we cannot swap Windows
; 
	push	CS
	pop	DS
	lea	SI, MouseModule
	mov	CX, 5			; 'MOUSE' is 5 bytes long
	mov	DI, word ptr ES:[NE_RESTAB]
	inc	DI			; ES:DI -> Module name
	cld
	rep	cmpsb
	jnz	IsComEnd

	xor	AX, AX			; Mouse found, flag it

IsComEnd:
	pop	DS
	pop	ES
	ret

IsCom 	Endp


.xlist
subttl	AllocateBuffers:  reserve memory blocks for Grabber
.list
page
;========  AllocateBuffers  ==================================================
;
;  - Assume that the grabber is already loaded.
;  - Allocate buffers for Context switch, Screen Grabber, HPSystem, EEMS
;  - Build a dummy DOS arena and flag the allocated memory as a DOS partition.
;
;  Outputs:
;    ZF:	1:	Not enough memory
;		0:	Partition allocated in SS:[partition]
;
;  Registers:
;    All but DS, BP
;
;=============================================================================

AllocateBuffers Proc	NEAR

	xor	AX, AX
	mov	AL, [PifBehavior]
	inc	AH			; Set Up AX= Pifbehavior and Exec_Func

	call	GetBufferSizes		; Determine buffer sizes
	call	AllocateSFT		; Allocate buffer for SFT link if needed

ABOk:
	call	GetFreeMem		; Set BX= memory left in paras
	mov	SI, BX

	mov	BX, PDB_PARAS +SwapAreaOpt ; Fudge factor for PDB, Arena, etc.
	add	BX, [BufPara]		; + Screen grabber buffer
	add	BX, [SwitchPara]	; + context switch buffer
	add	BX, [HPPara]		; + Saved HPSystem buffers (2)
	add	BX, [HPPara]
	add	BX, [MousePara] 	; + saved Microsoft mouse buffer
	add	BX, [SFTPara]		; + Save SFT link structure
;
;~~tqn 050687
;
	mov	AX, CS:[EEMSBlock].ContextSize ; +2 EEMS Save context buffers
	add	BX, AX
	add	BX, AX
	add	BX, CS:[EEMSBlock].WOAContextSize ; + Winoldap EEMS Save Area
;~~
;
;  Now, BX= number of paras required.
;	SI= number of paras available in memory

;  if (required <= (free memory - SwapAareaOpt)) then allocate(required); return
;  else if (FindPartition(required) == SUCCESS) then ok
;	else if (required <= (free memory -SwapArea)) then allocate(required); ok
;  return(false);

	cmp	BX, SI
	jb	AB1			; Ok to continue

;  We cannot find enough memory, so traverse the Windows arena to find a
;  valid DOS partition that we can swap out to free up more memory

	push	SI			; Save free memory size
	sub	BX, SwapAreaOpt 	; Excluding Swap area
	push	BX
	mov	DI, BX			; Set up to call FindPartition
	mov	AL, CS:[EEMSBlock].fEEMS
	ccall	FindPartition, <AX>	; Always parent in Extended mode

	pop	BX
	pop	SI			; AX= Segment of partition
	jnc	AB2			; Got it if not carry

	mov	CX, SwapArea
	mov	AX, CS:[EEMSBlock].BankLine
	or	AX, AX
	jz	AB_Conv
	cmp	AX, TOP_DOS		; Adjust size request with SwapAreaEMS
	jae	AB_Conv			; if EMS4.0 hardware is supported
	mov	CX, SwapAreaEMS

AB_Conv:
	add	BX, CX      		; enough free space with minimum swap area
	cmp	BX, SI
	ja 	AB_ErrorRet		; Nop
	sub	BX, CX      		; actual size to allocate
	call	MyAllocate
	jnc	AB2

AB_ErrorRet:
	xor	BX, BX			; Set ZF
	jmp short ABRet			; Return with error condition

AB1:	; Allocate buffers without having to Swap any DOS partition

	sub	BX, SwapAreaOpt 	; Exclude Swap Area
	call	MyAllocate		; On Entry, BX= Size required in paras
	jc	AB_ErrorRet		; THIS SHOULD NEVER HAPPENDED!!!

;  Now, DI,SI= allocated block size in bytes
;	BX=    allocated block size in Paras
;	AX=    allocated block handle

AB2:	; Create a dummy DOS arena for the newly created DOS partition

	xor	BX, BX			; Parent partition is always 0 in our case
	call	InitPartition		; Build up the DOS partition structure
	mov	AX, ES:[ga_size]
	mov	[OrgPartitionSize], AX

; Initialize the spare SFT structure to have a size of 1 entry and to point
; to NIL.

	mov	AX, [SFTBlockSize]	; Compute the Spare SFT segment
	mov	CX, [NumberSFT]
	mul	CX
	mov	CX, AX
	mov	AX, [SFTSeg]
	add	AX, CX
	mov	ES, AX			; ES -> Spare SFT
	mov	Word Ptr ES:[sftlink], -1 ; points to NIL
	mov	ES:[sftcount], 1	; only one entry
	or	AX, AX			; Reset ZF
ABRet:
	push	SS
	pop	DS			; Get back DS= Winoldap DATA segment
	ret

AllocateBuffers Endp


page
;	AllocateSFT
;
;  Check if the DOS System File Table (SFT) is already expanded by the
;  WIndows DOSTraps. If Yes, we have to reserve enough space to save
;  all SFT link structures created by Windows.
;
;  Inputs:
;    DS:	Winoldap DATA
;
;  Outputs:
;    [FileEntrySize], [lpFileTable], [lpDOSData], [SFTPara]
;
;  Registers:
;    AX, BX, CX, ES
;
AllocateSFT	Proc	Near

; Get DOS version and SFT entry size

	DOSCall GET_VERSION		; AL= Major rev, AH= Minor rev
	mov	CS:[DOS_version], AL
	mov	CS:[DOS_revision], AH
	mov	CS:[FileEntrySize], SF3_SIZE ; Assume we have DOS 3.x
	cmp	AL, 3
	jz	AllocSFT1
	mov	CS:[FileEntrySize], SF2_SIZE ; We must have DOS 2.x then

AllocSFT1:
	DOSCall GET_DOS_DATA		; ES:BX -> DOS Global state
	mov	Word Ptr CS:[lpDOSData], BX ; Save it
	mov	Word Ptr CS:[lpDOSData +2], ES

	lES	BX, ES:[BX +4]		; Save DOS first System file table
	mov	Word Ptr CS:[lpFileTable], BX
	mov	Word Ptr CS:[lpFileTable +2], ES

;  Allocate enough space to contain one free SFT link structure. It would
;  be use for the Swap file if the current SFT is full when a context
;  switch is requested.

	mov	Cl, 4
	mov	AX, CS:[FileEntrySize]
	add	AX, 6 + 15		; + (link) + (count) + para aligned
	shr	AX, CL			; AX= Size in paras
	mov	[SFTPara], AX
;
;  The first SFT link is allocated by DOS. Windows does it own allocation
;  starting from the second link if it is valid.

	LES	BX, ES:[BX]		; Get the first link in ES:BX
	xor	CX, CX			; CX= link count created by Windows

AllocSFTLoop:
	LES	BX, ES:[BX]		; Get the next link
	cmp	BX, -1			; Is next link valid
	jz	AllocSFTEnd		; No, current link is last

;  We got a link created by Windows. So, we have to reserve enough space
;  to save that link structure by looking at its arena header.

	inc	CX
	push	ES
	mov	AX, ES
	dec	AX
	mov	ES, AX			; ES= memory block header that contains
	mov	AX, ES:[ga_size]	; the current SFT link structure
	mov	[SFTBlockSize], AX	; Save the SFT block size
	add	[SFTPara], AX
	pop	ES
	jmp	AllocSFTLoop		; Search  until last link

AllocSFTEnd:
	mov	[NumberSFT], CX 	; Save number of allocated SFT blocks
	or	CX, CX
	ret				; return with success

AllocateSFT	endp


.xlist
Subttl	GetWinSizes:  Get Winoldap block sizes
.list
page
;========  GetWinSizes	======================================================
;
;  Get the original DOS Call entry
;  Get the Current DOS version and initialize the SFT entry size
;  Get the first TDB in the Windows task chain.
;  Get the Winoldap group owner
;  Get the sizes in paras of the following Winoldap blocks:
;  -  CODE
;  -  DATA
;  -  DOS partition
;
;  Inputs:
;    DS:	Winoldap Data
;
;  Outputs:
;    [TopTDB], [CODESize], [DATASize], [PartitionSize], [WinPara], [WinOwner]
;    [DOS_version], [DOS_revision], [FileEntrySize]
;
;  Registers:
;    All but DS, BP
;
;=============================================================================

GetWinSizes	Proc	NEAR

	ccall	GetCurrentTask		; DX= first TDB in the task queue
	mov	ES, DX

GetWSTDB:
	mov	ES, ES:[TDB_next]
	mov	AX, ES
	or	AX ,AX			; Is it the last task
	jz	GetWS1			; Yes, finish
	cmp	AX, DX			; Is the next task started first?
	ja	GetWSTDB		; No, compare the next one
	mov	DX, ES			; Mark it as the Top task
	jmp	GetWSTDB

GetWS1:
	mov	[TopTDB], DX		; Get the TopTDB

	mov	AX, CS			; Get CODE size by looking at its header
	dec	AX
	mov	ES, AX
	mov	AX, ES:[ga_owner]	; Get the Winoldap group owner
	mov	[WinOwner], AX
	mov	AX, ES:[ga_size]
	mov	[CODESize], AX
	mov	BX, AX

	mov	AX, DS			; Get DATA size by looking at its header
	dec	AX
	mov	ES, AX
	mov	AX, ES:[ga_size]
	mov	[DATASize], AX
	add	BX, AX

;  Only the Grabber buffer, the DOS partition structure, the HPSystem
;  original state buffer and the SFT buffer need to be reallocated down
;  to TopPDB
;
;  Special Note:
;    The switch and mouse buffers also need to be reallocated if the display is
;    the EGA.
;    The EGA grabber requires the DOS BAD application to be resident in memory
;    during a SaveScreen/DisableSave operations.

	mov	AX, [BufPara]		; Grabber buffer size in para
	inc	AX			; + Partition header structure
	add	AX, [SFTPara]		; + Saved SFT
	add	AX, [HPPara]		; + HPSystem original buffer
	add	AX, [HPPara]		; + HPSytem current buffer
;
;~~tqn 032787
;
	mov	CX, CS:[EEMSBlock].ContextSize
	shl	CX, 1
	add	AX, CX
	add	AX, CS:[EEMSBlock].WOAContextSize
	add	AX, [MousePara] 	; + Mouse buffer ~~TN 082086~~
;~~
;
;  Add SwitchPara if fEGA

	cmp	Byte Ptr CS:[fEGA], 0
	jz	GetWS2
	add	AX, [SwitchPara]	; + Switch buffer

GetWS2:
	mov	[PartitionSize], AX
	add	BX, AX
	add	BX, CS:[GrabSize]	; BX= Total Size of the Winoldap group
	add	BX, 10H 		; Add size of TopPDB PSP
	mov	[WinPara], BX
	ret

GetWinSizes	Endp


.xlist
Subttl	SaveWindows:  Save current Windows arena to swap disk
.list
page
;========  SaveWindows	======================================================
;
;  - Save the memory image from CS:[TopPDB] to the first TDB (Task Data Block)
;  - Traverse the Windows memory arena and save blocks which are not free
;    (ga_owner = 0) or don't belong to Winoldap. Blocks located above the
;    upper memory limit specified in the BAD oldap PIF are not saved.
;
;  Inputs:
;    DS:	Winoldap old Data segment
;
;  Outputs:
;    CY:	0:	Successful
;		1:	Error
;			AX: Exec Error mesage
;
;  Registers:
;    All but DS, BP
;
;=============================================================================

cProc	SaveWindows, <NEAR >
	parmW	fExt			; =1: Back form a context switch
					; =0: First save
	LocalW	SwapParas		; Swap block size in paras
					; = 0: Current block is completely swapped
	LocalW	SwapIndex		; Running index to the next SwapTable entry
	LocalW	SwapHandle		; Swap file handle
	LocalW	CurrentBlock		; Segment of the current block to swap
	LocalW	fWRITE


cBegin

; Assume:
;  The DOS current PDB is TopPDB and all Windows DOS traps as well as hardware
;  traps have been taken out.

	mov	BX, [PartitionSize]
	add	BX, CS:[GrabSize]
	add	BX, [CODESize]
	add	BX, [DATASize]		; BX= Buffer overhead
	mov	AX, CS:[TopPDB]
	dec	AX
	mov	ES, AX			; ES= TopPDB  header
	mov	CX, ES:[ga_size]	; CX= free memory remaining
	sub	CX, PDB_SIZE		; PDB aligned

	mov	AX, [PifMaxMem1]	; Get desired size
	or 	AX ,AX			; =0 if want all free memory
	jz 	SWAll    
	cmp	AX, -1			; Use Required?
	je	SWAll			; Yes
	add	AX, PDB_PARAS
	add	AX, BX			; Total size desired
	cmp	AX ,CX			; Fit ?
	jbe 	SaveWSetup		; Yes, set it up
					; No, try to get all memory
SWAll:
	mov	AX, [Pifminmem1]	; Minimum size required + Environment
	add	AX, BX			; AX= Total Size required
	cmp	[PifMaxMem1], -1	; Use required?
	jne	SaveWDesired		; No, try check for desired

	cmp	AX, CX			; Does it fit ?
	jbe	SaveWSetup		; Yes, go for it
	jmp short SaveWOutMem

SaveWDesired:
	cmp	AX, CX          	; Does it fit required?
	mov	AX, CX			; Give all free memory 
	jbe	SaveWSetup 		; Ok to run

SaveWOutMem:
	mov	AX, Exec_not_Enough_Memory ; Assume error
	jmp	SaveWError1		; Don't have memory to run PIF
					; even with Windows swapped
SaveWSetup:
	mov	SI, AX	
	sub	SI, BX
	mov	[AppMemSize], SI
	shr	SI, 1			; SI= 1/2 DOS app size
	mov	[TotalSize], AX
	mov	DI, AX			; Save Totalsize in DI

	cmp	CS:[EEMSBlock].fEEMS, 0
	jne	SaveWEEMS

SaveWDisk0:
	jmp	SaveWDisk

SaveWEEMS:
;
; AX= DI= TotalSize
; ES= TopPDB header
; BX= Buffer overhead

	mov	CX, ES
	inc  	CX
	add	CX, ES:[GA_SIZE]
	Save	<BX>
	ccall	GetBankSeg, <CX>	; Get the EEMS segment in AX
	jc	SaveWEEMS0		; CY set. It must be LIM 64K => OK
	sub	AX, CS:[TopPDB]
	sub	AX, PDB_SIZE
	cmp	DI ,AX			; Is TotalSize too large?
    	jbe	SaveWEEMS0  		; No, run it
	mov	CX, BX			; CX= Buffer overhead
	add	BX, [PifMinMem1]
	cmp	BX, AX			; Is Free < min required?
	ja 	SaveWOutMem		; Yes, Cannot run
	mov	DI, AX
	mov	[TotalSize], AX
	sub	AX ,CX
	mov	[AppMemSize], AX

SaveWEEMS0:
	mov	AX, DI
;
; Save the current Windows EEMS mapping context
;
	Save	<AX>
	ccall	Savecontext, <[EEMSSwapBlock].WinContextSeg>

	cmp	CS:[EEMSBlock].fEEMS, 1	; Support of LIM 4.0?
	jne	SaveWdisk0		; No, save to disk
;
;
; Set BX= max(WinPara, ConvSize)
;
	add	AX, CS:[EEMSBlock].PageSize ; add room for alignment
	mov	BX, CS:[EEMSBlock].ConvSize
	mov	CX, BX
	cmp	BX, [WinPara]
	jae	SaveEEMS1
	mov	BX, [WinPara]

SaveEEMS1:
	cmp    fExt, 0			; Back from Context switch?
	je	SaveAfirst		; No, must be first time so
;					  ..try to allocate EMS banks
;
; Back from context switch:
;  If App is already saved to disk AND ConvSize	< TotalSize
;   do not save Windows in EEMS area
;
	cmp	[EEMSSwapBlock].AppPID, 0 ; App allocated?
	jnz	SaveWCSwitch		; Yes, allocate banks to save conv area
	cmp	CX, DI
	jae	SaveAfirst
	jmp	SaveWdisk		; No, all swapping go to disk
;
; Back from a context switch: App is already allocated so there is no need
; to allocate pages for App portion residing in extended memory
;
; Since App owns banks, it means that TotalSize > ConvSize;
; If WinPara > ConvSize, banks are already allocated to SaveWinPID so we just
; have to map them back
;
SaveWCSwitch:
	push	[EEMSSwapBlock].SaveWinPID ; Align stack if jump to SaveEEMS2
	cmp	BX, CX
	ja	SaveEEMS2
	pop	AX
	mov	AX, CX			;AX = ConvSize (requested size)
	mov	DI, AX			;DI = all we need to save to disk if
					; short of EMS memory.
SaveAFirst:
;
; if ([SaveWinPID] <> 0) GrowPID(Size[PartitionPages]+AX)
; else AllocPID(AX);
;
	cmp	[EEMSSwapBlock].SaveWinPID, 0
	je	SaveWAlloc
	push	AX			; Save requested size
	mov	AX, [EEMSSwapBlock].PartitionPages
	mul	CS:[EEMSBlock].PageSize
	mov	DX, AX			; DX= Partition size in paras
	pop	AX
	add	AX, DX			; AX= Size in paras to grow
	Save	<BX, CX, DX>
	ccall	ReallocPID, <[EEMSSwapBlock].SaveWinPID,AX>
	mov	AX, [EEMSSwapBlock].SaveWinPID
	jnc	SaveWAlloc1
	jmp short SaveWDisk

SaveWAlloc:
	xor	DX, DX			; DX= Partition size
	Save	<BX, CX, DX>
	ccall	AllocEEMSpool,<AX>	; Can we allocate from EEMS banks?
	jc	SaveWdisk		; No, get from disk
;
; We reallocate the SaveWinPID to the size needed to save the windows context
; in conventional memory and we allocate the neccessary banks for the AppPID
;
	mov	[EEMSSwapBlock].SaveWINPID, AX

SaveWAlloc1:
	push	AX			; Save SaveWinPID
;
	cmp	[fExt], 0		; We're done if back from context switch
	jne	SaveEEMS2
;
; if (TotalSize < ConvSize) skip reallocation
;
	cmp	DI, CX
	jbe	SaveEEMS2		; Yes, we don't need reallocation

; Reallocate BX = max(WinPara, ConvSize)
;	     DX = Partition size in paras (0 if no partition)
;
	add	DX, BX
	Save	<BX>
	ccall	ReallocPID, <AX, DX>	; shrink size
	mov	AX, DI			; get back total size requested
	sub	AX, BX
	ccall	AllocEEMSpool, <AX>	; Allocate banks for the app
	mov	[EEMSSwapBlock].AppPID, AX
	mov	[EEMSSwapBlock].AppPages, BX

SaveEEMS2:
;
; Init [CurrentPara] to be after PartitionPages
; Map the necessary banks to the LIM pageframe (64Kb)
;
	mov	AX, [EEMSSwapBlock].PartitionPages
	mov	[EEMSSwapBlock].CurrentPage, AX
;
	pop	AX			; Get back SaveWinPID
	mov	BX, FRAME_SIZE
	ccall	MapBanks, <CS:[EEMSBlock].Startframe, BX, AX>
;
	xor	BX, BX			; We don't have a file swap handle
	inc	[fEEMSWin]		; flag EEMS mode is active
	mov	fWRITE, BX		; 0= RWBANK write mode
;
	cmp	CS:[EEMSBlock].ConvSize, 0 ; We have to allocate banks to save
	je	SaveEEMSDone		; ..the conventional memory if present
	jmp	SaveW1a 		; Start swapping

SaveEEMSDone:
;
; if ((TopPDB +PDB_Size) in EEMS) RWBank(TopPDBBank+pagesize,
;					 TopPDBBank+PageSize -(TopPDB+PDB_SIZE)
;
	mov	BX, CS:[EEMSBlock].TopPDBBank
	add	BX, CS:[EEMSBlock].PageSize
	mov	AX, CS:[TopPDB]
	add	AX, PDB_SIZE
	sub	BX, AX
	xor	SI, SI			; Set up registers for RW
	mov	SwapHandle, SI
	mov	[SwapParas], BX
	jmp	SaveW2

SaveWdisk:
;~~
	mov	BX, DI			;~~tqn 032187
	Save	<BX, DI>
	ccall	GetDiskFree		; DX:AX= Half of swap disk memory in paras
	or	DX, DX			; No Problem if over 1mb
	jnz	SaveW1
	inc	BX			; Upper bound adjustment
	shr	BX, 1			; /2 to match value return by GetDiskFree
	cmp	[fExt], EXT_MODE	; Back from a context switch
	je	SaveW0			; Yes, only need enough room to save Windows
	add	BX, SI			; No, Need swap space for both Windows
					; and DOS app
	add	BX, EGA_BUFFER_SIZE/2	; EGA hi-res needs 128kb of disk space
					; ..to save its display.

SaveW0:
	cmp	BX, AX			; Enough space ?
	jb	SaveW1
	jmp	SaveWError

SaveW1:
	mov	BX, DS
	mov	AX, [Partition]
	xor	BX, AX			; Get an exclusive number
	mov	[SwapNumber], BX	; Save it for later delete
	mov	SI, CREATE_FILE
	Save	<DI>
	ccall	GetSwapFile		; Get an unique swap file for Windows
	jnc	SaveW1a 		;~~tqn 032187
	jmp	SaveWError		; BX= File handle

;  Swap to disk the memory image from [TopPDB +PDB_SIZE] to [TopTDB]

SaveW1a:	;~~tqn 031587

	mov	[SwapHandle], BX	; Save swapfile handle
	mov	BX, [TopTDB]
	dec	BX
	mov	[CurrentBlock], BX	; Set current block as the TopTDB header
	mov	AX, CS:[TopPDB]
	add	AX, PDB_SIZE
	sub	BX, AX			; BX = Number of paras to swap

	xor	SI, SI			; SI= Running Swap table index
	mov	CX, DI			; CX= Size requested to be saved
;
;~~tqn 031587
;
	cmp	[fEEMSWin], 0	; EEMS config ?
	je	SaveW2a 		; No, Proceed as normal
					; YEs, we only have to swap the conventional
	cmp	CX, CS:[EEMSBlock].ConvSize
	jbe	SaveW2a
	mov	CX, CS:[EEMSBlock].ConvSize
SaveW2a:
	mov	[SwapParas], CX 	; Save remaining paras to swap after
					; ..this block
	cmp	BX, CX
	jbe	SaveW2
	mov	BX, CX			;~~tqn Only need to swap required size
;~~
;
SaveW2:
;
; Registers setup:
;   AX: Swappable Block Segment
;   BX: Its size in paras
;   SI: Current free Swap table Index
;   DS: Windoldap data segment

	sub	[SwapParas], BX 	; Swappara= remaining paras to save after
					; ..this write
	cmp	[SwapTable +SI].SWAddress, -1 ; Sentinel
	je	SaveWClose		; YES, then error

	mov	[SwapTable +SI].SWAddress, AX
	mov	[SwapTable +SI].SWSize, BX ; Update first Swap table entry
	add	SI, SWLength
	mov	[SwapIndex], SI 	; Points to next Swap table free entry
;
;~~tqn 031587
;
; if (fEEMSActive) then RWBank
; else RWBlock
;
	cmp	[fEEMSWin],0
	je	SaveW2Disk
	ccall	RWBank, <fWRITE,AX,BX,[EEMSSwapBlock].SaveWinPID, DS>
	jmp short SaveW3		; continue
SaveW2Disk:
;~~					; Setup to call RWBlock:
	call	GetBytes		;  DI,SI= Bytes to swap
	mov	BX, [SwapHandle]	;  BX= Swapfile handle
	mov	DS, AX			;  DS= ptr to [TopPDB +PDB_SIZE]
	mov	AH, WRITE		;  AH= DOS Write function
	call	RWBlock

	jnc	SaveW3			; Continue of CY Reset

SaveWClose:				; Close swapfile and return error
	mov	BX, [SwapHandle]
	DOSCall CLOSE			; Close swap file: BX= handle
	mov	BX, [SwapNumber]
	call	DeleteSwapFile		; Something weird happened so
					; ..delete the swap file and return
SaveWError:
	mov	AX, Msg_Not_Enough_disk
SaveWError1:
	stc				; Flag error condition and return
	jmp	SaveWDone2

SaveW3: ; Traverse the Windows memory arena to swap as many blocks as needed

	cmp	[SwapParas], 0		; Is it done ?
;~~TN 092686~~
	je	SaveWRet		; Yeah!!, set CY= 0 and return succes
	cmp	[SwapParas], -1
	je	SaveWRet
;~~
	xor	CX, CX
	mov	ES, [CurrentBlock]	; ES= CurrentBlock header

SaveWLoop:
	add	CX, ES:[ga_size]	; CX= Cumulative block size
	inc	CX			; including PDB header
	cmp	CX, [SwapParas] 	; Is it more than we needed?
	jb	SaveW3a 		; No, find next contiguous block

	mov	CX, [SwapParas] 	; Yes, we're done
	dec	CX			; SaveW6 will increment it back
	jmp	SaveW6

;
;
;**  This procedure returns here ****
;
;

SaveWRet:	; Close the swap file, set the success flag and return

	mov	SI, [SwapIndex]
	cmp	[SwapTable +SI].SWAddress, -1 ; Sentinel ?
	jz	SaveWDone1
	xor	BX, BX
	mov	[SwapTable +SI], BX	; Mark last entry
	mov	BX, [SwapHandle]	; Free up swap handle
	or	BX, BX
	jz	SaveWDone1		; Don't close if there is no handle
	DOSCall CLOSE
SaveWDone1:
	call	InitSFT 		; Save the SFT map created by Windows
	clc				; No Error
	jmp	SaveWDone

;
;************************************
;


SaveW3a:
	cmp	ES:[ga_sig], GA_ENDSIG	; is it the last arena?
;
;~~tqn 050287
;
; If the EEMS bank line is below 640K, we have two globalheaps to save.
; The first one has a sentinal with GA_ENDSIG right below the line and is
; the conventional heap. The second one starts at the line as a sentinal
; with GA_SIGNATURE and ends with GA_ENDSIG somewhere in the 640Kb-1Mb range.
; It represents the EEMS heap associated to the current task.
;
	jne	SaveW3b 		; No, merge blocks
	mov	AX, ES
	add	AX, ES:[ga_size]
	inc	AX			; AX -> Next mem header
	cmp	AX, CS:[EEMSBlock].BankLine ; Is it the start of 2nd heap?
	jne	SaveWError		; No, error: not enough memory
					; Yes, continue block merging
SaveW3b:
;~~
	call	CheckBlock
	je	SaveW4			; Save previous block if current is
	or	AX, AX			; free or belongs to Winoldap
	jnz	SaveWLoop		; If not, merge another block

SaveW4: ; In case of a free/Winoldap block, only the arena header is needed
;	   to be saved. A SwapTable entry is then allocated.

	push	CX			; Save current size
	add	CX, ES:[ga_size]	; CX= Cumulative block size
	inc	CX			; Add in Header size
	cmp	CX, [SwapParas] 	; Is it more than we needed?
	pop	CX
	jb	SaveW5			; No, find next contiguous block
	mov	[SwapParas], CX 	; Yes, we're done
	jmp	SaveW6

SaveW5: ; Check if there is any back-to-back free/Winoldap blocks

	mov	AX, ES:[ga_size]	; we have less this Winoldap Block
	sub	[SwapParas], AX 	; or free block to swap

	cmp	ES:[ga_sig],GA_ENDSIG	; is it the last arena?
	jne	SaveW5c 		; No, merge blocks
;
;~~tqn 050287
;
	mov	AX, ES
	add	AX, ES:[ga_size]
	inc	AX			; AX -> Next mem header
	cmp	AX, CS:[EEMSBlock].BankLine ; Is it the start of 2nd heap?
	je	SaveW5c 		; Yes, continue block merging
	jmp	SaveWError		; No, error: not enough memory
;~~
SaveW5c:
	call	CheckBlock		; Is the following block a Winoldap?
	je	SaveW5a 		; Update remaining paras to swap
	or	AX, AX			; or Is it a free block
	jne	SaveW6			; Nop, swap the currentblock

; At this point we have the situation where either:
; - Two free blocks are arranged back-to-back	OR
; - a free block is located after a Winoldap block or vice-versa
; So, we have to save the free/Winoldap block header and update the Swap table

SaveW5a:
	push	CX			; Save current size
	mov	SI, [SwapIndex]
	cmp	[SwapTable +SI].SWAddress, -1 ; Sentinel ?
	jne	SaveW5b
	jmp	SaveWClose		; Out of Swap table error
SaveW5b:
	mov	AX, 1			; The arena header is 1 para long
	mov	[SwapTable +SI].SWSize, AX ; Update Swap table entry
	mov	[SwapTable +SI].SWAddress, ES
	add	SI, SWLength
	mov	[SwapIndex], SI 	; Points to next Swap table free entry
;
;~~tqn 031587
;
; if (fEEMSActive) then RWBank
; else DOSWrite
;
	cmp	[fEEMSWin], 0
	je	SaveW5disk
	ccall	RWBank, <fWRITE,ES,AX,[EEMSSwapBlock].SaveWinPID, DS>
	jmp short SaveW5e

SaveW5disk:
;~~
	mov	AX, ES
	mov	BX, [SwapHandle]
	mov	DS, AX			; DS= Arena header segment
	xor	DX, DX
	DOSCall WRITE,,, 16		; Write 1 para long to swap file
	push	SS
	pop	DS			; Get back our DS
SaveW5e:				;~~tqn 032187
	pop	CX
	dec	[SwapParas]		; one less header
	jmp	SaveW4			; Search for the next block type

SaveW6: ; Set up registers to update Swap table and swap the currentBlock
;	  to the swap file.

	mov	AX, ES
	xchg	AX, [CurrentBlock]	; Update the next Windows block header
	mov	BX, CX
	inc	BX			; Also save the arena header of the
	mov	SI, [SwapIndex] 	; following free or winoldap block
	jmp	SaveW2

SaveWDone:
	cmp	[fEEMSWin], 0		; CY= 0 after the CMP (Success)
	je	SaveWDone2
	call	UnmapBanks		; release unused pages mapped to LIM
SaveWDone2:

cEnd	;	SaveWindows

page
;	RWBlock
;
;  This routine reads or writes to a file specified by its handle. If
;  an error occured in the Read/Write process, the file is closed.
;
;  Inputs:
;   AX= 	Read or Write DOS function
;   BX= 	File Handle
;   DS= 	Block to Read/Write
;   DI,SI=	Number of bytes to read/write
;
;  Outputs:
;    CY=	0: Successful
;		1: Error
;    DS=	Winoldap DATA segment
;
;  Registers:
;    CX, DX, DI, SI
;

RWBlock 	Proc	Near

	xor	DX, DX
	mov	CX, 15 *(4096)		; Try to write 64Kb chunk whenever
	or	DI, DI			; ..possible. It is the quickest way
	jnz	RWB2			; ..to swap to disk
	cmp	SI, CX
	jae	RWB2
	mov	CX, SI			; SI remaining bytes to write

RWB2:
	push	AX			; Save function
	pushf       			; R/W block to disk
	call	CS:[OrgInt21Proc]	; Success if NC
	jc	RWBError		; No, show trouble
	pop	AX
;
	mov	DX, DS
	add	DX, (15 *4096)/16	; Update Buffer pointer
	mov	DS, DX
	sub	SI, CX
	sbb	DI, 0			; Update remaining bytes
	mov	DX, DI
	or	DX, SI
	jnz	RWBlock
	clc				; Mark as successful
	jmp	RWBRet

RWBError:
	push	AX			; Save error code
	DOSCall CLOSE			; Close file
	pop	AX			; Get back error code
	pop	CX			; Clean up stack
	stc				; Flag error

RWBRet:
	push	SS
	pop	DS			; Get back our DATA
	ret

RWBlock Endp

page
;	CheckBlock
;
;  This routine checks the next arena header to see if it is free or belongs
;  to the current instance of Winoldap.
;
;  Inputs:
;    ES:	Current arena header segment
;    DS:	Winoldap Data segment
;
;  Outputs:
;    ES:	Next arena header segment
;    ZF:	1: ES: Winoldap block header
;		0: AX=	0: ES: Free block header
;		   AX<> 0: ES: Swappable block header
;
;  Registers:
;    AX, ES
;

CheckBlock	Proc	Near

	push	BX
	push	DX

	mov	DX, DS
;
;~~tqn 050587
;
; We can't no longer rely on ga_next since it would be pointing to itself
; if we have a sentinal header.
; So, use ga_size instead.
;
	mov	AX, ES
	add	AX, ES:[ga_size]
	inc	AX
;~~
	mov	BX, AX
	mov	ES, AX			; ES= Next arena header
	mov	AX, ES:[ga_owner]	; AX= Next owner
;
;~~tqn 051287
;
; Since our Data, Code, Partition and Grabber (Winoldap group) are all FIXED
; segments, there is no need for checking the ownership before matching the
; current block with member of the winoldap group.
;
;	cmp	AX, [WinOwner]		; Is it a Winoldap block?
;	jnz	CheckBRet		; Nop, End
;~~
	inc	BX			; BX= Next block segment
	cmp	BX, DX			; Is it our DATA segment?
	jz	CheckBRet		; Yes, End
	mov	DX, CS
	cmp	BX, DX			; Is it our CODE segment?
	jz	CheckBRet
	cmp	BX, [Partition] 	; Is it our DOS partition?
	jz	CheckBRet
	cmp	BX, CS:[GrabSeg]	; Is it our grabber?

CheckBRet:
	pop	DX
	pop	BX
	ret

CheckBlock	Endp

page
;	InitSFT
;
;  Save the SFT map created by windows in [SFTSeg].
;
;  Inputs:
;    DS:	Winoldap DATA
;
;  Registers:
;    All but DS, BP
;
InitSFT 	Proc	Near

	lDS	SI, CS:[lpFileTable]
	LDS	SI, DS:[SI]		; DS:SI -> last DOS SFT
	cmp	SS:[SFTBLockSize], 0	; Do we have any SFT link created by Windows?
	jnz	InitS1			; Yes
	cmp	Word ptr[SI+sftlink], -1; Does Windows grow more SFT since last
					; context switch
	jz	InitSFTRet		; Nop, so end
	mov	AX, word ptr [SI +sftlink+2] ; Save the first Windows SFT
	mov	SS:[WinSFTLink], AX
	mov	AX, DS
	mov	ES, AX
	jmp	InitSFTDone

InitS1:
	cld
	mov	CX, SS:[SFTBlockSize]	; Get SFT size in paras
	shl	CX, 1
	shl	CX, 1
	shl	CX, 1			; Convert size in words
	mov	AX, SS:[NumberSFT]	; AX= Number of save SFT blocks
	mov	DX, SS:[SFTSeg] 	; DX:0 -> Saved SFT structure
	mov	BX, DS:[SI +2]		; BX= first Windows SFT link
	mov	SS:[WinSFTLink], BX	; Save it

InitSLoop:
	lDS	SI, DS:[SI]		; DS:SI -> 1st Windows SFT link
	cmp	SI ,-1			; End of list
	jz	InitSFTRet
	dec	AX			; All SFT saved ?
	js	InitSFTDone
	push	CX
	push	SI
	xor	DI, DI			; DS:SI -> Windows SFT
	mov	ES, DX			; ES:DI -> Saved SFT
	rep movsw			; Init Saved SFT
	add	DX, SS:[SFTBlockSize]	; Bump to next SFT block
	pop	SI
	pop	CX
	jmp	InitSLoop

InitSFTDone:	; Mark the current SFT to be the last one

	mov	AX, -1
	mov	word ptr ES:[sftlink], AX
	inc	AX
	mov	word ptr ES:[sftlink +2], AX

InitSFTRet:
	push	SS
	pop	DS			; DS= Winoldap Data
	ret

InitSFT 	Endp


.xlist
Subttl MoveWindows:  Move Winoldap UP/DOWN in the DOS arena
.list
page
;========  MoveWindows	=======================================================
;
;  Move the Winoldap code, data, stack, DOS partition, Grabber to either:
;  -  Low memory where CS:[TopPDB] is originally located or
;  -  High memory back to their original locations under Windows.
;
;  The repective memory arena headers are not moved. They are saved/restored
;  by "SaveWindows/RestoreWindows".
;  Moving the Winoldap group allows to free up more memory to run larger
;  BAD oldapps.
;
;  Keyboard interrupt is masked off during the relocation operation.
;
;  Inputs:
;    MoveFlag:	UP/DOWN direction
;    DS:	Winoldap current DATA segment
;
;  Outputs:
;    DS:	New Winoldap DATA segment
;    ES:	New Winoldap CODE segment
;
;  Registers:
;    All but DS, BP
;
;=============================================================================

cProc	MoveWindows, <NEAR>
	parmW	MoveFlag

cBegin
;
;~~tqn 061687
;
;!! WARNING: OEM-DEPENDENT section
;
; We have to mask off the keyboard interrupt since its Winoldap handler 
; could be overwritten during the relocation operation. It will be renabled
; at the end of MoveWindows.
;
	cli
	in	AL, 21h			; read 8259 mask
	or 	AL, 00000010b		; mask off Keyboard (IRQ1)
	out	21h, AL
	sti
;~~
   	cmp	MoveFlag, UP		; Do we want to move UP
	jne	MoveDOWN
	jmp	MoveWUP 		; Yes
					; No, we must move down then
;
;~~tqn 032287
;
; The relocated Winoldap group memory map is as follows:
;
;	TopPDB		-> +----------+ LOW  ^
;			   | PDB_SIZE |      |
;	Grabber 	-> +----------+      |
;			   |	      |      |
;	WOA data 	-> +----------+      |
;			   |	      |    WinParas
;	Partition	-> +----------+      |
;			   |	      |      |
;	WOA code	-> +----------+      |
;			   |	      |      |
;	App environment -> +----------+      -
;			   |	      |
;	App start (PDB) -> +----------+ HIGH
;				.
;~~				.
MoveDOWN:
	mov	AX, CS:[TopPDB]
	mov	BX, AX			; Save TopPDB in BX
	add	AX, PDB_SIZE		; Do not overlay the PSP
	mov	ES, AX			; ES= Starting dest segment
;
;~~tqn 032187
;
; Depending on where the TopPDB is located relative to the start of EEMS,
; map enough new banks to allow moving the Winoldap group to the TopPDB area.
; Asuming the App is fully in EEMS, note that the bank where the App environment
; resides must belong to AppPID. The reason is that we simply need to save
; the AppPID context and not the SaveWinPID one when we switch back to Windows.
;
; If TopPDB+PDB_SIZE in EEMS MapBanks(TopPDBBank+PageSize,
;				      WinPara -(TopPDBBank+2*PageSize -TopPDB))
; else MapBanks(LowestBankSeg, WinPara -(LowestBankSeg -TopPDB +PageSize));
;
	cmp	[fEEMSWin], 0	; Only interested if EEMS present
	je	MoveWDisk
	mov	AX, [WinPara]
;
; Assume TopPDB+PDB_SIZE is not fully inside EEMS
;
	mov	DX, CS:[EEMSBlock].LowestBankSeg
	mov	CX, CS:[EEMSBlock].PageSize
;
	cmp	CS:[EEMSBlock].TopPDBBank, 0 ; jmp if not fully in EEMS
	je	MoveWBank
	mov	DX, CS:[EEMSBlock].TopPDBBank
	add	DX, CX
;
MoveWBank:
	add	AX, BX			; Get needed size in paras
	sub	AX, DX
	jbe	MoveWDisk		; Do not map if Winoldap is relocated
					; only in conventional memory
	sub	AX, CX			; Do not map SaveWinPID if AX <= 0
	jle	MoveWApp
					; !!ES must be preserved
	Save	<AX,CX,DX>
	ccall	MapBanks, <DX, AX, [EEMSSwapBlock].SaveWinPID>

MoveWApp:
	add	AX, CX
	add	AX, DX
	ccall	GetBankSeg, <AX>	; AX= Bank seg where App resides
;
; Assume DS=SS
; Map in the first app page
;
	mov	DI, SP
	sub	SP, 4			; four bytes for 1 entry
	mov	SI, SP
	mov	word ptr [SI], 0	; first logical page
	mov	[SI +2], AX		; Bank segment to map
	EMSCall wEMM_MAP_PHYSICAL_PAGES,,1,[EEMSSwapBlock].AppPID
	mov	SP, DI
;
MoveWDisk:
;~~
	mov	DX, CS:[GrabSeg]
	or	DX, DX			; Skip if there is no grabber
	jz	MoveW1
	mov	BX, CS:[GrabSize]	; Move Grabber
	mov	[OldGrabSeg], DX	; Save original Grabber segment
	mov	CS:[GrabSeg], ES
	call	MoveBlock		; ES= Next dest segment

MoveW1:
	push	ES			; Save new DATA segment
	mov	BX, [DATASize]		; Move DATA
	mov	DX, DS
	mov	[OldDATA], DS		; Save original DS
	call	MoveBlock		; ES= Next dest segment
	pop	DS
	push	DS			; DS,SS=new relocated DS,SS
	pop	SS

	mov	BX, [PartitionSize]	; Move DOS Partition
	mov	DX, [Partition]
	mov	[SavePartition], DX
	mov	[Partition], ES
	call	MoveBlock		; ES= Next dest segment

	push	ES			; Save New code segment
	mov	BX, [CODESize]		; Move CODE
	mov	DX, CS
	mov	[OldCODE], DX		; Save original CS
	call	MoveBlock		; ES= Next dest segment

	mov	BX, ES			;~~tqn 032187
	jmp	MoveWDone

MoveWUP:
;
;~~tqn 032787
;
; In order to support the case where the original grabber and/or
; partition might be overlaid by the relocated Winoldap Code or Data,
; moving the Winoldap group back to its original layout has to obey
; the following order:
;
;	- move Code
;	- switch control to original Winoldap code/ relocated data
;	- move partition				      
;	- move Data	/ swtich to original data-stack
;	- move grabber
;
; If EEMS is supported, we have to restore the logical pages where the Winoldap
; group originally resides. Those pages could belong to either PID0 or the
; Winoldap PID (WoaPID). It is assumed that a member of the Winoldap group
; cannot belong to both PID0 and WoaPID at the same time.
;
	mov	ES, [OldCode]		; Restore CODE
	push	ES			; Save it
	mov	BX, [CODESize]
	Save	<BX>
	ccall	RestorePages, <ES, BX>	; Restore original map if any
	mov	DX, CS
	call	MoveBlock
;
;
; Switch control to original Winoldap code
; with relocated Winoldap data



	jmp	MoveWDone

MoveWUP0:	; Now, we are executing from the original Winoldap code
;
;
;~~tqn 041287
;
; If we have an outstanding partition mapping (i.e. SaveWinPID already owns
; the banks where Partition resides), map PartitionPages on top of the
; Winoldap original pages.
;
	mov	ES, [SavePartition]	; Restore Partition
	mov	BX, [PartitionSize]
	push	BX
	ccall	RestorePages, <ES, BX>	; Restore original map if any
	jc	MoveWUP1a		; CY set if no EMS
	xor	AX, AX
	cmp	[fOrgPartition], AL	; Do not map pages if original
	jnz	MoveWUP1a
	cmp	[EEMSSwapBlock].PartitionPages, AX
	jz	MoveWUP1a		; jump if we don't own Partition pages
;
; Try to map back our partition pages without restoring Head/tail
;
	mov	SI, ES
	ccall	GetBankSeg, <SI>
	cmp	AX, SI
	je	MoveWUP1b
	add	AX, CS:[EEMSBlock].PageSize

MoveWUP1b:
	mov 	BX, [OrgPartitionSize]	; Save Partition size
	sub	BX, [HeadTailSize]	; Actual size to be mapped
	or	BX, BX
	jz	MoveWUP1a 
	mov	[EEMSSwapBlock].CurrentPage, 0
	ccall	MapBanks, <AX, BX, [EEMSSwapBlock].SaveWinPID>
;
MoveWUP1a:
	mov	DX, [Partition]
	mov	[SavePartition], DX
	mov	[Partition], ES
	pop	BX
	call	MoveBlock

; WARNING - WARNING - Alians Approaching!!!!
;
; A bug was discovered when both the data and grabber are in the same
; 64K page.  After switching to the origional stack (up high) if the
; grabber is in the same page things go away when "mapping" back the
; grabber pages.  The solution was to move the grabber first then move
; the data segment and switch to it.  (Mon 16-May-1988 : bobgu)
;

if 0
;  Old code here.........
	mov	ES, [OldDATA]		; Restore DATA
	push	ES			; Save it
	mov	BX, [DATASize]
	Save	<BX>
	ccall	RestorePages, <ES, BX>	; Restore original map if any
	mov	DX, DS
	call	MoveBlock
	pop	DS			; Switch to original Winoldap data/stack
	push	DS
	pop	SS
;
	mov	ES, [OldGrabSeg]	; Restore Grabber
	mov	BX, CS:[GrabSize]
	mov	DX, CS:[GrabSeg]
	or	DX, DX			; Skip if there is no grabber
	jz	MoveWRet1

	Save	<BX,DX>
	ccall	RestorePages, <ES, BX>	; Restore original map if any
	mov	CS:[GrabSeg], ES
	call	MoveBlock
	jmp short MoveWRet1		; We're done
;  End of old code......
else
;  New code here.........
	mov	ES, [OldGrabSeg]	; Restore Grabber
	mov	BX, CS:[GrabSize]
	mov	DX, CS:[GrabSeg]
	or	DX, DX			; Skip if there is no grabber
        jz      t1
	Save	<BX,DX>
	ccall	RestorePages, <ES, BX>	; Restore original map if any
	mov	CS:[GrabSeg], ES
	call	MoveBlock

t1:
	mov	ES, [OldDATA]		; Restore DATA
	push	ES			; Save it
	mov	BX, [DATASize]
	Save	<BX>
	ccall	RestorePages, <ES, BX>	; Restore original map if any
	mov	DX, DS
	call	MoveBlock
	pop	DS			; Switch to original Winoldap data/stack
	push	DS
	pop	SS
	jmp short MoveWRet1		; We're done
;  End of new code........
endif

MoveWDone:

; Now, DS= New/Original Winoldap Data Segment
;      SP->		     code
	   
	lea	AX, MoveWRet		; Transfer control back to the new/original
	push	AX			; ..Winoldap at label MoveWRet
x	proc	FAR
	ret
x	endp

MoveWRet:
;
;~~tqn 032787
;
	cmp	MoveFlag, UP		; Not yet done if moving UP
	jne	MoveWD1
	jmp	MoveWUP0
;
;~~tqn 032187
;
; CASE DOWN:
;  Now that all the Winoldap group is reallocated down to TopPDB, we have to
;  map the necessary banks to load the oldapp.
;
MoveWD1:
	cmp	[EEMSSwapBlock].AppPID, 0
	je	MoveWRet1
	Save	<BX>
	ccall	GetBankSeg, <BX>	; BX= Starting Segment of App
	mov	DX, AX			; DX= physical segment to be mapped
	mov	CX, [AppMemSize]
	jnc	MoveWRet2		; CY=1 if not residing in EEMS
;
; If CY = 0
;  { App fully resides in EEMS. Since the App start bank belongs to AppPID, the
;    final size (in paras) to be mapped is:
;
;    CX= [AppMemSize] -(BankSeg(AppStart) -AppStart)
;    with BankSeg(AppStart) <= AppStart
;  }
; else
;  { The DOS app starting segment resides in conventional memory
;    which is already saved by "SaveWindows"
;
;    CX= [AppMemSize] -(LowestBankSeg -AppStart);
;  }

	mov	AX, CS:[EEMSBlock].lowestBankSeg
	mov	DX, AX

MoveWRet2:
	sub	AX, BX			; AX= Area in the last Winoldap bank
					;     reserved for App
	sub	CX, AX			; CX= Size to be mapped for app
	jle	MoveWRet1		; Do not map if app fully resides in
					; ..conventional memory
	Save	<CX,DX>
;
; Make sure AppPID have enough banks before mapping them
;
	ccall	ReallocPID, <[EEMSSwapBlock].AppPID, CX>
	ccall	MapBanks,<DX,CX,[EEMSSwapBlock].AppPID>

MoveWRet1:
	mov	AX, DS
	mov	CS:[WinoldapDS], AX	; Save new winoldap data
	mov	CS:[Win_taskSS], AX
;~~
	call	PatchHooks		; Install our DOS trap and patch trapped
					; ..hardware interrupts with the new code
					; ..segment.
;
;~~tqn 061687
;
;!! WARNING: OEM-DEPENDENT SECTION
;
; Reenable the keyboard interrupt
;
	cli
	in	AL, 21h			; read 8259 mask
	and	AL, 11111101b		; remove Keyboard mask (IRQ1)
	out	21h, AL
	sti
;~~
	call	ChainSFT		; Connect the DOS SFT link to SFTseg
cEnd


page
;	MoveBlock
;
;  Move Blocks larger than 64K from memory to memory.
;  Blocks are paragraphs aligned.
;
;  Inputs:
;    BX:	Block Size in paras
;    DX:	Source segment
;    ES:	Destination segment
;    DS:	Our current DATA segment
;
;  Outputs:
;    ES:	Next destination segment
;
;  Registers:
;    All but DS, BP
;

MoveBlock	Proc	Near

	push	DS		; Save our DATA

	call	GetBytes	; DI,SI= Size in bytes
	cld
	mov	AX, DI
	mov	DS, DX
	mov	BX, SI

;  Now: AX,BX=	Size in Bytes
;	DS:SI, ES:DI=	Source, Destination

	mov	CX, 0FFF0H	; Try to write as many paras as possible in

MoveBLoop:
	or	AX, AX		; ..1 iteration without segment wrap around
	jnz	MoveBLong	; Greater than 64K in Size
	mov	CX, BX		; Last iteration

MoveBLong:
	push	CX		; Save loop size
	xor	SI, SI		; Zeros offsets of both source and destination
	xor	DI, DI
	shr	CX, 1		; Convert into words
	REP	movsw		; Move it
;
; SI,DI must be para-aligned
;
	mov	CX, 4
	shr	SI, CL		; Offsets in paras
	shr	DI, CL

	mov	CX, DS		; Adjust Src, Dest segments
	add	CX, SI
	mov	DS, CX
	mov	CX, ES
	add	CX, DI
	mov	ES, CX

	pop	CX		; CX= loop size
	sub	BX, CX		; No, Take out 1 loop size
	sbb	AX, 0
	mov	DX, AX
	or	DX, BX		; Are all bytes transferred?
	jnz	MoveBLoop	; No, next loop

	pop	DS		; Restore our DATA
	ret			; ES:0= Next available dest segment

MoveBlock	Endp


.xlist
Subttl	PatchHooks:  Patch interrupt vectors with new/old CS
.list
;========  PatchHooks  =======================================================
;
;  ****  This routine is OEM-dependent !!  *****
;
;  - Patch our Clipboard dispatcher it present (i.e int 2FH)
;  - Install/Remove our DOS traps.
;  - Patch the grabber Grab and Switch buffer segments and notify the Grabber
;    of these changes.
;  - Patch the hardware interrupt vectors taken over by Winoldap with either
;    the new or original CODE segment resulted in the swapping of Windows.
;    The vector numbers are dependent of what class the PIF belongs to.
;  - For BAD applications, vectors are 09H, 12H, and 16H.
;  - For Ugly or non-switchable applications, the only vector is 12H.
;
;  Inputs:
;    DS:	Winoldap new DATA segment
;  Outputs:
;    CS:[fExtend]:	1: flag that we are in the EXTENDED MODE (i.e. Windows swapped)
;			0: flag that we are out of the EXTENDED MODE
;  Registers:
;    AX, BX, DX
;
;=============================================================================

PatchHooks	Proc	NEAR

	push	ES

;  Patch Grabber buffers accordingly

	push	DS

;
;~~tqn 052287
;
; Int 2Fh has to be patched before enabling/disabling our DOS handler because
; DOS may issue its own calls to int 2Fh for some DOS functions (Set directory
; for example).
;
	xor	AX, AX
	mov	ES, AX
	mov	AX, CS
	test	CS:[fHook], ClipHook	; Do we need to patch Int2FH?
	jz	PatchhDOS
	mov	ES:[INT2F_CS], AX

PatchhDOS:
;
;  Install or Remove our DOS traps.
;
	cmp	CS:[fInt21], 0
	jz	PatchInstall

	ccall	DisableInt21       	; Reallocate our DOS trap to the original
	jmp short PatchGrab		; the Winoldap code

PatchInstall:
	ccall	EnableInt21		; Save the original DOS vector
					; and Install our DOS trap
PatchGrab:
	mov	AX, [Partition] 	; Patch Grab buffer segments
	inc	AX			; AX -> Start of buffers

	cmp	[BufSeg], 0		; Don't patch if there is no buffer
	jz	PatchGrab1
	mov	[BufSeg], AX
	mov	BX, AX
;
;~~tqn 050787
;
	Save	<AX, BX>
	ccall	DisableGrabber
;
	add	BX, [BufPara]
	sub	BX, [CopyPara]
	mov	[CopySeg], BX		; Patch CopySeg
	cmp	[MacroPara], 0
	jz	PGSkipMacro
	sub	BX, [MacroPara]
	mov	[MacroSeg], BX		; Patch MacroSeg

PGSkipMacro:
	cmp	[MenuPara], 0
	jz	PGSkipMenu
	sub	BX, [MenuPara]
	mov	[MenuSeg], BX		; Patch MenuSeg
PGSkipMenu:
;~~
	add	AX, [BufPara]

PatchGrab1:
	cmp	[HPSeg], 0		; Don't patch if there is no buffer
	jz	PatchGrab2
	mov	[HPSeg], AX
	add	AX, [HPPara]
	add	AX, [HPPara]		; AX -> SFTSeg

PatchGrab2:
	mov	[SFTSeg], AX		; patch SFTSeg
;
;~~tqn 032787
;
; If EEMS is supported, patch the pointers WOAContextSeg, AppContextSeg.
; Note that WinContextSeg never needs to be patched since it is only referred
; to when Windows is resident, i.e. Winoldap is not relocated down.
;
	add	AX, [SFTPara]		; AX -> EEMS Save seg
	cmp	CS:[EEMSBlock].fEEMS, 0
	je	PG2a			; need not to patch
	add	AX, CS:[EEMSBlock].ContextSize
	mov	[EEMSSwapBlock].WOAcontextSeg, AX
	add	AX, CS:[EEMSBlock].WOAContextSize
	mov	[EEMSSwapBlock].AppContextSeg, AX ; Patch AppContextSeg
	add	AX, CS:[EEMSBlock].ContextSize ; AX -> MouseSeg

PG2a:
	cmp	[MouseSeg], 0		; Don't relocate if no mouse
	jz	PGSkipMouse
	mov	[MouseSeg], AX
 	add	AX, [MousePara]		; AX -> SwitchSeg
;~~
PGSkipMouse:
 	cmp	[SwitchSeg], 0		; Don't patch if there is no buffer
 	jz	PatchGrab3		; or if not fEGA
 	cmp	Byte Ptr CS:[fEGA], 0
 	jz	PatchGrab3
	mov	[SwitchSeg], AX

PatchGrab3:
	pop	DS
	xor	AX, AX
	mov	ES, AX			; ES= Vector table segment
	mov	AX, CS
	cli				; Make sure we don't have nested ints
	mov	ES:[INT12_CS], AX	; Patch Int12H

	test	CS:[fHook], KeyHook	; Do we need to patch Int16H ?
	jz	PatchH1 		; No
	mov	ES:[INT16_CS], AX	; Yes, do it

PatchH1:
	test	CS:[fHook], SwiHook	; Do we need to patch Int09H ?
	jz	PatchH2
	mov	ES:[INT09_CS], AX

PatchH2:
	sti
	test	CS:[fHook], Int23Hook
	jz	PatchH3
	mov	ES:[4* 23h +2], AX
PatchH3:
	mov	CX, [BufSeg]		; Enable Grab if needed
	or	CX, CX
	jz	PatchEnd
;
	ccall	EnableGrabber		;~~tqn 050787

PatchEnd:
	mov	AX, CS:[fExtend]	; Update Extended flag
	inc	AX
	and	AX, EXTEND_MASK 	; Toogle fExtend
	mov	CS:[fExtend], AX
	pop	ES
	ret

PatchHooks	ENDP

page
;	ChainSFT
;
;  Connect the DOS SFT link with the saved Windows SFT chain in [SFTSeg]
;
;  Inputs:
;    DS:	Winoldap data
;
;  Regs:
;   AX, DI, ES
;
ChainSFT	Proc	Near

	cmp	[SFTBlockSize], 0
	jnz	ChainSFT1		; Don't proceed if there are no
	ret				; saved SFT links

ChainSFT1:
	lES	DI, CS:[lpFileTable]
	lES	DI, ES:[DI +sftlink]	; ES:DI -> last DOS SFT
	mov	AX, [SFTSeg]

ChainLoop:
	mov	word ptr ES:[DI +sftlink+2], AX ; Connect with 1st saved SFT
	mov	ES, AX
	xor	DI, DI			; Offset of sftlink always 0 when
					; ..created by Windows
	cmp	Word Ptr ES:[sftlink], -1 ; Is it the last link
	jz	ChainRet		; Yes
	add	AX, [SFTBlockSize]
	jmp	ChainLoop		; Chain saved links together

ChainRet:
	ret

ChainSFT	endp


.xlist
Subttl	RestoreWindows:  Swap back Windows arena
.list
page
;========  RestoreWindows  ====================================================
;
;  - Restore the Windows arena image from the swapdisk by looking up the
;    [SwapTable].
;
;  Entry:
;    DS:		Winoldap DATA
;
;  Registers:
;    All but DS, BP
;
;==============================================================================

;~~tqn 032187
;
cproc	RestoreWindows, <NEAR>
	localW	fREAD			; READ flag for RWBank
cBegin
	mov	fREAD, 1		; Flag RWBank as read function
	cmp	[fEEMSWin], 0		; Do not open swapfile if EEMS supported
	je	RestoreW1
;
; Reset LIM frame pointers to Start and map LIM frame to SaveWinPID
;
	mov	AX, [EEMSSwapBlock].PartitionPages
	mov	[EEMSSwapBlock].CurrentPage, AX
	mov	AX, FRAME_SIZE
	ccall	MapBanks, <CS:[EEMSBlock].StartFrame, AX, [EEMSSwapBlock].SaveWinPID>
	xor	BX, BX			; Assume EEMS,so set invalid swap handle
	jmp short RestoreW2

RestoreW1:
;~~
	call	GrowSFT 		; make sure we have at least 1 free handle
	mov	BX, DS			; Set up to open the Windows swapfile
	mov	AX, [Partition]
	xor	BX, AX
	push	BX			; Save swap number
	mov	SI, OPEN_FILE
	call	GetSwapFile		; BX= Swapfile handle

RestoreW2:
	xor	SI, SI			; Index to start of Swaptable

RestoreWLoop:
;
; Setup:
;	BX= Swap Handle (0 if EEMS)
;	SI= Swap index

	mov	AX, [SwapTable +SI].SWAddress ; Get block info to load
	or	AX, AX			; Is it the end ?
	jz	RestoreWEnd		; Yes
	cmp	AX, -1			; or is it the sentinel ?
	je	RestoreWEnd		; Yes, so end

	push	SI			; Save swap index
	push	BX			; Save handle
	mov	BX, [SwapTable +SI].SWSize
	push	BX			;~~tqn 032187 Save size in paras
	call	GetBytes		; DI,SI= Block size in bytes
	pop	DX			;~~tqn 032187 DX= Size in paras
	pop	BX			; BX= Swap handle
	push	DS
	mov	DS, AX
;
;~~tqn 032187
;
; DS <> SS (Winoldap Data) !!
;
	or	BX, BX			; if 0 then it must be EEMS
	jne	RWinDisk
	save	<BX>
	ccall	RWBank, <fREAD, AX, DX, SS:[EEMSSwapBlock].SaveWinPID, SS>
	jmp short RestOK

RWinDisk:
;~~
	mov	AH, READ
	call	RWBlock 		; Retrieve Windows arena block
	jnc	RestOK
	call	PrintErr		; Print fatal error message
	hlt

RestOK:
	pop	DS
	pop	SI
	add	SI, SWLength		; Bump to next swap index
	jmp	RestoreWLoop

RestoreWEnd:
;
;~~tqn 032187
;
	or	BX, BX			; if 0 then it must be EEMS
	jne	RWinEnd
;
; Keep saveWinPID banks intact if WinPara > ConvSize since we need them back
; after the context switch to restore the app context correctly
;
	mov	AX, CS:[EEMSBlock].ConvSize
	cmp	AX, [WinPara]
	jb	RWinEnd1
;
; Release the SavewinPID
; only if it does not contain the instance partition
;
	mov	BX, [EEMSSwapBlock].PartitionPages
	mov	DX, [EEMSSwapBlock].SaveWinPID
	mov	AL, [fEEMSWin]
	dec	[fEEMSWin]		; Reset EEMS active flag
	or	BX, BX			; reallocate if partition owns banks
	je	RWinFree
	EMSCall wEMM_REALLOCATE_FOR_PID
	jmp short RWinEnd1
					; only free PID if no partition
RWinFree:
	mov	[EEMSSwapBlock].SaveWinPID, BX ; Mark it as free
	EMSCall wEMM_FREE_PID
	jmp short RWinEnd1

RWinEnd:

	DOSCall CLOSE			; Close swap file; BX= handle
	pop	BX			; BX= swap number
	call	DeleteSwapFile		; We're done, so delete the temp file
	call	shrinkSFT

RWinEnd1:
	cmp	CS:[EEMSBlock].fEEMS, 0	; Restore context if EEMS is present
	je	RWinEnd2
	ccall	RestoreContext, <[EEMSSwapBlock].WinContextSeg>

RWinEnd2:
	call	RestoreSFT		; Put SFT back into Windows arena

cEnd	; RestoreWindows		;~~tqn 032187

page
;	RestoreSFT
;
;  Copy the saved SFT back to its original location in the windows arena.
;  Fix up the SFT links.
;
;  Inputs:
;    DS:	Winoldap Data
;
;  Registers:
;    All but BX, DX, DS, BP
;
RestoreSFT	Proc	near

	LES	DI, CS:[lpFileTable]
	LES	DI, ES:[DI]		; ES:[DI] -> 2nd DOS SFT
	cmp	[SFTBlockSize], 0	; Did we save any SFT link
	jnz	RestSFT1		; Yes
	cmp	[WinSFTlink], 0 	; Did SFT grow after a switch?
	jz	RestSFTEnd		; Nop
	xor	AX, AX			; reconnect the Windows SFT link
	mov	Word Ptr ES:[DI +sftlink], AX
	xchg	AX, [WinSFTLink]
	mov	Word Ptr ES:[DI +sftlink+2], AX
	jmp	RestSFTEnd

RestSFT1:
	mov	CX, SS:[SFTBlockSize]
	shl	CX, 1
	shl	CX, 1
	shl	CX, 1			; CX= SFT block size in words
	sub	CX, 2			;     - size (link)
	mov	DS, [SFTSeg]		; DS:SI -> Saved SFT
	mov	SI, sftcount
	mov	DI, SI			; Don't restore link portion
	xor	AX, AX
	xchg	AX, SS:[WinSFTLink]
	mov	word ptr ES:[sftlink +2], AX ; link DOS SFT with Windows SFT
	mov	ES, AX			; Get Windows SFT segment
	cld
RestSFTLoop:
	push	CX
	push	SI
	push	DI
	rep movsw			; move Saved SFT structure to Windows
	pop	DI
	pop	SI
	pop	CX
	cmp	Word Ptr DS:[sftlink], -1 ; is current SFT the last one ?
	jz	RestSFTEnd
	mov	AX, DS
	add	AX, SS:[SFTBlockSize]	; Bump to next saved  SFT
	mov	DS, AX
	mov	ES, word ptr ES:[sftlink +2]
	jmp	RestSFTLoop

RestSFTEnd:
	push	SS
	pop	DS
	ret

RestoreSFT	Endp

page
;	PrintErr
;
;  Print error code number, error message and halt the system
;
;  Inputs:
;    AX:	DOS Error code
;

ErrMsg		db	'FATAL DISK READ ERROR '
;L			 ^^^^^^^^^^^^^^^^^^^^^
		db	'DURING SWAPPING ( '
;L			 ^^^^^^^^^^^^^^^^^^
ErrCode 	label	byte
		db	'xx )...'
;L			    ^^^^
ErrMsgLength	equ	$ - ErrMsg

public		PrintErr

PrintErr	Proc	Near

	mov	BX, AX			; BX= Error code
	mov	AX, 0B800H
	mov	ES, AX
	xor	DI, DI			; ES:DI -> top left of screen

	push	CS
	pop	DS
	mov	SI, CodeOffset ErrCode

	mov	CL, 4			; Insert the two HEX byte error code
	mov	BH,BL			; in the error string
	and	BL,0FH
	and	BH, 0F0H
	ror	BH, CL
	mov	CX, 2
PLoop:
	add	BH, '0'
	cmp	BH, '9'
	jbe	Print1
	add	BH, 'A'-':'
Print1:
	mov	[SI], BH
	inc	SI
	xchg	BH, BL
	loop	PLoop
	mov	SI, CodeOffset ErrMsg	; Print the error message
	mov	CX, ErrMsgLength
	cld
	mov	AX, 700H		; Normal attribute
Ploop1:
	mov	AL, [SI]
	mov	ES:[DI], AX
	inc	SI
	inc	DI
	inc	DI
	Loop	Ploop1
	ret

PrintErr	EndP


sEnd	CODE
END
