page ,132
;-------------------------------------------------------------------------
;
; MODULE TTYTASK.ASM - Code for switching in/out of WINOLDAP apps and
;			code for accessing the grabber.
;			This module also contains OEM code for dealing
;			with the timer, sound, and 8087.
;
;			$Author:   tqn  $
;			$Revision:   1.63  $
;			$Date:   15 Nov 1987 19:08:02  $
;
	.xlist
	include cmacros.inc
	WOAOPT = 1		;~~tqn 050187	Short Windows.inc
	include windows.inc
	include pdb.inc
	include tty.inc
	include ems.inc

ifdef MOUSE
	include mouse.inc
endif
	.list


;********  VECTRA Adaptation  *************************************************
;
;  Beside being an IBM-AT compatible at the BIOS level, Vectra also maintains
;  the HP Extended BIOS data area which needs to be saved and restored during
;  a context switch. The HP Extended BIOS data area referred to as the HPSYSTEM
;  is managed by making 2 external calls: SaveHPSystem and RestoreHPSystem.
;  The following items are saved and restored:
;
;	HP Vector Table
;	LTABLET 	data structure
;	LPOINTER	data structure
;	LTOUCH		data structure
;	LHPMouse	data structure
;	STRACK		data structure
;	All physical drivers MAPPING
;
;  This module performs a ID check for the Vectra PC before saving or restoring
;  the HPSYSTEM so that it can be run on both the IBM and HP machines.
;
;  For the Extended mode support, the "GoWindows" procedure is changed to be able
;  to swap the Windows arena to the swap drive.
;
;  Programmer:		Tuan Nguyen
;
;  History:		09/20/86	Support of Version 1.03
;			03/05/87	Integrate with the documented version
;					of Winoldap
;			03/25/87	jhc
;					Changed call to SetSwapDrive in LoadGrab
;					to pass a pointer to SwapName
;			05/31/87	jhc
;					Changed call to SetSwapDrive in LoadGrab
;					to show that es:di has a valid pointer
;					(Prevents assumptions; promotes forward
;					compatibility)
;
;******************************************************************************

page
;
;!! HP
;
BIOS_SEG	EQU	0F000H		; Vectra BIOS ROM Segment
ID_OFFSET	EQU	0F8H		; Vectra ROM ID Offset
;
TRUE		EQU	1
FALSE		EQU	0
;
RESTORE_W	EQU	0		; Restore back to original layout
SAVE_W		EQU	1		; Swap Windows and reload Oldapp
;
; Port locations on IBM machines for various hardware stuff
;
timer0		equ	40H		; Timer ports
timer2		equ	42H
SpeakerCtl	equ	61H		; Speaker port
x287		equ    0F0H		; 8087/80287 ports
mskreg		equ	21H		; IRQ int mask register


	include partition.inc

;
; Bring in only grabber stuff we need
;
NO_GRABREQUEST	equ	1
NO_LC		equ	1
OEM_ID		equ	04A43h

	include grabber.inc


;-----------------------------------------------
;
; External Windows Procedures
;
externFP	<OpenClipboard,GetClipboardData,GlobalSize,GlobalLock>
externFP	<CloseClipboard,ANSItoOEM, GlobalFree>
externFP	<InquireSystem>
externFP	<SetTaskSwitchProc,SetTaskInterchange>
externFP	<GetCurrentTask,GetCurrentPDB,CreateBitMap,SetBitmapDimension>
externFP	<GlobalFree,GetClipboardOwner,DeleteObject>
externFP	<OpenFile,PostQuitMessage,PostMessage>
externFP	<DisableDOS,EnableDOS>
externFP	<A20Proc>
externFP	<DisableOEMLayer,EnableOEMLayer>
externFP	<ShowWindow,UpdateWindow,DestroyWindow>
externFP	GlobalUnlock
externFP	SetMetafileBits
externFP	SetWindowPos

;-----------------------------------------------
;
; External WINOLDAP Procedures
;
externNP	<EnableVideo,DisableVideo>
externNP	<EnableKeyboard,DisableKeyboard>
externNP	<EnableSwitcher,DisableSwitcher>
externNP	<EnableMemory,DisableMemory>
externNP	<EnableMouse,DisableMouse>
externNP	<SetMode,DestroyExit,ShowError,CheckCOM>
externNP	<GetParagraphs,RestorePartition>
externNP	<SaveMouse,RestoreMouse,InquireMouse>
externNP	<MySetFocus,DoDataX>
externFP	Interchange
externNP	<Allocate,MyAlloc,MyUnlock>
externNP	<ProcessMessage,ShowMessage,ShowMessage2>
externNP	<PostHalted>
externNP	<EnableIntMask>
externNP	<PrepMenusForClose>

ExternNP	<SaveHPSystem, RestoreHPSystem> 	; TTYSYS
ExternNP	<FindVectra, EnableVectra, DisableVectra>
ExternNP	<CheckNotify,SetMenuMode>		; TTYOEM
ExternNP	<MoveWinGroup, GrowSFT, ShrinkSFT>	; TTYEXT
ExternNP	<SetWindowList, ActivateNextWindow>	; HOTKEY
ExternNP	<DDESwitchIn, DDESwitchOut>		; DDE
externNP	<MyYield, SetInactiveText>		; TTYSEL
ExternNP	<EnableGrabber,DisableGrabber>		; MENUDISP
ExternNP	AddANSIToClipboard
ExternNP	<GetBadClipboard, BadAppDataX>
ExternNP	<EnableClipboard, DisableClipboard>
ExternNP	<RestoreMark>				; TTYCOPY
ExternNP	<BadEndPaint, BadBeginPaint>
ExternFP	<Int23Handler>				; TTYDOS

;
ExternW 	fExtend 				; TTYEXT
ExternB 	SwapName				; TTYSWAP
ExternD 	Caller_Stack				; TTYFUNC
ExternB 	fUserLock				; DDE
ExternB 	fExecuting				; EXECMAC
ExternW 	EEMSBlock				; TTYEMS
ExternW 	A20ProcRet


page
;-----------------------------------------------
;
; Per task data stored in app DS
;
sBegin	data
;
ExternB 	GrabData				; MENUDISP
;
; Pointer to table of swap areas in low memory
;
externDP	SwapBlock
;
; Pointer to low level timer int
;
externD 	int$8
;
; Window handle of Window app is running in (if it is in a window)
;
externW 	hTTYWnd
;
; This is the nCmdShow parameter to ShowWindow
;
externW 	ShowParm
;
; Flag to indicate that app has terminated (is no longer runable)
;
externW 	OldApState
;
; Seg addr of logical video buffer
;
externW 	VidSegment
;
; Swap storage area of RS232 comm stuff in low memory
;
externW 	RS232area
;
; Address of old app partition
;
externW 	Partition
;
; Flag for which task is swapped
;
externW 	SwapPDB
;
; Pif behavior bits
;
externW 	pifSysMem
externB 	pifBehavior
externB 	pifMsFlags
;
; App title strings
;
externW 	pWindowTitle		; The current window title (HOTHELP)
externW 	TitleLen
externB 	szInactive		; The "Inactive" string (TTYWIN)
externW 	InactiveLen
externB 	OutBuf
;
; WinOldAp clipboard
;
externB 	bCBWrite
;
; Flag to inidicate if the OEM layer and DOS are enabled
;
globalW 	fWindowsEnabled, 1	; Initially, yes
;
; Flags that indicate the ICONIC state of the app
;
globalW 	fIcon,SIZEICONIC
globalW 	prevFicon,SIZEICONIC	; Storage for prev value of fIcon
;
; The following three sets of variables are used to indicate storage areas
;    in the app partition for the partition areas.
;
;
; Buf (screen grab) area variables
;
globalW 	BufSeg,0		; Address in partition (=0 if no area)
globalD 	BufSize,0		; Size in bytes of area
globalW 	BufPara,0		; Size in para rounded up to para boundary

;!!HP MCP
globalW 	CopySeg,0
globalW 	CopyPara,270		;270*16 bytes for copy buffer
;!!HP Done
;
;!!HP ~~RCP 031787
globalW 	MenuSeg,0
globalW 	MenuPara,132		;132*16 bytes allows 50 42 byte menu it.
globalW 	hMenuSeg,0		;good app handle to menu segment
;!! HP VVR 042487
globalW 	MacroSeg,0
globalW 	MacroPara,128		;2 Kb of macro memory
globalW 	hMacroSeg,0		;good app handle of MacroSeg
;***
;
; Switch (screen save) area variables
;
globalW 	SwitchSeg,0		; Address in partition (=0 if no area)
globalD 	SwitchSize,0		; Size in bytes of area
globalW 	SwitchPara,0		; Size in para rounded up to para boundary
;
; Mouse (save) area variables
;
globalW 	MouseSeg,0		; Address in partition (=0 if no area)
globalW 	MousePara,0		; Size in para rounded up to para boundary
;
;!! HP
;
GlobalW 	HPSeg, 0		; HP saved buffer segment
GlobalW 	HPPara, 0		; Its size in paras
;
ExtMode 	db	0		; = 1: Extended mode (Windows swapped)
fBreak		db	0		; ^C checking state when in extended mode
TopPDB_Stack	dd	0		; Original TopPDB user stack
LocalCallerSS	dd	0		; Local caller stack ~~TN 082086 ~~
;
;*******
;
; ExecVars allows Behavior and TopLevel to be accessed as a word
;
ExecVars	label	word
globalB 	Behavior,?		; Pif bits
globalB 	TopLevel,?		;
;
; Data area for 8087 state saves
;
globalB 	Buf8087,?,94
;
; Data area for speaker state saves (only low two bits important)
;
globalB 	Speaker,0
;
; Data for dummy write to CON to clear EOF flag
;
globalB 	NoEOF,8 		; Back space char
;
; These two vars are used to set up grabber data exchanges
;
globalW 	ClipHandle,0	; handle to clip data, 0 if none, 1 if not enough memory
				;	else is actual handle to grab object
globalW 	ClipFormat,0	; format of clip data


globalB 	ShowWindowKludge,0 ; This flag is set while calling ShowWindow
				 ; see BeginExecution comments for more info

sEnd	data

sBegin	code
	assumes cs,code
	assumes ds,data

	public	WaitIcon,GetBufferSizes,LoadGrab,Func27,GoGrab
	public	BeginExecution,EndExecution,TaskSwitch
	public	DoWindows
	public	TTYOpen, OpenDirect, SwapVectors
	public	EnableBadMenus
	public	HookGood		;~~tqn 050187

;-----------------------------------------------
;
; Global task data stored in WINOLDAP code segment
;

;
; Pointers to DOS internal INDOS ERRORMODE variables
;
externD pInDOS
externD pErrMode

externW WinoldapDS

	public	GoodCount,IconCount,BadDepth,fHook,GrabSeg,GrabSize
GoodCount	db	0		; number/flag of good apps running
BadDepth	db	0		; number/flag of bad apps running
IconCount	db	0		; Number/flag of apps in the Iconic state
fHook		db	0		; Bit flags of which hooks are in
NoSwapping	db	0		; set if int swapping not allowed

;~~RCP 050987
ExternW 	fMark			; TTYCOPY
ExternW 	fEndBlock		; TTYCOPY

ExternW Win_taskSS			;!! HP (TTYDOS)
;
; Name of the grabber
;
GName		db	'WINOLDAP.GRB',0
;
; Far call address of a specific grabber function
;
GrabAddr	label	dword
GrabOffset	dw	0
GrabSeg 	dw	0		; segment of WINOLDAP.GRB
;
; Size of the grabber in paragraphs
;
GrabSize	dw	0

;!!  HP
;
GlobalB 	fVectra,0		; = 1: Vectra PC with EX-BIOS
GlobalB 	fHPGrab,0		; = 1: HP grabber
GlobalW 	fEGA,0			; = 1: Display is IBM color EGA
GlobalW 	COMMCount,0		; Number of DOS apps using COMM
					; without notification
GlobalW 	HPSize, 0		; Size in bytes of HP saved buffer
GlobalW 	GrabVersion,0		; Grabber Version Number
prevInt23Proc	dd	0		; Saved area for int23 handler
;
;*******

;**
;
; GoWindows - Start up windows from a bad app
;
;    This routine is basically the same as DoWindows except
;	that for GoWindows DS and SS do not have to be the old app DS
;
; ENTRY:
;	None
; EXIT:
;	Carry Clear
;		Switched to Windows and came back or
;		Could not switch to Windows and there is no hope of doing so later
;	Carry Set
;		Could not switch to Windows right now, but may be able
;		to later on
; USES:
;	ALL but SS,SP
;
cProc	GoWindows,<PUBLIC,NEAR>
cBegin	nogen
    ;
    ; First see if we are running any bad apps
    ;
	cmp	cs:[BadDepth],0 	; any bad apps?
	clc
	jnz	GoWTop
	jmp	GoWSave 		; no, ignore request to switch to win

;!! HP
;
;  In the Extended mode, this procedure can only be called from the winoldap code
;  So we know that DS= SS= Our Winoldap DATA segment. No Stack or Data segment
;  switch is necessary..

GoWTop:
	cmp	CS:[fExtend], 1 	; Is Windows swapped ?
	je	GoWExt
	jmp	GoWHP1			; No, continue as usual

GowExt:

;  Save the state of HP-SYSTEMS if we have a Vectra PC

	cmp	CS:[fVectra], 1 	; Do we have a Vectra PC
	jnz	GoWSave1		; No, so continue
	push	DI
	push	ES			; Save needed regs
	xor	DI, DI
	mov	AX, [HPSeg]
	add	AX, [HPPara]		; Get pointer to saved HP area
	mov	ES, AX

	regptr	ESDI, ES, DI
	ccall	SaveHPSystem, <ESDI>

;  Restore the original HP state before switching back to Windows

	mov	ES, [HPSeg]		; ES= Segment of the original HP State
	regptr	ESDI, ES, DI
	ccall	RestoreHPSystem, <ESDI>
	pop	ES
	pop	DI			; Restore regs

GoWSave1:

;  Call SaveScreen/DisableSave if fEGA

	cmp	Byte Ptr CS:[fEGA], 0	; EGA ?
	jz	GoWRest 		; No
	call	SaveMouse		; ~~TN 082986~~
	call	GrowSFT 		; Make sure we have at least 1 free handle
	mov	SI, SaveScreen
	call	SaveRestoreScr
	pushf
	call	ShrinkSFT		; back to original SFT layout
	popf
	jc	GoWSaveM		; Trouble, so abort
	mov	SI, InquireGrab
	mov	AX, GRAB_DISABLESAVE
	call	GoGrab

GoWRest:
	mov	AX, RESTORE_W		; Restore back to Windows
	call	MoveWinGroup
	jc	GoWSave
	call	DoWindows		; Switch back to Windows

	mov	AX, SAVE_W		; Swap Windows out and get back the
	call	MoveWinGroup		;..suspended oldapp.

;  Call EnableSave/RestoreScreen if EGA

	cmp	Byte Ptr CS:[fEGA], 0	; EGA ?
	jz	GoWRestHP		; No
	call	GrowSFT
	mov	SI, InquireGrab
	mov	AX, GRAB_ENABLESAVE
	call	GoGrab
	mov	SI, RestoreScreen
	call	SaveRestoreScr
	call	ShrinkSFT
GoWSaveM:
	call	RestoreMouse		; ~~TN 082086~~

;  Restore the state of HP-SYSTEMS if we have a Vectra PC

GoWRestHP:
	cmp	CS:[fVectra], 1 	; Do we have a Vectra PC
	jnz	GoWSave 		; No, so continue
	push	ES			; until the app arena is restored
	push	DI			; Save needed regs
	xor	DI, DI
	mov	AX, [HPSeg]
	add	AX, [HPPara]		; Get pointer to saved HP area
	mov	ES, AX
	regptr	ESDI, ES, DI
	ccall	RestoreHPSystem, < ESDI>
	pop	DI			; Restore needed regs
	pop	ES

GoWSave:
	clc
	jmp	gowx			; no, ignore request to switch to win

GoWHP1:
;******
    ;
    ; It might be a bad app, Make sure stack reasonable and switch via DoWindows
    ;
	cCall	GetCurrentTask		; Get current task pointer
	mov	ds,ax			; to DS
	mov	di,ss			; Stack of task to leave
	mov	si,sp
	cmp	di,ds:[TDB_taskSS]	; can we do it the easy way (SS = Task SS)?
	jnz	gow1			; No
	jmp	DoWindows		; Go switch
    ;
    ; The stack we are on is not the task stack, switch to task stack
    ;
gow1:	cli
;
;!! V1.03
;
	cmp	ds:[TDB_taskSP],0	; Task stack already in use?
	je	gowErr			; Yes, ignore request
	mov	ss,ds:[TDB_taskSS]
	mov	sp,ds:[TDB_taskSP]
	mov	ds:[TDB_taskSP],0
;****
	sti
	mov	bp,sp
	add	bp,TASKBP		; set up valid BP
	push	ds			; DoWindows bops DS
	call	DoWindows		; SETS CARRY, DO NOT DISTURB!!!
	pop	ds
	mov	ds:[TDB_taskSP],sp
	cli
	mov	ss,di			; Recover input stack
	mov	sp,si
	sti
gowx:	ret				; Done

;!! V1.03

gowErr:
	stc
	sti
	ret
;*****

cEnd	nogen

;**
;
; GoFunc - Dispatch to one of the TTY tasks from an INT service
;
;    This routine is the one used by the various TTY func handlers
;	for which SS and DS MIGHT not point to WINOLDAP DS to dispatch
;	to the correct TTY func
;    What it does is set SS:SP and BP reasonably and then dispatch
;	via Interchange.
;
; ENTRY:
;	DS MAY NOT BE OLD APP DS
;	SS MAY NOT BE OLD APP DS
;	AH = TTY function number to dispatch to.
;	Other regs as for particular TTY func
; EXIT:
;	Regs as for particular TTY func
; USES:
;	ALL but DS,SS:SP
;
cProc	GoFunc,<PUBLIC,NEAR>
cBegin	nogen
	push	ds

;!! HP
;
;  Switch to our stack if we are in the extended mode
;
	cmp	CS:[fExtend], TRUE	; Are we in an extended mode?
	jne	GoFuncNorm		; NOP
	mov	DI, SS			; Save caller stack in DI, SI

; Don't switch stack if it is ours

	cmp	DI, CS:[Win_TaskSS]
	jz	GoF2

	mov	SI, SP
	cmp	CS:[Win_TaskSS +2], 0	; Stack in use?
	jne	GoF3			; No, use it
	jmp	ixErr			; Yes, mark error and abort

GoF3:
	cli
	mov	SS, CS:[Win_TaskSS]	; Switch to our stack
	mov	SP, CS:[Win_TaskSS +2]
	mov	CS:[Win_taskSS +2], 0	; Mark as in use
	sti
	mov	BP, SP			; Set up a valid frame
	add	BP, TASKBP
GoF1:
	call	interchange
	mov	CS:[Win_TaskSS +2], SP
	jmp	GoFuncNext

GoF2:
	call	interchange
	pop	DS
	ret
;******

GoFuncNorm:
    ;
    ; Get the current task
    ;
	push	dx
	push	ax
	cCall	GetCurrentTask
	mov	ds,ax
	pop	ax
	pop	dx
    ;
    ; Is input SS = Task SS??
    ;
	mov	di,ss
	cmp	ds:[TDB_taskSS],di	; INPUT SS = Task SS?
	jz	ix2			; Yes, we're happy
    ;
    ; Input stack is bogus, set up reasonable stack
    ;
	mov	si,sp			; Input SS:SP saved in DI:SI
	cli

;!! V1.03
	cmp	ds:[TDB_taskSP],0	; Task stack already in use?
	je	ixErr			; Yes, ignore request
	mov	ss,ds:[TDB_taskSS]
	mov	sp,ds:[TDB_taskSP]
	mov	ds:[TDB_taskSP],0
;*****
	sti
	mov	bp,sp
	add	bp,TASKBP		; set up valid BP
    ;
    ; Perform the function then restore input stack
    ;
	call	Interchange
	mov	ds:[TDB_taskSP],sp

GoFuncNext:				;!! HP
	cli
	mov	ss,di			; Recover input stack
	mov	sp,si
	sti

ix1:
	pop	ds
	ret

    ;
    ; Here if Input SS == TASK SS, just dispatch via Interchange
    ;
ix2:	call	Interchange
	pop	ds
	ret

;!! V1.03
;
ixErr:
	stc				; Flag error (stack in use)
	jmp short ix1
;******

cEnd	nogen

;**
;
; TaskSwitch - Perform data save/restore aspects of task switching
;
;   This routine is installed as the task switch proc for old apps
;
; ENTRY:
;	DS = Data of app to save into, or restore out of
;	AX == 0 if Save
;	AX != 0 if Restore
;	CX == 0 if DO NOT bother with 8087 save/restore
;		CX must = 0 AND 8087 pif bit must be set to cause 8087 save/rest
; EXIT:
;	INTERRUPTS DISABLED!!!! if vectors swapped
;	Data areas saved or restored
;	NOTE: If this is a Bad app, ONLY the 8087 is saved/restored
;	SwapPDB = Current PDB if app is good and does not own its partition
;			on restore
;	Current PDB = TopPDB if app is GOOD and does not own its partition
;			on restore
; USES:
;	ALL but DS,BP,SS
;
TaskSwitch	proc	far
;
	mov	si,dataOffset Buf8087	; Point to 8087 save area
	or	ax,ax			; save?
	jnz	TaskRestore		; no, restore
;
; Save Task data
;
TaskSave:
	jcxz	ts1			; if no 8087, continue
	test	[pifBehavior],f8087	; 8087 indicated in PIF?
	jz	ts1			; no, continue
	FSAVE	[si]			; Save 8087

ts1:	test	[pifBehavior],BADBITS	; Bad app?
	jnz	trs3			; Yes, no more to save
	cmp	cs:[NoSwapping],0	; swapping allowed?
	jnz	trs3			; no, no more to save
    ;
    ; See if app is using sound
    ;
	call	TestTimer		; Using sound?
	jz	ts2			; No
	call	SaveSound2		; Save sound
    ;
    ; If app owns its partition, we swap vectors, else we're done
    ;
ts2:	mov	es,[Partition]		; make sure we own partition
	mov	ax,ds
	cmp	word ptr es:[prtOwner],ax  ; Own partition?
;
;~~tqn 050187
;
; In Winoldap version <2.0, WOA (WinOldAp) hooks for Good apps (int 12h,15h,16h)
; are left around in the Windows environment. This was fine since the WOA CODE
; (FIXED segment type) was guaranteed to be always resident. But, this is not
; true in the new EEMS context. Fixed segments loaded above the EEMS line
; can be banked out, so we have to clean up all WOA hooks before yielding.
;
	jne	trs3			; No, done
	call	swapvectors		; Restore vectors, ROM with WOA hooks
	call	UnhookAll		; Take off WOA hooks if any
	cli
	jmp	short trs3		; finish
;~~
;
; Restore task data
;
TaskRestore:
	jcxz	tr1			; if no 8087, continue
	test	[pifBehavior],f8087	; 8087 in PIF?
	jz	tr1			; No, continue
	xor	ax,ax
	out	x287,al 		; remove any pending exceptions
	FRSTOR	[si]			; Restore 8087

tr1:	test	[pifBehavior],BADBITS	; Good app?
	jnz	trs3			; no, all done
    ;
    ; If good app, and it owns its partition, and it is swapable, swap data
    ;	else set SwapPDB and output BX
    ;
	mov	es,[Partition]		; make sure we own partition
	mov	ax,ds
tr1a:
	cmp	word ptr es:[prtOwner],ax ; Own partition?
	jnz	MarkSwapped		; No, mark as swapped
;
;~~tqn 082087
;
	mov	BX, ES:[PrtParent]
	or	BX, BX			; Any Parent?
	mov	ES, BX
	jnz	tr1a			; Yes, check parent owner
;~~
	cmp	cs:[NoSwapping],0	; Is swapping allowed?
	jnz	trs3			; No, all done

	call	TestTimer		; Using sound?
	jz	trsw			; No
	call	RestoreSound2		; Restore sound

trsw:	call	HookGood		;~~tqn 040187
	call	SwapVectors		; Swap int vectors and ROM areas
trs3:
	ret				; done

    ;
    ; Here if restore on a good app which doesn't own its partition
    ;
MarkSwapped:

	cmp	[SwapPDB],0		; have we already marked as swapped?
	jnz	trs5			; Yes, don't reset
	cCall	GetCurrentPDB		; get current PDB
	mov	[SwapPDB],ax		; mark segment as swapped  with current
	mov	AH, 50H 		; PDB
	mov	bx,dx			; switch task to top PDB
	int	21H
trs5:	ret

TaskSwitch	endp

;-------------------------------------------------
;
; Code for manipulating the SOUND hardware
;

;**
;
; SaveSound SaveSound2 - Save sound
;
;    SaveSound is used to save if Timer has been didled
;    SaveSound2 is used to save if it is KNOWN timer has been diddled
;
; ENTRY:
;	DS = Data of app to save sound state into
; EXIT:
;	Sound saved
; USES:
;	AX,ES,DI,FLAGS
;
SaveSound:
	call	TestTimer		; See if timer stuffed
	jz	rs1			; no difference, just swap sound value
SaveSound2:
	xor	cx,cx			; slow down timer to 18.2 tics/sec
	jmp	short rs0

;**
;
; RestoreSound RestoreSound2 - Restore sound
;
;    RestoreSound is used to restore if Timer has been didled
;    RestoreSound2 is used to restore if it is KNOWN timer has been diddled
;
;    NOTE WARNING!!!! It is assumed that an app which has didddled the timer
;	vector and turned the speaker on has jacked the timer up by a factor
;	of 4. This is what BASIC does for sound and play on an IBM PC. This
;	is the app we are interested in supporting.
;
; ENTRY:
;	DS = Data of app to save sound state into
; EXIT:
;	Sound restored
; USES:
;	AX,ES,DI,FLAGS
;
RestoreSound:
	call	TestTimer		; Timer diddled?
	jz	rs1			; no, just restore value
RestoreSound2:
	test	[Speaker],00000011b	; was speaker on?
	jz	rs1			; no, just restore value
    ;
    ; Here to jack up timer by BASIC standard speed up
    ;
	mov	cx,2048 		; speed up timer
    ;
    ; Here to change timer, CX has timer value
    ;
rs0:	call	SetTimer
    ;
    ; Here to just save/restore the speaker control value
    ;	We only diddle the low two bits which are the gate control
    ;	bits which determine whether the speaker is ON. The other
    ;	bits we leave alone.
    ;
rs1:	in	al,SpeakerCtl		; Get current value
	mov	ah,al
	xchg	[Speaker],al		; Swap with saved value
	and	ah,11111100b		; High six bits from current value
	and	al,00000011b		; Low two bits from saved value
	or	al,ah			; all 8 bits
	out	SpeakerCtl,al		; restore
rsx:	ret

;**
;
; TestTimer - See if Timer int vector value has been diddled
;
; ENTRY:
;	int$8 points to "correct" (default) timer int vector
; EXIT:
;	Zero set
;	    current timer vector is same as int$8
;	Zero re-set
;	    current timer vector is different from int$8
; USES:
;	ES,DI,FLAGS
;
TestTimer:
	xor	di,di
	mov	es,di
	mov	di,es:[8*4+2]		; get segment of timer interrupt
	cmp	di,word ptr [int$8+2]	; compare with original
	ret

;**
;
; SetTimer - Modify the timer rate
;
; ENTRY:
;	CX contains new timer rate for timer0 port
; EXIT:
;	Timer rate set
; USES:
;	AX,FLAGS
;
SetTimer:
	in	al,mskreg		; disable timer interrupt
	or	al,00000001b
	jmp	$+2			; No back/back I/O
	out	mskreg,al
	mov	al,cl			; low 8 bits new timer rate
	out	timer0,al		; Set new rate
	jmp	$+2
	mov	al,ch			; high 8 bits new timer rate
	out	timer0,al
	in	al,mskreg		; Turn ints back on
	and	al,1111110b
	jmp	$+2
	out	mskreg,al
stx:	ret

;**
;
; SoundOff - Turn of timmer 2 port
;
; ENTRY:
;	None
; EXIT:
;	0001 written to timer2 port
; USES:
;	AL
;
SoundOff:
	mov	al,1
	out	timer2,al
	jmp	$+2
	mov	al,0
	out	timer2,al
	ret

;**
;
; SaveVectors - Save low mem regions in app DS and set task procs
;
; ENTRY:
;	SS = DS = Data of app to save into
; EXIT:
;	Data defined at SwapBlock saved in indicated area
; USES:
;	ALL but DS,SS,BP
;
SaveVectors:
    ;
    ; Set the task switch and interchange procs
    ;
	xor	ax,ax
	mov	bx,codeOffset TaskSwitch
	regptr	csbx,cs,bx
	cCall	SetTaskSwitchProc,<ax,csbx>	; Set task switch proc
	xor	ax,ax
	mov	bx,codeOffset Interchange
	cCall	SetTaskInterchange,<ax,csbx>	; Set task interchange proc
    ;
    ; Save the data indicated by SwapBlock
    ;
	mov	bx,dataOffset SwapBlock 	; DS:BX -> swap block
	mov	dx,ds				; Save DS in DX
ssw0:	mov	ds,dx				; Recover DS = Data
	mov	cx,[bx] 			; Get word count of area
	jcxz	stx				; 0 if end of table
	mov	si,[bx+2]			; Source at 0:SI
	les	di,[bx+4]			; Dest in app Data
	xor	ax,ax
	mov	ds,ax				; Seg of source is 0
	add	bx,8				; Next entry in table
ssw1:	rep	movsw				; Save
	jmp	ssw0				; Next table entry

;**
;
; RestoreVectors - Restore low mem regions stored in app DS and clear task procs
;
; ENTRY:
;	SS = DS = Data of app to save into
; EXIT:
;	Data defined at SwapBlock restored from indicated area
; USES:
;	ALL but DS,SS,BP
;
RestoreVectors:
    ;
    ; Clear task switch and interchange procs
    ;
	xor	di,di
	cCall	SetTaskSwitchProc,<di,di,di>
	cCall	SetTaskInterchange,<di,di,di>
    ;
    ; Restore low mem area from SwapBlock
    ;
	cld					; Make sure
	mov	bx,dataOffset SwapBlock 	; DS:BX -> swap block
	mov	dx,ds				; Save DS in DX
rsw0:	mov	ds,dx				; Recover DS = Data
	mov	cx,[bx] 			; Get word count of area
	jcxz	stx				; 0 if end of table
	xor	di,di				; dest at 0:SI
	mov	es,di				; Dest in low mem
	mov	di,[bx+2]			; ES:DI is dest in low mem
	lds	si,[bx+4]			; DS:SI is source in app data
	add	bx,8				; Next entry in table
	cli					; Don't int with vector half set
	rep	movsw				; Restore
	sti					; Ints back on
	jmp	rsw0				; Next table entry

;**
;
; SwapVectors - Any low memory areas that do not agree with that saved in app DS
;		are restored/saved from/to app DS.
;
;	This routine RESTORES low memory from app DS and simultaneously
;	saves the current low memory in the app DS.
;
;	What we do is start looking for a difference in low memory areas
;	WITH INTs ENABLED. As soon as we detect a difference, we turn
;	INTs OFF and LEAVE them off for the balance of the swap. The
;	reason for this is that some apps barf if INTs are allowed to occur
;	when one of the areas has been restored, but another has not
;	(BASIC is an example, COM int vector is in int vector region, BASIC
;	DS is in one of the 40:0 areas, BASIC COM INT handler doesn't work
;	if vector is set but DS is not).
;
;	It is a goal to have INTs disabled for as LITTLE time as possible.
;	This is why we scan each SwapBlock area BACKWARDS. Testing
;	reveals that block modifications are TYPICALLY at the beginning
;	of blocks. By scanning backwards we increase the chances
;	of leaving INTs enabled longer.
;
; ENTRY:
;	SS = DS = Data of app to save into
; EXIT:
;	Data defined at SwapBlock saved/restored from indicated area
;	INTERRUPTS DISABLED!!!!!
;	DIRECTION FLAG CLEARED
; USES:
;	ALL but DS,SS,BP
;
SwapVectors:
	std				; Go backwards
	sti				; INTS ON
	mov	bx,dataOffset SwapBlock ; Point to swap table
	mov	dx,ds			; Save app DS in DX
exv0:	mov	ds,dx			; DS = App DS
	mov	cx,[bx] 		; Word count of area
	jcxz	exv2			; 0 means end of table
	xor	di,di
	mov	es,di
	mov	di,[bx+2]		; ES:DI -> low memory area
	lds	si,[bx+4]		; DS:SI -> app saved area
    ;
    ; Check out this block
    ; First make SI and DI point to END of block since going backwards
    ;
	mov	ax,cx
	dec	ax
	shl	ax,1
	add	di,ax			; point to last element in each area
	add	si,ax
    ;
    ; Now check the block
    ;
	add	bx,8			; Next table entry
	rep	cmpsw			; Check
	jz	exv0			; Block matched, next area
	cli				; difference, INTs off for rest of scan
exv1:	mov	ax,[si+2]		; save/restore
	xchg	ax,es:[di+2]
	mov	[si+2],ax
	jcxz	exv0			; Next area
	rep	cmpsw			; Continue check of this block
	jnz	exv1			; another dif
	jmp	exv0			; Next table entry

exv2:	cli				; Return DISABLED
	cld				; Direction clear
	ret


;**
;
; DoWindows - Switch from BAD old app to Windows
;
;    This routine brings down the old app and waits while it is Iconic
;	until the app goes out of the ICONIC state
;
; ENTRY:
;	SS = Data of BAD app being switched out of
; EXIT:
;	DS = Input SS
;	Carry Clear
;		Switched to Windows and came back or
;		Could not switch to Windows and there is no hope of doing so later
;	Carry Set
;		Could not switch to Windows right now, but may be able
;		to later on
; USES:
;	ALL but DS,SS,BP
;
DoWindows:

	push	si
	push	di
	push	ss			; make sure DS is valid
	pop	ds
    ;
    ; If the application is dead, then we can not switch. However,
    ; we can exit by setting OldApState to closed
    ;
	cmp	[OldApState], SF_TERMINATED
	jl	dwAppAlive
	mov	[OldApState], SF_CLOSED
	jmp	dow2B

    ;
    ;!! HP
    ;
    ; The checks for DOS reentrancy previously located here are moved to
    ; "CheckApp" in TTYOEM.
    ;
    ; If the notification hooks are installed, mark the current DOS partition
    ; as swappable.
dwAppAlive:
	mov	ES, [Partition]
	cmp	ES:[prtfSwap], 0	; is it swappable?
	jnz	Dowin2a 		; Yes, proceed
	call	CheckNotify		; Notify hooks?
	jz	Dowin2a 		; No, proceed as usual
	mov	ES:[prtfSwap], 1	; Yes, force partition to be swappable
	dec	CS:[COMMCount]		; One less guy to use COMM without
					; notification

Dowin2a:
;
;  Save the state of HP-SYSTEMS if we have a Vectra PC

	cmp	CS:[fVectra], 1 	; Do we have a Vectra PC
	jnz	Dow1A			; No, so continue
	cmp	[ExtMode], 1		; Are we in Extended mode
	jz	Dow1A			; Yes, HPsystem already saved
	push	DI
	push	ES			; Save needed regs
	xor	DI, DI
	mov	AX, [HPSeg]
	add	AX, [HPPara]		; Get pointer to saved HP area
	mov	ES, AX

	regptr	ESDI, ES, DI
	ccall	SaveHPSystem, <ESDI>

;  Restore the original HP state before switching back to Windows

	mov	ES, [HPSeg]		; ES= Segment of the original HP State
	regptr	ESDI, ES, DI
	ccall	RestoreHPSystem, <ESDI>
	pop	ES
	pop	DI			; Restore regs
;
;******
Dow1A:


;!! HP
;
;   Do not call Savescreen if in extended mode and in color EGA

	cmp	[ExtMode], 0		; Extended mode ?
	jz	DowSaveS		; No
	cmp	Byte Ptr CS:[fEGA], 0	; EGA ?
	jnz	Dow1AHP 		; Yes, skip Savescreen	and SaveMouse

DowSaveS:				;~~TN 082086~~
	call	SaveMouse		; save state of mouse driver
	mov	si,SaveScreen		; save the screen state
	call	SaveRestoreScr
	jnc	DowNext
	jmp	Dow1bHP

DowNext:
;
;!! HP
;
;  Call DisableSave

	mov	AX, GRAB_DISABLESAVE
	mov	SI, InquireGrab
	call	GoGrab
dow1AHP:
;******
	mov	[fIcon],SizeIconic	; Bad apps go Iconic when back to Windows
	mov	al,0
	xchg	al,cs:[BadDepth]	; set bad count = 0, get prev setting

	push	ax			; save previous BadDepth value

	cmp	[BufSeg],0		; Does app have grabber?
	jz	dow1XHP 		; no
	call	DisableGrabber		; Grabber off
dow1XHP:
	cli
	call	SwapVectors		; Doodle vectors
;!! HP
;
	push	Word Ptr CS:[Caller_Stack] ; Save caller's stack locally
	pop	Word Ptr [LocalCallerSS]
	push	Word Ptr CS:[Caller_Stack +2]
	pop	Word Ptr [LocalCallerSS +2]
	mov	Word Ptr CS:[Caller_Stack+2], 0
;*****
	call	SoundOff		; turn off timer 2 stuff
	call	SaveSound		; Save sound if appropriate
	sti				; turn interrupts back on
	call	UnHookAll		; take out hooks
	call	DisableBadApp		; take out more hooks
	inc	cs:[IconCount]		; one more icon

	cCall	GetCurrentPDB
	push	ax			; PDB of this bad app
	mov	bx,dx			; switch back to TopPDB (Windows proc)
	mov	ah,50H
	int	21h
;!! HP
	mov	ES, DX			; ES= TopPDB
	push	SS
	push	SS
	pop	DS
	lDS	AX, [TopPDB_stack]	; Restore original TopPDB user stack
	mov	Word Ptr ES:[PDB_user_stack], AX
	mov	Word Ptr ES:[PDB_user_stack +2], DS
	pop	DS			; DS= our DATA
;*****
	call	DDESwitchOut
	call	ActivateNextWindow
	call	WaitIcon		; wait until we become full screen again
	call	SetWindowList
	pop	bx			; restore PDB
	mov	ah,50H
	int	21h
	dec	cs:[IconCount]		; one less icon
	mov	CS:WinoldapDS,DS	; update DS var before installing hooks
	call	EnableBadApp		; Hooks in, partition in
	cli
	call	RestoreSound		; Sound state back in if appropriate
;!! HP
;
	push	Word Ptr [LocalCallerSS]
	pop	Word Ptr CS:[Caller_Stack] ; Restore caller's stack
	push	Word Ptr [LocalCallerSS +2]
	pop	Word Ptr CS:[Caller_Stack +2]
;*****
	call	SwapVectors		; Doodle vectors
	sti				; turn interrupts back on
	mov	cx,[BufSeg]
	jcxz	dow2HP			; no segment defined, so no grabber
	call	EnableGrabber
dow2HP:
	pop	ax			; get bad count

	mov	cs:[BadDepth],al	; restore previous value

;!! HP
;
;   Do not call RestoreScreen if in extended mode and in color EGA

	cmp	[ExtMode], 0		; Extended mode ?
	jz	DowRestS		; No
	cmp	Byte Ptr CS:[fEGA], 0	; EGA ?
	jnz	Dow1HP			; Yes, skip RestoreScreen

DowRestS:

;  Call EnableSave

	mov	AX, GRAB_ENABLESAVE
	mov	SI, InquireGrab
	call	GoGrab
;****
	mov	si,RestoreScreen
	call	SaveRestoreScr
dow1bHP:
	call	RestoreMouse		; ~~TN082086~~

dow1HP: 				;!! HP


;
;!! HP
;
;  Restore the state of HP-SYSTEMS if we have a Vectra PC

	cmp	CS:[fVectra], 1 	; Do we have a Vectra PC
	jnz	Dow2B			; No, so continue
	cmp	[ExtMode], 1		; are we in extended mode ?
	jz	Dow2B			; Yes, postpone restoring HPsystem
	push	ES			; until the app arena is restored
	push	DI			; Save needed regs
	xor	DI, DI
	mov	AX, [HPSeg]
	add	AX, [HPPara]		; Get pointer to saved HP area
	mov	ES, AX
	regptr	ESDI, ES, DI
	ccall	RestoreHPSystem, < ESDI>
	pop	DI			; Restore needed regs
	pop	ES
;
;******
Dow2B:
	clc				; indicate success
dow2:	pop	di
	pop	si
	ret

;**
;
; SaveRestoreScr - Do a save/restore screen operation by calling the grabber
;
; ENTRY:
;	DS = Data of app involved
;	SI = RestoreScreen or SaveScreen
; EXIT:
;	Screen saved or restored
;	Regs/flags as per SaveScreen or RestoreScreen
; USES:
;	ALL but DS,SS,BP
;
SaveRestoreScr:
	les	ax,[SwitchSize] 	; Size for save/restore
	mov	dx,es			;   to DX:AX
	mov	es,[SwitchSeg]		; Source/Dest ES:DI
	xor	di,di
	jmp	GoGrab			; Go do it

;**
;
; WaitIcon - Wait for old app to become non-iconic
;
; ENTRY:
;	DS,SS = Data of iconic old app
; EXIT:
;	App changes out of Iconic state
; USES:
;	AX,DI,FLAGS
;
WaitIcon:
	push	bx
	push	cx
	push	dx
	push	es
	push	si
wi1:	call	ProcessMessage		; Do some messages
	cmp	[fIcon],SIZEICONIC	; Still Iconic?
	jz	wi1			; Yes, continue to wait
	pop	si
	pop	es
	pop	dx
	pop	cx
	pop	bx
	ret

page
;**
;
; BeginExecution - Start up an old app
;
;	Good apps get thier Windows displayed and startup
;	Bad apps get started if not ICONIC, else they wait till
;		they're not Iconic and COM ports are available (if needed)
;
;  If we are in the Extended mode, call DisableDOS to yield back the original
;  DOS dispatcher for Int 21H.
;
; ENTRY:
;	SS = Data of app to start
;	AH = value for TopLevel variable
;	AL = value for Behavior variable
;	CX <> 0: We are in the extended mode case
; EXIT:
;	App is ready to start
;	OldApState is set to SF_RUNNING
;	DS = Input SS
; USES:
;	ALL but SS,BP
;
; NOTES:
;	ShowWindowKludge is used to tell winoldap that messages received when
;	this flag is set are sent during the call to ShowWindow.  The perfect
;	example of this problem is when you ask for program parameters by
;	setting the parameters line to a ? in the pif file.  This causes
;	ShowWindows to send a message to Winoldap to lose the focus.  This
;	causes TTYKillFocus to try to execute a lose focus macro when the
;	app isn't even started yet.  THIS IS A KLUDGE.  It should be fixed in
;	a more general way in the future but was discovered too late to
;	redesign the code.  This scenario also works around a bug in the
;	Macro Manager.	This has to do with the Macro Manager updateing
;	MacroSeg which it shouldn'y do.  The bug only shows up in the
;	TTYKillFocus scenario.	The flag is only read in TTYKillFocus in
;	TTYWIN.ASM.

BeginExecution:
	push	ss				; Get DS = SS
	pop	ds
	mov	CS:WinoldapDS,DS
	mov	[ExecVars],ax			; Set Behavior and TopLevel
;!! HP
	mov	[ExtMode], CL		; Save Extended mode flag
	ccall	GetCurrentPDB
	mov	ES, DX			; Save original TopPDB stack
	mov	AX, Word Ptr ES:[PDB_user_stack]
	mov	Word Ptr [TopPDB_stack], AX
	mov	AX, Word Ptr ES:[PDB_user_stack +2]
	mov	Word Ptr [TopPDB_stack +2], AX

;*****
;
; If the app uses COM port AND EMS is active AND we are in Extended mode,
; mark it as switchable. Extended mode app does not leave COMM handlers
; around when they are switched back to Windows, so we're OK.
;
	test	[pifMSFlags], fCOM1 +fCOM2 ; Is it a COMM app
	jz	BegEx1_OK		   ; No, go on
	cmp	CS:[EEMSBlock].BankLine, 0
	je	BegEx1_OK
  	cmp	CS:[EEMSBlock].BankLine, -2 ; Is EEMS active?
	jae	BegEx1_OK		   ; No, go on
   
	test	[Behavior], fkeyboard	   ; If the app already modifies the
	jz 	Beg_Ex1_1		   ; keyboard, then force it full-screen
	test	[Behavior], fScreen	   ; and don't reset its keyboard flag
	jnz	BegEx1_OK		   ; if in Extended mode.
	or 	[PifBehavior], fScreen
	or	[Behavior], fScreen
	jmp short BegEx1_OK

Beg_Ex1_1:
	or 	[PifBehavior], fscreen +fkeyboard
	or	[Behavior], fscreen +fkeyboard

	cmp	[ExtMode], TRUE		   ; Are we in Extended mode?
	jnz	BegEx1_OK		   ; No, go on


	and	[PifBehavior], Not fKeyBoard ; Reset keyboard mode
	and	[Behavior], Not fKeyBoard

BegEx1_OK:

	mov	si,[hTTYWnd]			; Window handle (if good app)
	mov	di,[ShowParm]			; ShowWindow argument
	test	[Behavior],BADBITS		; is this a bad app?
	jnz	beg3				; Yes
	cmp	cs:[BadDepth],0 		; are we inside a bad app?
	jnz	beg5				; yes, then we are bad too
;!! HP
    ;
    ; Start up a good old app
    ;
	inc	cs:[GoodCount]			; one more good app running
	cmp	[TopLevel],0			; top level guy?
	jnz	beg1
	jmp	cret				; nope, can't put it in

beg1:
;******
	mov	[OldApState], SF_RUNNING
	mov	byte ptr ShowWindowKludge,1
	cCall	ShowWindow,<si,di>		; Show the window
	mov	byte ptr ShowWindowKludge,0
	cCall	UpdateWindow,<si>		; Make it current
	call	WaitIcon			; wait till were running
	call	RestorePartition		; make sure we're in memory
	call	MySetFocus			; Get the focus
	call	HookGood			; Finish my hooks
	jmp	aret				; Do vectors and COM
    ;
    ; Start up a bad old app
    ;
beg3:	cmp	cs:[BadDepth],0 		; First bad app in this group?
	jnz	beg5				; Nope, start it up
beg4:	mov	di,SW_SHOWMINNOACTIVE		; show us as an icon
	mov	byte ptr ShowWindowKludge,1
	cCall	ShowWindow,<si,di>		; Show the ICON
	mov	byte  ptr ShowWindowKludge,0
	cmp	[ShowParm],di			; Did the user want that?
	je	begIcon 			;  Yes, load the icon.
	cmp	[ShowParm],SHOW_ICONWINDOW	; Did he want it the old way?
	jne	beg5				;  No, start the application
	errn$	begIcon

    ;
    ; Start up a bad old app in the ICONIC state (windows not disabled)
    ;
begIcon:
	inc	cs:[IconCount]			; we have one more icon
	call	WaitIcon			; wait until opened...
	dec	cs:[IconCount]			; Not ICONIC anymore
	call	DoubleCheckCOM			; make sure com ports still avail
	jc	begIcon 			; no, keep waiting
    ;
    ; Start up a BAD old app
    ;
beg5:
	mov	di, [hTTYWnd]			; Make the icon active and
	xor	ax, ax				;  on top.
	mov	bx, SWP_NOMOVE or SWP_NOREDRAW or SWP_NOSIZE
	cCall	SetWindowPos, <di, ax, ax, ax, ax, ax, bx>

	mov	[OldApState], SF_RUNNING	; Update the app state
	call	SetWindowList			; Retrive a list of all top
						;  windows for the hotkey.
	cmp	[TopLevel], 0			; Don't copy Data if child process
	jz	BegEx1a
	call	CopyMacroData			;to static storage
	call	EnableBadMenus
BegEx1a:
	call	EnableBadApp			; Turn app on

	mov	cx,[BufSeg]
	jcxz	beg5a				; no segment defined, so no grabber
	call	EnableGrabber
beg5a:
;
;!! HP
;
;  Call EnableSave if fHPGrab and Toplevel BAD app and Not extended mode

	mov	AX, GRAB_ENABLESAVE
	call	EnDisSave
;******
	mov	dx,dataOffset NoEOF		; clean out MS-DOS EOF flag
	mov	cx,1
	mov	bx,cx
	mov	ah,40H
	int	21H

	xor	ax,ax
	mov	[fIcon],ax		; we aren't ICONIC
	cmp	[TopLevel],al		; top level guy?
	jz	cret1			; no, just return
aret:	call	SaveVectors		; save current Low mem areas in app DS
;
;!! HP
;
cret1:
	test	[pifMSFlags], fCOM1 +fCOM2 ; Is it a COMM app
	jz	cret			; No
	mov	ES, [Partition]
;
;  A partition in extended mode is always swappable even if the fCOM1 and fCOM2
;  flags are set in the Pif.

	inc	CS:[COMMCount]		; One more guy using COMM without
					; notification
	cmp	[Extmode], 1		; are we in extended mode
	jnz	cret			; No
	mov	ES:[prtfSwap], 1	; Yes, mark partition as swappable
	dec	CS:[COMMCount]		; Do not need to update count if in
					; extended mode
;*****
    ;
    ; Finish starting up app. Clear out key queue and set COM area up
    ;
cret:	mov	ax,0C00H		; clean out any pending chars
	int	21H
	xor	ax,ax
	test	[pifMsFlags],fCOM1	; Do I need exclusive use of COM1?
	jz	ssw2a			; No
	mov	[RS232area],ax		; zero out so no one else uses it
ssw2a:	test	[pifMsFlags],fCOM2	; Do I need exclusive use of COM2?
	jz	ssw3			; No
	mov	[RS232area+2],ax	; zero out so no one else uses it
ssw3:	ret

;**
;
; EndExecution - Stop an old app
;
;    If the app is TERMINATED, correct actions to free all of its
;	resources
;
; ENTRY:
;	SS = Data of app to start
;	AH = value for TopLevel variable
;	AL = value for Behavior variable
; EXIT:
;	App is ready to start
;	DS = Input SS
; USES:
;	ALL but SS,BP
;
EndExecution:
	push	ss			; Set DS
	pop	ds
	mov	[ExecVars],ax		; Set Behavior and TopLevel
;!! HP
	mov	[ExtMode], CL		; Save Extended mode flag
;*****
    ;
    ; reset COM1,COM2 stuff
    ;
	xor	cx,cx
	mov	es,cx
	cmp	[RS232area],cx		; was the non-task value zero?
	jnz	ene1			; No
	mov	cx,es:[RS232_base]	; yes, get task value
	mov	[RS232area],cx		; and make it non-task value
ene1:	cmp	word ptr [RS232area+2],0 ; was the non-task value zero?
	jnz	ene2			; No
	mov	cx,es:[RS232_base+2]	; yes, get task value
	mov	[RS232area+2],cx	; and make it non-task value
    ;
    ; Make sure interrupt state is good and restore vectors
    ;
ene2:	call	EnableIntMask		; make sure COM interrupts are still on
	cmp	[TopLevel],0		; top level guy?
	jz	rsw2			; no, don't restore ints
	call	RestoreVectors		; Recover ints from app DS

rsw2:	test	[Behavior],BADBITS	; Bad?
	jnz	enbad			; Yes
    ;
    ; Stop a good app
    ;
	call	UnHookGood		; Un hook good app
	cmp	[TopLevel],0		; Are we top level?
	jz	cret			;  No, continue
	mov	ax, -1			;  Yes, successful exit
	jmp	DestroyExit		;

    ;
    ; Stop a bad app
    ;
enbad:
	mov	AX, GRAB_DISABLESAVE
	call	EnDisSave
	cmp	[TopLevel],0		; Is it gone?
	jnz	enb2			; Yes
	jmp	UnHookBad		; No, just unhook it

    ;
    ; Bad app has terminated:
    ;
enb2:
	call	UnHookBad		; Unhook it
;
;  Do not directly jump to TERMINATE code if we are in the Extended mode
;
	cmp	[ExtMode], 1
	je	EndEx1
	mov	ax, -1			; No errors
	jmp	DestroyExit
EndEx1: ret

;**
;
; UnHookBad - Take a bad app out of the system (but not permanently)
;
;    If this app is the last in a bad app group, prompt
;	for screen down via grabber, if last bad app in system, chuck
;	grabber.
;
;    The primary function is to DEC BadDepth
;
; ENTRY:
;	DS = Data of app to unhook
;	BadDepth indicates what to do
; EXIT:
;	BadDepth decremented by one
;	Grabber disposed of if last bad app in system
;	User prompted if BadDepth -> 0 (last bad app in app group)
;	BufSeg = 0 if BadDepth -> 0 (last bad app in app group)
; USES:
;	ALL but DS,SS,BP
;
UnHookBad:
	dec	cs:[BadDepth]		; Dec BadDepth and ask last bad guy?
	jnz	nret			; Not last guy, done
    ;
    ; :
    ;
    ;
	cmp	[BufSeg],0		; Does app have grabber?
	jz	uh1			; no
	call	DisableGrabber		; Grabber off
uh1:
	call	UnHookAll		; Take out gen hooks
;	test	[pifMsFlags],fDestroy	; always destroy window?
;	jnz	bd1			; Yes, don't prompt
;	mov	si,PromptGrab		; prompt user
;	mov	ax,si			; ax != 0 means only prompt if not blank
;	call	GoGrab			; Do prompt
bd1:	call	DisableBadApp		; take out the app hooks
	xor	ax,ax
	mov	[BufSeg],ax		; No screen grab area
    ;
    ; If last bad app in system, chuck grabber
    ;
	cmp	cs:[IconCount],al	; any Iconic bad apps running?
	jnz	nret			; yes, don't take out screen grabber
	xchg	ax,cs:[GrabSeg] 	; blow it away
	cCall	GlobalFree,<ax>
nret:	ret

;!! HP VVR 042487

;  CopyMacroData - copy macro info from windows storage to static storage
;
; ENTRY
;	DS = Winoldap data
;	hMacroSeg = handle to global windows memory( could be 0)
;
; USES
;	All but DS and BP
CopyMacroData:

	push	BP
	cmp	BufSeg,0
	jnz	CMD10
	xor	AX,AX
	mov	MacroSeg,AX		; Show that Macros are not available
	jmp	CMDFree
CMD10:
	cCall	GlobalLock,hMacroSeg	 ;lock the data in place
	mov	AX,MacroPara
	shl	AX,1		       ;convert to bytes
	shl	AX,1
	shl	AX,1
	shl	AX,1
	mov	CX,AX
	mov	ES,MacroSeg
	xor	DI,DI
	xor	SI,SI
	push	DS
	mov	DS,DX			;DX has MacroSeg
	rep	movsb			;copy the data
	pop	DS
	cCall	GlobalUnlock,hMacroSeg
CMDFree:
	mov	ax, hMacroSeg
	or	ax, ax
	jz	CMDRet
	cCall	GlobalFree,ax		 ;discard the memory
	mov	hMacroSeg, 0
CMDRet:
	pop	BP
	ret

;***

;!! HP ~~RCP 032487
;
;  EnableBadMenus  - copy menu info from windows storage to static storage
;
; ENTRY
;	DS = Winoldap data
;	hMenuSeg = handle to global windows memory
;
; USES
;	All but DS and BP
; NOTES:
;	MenuSeg will be zero if there is no BufSeg or if the InitMenuMem
;	could not allocate windows memory for temporary menu storage.  In
;	this case no menus will be available.
;
EnableBadMenus:

	push	BP
	cmp	MenuSeg,0		 ;is there buffer areas
	jnz	ebm10
	jmp	ebmret
ebm10:
	cCall	GlobalLock,hMenuSeg	;lock the data in place
	shr	AX,1
	shr	AX,1
	shr	AX,1
	shr	AX,1
	add	DX,AX			;DX has segment of source
	mov	AX,MenuPara		;get number of paragraphs
	shl	AX,1			;convert to bytes
	shl	AX,1
	shl	AX,1
	shl	AX,1
	mov	CX,AX
	mov	ES,MenuSeg
	xor	DI,DI
	push	DS
	mov	DS,DX
	xor	SI,SI
	rep	movsb			;copy the data
	pop	DS
	cCall	GlobalUnlock,hMenuSeg
ebmret:
	cmp	hMenuSeg, 0
	jz	ebmret1
	cCall	GlobalFree,hMenuSeg	;discard the memory
	mov	hMenuSeg, 0
ebmret1:
	pop	BP
	ret


;**
;
; EnableBadApp - Get the system ready to run a bad app
;
;	If this is the first bad app
;		Enable the grabber if indicated
;		Enable the switcher if indicated
;		Set approp DOS hooks in kernel
;
; ENTRY:
;	SS = Data of app to start
; EXIT:
;	BadDepth incremented by one
;	DS = Input SS
; USES:
;	ALL but SS,BP
;
EnableBadApp:
;
;!!HP MCP
	call	GetBadClipboard 	;get clipboard data if available
;!!HP Done
;
	inc	cs:[BadDepth]		; we have one more bad app
	cmp	cs:[BadDepth],1 	; first time in?
	jz	eba5

	; Disable A20 line
	push	ax
	xor	ax,ax
	cCall	A20Proc,<ax>
	pop	ax

	jmp	hkb4			; no, done
eba5:
    ;
    ; Starting first bad app
    ;
	mov	[fWindowsEnabled], 0
	call	UnHookAll		; remove all hooks
	cCall	DisableOEMLayer 	; Zap Windows, get back the keyboard, mouse
	call	SetMode 		; Set the initial bad app screen mode
	push	ss			; DS = Data
	pop	ds
	call	HookMemory		; put in memory hooks
	call	HookInt23		; Install our own int23
    ;
    ; If app switchable, hook up switcher
    ;
	mov	cx,[SwitchSeg]
	jcxz	hkb3			; No switcher, DOS hooks are level 0
	call	HookKeyboard		; put in the keyboard hooks
	call	HookClipboard
	call	HookSwitcher		; hook up the switcher
	mov	cx,1			; Switchable app, DOS hooks are level 1
;!! HP
	cmp	[ExtMode], 1		; Are we in the extended mode?
	jne	hkb3			; Yes, Force Windows to install back the
	xor	CX, CX			; ..original DOS console driver
	ccall	DisableDOS,<CX>
;
;~~tqn 062487
;
	mov	AX, 3300h		; Save Current ^C checking state
	int	21h
	mov	[fBreak], DL
	xor	DL, DL
	mov	AX, 3301h		; Turn it off
	int	21h
	jmp short hkb3a
;~~

hkb3:	cCall	DisableDOS,<cx> 	; alter dos patches
hkb3a:
	; Disable A20 line
	push	ax
	xor	ax,ax
	cCall	A20Proc,<ax>
	pop	ax

	cCall	EnableVectra
	ret
;**
;
; GoGrab - Call the indicated grabber function
;
; ENTRY:
;	SI is grabber function offset to call
;	Other regs as for specific grabber function
;	GrabSeg MUST BE VALID!!!!!!!!!!!!!
; EXIT:
;	DS = Input SS  if SS = Winoldap data
;		       else DS preserved	;~~tqn 071887
;	Other regs as for specific grabber function
; USES:
;	DS and others as per specific grabber function.
;
GoGrab:
	mov	cs:[GrabOffset],si	; Function to call
	push	DS			;~~tqn 071887
	mov	ds,cs:[GrabSeg] 	; DS = CS of grabber
	call	cs:[GrabAddr]		; Call grabber
;
;~~tqn 071887
;
; GoGrab can now be called from a hardware ISR (i.e. int09h) and thus, SS
; is no longer guaranteed to be our Winoldap DATA. CS:[WinoldapDS] is.
;
	pop	DS
	pushf
	push	AX
	mov	AX, SS
	cmp	AX, CS:[WinoldapDS]
	pop	AX
	je	hkb4a
	popf
	ret
;~~

hkb4a:	popf
hkb4:
	push	ss			; Set DS
	pop	ds
	ret

page
;**
;
; DoubleCheckCom - Make sure app COM stuff is available
;
; ENTRY:
;	DS = Data of app to ask about
; EXIT:
;	Carry Clear
;	    app COM requirements OK
;	Carry Set
;	    app COM requirements NOT OK
;	    fIcon = SIZEICONIC
;	    Error dialog box displayed
; USES:
;	ALL but DS,SS,BP
;
DoubleCheckCom:
	mov	al,[pifMsFlags] 	; make sure the com ports are still okay
	call	CheckCOM		; still there?
	jnc	dccx			; yes
	mov	[fIcon],SIZEICONIC	; otherwise still iconic
	call	ShowError		; show the error
	stc				; error indicator
dccx:	ret

;**
;
; LoadGrab - load in grabber if it isn't already loaded.
;
; ENTRY:
;	GrabSeg == 0 if grabber not loaded
;	DS = Data of app who wants grabber
; EXIT:
;	Carry Set
;	    Grabber could not be loaded
;	Carry Clear
;	    GrabSeg = Segment of grabber
;	    GrabSize = Size in para of grabber
;	    BufSeg of this app = 0 IF THE GRABBER HAD TO BE LOADED
; USES:
;	ALL but DS,SS,BP
;
LoadGrab:
	call	FindVectra
	xor	ax,ax
	cmp	cs:[GrabSeg],ax 	; if already loaded, we're done
	jz	Lo1			; Carry clear if jump
	jmp	hkb4
    ;
    ; Load in the grabber
    ;
Lo1:
	mov	[BufSeg],ax
	mov	dx,codeOffset GName	; open winoldap.grb
	regptr	csdx,cs,dx
	sub	sp,128
	mov	cx,sp
	regptr	sscx,ss,cx
	mov	ax,OF_PROMPT+OF_CANCEL
	cCall	OpenFile,<csdx,sscx,ax>
	add	sp,128
	cmp	ax,-1			; Did open work?
	stc				; Assume barf
	jz	dccx			;!! HP: Fix relative jump range
    ;
    ; Get size of grabber
    ;
	xor	cx,cx
	xor	dx,dx
	mov	ax,4202h		; get file size
	int	21h
	jc	dccx		       ;!! HP: Fix relative jump range
    ;
    ; Allocate space in mem for grabber
    ;
	push	dx
	push	ax
	call	Allocate
	mov	si,ax
	pop	ax
	pop	dx
	stc				; Assume barf
	jz	dccx			;!! HP: Fix relative jump range

    ;
    ; Set GrabSeg/GrabSize
    ;
	mov	cs:[GrabSeg],si
	push	bx
	call	GetParagraphs
	mov	cs:[GrabSize],bx
	pop	bx
    ;
    ; Read in the grabber
    ;
	xor	cx,cx		; rewind the file
	xor	dx,dx
	mov	ax,4200h
	int	21h
	jc	lg1skip
	push	ds		; read in the file
	mov	ds,si
	mov	cx,-1
	xor	dx,dx
	mov	ah,3fh
	int	21h
	mov	ah,3eH		; close the file
	int	21h
	pop	ds
;
;!! HP
lg1skip:
	jc	lg1		; return if DOS error

;
;  Get Grabber ID

	mov	AX, GRAB_GETID		; Subfunction of InquireGrab
	mov	SI, InquireGrab
	call	GoGrab			; AX = OEMID if we have an HP grabber
	clc
	cmp	AX, OEM_ID
	jnz	lg1a			; Return if not HP
	inc	Byte Ptr CS:[fHPGrab]	; Remember we have an HP grabber
	mov	AX, GRAB_GETVERSION	; Get the version number
	mov	SI, InquireGrab
	call	GoGrab
	mov	GrabVersion,AX		; save it
NoKBRT:

;  Get display type ID and flag [fEGA] if appropriate

	push	DS			; ~~TN 082086~~
	pop	ES
	lea	DI, GrabData
	mov	AX, GRAB_GETINFO	; SI= InquireGrab
	push	DI
	call	GoGrab
	pop	DI
	cmp	Byte Ptr ES:[DI +giDisplayId], DI_EGA
	jne	lgNotEGA
	inc	Byte Ptr CS:[fEGA]	; We have a color EGA display

;  Get swap drive type and notify grabber

lgNOTEGA:
	xor	AX, AX
	mov	AL, CS:[SwapName]
	push	AX			; Save drive name
	sub	AL, 'A'                 ; AX= media device (Drive A= 0)
	mov	BX, 1			; BX= Inquire dirve status
	ccall	InquireSystem, <BX, AX>
	pop	BX			; BL= Swap drive name
	cmp	AX, 3			; Is it a fixed drive ?
	je	lgFD			; Yes
	cmp	AX ,1			; Is it mapped to a fixed drive
	or	DX, DX
	jne	lgFD			; Yes
	mov	BL, 0FFH		; Mark as no swapdrive

lgFD:
	mov	ax,cs			; ~~ jhc 032587 ~~ es:di -> a path!
	mov	es,ax			; ~~ jhc
	mov	di,offset SwapName	; ~~ jhc
	mov	bh,10000000b		; ~~ jhc 053187 ~~ show es:di is valid
	mov	AX, GRAB_SETSWAPDRIVE	; InquireGrab subfunction
	call	GoGrab			; SI= InquireGrab
lg1a:
	clc				; Flag success
;*******
lg1:	ret

;**
;
; TTYOpen - Handles WM_QUERYOPEN message to open ICONIC old app
;
;   This handles the WM_QUERYOPEN message which asks if it
;	is ok to open up the ICONIC app
;
;   If the app is a bad app and fUserLock is not set
;	we post WM_USER+1 to the app which is what actually
;	causes the app to open up (app remains ICONIC as far as
;	Windows is concerned). Also, we notify any DDE shells
;	that the user wants to switch.
;
;   If app is good we just return OK to open it up, and normal
;	Windows paint and size messages get it displayed. The
;	WM_SETFOCUS message will notify the shell that the
;	user tried to switch.
;
; ENTRY:
;	DS = Data of Iconic old app to open
; EXIT:
;	DX:AX = TRUE if OK to open ICONIC app (only for good apps)
;	DX:AX = 0 if NOT OK to open ICONIC app (problem or bad app)
;		Error dialog put up if partition not accessable
;		WM_USER+1 message posted for bad apps if partition OK
; USES:
;	ALL but DS,SS,BP
;
TTYOpen:
    ;
    ; Is this a bad or a good application
    ;
	mov	al, TRUE
	test	[pifBehavior],BadBits		; Bad app?
	jz	opn2				; no, open it up
    ;
    ; Bad application: Notify the shell and check fUserLock
    ;
toBad:
	call	DDESwitchIn
	test	[fUserLock], UL_ON
	jnz	opn1a				; UserLock set don't do anything
	errnz	<$-OpenDirect>			;
    ;
    ; The user wants to swtich. However, a shell may still have
    ; messages it wants to send us before we switch. We must
    ; empty our message queue before we switch.
    ; MyYield will do this for us.
    ;
OpenDirect:
	call	MyYield
	jnz	OpenDirect
    ;
    ; restore the partition
    ;
	call	RestorePartition		; Get the partition
	jnc	opn1				; Partition OK
    ;
    ; Could not access bad old app partition, put up error
    ;
	mov	si,MB_OK+MB_ICONEXCLAMATION+MB_SYSTEMMODAL
	call	ShowMessage2			; Show error
	jmp	short opn1a			; Return false
    ;
    ; OK to open up the bad old app, post WM_USER+1 message
    ;
opn1:	mov	ax,WM_USER+1
	mov	bx,SIZENORMAL+8000H
	cCall	PostMessage,<hTTYwnd,ax,bx,bx,bx>
opn1a:	xor	ax,ax				; Return FALSE, don't open
opn2:	cwd
	ret

;**
;
; GetBufferSizes - Set sizes for various partition areas
;
; ENTRY:
;	DS = Data of app to set
;	AX = Pif bits
; EXIT:
;	BufSize BufPara 	 set if appropriate
;	SwitchSize SwitchPara	 set if appropriate
;	MousePara		 set if appropriate
; USES:
;	ALL but DS,SS,BP
;
GetBufferSizes:
    ;
    ; See about screen grab area
    ;
	push	ax
	cmp	[BufSeg],0		; Swap buffer already allocated?
	jnz	abf2			; yes, look for screen grab buffer
	test	al,fNoGrab		; Do we want a screen grabber?
	jnz	abf2			; No, look for screen buffer
    ;
    ; Set up screen grab
    ;
	test	al,fGraphics		; Graphics or text?
	mov	ax,1			; Assume text
	jz	abf1			; Is text
	inc	ax			; is graphics
abf1:	mov	si,InquireGrab
	call	GoGrab			; Get grab size
	mov	word ptr [BufSize+0],ax ; Set size
	mov	word ptr [BufSize+2],dx
	call	GetParagraphs		; Convert

;!! HP	MCP
	add	bx,[CopyPara]		;allocate 4K for copy buffer
;!! HP Done

;!! HP VVR 042487
	add	bx,[MacroPara]

;!! HP ~~RCP 031787
	add	bx,[MenuPara]		 ;allocate Menu data area
;***

	mov	[BufPara],bx
    ;
    ; See about screen switch area
    ;
abf2:	pop	ax			; Get input ax back
	or	ah,ah			; top level bad app?
	jz	abf3			; No

;!!  HP
;
	push	AX			; Save PifBehavior
	cmp	CS:[HPSize], 0		; Allocate HPsystem buffer if <> 0
	jz	abf2b
	mov	AX, CS:[HPSize]
	xor	DX, DX			; DX:AX= Size of requested buffer
	call	GetParagraphs
	mov	[HPPara], BX		; BX= SIze in paras of HP buffer

abf2b:
	pop	AX			; AL = Pifbehavior
;******
	test	al,fNoSwitch+fKeyboard	; do we want to switch?
	jnz	abf3			; No
	cmp	[SwitchSeg],0		; already allocated?
	jnz	abf3			; Yes
    ;
    ; We are switchable, set up mouse and screen save area
    ;
	call	InquireMouse
	mov	[MousePara],ax		; Set mouse area size

	mov	ax,[pifSysMem]		; Are we a text or graphics app?
	cmp	ax,16
	mov	ax,1			; Assume text save
	jb	abf2a			; is text
	inc	ax			; is graphics save
abf2a:	mov	si,InquireSave
	call	GoGrab			; Get save size
	mov	word ptr [SwitchSize+0],ax
	mov	word ptr [SwitchSize+2],dx
	call	GetParagraphs		; Convert
	mov	[SwitchPara],bx
abf3:	ret

;**
;
; DisableBadApp - Set system up after comming out of old app
;
;   Bring Windows up, with good app hooks if any good apps
;	and process any data exchange into the clipboard
;
; ENTRY:
;	DS = Data of the bad app
; EXIT:
;	Windows enabled
;	Any data exchange from bad app into clipboard
; USES:
;	ALL but DS,SS,BP
;
DisableBadApp:
	; Enable the A20 line
	push	ax
	mov	ax,1
	cCall	A20Proc,<ax>
	pop	ax

	inc	cs:[NoSwapping] 	; don't swap any Int Vectors
	ccall	DisableVectra		;~~tqn 060887
;
;~~tqn 062487
;
	cmp	[ExtMode], 1
	jne	dba1b
	mov	DL, [fBreak]		; Restore original ^C checking state
	mov	AX, 3301h		; ..if coming from Extended mode
	int	21h			;
dba1b:
;~~
	cCall	EnableDOS		; put back in dos patches
	cCall	EnableOEMLayer		; windows gets control back
	mov	[fWindowsEnabled], 1
;
;~~tqn 050187
;
; WOA hooks cannot be around now because of EEMS support since they may be
; banked out. So don't HookGood even if Good apps are active.
;
;	cmp	cs:[GoodCount],0	; any good apps around?
;	jz	dba1			; no, no app hooks
;	call	HookGood		; Hooks for the good apps in
dba1:	dec	cs:[NoSwapping] 	; okay to swap int vectors now
;~~

;!! HP
	cmp	[ClipHandle], 0 	; Skip if handle is alreadly allocated
	jnz	dba1A
;*****
	call	BadAppDataX		; Process any data exchange from this app
dba1A:
	xor	di,di			; get cliphandle and zero previous
	xchg	di,[ClipHandle]
	cmp	di,1			; was an error detected?
	jb	abf3			; 0 means nothing to do
	ja	dba2			; Have some clipboard data
    ;
    ; No memory error doing data exchange set up
    ;
	mov	ax,msgNoClip		; di = 1 means not enough memory
	jmp	ShowMessage		; Error dialog

    ;
    ; Have some data exchange
    ;
dba2:	mov	si,[ClipFormat] 	; get format
	call	DoDataX 		; post the data
	jmp	AddANSIToClipboard
;**
;
; HookGood HookMemory - Set app hooks
;
;    HookGood
;	Video hook
;	Keyboard hook
;	Memory hook
;
; ENTRY:
;	None
; EXIT:
;	Hooks set
; USES:
;	ALL but DS,SS.BP
;
HookGood:
	call	HookVideo
	call	HookKeyboard
	call	HookClipboard
	call	HookMouse
	call	HookInt23

HookMemory:
	test	cs:[fHook],MemHook	; Hook already in?
	jnz	hm1			; Yes, don't do it again
	call	EnableMemory
	or	cs:[fHook],MemHook
hm1:	ret

;**
; UnHookGood UnHookAll UnHookMemory - Take out hooks
;
; !!WARNING!!
;
; It is assumed that UnhookGood is only called by EndExecution.
;
;    UnHookGood
;	Dec GoodCount
;	If last good app
;		Video out
;		Keyboard out
;		Switcher out
;		Memory out
;    UnHookAll
;	Video out
;	Keyboard out
;	Switcher out
;	Memory out
;
; ENTRY:
;	None
; EXIT:
;	Hooks out
; USES:
;	ALL but DS,SS,BP
;
UnHookGood:
	dec	cs:[GoodCount]		; one less good guy, last good guy?
;
;~~tqn 060287
;
; Unhook Good apps on exit only if Toplevel. Leave hooks in if child since DOS
; returns to the parent without a task switch.
;
	cmp	[TopLevel], 0		; Child ?
	jz	uhm1			; Yes, leave hooks in: parent needs it
;~~
UnHookAll:
	call	UnHookVideo
	call	UnHookKeyboard
	call	UnHookClipboard
	call	UnHookSwitcher
	call	UnHookMouse
	call	UnHookInt23

UnHookMemory:
	test	cs:[fHook],MemHook		; Mem hooks in?
	jz	uhm1				; no
	call	DisableMemory
	and	cs:[fHook],NotMemHook
uhm1:	ret

;**
;
; HookVideo UnHookVideo - Manage video hooks
;
; ENTRY:
;	None
; EXIT:
;	Hook modified
; USES:
;	ALL but DS,SS,BP
;
HookVideo:
	test	cs:[fHook],VidHook		; Hook already in?
	jnz	hv1				; Yes,done
	call	EnableVideo			; Hook on
	or	cs:[fHook],VidHook
hv1:	ret

UnHookVideo:
	test	cs:[fHook],VidHook		; Hook in?
	jz	uhv1				; no,done
	call	DisableVideo
	and	cs:[fHook],NotVidHook
uhv1:	ret

;**
;
; HookKeyboard UnHookKeyboard - Manage Keyboard hooks
;
; ENTRY:
;	None
; EXIT:
;	Hook modified
; USES:
;	ALL but DS,SS,BP
;
HookKeyboard:
	test	cs:[fHook],KeyHook		; Hook already in?
	jnz	hk1				; yes, done
	call	EnableKeyboard			; Hook in
	or	cs:[fHook],KeyHook
hk1:	ret

UnHookKeyboard:
	test	cs:[fHook],KeyHook		; Hook in?
	jz	uhk1				; Nope, done
	call	DisableKeyboard 		; Hook out
	and	cs:[fHook],NotKeyHook
uhk1:	ret

;**
;
; HookClipboard UnHookClipboard - Manage Clipboard hooks
;
; ENTRY:
;	None
; EXIT:
;	Hook modified
; USES:
;	ALL but DS,SS,BP
;
; HP KMF 05/20/87
HookClipboard:
	test	cs:[fHook],ClipHook		; Hook already in?
	jnz	hcb1				; yes, done
	call	EnableClipboard 		; Hook in
	or	cs:[fHook],ClipHook
hcb1:	ret

UnHookClipboard:
	test	cs:[fHook],ClipHook		; Hook in?
	jz	uhcb1				; Nope, done
	call	DisableClipboard		; Hook out
	and	cs:[fHook],NotClipHook
uhcb1:	ret


;**
;
; HookMouse UnHookMouse - Manage INT 33 Mouse hook
;
;	The INT33 mouse hook is needed because some good applications
;	(ie Mulitplan) issuse INT33 mouse calls which will screw
;	up some mice (ie HP Mouse). It is only installed in
;	good apps.
;
; ENTRY:
;	None
; EXIT:
;	Hook modified
; USES:
;	ALL but DS,SS,BP
;
HookMouse:
	test	cs:[fHook],MsHook		; Hook already in?
	jnz	hms1				; yes, done
	call	EnableMouse			; Hook in
	or	cs:[fHook],MsHook
hms1:	ret

UnHookMouse:
	test	cs:[fHook],MsHook		; Hook in?
	jz	ums1				; Nope, done
	call	DisableMouse			; Hook out
	and	cs:[fHook],NotMsHook
ums1:	ret

;**
;
; HookInt23 UnHookInt23 - Install/Uninstall our ^C handler
;
; ENTRY:
;	None
; EXIT:
;	Hook modified
; USES:
;	ALL but DS,SS,BP
;
HookInt23:
	test	cs:[fHook],Int23Hook		; Hook already in?
	jnz	hit1				; yes, done

	push	DS
	xor	AX, AX
	mov	ES, AX
	mov	BX, 4 *23h
	lDS	AX, ES:[BX]
	mov	word ptr cs:[prevInt23proc],AX
	mov	word ptr cs:[prevInt23proc+2],DS

	mov	AX,codeOffset Int23Handler
	mov	ES:[BX], AX
	mov	AX, CS
	mov	ES:[BX +2], AX
	pop	DS
	or	cs:[fHook],Int23Hook
hit1:	ret

UnHookInt23:
	test	cs:[fHook],Int23Hook		; Hook in?
	jz	uit1				; Nope, done
	push	DS
	xor	AX, AX
	mov	ES, AX
	mov	BX, 4 *23h
	lDS	AX, cs:[prevInt23Proc]
	mov	word ptr ES:[BX],AX
	mov	word ptr ES:[BX +2],DS
	pop	DS
	and	cs:[fHook],NotInt23Hook
uit1:	ret



;**
;
; HookSwitcher UnHookSwitcher - Manage Switcher hooks
;
; ENTRY:
;	None
; EXIT:
;	Hook modified
; USES:
;	ALL but DS,SS,BP
;
HookSwitcher:
	test	cs:[fHook],SwiHook		; Hook already in?
	jnz	hs1				; Yes, done
	call	EnableSwitcher			; Hook in
	or	cs:[fHook],SwiHook
hs1:	ret

UnHookSwitcher:
	test	cs:[fHook],SwiHook		; Hook in?
	jz	uhs1				; no, done
	call	DisableSwitcher 		; hook out
	and	cs:[fHook],NotSwiHook
uhs1:	ret


;**
;
; Func27 - Halt error
;
;      This routine handles the case where a system halting error
;	    detected. Currently INT 27H or DOS CALL 31H without m
;	    PIF file is the only case.
;
;      We make sure Windows is on, display the bad memory dialog
;
; ENTRY:
;	DS = SS = Data
; EXIT:
;	NONE, DOESN'T EXIT, HALTS
; USES:
;	ALL BUT DS,BP
;
Func27:
	cCall	GetCurrentPDB
	mov	bx,dx
	mov	ah,50H			; switch to Top PDB
	int	21h

	cmp	[BufSeg],0		; Does app have grabber?
	jz	F27a			; no
	call	DisableGrabber		; Grabber off
F27a:
	call	UnHookAll		; take out any hooks
	test	[Behavior],BADBITS	; Do we need to turn Wind
	jz	f27c		; No, it's already on
	cCall	EnableDOS	; put back in dos patches
	cCall	EnableOEMLayer	; Turn Windows on
	mov	[fWindowsEnabled], 1
f27c:	jmp	PostHalted	; Display Bad memory dialog and
				; Halt the system (never returns)
page
;
;!! HP
;	EnDisSave
;
;  Execute the InquireGrab subfunction if
;    - Switch buffer is available
;    - HP grabber is in palce
;    - not in the Extended mode
;    - First time in (i.e. a parent process)
;
;  Inputs:
;    AX:	EnableSave/DisableSave subfunctions
;    DS:	Winoldap Data
;
;  Regs:
;    AX, SI
;

EnDisSave	Proc	near

	test	[Behavior],fNoSwitch + fKeyboard
	jnz	EDSaveRet
	cmp	[Extmode], 0
	jnz	EDSaveRet
	cmp	Byte Ptr CS:[fHPGrab], 0
	jz	EDSaveRet
	cmp	CS:[BadDepth], 1	; First time in ?
	jnz	EDSaveRet		; No
	mov	SI, InquireGrab
	call	GoGrab

EdSaveRet:
	ret

EnDisSave	Endp
;
;*****


sEnd	code
	end
