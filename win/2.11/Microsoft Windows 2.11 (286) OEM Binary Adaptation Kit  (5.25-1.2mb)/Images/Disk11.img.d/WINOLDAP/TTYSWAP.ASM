page ,132
;-------------------------------------------------------------------------
;
; MODULE TTYSWAP.ASM - Code to swap old app partitions in and out
;
;	$Author:   tqn  $
;	$Revision:   1.26  $
;	$Date:   20 Nov 1987 12:21:28  $
;
;
;========  HP MODIFICATION  ==================================================
;
;  A new field is added to the partition structure:
;    HPSeg is the segment of the HP saved buffer
;  A check is added to inquire the HPmouse.
;
;  - Change "FindPartition" to look for a best-fit partition instead of
;    the largest one.
;
;  - Reset [Partition] in "FreePartition" only after "BadAppDataX" is accessed.
;
;  Author:	Tuan Nguyen
;
;  History:	Version 1.03
;		08/14/86
;		03/05/87	Integrate with the documented version of
;				Winoldap including support for Logitech mouse
;		05/12/87	Allocate Macro, Menu, EMS Save buffers
;				EMS support in swapping partitions
;		06/15/87	Moved mouse code to ttymouse.asm
;
;=============================================================================

	.xlist
	include cmacros.inc
	WOAOPT	= 1		;~~tqn 042687 short Windows.inc
	include windows.inc
	include pdb.inc
	include tty.inc
	include galloc.inc
	.list
	include partition.inc
	include ems.inc 	;~~tqn 032787
	include mac.inc 	;~~tqn 040787
page

;-----------------------------------------------
;
; External Windows Procedures
;
externFP	<GlobalSize,GlobalFree,GetCurrentTask,GetCurrentPDB>
externFP	<GetProfileString,GetTempFileName,GlobalRealloc>
externFP	<GetModuleHandle,GetModuleFilename>
;
ExternFP	WinoldapCall		;~~tqn 050387 (KERNEL)

;-----------------------------------------------
;
; External WINOLDAP Procedures
;
externNP	<BadAppDataX,GetUserProCS,FlushComBufferNoYield>
externNP	<SwapVectors,ProcessMessage,GetPureName,GetBytes>
ExternNP	<CheckWinCom>				; (VECEXT)
ExternNP	<AllocEEMSPool,GetBankSeg,RWBank>	; (TTYEEMS)
ExternNP	<MapBanks, ReallocPID,InitEEMS, MapWOAPID>
ExternNP	<SaveContext, RestoreContext>
ExternNP	<HookGood>				; (TTYTASK)
ExternNP	<InitMouseVars> 			; (TTYMOUSE)

;-----------------------------------------------
;
; Per task data stored in app DS
;
sBegin	DATA

;
; Pointers to screen swap area if this app has one
;
externW 	SwitchPara
externW 	SwitchSeg
;
; Pointers to screen grab area if this app has one
;
externW 	BufPara
externW 	BufSeg
;
; Pointers to mouse swap area if this app has one
;
externW 	MousePara
externW 	MouseSeg

;!!  HP
;
ExternW 	HPSeg			; HP saved buffer segment (TTYTASK)
ExternW 	HPPara			; Its size in paragrahs
ExternW 	TotalSize		; Swap Windows size (Extended mode)
ExternW 	<CopySeg, CopyPara>

ExternW 	<MenuSeg, MenuPara>	;~~RCP 031787
ExternW 	<MacroSeg,MacroPara>	;~~VVR 042487
ExternW 	<SFTSeg, SFTPara>	; (TTYEXT)
ExternW 	<EEMSSwapBlock, WOAPID> ;~~tqn 032787
;
;******
;
; PIF flags for this app
;
externB 	pifBehavior
externB 	pifMsFlags
;
; Flag that tells us if this app has the focus
;
externB 	fFocus
;
; Tags to indicate an old app DS
;
externB 	Signature
externW 	Partition
;
; This is a flag which is set when we are performing partition swapping
;	operations. It is primarily used to disable the Interchange proc
;	in TTYFUNC
;
globalB fChecking,0
;
;~~tqn 040787
;
GlobalB fOrgPartition, 1		; = 1: Original owner of partition
;
; For Use by EEMS support code:
;
HTExtraPage	dw	1		; = 1: (Head+Tail) <= PageSize
					;   2: OtherWise
HeadSize	dw	0		;
GlobalW 	HeadTailSize,0
;
;~~

;
; Flag which indicates whether this app partition is in (=0 if app is in)
;   If app is swapped out it is non-zero and the value is the apps PDB address
;
globalW SwapPDB,0

sEnd	DATA

sBegin	CODE
	assumes cs,code
	assumes ds,data

	public	InitPartition,FreePartition,CheckPartition
	public	RestorePartition,FindPartition
	public	WritePartition,ReadPartition
	public	RWpartition,GetSwapFile
	public	InitSwapVars,IsSwappable
	public	StartUpPathname
	public	GetDiskFree, DeleteSwapFile
;
;~~tqn 040787
;
	public	WPEEMS, RPEEMS, RestoreOrg, MapPartition,RWHeadTail ;!! DEBUGGING
;~~

;!!  HP
;
ExternW 	<EEMSBlock>		;~~tqn 032787
ExternD 	<OrgInt21Proc>
;
;******

;-----------------------------------------------
;
; Global task data stored in WINOLDAP code segment
;
	externB szPif			; PIF extension string

SwapDrive	db	"?",0           ; Swapdrive from WIN.INI or calced
		DW	7 dup(0)	;~~tqn 042487

globalW SwapSize,0			; Swapsize from WIN.INI
;
; Storage for the Windows INT 3F vector, we need to track this and manage it
;	as some old apps use INT 3F
;
globalD Int3FVector,0
;
; Pointer iside DOS to the INDOS variable
;
globalD pInDOS,0
;
; Pointer to kernel Int 24H handler for good apps
globalD CritErrPtr,0
;
; Pointer iside DOS to the ErrorMode variable
;
globalD pErrMode,0
;
; Module name for the KERNEL
;
szKernel db "KERNEL",0

GlobalB 	SwapName,0,64		;!! HP: name to use for partition swaps
pSwapNumber	dw	?		; pointer to number field of
					; swap file name


GlobalB 	fATIIBIOS,0		; <>0 if AT II compatable BIOS exists

;!! HP
;
msg_swap_win	equ	0DH		; "COM in use. Cannot swap Windows"
;
;*****

;**
;
; InitPartition - Initialize the partition structure
;
; ENTRY:
;	DS = Data
;	AX = segment addr of partition
;	BX = segment addr of Parent partition (0 if none)
;	SwitchPara set to size of switch area in para
;	BufPara set to size of buf area in para
;	MousePara set to size of mouse area in para
; EXIT:
;	Partition in DS = Input AX
;	SwitchSeg set if SwitchPara != 0
;	MouseSeg set if MousePara != 0
;	BufSeg set if BufPara != 0
;	Inputs to InitHeap
;		BX = last paragraph in partition plus 1
;		ES = arena header of partition
; USES:
;	All but DS, BP
;
InitPartition:
	mov	[Partition],ax			; Save partition addr in DS
	mov	es,ax				; Point to structure
;
;~~tqn 032487
;
	cmp	[fOrgPartition], 0	; Are we the original owner
	je	intP1			; No, continue
	mov	ES:[PrtOrgOwner], DS	; Yes, mark it

intP1:
;~~
	mov	word ptr es:[prtOwner],ss	; Owner is this guys DS
	mov	word ptr es:[prtParent],bx	; Parent
	mov	bx,ax
	inc	byte ptr es:[prtCount]		; Have a ref

;	mov	byte ptr es:[prtfSwap],0	; already zero
;	mov	byte ptr es:[prtfSwitch],0
;	mov	byte ptr es:[prtfBuf],0

	call	IsSwappable
	jcxz	inip0			; cx = 0 means no it isn't
	inc	es:[prtfSwap]		; mark partition as swappable

inip0:	add	bx,((SIZE PartitionStruc)+15)/16 ; Point to next part of partition

	cmp	[BufPara],0		; Is there a Buf (screen grab) part?
	jz	inip1A			; no
	inc	es:[prtfBuf]		; Mark that we have a buf part
	mov	[BufSeg],bx		; Set address
	add	bx,[BufPara]		; Point to next part
;!! HP	MCP
	push	BX			;~~tqn 051287
	sub	bx,[CopyPara]
	mov	[CopySeg],bx
;!! HP VVR 042487
	cmp	[MacroPara], 0		; Leave MacroSeg alone if no Macro
	jz	IPSkipMacro
	sub	bx,[MacroPara]
	mov	[MacroSeg],bx
;!! HP	~~RCP 031787

IPSkipMacro:
	cmp	[MenuPara], 0		; Leave MenuSeg alone if no menu
	jz	IPskipMenu
	sub	bx,[MenuPara]
	mov	[MenuSeg],bx
IPskipMenu:
	pop	BX			;~~tqn 051287

;!!  HP
;
inip1A:
	cmp	[HPSeg], 0		; Already allocated ?
	jnz	inip1
	cmp	[HPPara], 0
	jz	inip1			; NO HP buffer
	inc	ES:[prtfHP]
	mov	[HPSeg], BX
	add	BX, [HPPara]
	add	BX, [HPPara]		; 2 HP buffers

inip1:
	cmp	[SFTPara], 0		; No saved SFT ?
	jz	inipsw
	mov	[SFTSeg], BX
	add	BX, [SFTPara]
inipsw:
;
;~~tqn 032287
;
; allocate room for 2 EEMS context save areas
;
	cmp	CS:[EEMSBlock].fEEMS, 0 ; Skip of no EEMS support
	je	inip1B
	mov	[EEMSSwapBlock].WinContextSeg, BX
	add	BX, CS:[EEMSBlock].ContextSize
	cmp	CS:[EEMSBlock].fEEMS, 2 ; Only LIM32?
	je	inipLIM32
	mov	[EEMSSwapBlock].WOAContextSeg, BX
;
	push	AX			; Save needed context
	push	BX
	push	ES
;
	push	BX
	call	Winoldapcall		; Get WOA map array
;
; Build WOA map array in [WOAContextSeg]
;
	mov	AX, ES			; Don't copy Map array if not there 
	or	AX, AX
	jnz	inipswEMS
	pop	AX			; Clean up stack
	jmp short inipswNext

inipswEMS:
	mov	AX, BX			; BankLine
	push	AX

	xor	DX, DX
	mov	BX, CS:[EEMSBlock].PageSize
	div	BX
	mov	SI, AX			; DS:SI -> WOA Mapping Array
	mov	AX, ES
	mov	DS, AX
	xor	DI, DI			; ES:DI = WOAContextSeg:1
	pop	AX			; AX= BankLine
	pop	ES
	mov	DX, 2			; DX= increment to next entry
	mov	CX, CS:[EEMSBlock].WOAMapWords
	cld

initpBank:
	movsb				; Copy Logical map
	inc	DI
	mov	ES:[DI], AX
	add	AX, BX			; Next physical segment
	add	DI, DX			; Next entry
	loop	initpBank

;
inipswNext:
	push	SS			; Restore needed context
	pop	DS
	pop	ES
	pop	BX
	pop	AX
;
	add	BX, CS:[EEMSBlock].WOAContextSize ; BX -> next buffer

inipLIM32:
	mov	[EEMSSwapBlock].AppContextSeg, BX
	add	BX, CS:[EEMSBlock].ContextSize

inip1B:
;~~
	cmp	[MousePara],0		; Is there a mouse part?
	jz	inip2			; no
	inc	es:[prtfMouse]		; Mark that we have a mouse part
	mov	[MouseSeg],bx		; Set address
	add	bx,[MousePara]		; Point to next part

inip2:	cmp	[SwitchPara],0		; Is there a switch part?
	jz	inip3			; No
	inc	es:[prtfSwitch] 	; Mark that we have a switch part
	mov	[SwitchSeg],bx		; Set switch address
	add	bx,[SwitchPara] 	; Point to next part
;*****


inip3:	dec	ax			; Point to arena
	mov	es,ax			; With ES
	ret

;**
;
; ZeroPartition - Zero out partition memory
;
; ENTRY:
;	AX = Seg addr (also HANDLE) of partition
; EXIT:
;	CARRY CLEAR
;	Partition completely zeroed
; USES:
;	ALL but DS,BP
;
ZeroPartition:
	push	ax
	cCall	GlobalSize,<ax> 	; get size in bytes of partition
	pop	es			; ES:) -> partition
	mov	bx,ax			; low 16 bits of size to BX
	xor	ax,ax			; 0 is what we are going to store
	cld				; Make sure
iniz1:	xor	di,di			; Point to start
	or	dx,dx			; 64K piece left?
	jz	iniz2			; No, do tail, carry clear
	mov	cx,8000H		; 64K bytes
	rep	stosw			; Do 64K piece
	mov	cx,es
	add	cx,1000H		; 64k bytes of paragraphs
	mov	es,cx			; New pointer
	dec	dx			; Did a 64k piece
	jmp	iniz1			; loop on DX

iniz2:	mov	cx,bx			; Tail count in BX
	rep	stosb			; Last piece
	ret

;**
;
;  FreePartition - Free the partition structure
;
; ENTRY:
;	DS = Data
;	Partition = segment of partition
;	ES = PDB of exiting task
; EXIT
;	Partition = 0
;	SwitchSeg = 0 if partition had a switch area
;	MouseSeg = 0 if partition had a mouse area
;	prtCount decremented, partition freed if now 0
;	prtOwner = 0
;	Inputs to InitHeap
;		BX = last paragraph in partition plus 1
;		ES = arena header of partition
; USES:
;	ALL but DS,BP
;
FreePartition:
	mov	bx,es			; bx = pdb of exitting task
	mov	es,[Partition]
	mov	ax,es
	sub	bx,ax			; bx = paragraphs of other stuff
	xor	ax,ax
;	mov	[Partition],ax		;!! HP
;
;~~tqn 042687
;
	cmp	[fOrgPartition], AL	;
	je	FreeP1			; Do not reset PrtOrgOwner if we're not
	mov	ES:[PrtOrgOwner], AX	; ..the original owner.
FreeP1:
;~~
	cmp	es:[prtfSwitch],al	; Switch area?
	jz	frep1A			; no
	mov	[SwitchSeg],ax		; zap switch area

frep1A:
	cmp	ES:[prtfHP], AL
	jz	frep1
	mov	[HPSeg], AX		; Zero the HP saved buffer

frep1:	cmp	es:[prtfMouse],al	; Mouse area?
	jz	frep2			; no
	mov	[MouseSeg],ax		; zap mouse area
frep2:	cmp	es:[prtfBuf],al 	; is there a screen grab buffer
	jz	frep3			; no, don't resize
    ;
    ; App has a grab area. Resize partition if down to last guy
    ;	and set up data exchange
    ;
	push	es
	cmp	es:[prtCount],1 	; last guy in partition?
	ja	frep2a			; no, so don't shrink
	call	GetBytes		; convert paras to bytes
	regptr	xsize,di,si
	cCall	GlobalRealloc,<es,xsize,ax>	; get some room
frep2a: call	BadAppDataX		; Convert grab into Windows form
	pop	es

frep3:
;
;~~tqn 042187
;
	mov	SI, ES
	mov	AX, [EEMSSwapBlock].SaveWinPID
	or	AX, AX
	jz	frep4			; Process as normal if not EEMS
	dec	SI
	mov	ES, SI			; ES: Partition arena header
	inc	SI			; SI: Partition start
	cmp	ES:[PrtParent +10h], 0	; Are we child?
	jnz	frep4			; Yes, go the normal way
	cmp	[fOrgPartition], 0
	je	FreePorg1		; jump if we are not the original

; We are the original and we owns Head/tail banks. Free them!
;
	mov	DX, AX
	EMSCall wEMM_FREE_PID
	jmp short frep4

FreePOrg1:
;
; This instance owns a set of banks (SaveWinPID) which maps to its partition.
; We want to release this partition, so we need to do the following:
; - Find out the mapping of the original partition owner (WOAPID) and map it
;    back to the partition physical address.
; - Free SaveWinpID.
; - Decrement owner count in the original partition.
;
;
	mov	BX, [EEMSSwapBlock].PartitionPages
	push	AX
	xor	AX, AX			; Do not save current Head/Tail
	call	RestoreOrg		; restore original partition and free
					; ..current one.
	pop	DX			; Get back our PID
	EMScall wEMM_FREE_PID		; free it
;
	mov	AX, DS
	mov	ES, SI
	cmp	AX, ES:[prtOwner]
	jne	frep4a			; If <>, we're all set
;
; Orignal Head/Tail has not been restored (i.e. OrgPartition exited or
; swapped to disk). So we have to release the current partition by marking
; its owner as 0.
;
;~~

frep4:
	mov	ES, SI			; ES= Partition start
	mov word ptr es:[prtowner], 0
frep4a: mov	[Partition], 0		; Release the DOS partition
	dec	byte ptr es:[prtCount]	; One less ref
	jnz	fret			; Still has ref, don't free it
	cCall	GlobalFree,<es> 	; Free partition

fret:
	ret

;**
;
; FindPartition - Scan the set of partitions looking for one
;		  that can be used to hold the app we want to run
;
; ENTRY:
;	fParent = bit 0: set if EEMS supported
;		  bit 7; set if child
;	SS = DS = Data of NEW app
;	DI = minimum number of paragraphs needed to run this app
; EXIT:
;	Carry Clear
;	   AX = Address of partition to use
;	   Current partition swaped out to disk
;	   Old app partition zeroed out
;	Carry Set
;	   No partition suitable
;		   No Swap disk
;		   App not switchable
;		   Not enough room on swap disk
;		   Error writing to swap disk
;		   No swappable partitions
;		   No WINOLDAP tasks
; USES:
;	ALL but DS,BP
;
cproc	FindPartition, <NEAR,PUBLIC>
	ParmW	fParent
cbegin

	cmp	cs:[SwapName],'A'               ; if no swap drive...
	jb	fpRet1				; return carry set
	call	SwapCompatible			; is it swap compatible?
	jnc	fpCont
						; no, return with carry set
fpRet1: jmp	sor1
    ;
    ; Scan partition list by scanning task list looking at partitions
    ;

fpCont: cCall	GetCurrentTask			; DX -> first task
	xor	bx,bx				; bx = size of largest found so far
	xor	si,si				; si -> largest partition
fpar1:	mov	es,dx				; point to task
	mov	cx,ss				; This is me
	mov	dx,es:[TDB_TaskSS]		; get the task ds/ss
	cmp	dx,cx				; is it us?
	jz	fparF				; yes, can't swap ourselves, next
    ;
    ; Is this task an OLD app? It is if it has the old app signature
    ;	NOTE THAT THE SIGNATURE IS NOT PROFOUND AND WE CAN BE FOOLED!!!!!!
    ;
	mov	ds,dx
	cmp	word ptr ds:[Signature],'PC'    ; old app DS?
	jnz	fparF				; no, next
    ;
    ; We have found a WINOLDAP task
    ;
	mov	cx,ds:[Partition]		; point at its partition
fparL:	jcxz	fparF				; No partition, next
	mov	ds,cx
    ;
    ; We now look at the partition owner field. If it is 0
    ;	it means the task is swapped out and there is currently
    ;	nothing in the partition, this partition is available for
    ;	partition swapping. If the owner != this TASKs DS, then the
    ;	partition is actually being used by another TASK which
    ;	is using this TASKs partition (this task is swapped out),
    ;	keep looking till we find the task which is actually using the partition.
    ;	If the owner == this TASKs DS, then we have found the TASK currently
    ;	using the partition and we can think about swapping with it.
    ;
	mov	cx,ds:[prtOwner]		; get the owner
	jcxz	fparS				; already swapped or terminated, use it.
	cmp	cx,dx				; is it owned by itself?
	jnz	fpar2				; no, don't use it, check parent
	cmp	ds:[prtfSwap],0 		; is it swappable?
	jz	fpar2				; no, don't use it, check parent
fparS:	mov	ax,ds
	dec	ax				; Point to partition arena
	mov	ds,ax
	inc	ax
;
;  if (DI <= Current_size < BX) then BX = Current_Size
;  else
;   if (BX < Current_Size) then BX = Current_Size
;
	cmp	DI, DS:[ga_size]	; Does Current_size meet minimum
	ja	fParT			; Nop
	or	BX, BX			; Never initialized?
	jz	fParHP1
	cmp	BX, DS:[ga_size]	; Does Current_size fitter than current
	jbe	fParT			; No, continue

fParHP1:				; Mark current as the fittest so far

;  If we are child and EMS4.0 is supported, we can only swap with a child
;  partition.

	test	fParent, 1		; Is EEMS there?
	jz	fParHP2 		; No, keep going

	test	fParent, 80h		; Are we child?
	jnz	fParChild		; Yes, look only for child partition
	cmp	ds:[prtParent +10h], 0	; Is current partition a parent?
	jnz	fParT			; No, cannot use it; keep searching
	jmp short fParHP2

fParChild:
	cmp	ds:[prtParent +10h], 0	; Is current partition a child?
	jz	fParT			; No, cannot use it; keep searching

fParHP2:
	mov	bx,ds:[ga_size] 		; Get new size
	mov	si,ax				; Have new fittest
fParT:	mov	ds,ax				; Point back at partition
fpar2:	mov	cx,ds:[prtParent]		; Check its parent
	jmp short fParL
    ;
    ; Next TASK
    ;
fparF:	mov	dx,es:[TDB_next]		; Next
	or	dx,dx				; End of list?
	jnz	fpar1				; No, keep scanning
    ;
    ; We are finished with the partition scan
    ;
	or	si,si			; did we find anybody?
	stc				; carry set if no
	jz	sor1			; no we didn't
    ;
    ; We found a partition, is it big enough?
    ;
	mov	ds,si			; ds points at the largest partition
	cmp	bx,di			; if below minimum
	jb	sor1			; then no dice
    ;
    ; We found a partition big enough, is there room to swap?
    ;	There must be room for TWO partitions of this size
    ;	on the disk because we must have BOTH partitions out
    ;	on disk when we are switching between them
    ;
	push	bx
	call	GetDiskFree	; dx:ax = (free paragraphs on disk)/2
	pop	bx
	or	dx,dx		; if over 2 megabytes, no problem
	jnz	fpar3		;   MAX size of a partition is 640K and
				;   2 meg is enough to hold two.
	cmp	ax,bx		; enough room for two partitions of this size?
	jb	sor1		; no...return error
    ;
    ; We have found a partition and it is OK to swap
    ;
fpar3:	mov	bx,word ptr ds:[prtOwner]	; get the ID of current partition
;
;~~tqn 040787
;
	mov	CL, 1			; Mark as not coming from RestorePartition
	call	WPEEMS			; try to use EEMS for swapping
	jnc	fpar4			; If success, do not save to disk
;~~
	or	bx,bx			; Already swapped out or terminated?
	jz	fpar4			; yes, don't write him out again
	call	WritePartition		; save it to disk
	jc	sor1			; Oops couldn't write, bomb out

fpar4:	push	word ptr ds:[prtCount]	; don't zero out this part
;
;~~tqn 042487
;
	push	DS:[PrtOrgowner]
	dec	SS:[fOrgPartition]	; We're not the original owner
;~~
	mov	ax,ds
	call	ZeroPartition			; clean it out, clear carry
	pop	DS:[PrtOrgOwner]	;~~tqn 042687
	pop	ax
	mov	ds:[prtCount],al		; Restore the count
	mov	ax,ds				; return address of partition
sor1:	push	ss
	pop	ds				; restore DS
;
cend	; FindPartition

;**
;
; SwapCompatible - Is this app compatible with partition swapping?
;
;	The app is swapable if it has the keyboard bit set
;	    (This is true because the app is NOT
;	     switchable in the ALT-TAB sense, so it must run to completion
;	     before anything else can run)
;	If the app has keyboard bit clear, it is swapable if
;	     it does not have any COM bits set, otherwise it is not
;
; ENTRY:
;	DS = Data
; EXIT:
;	Carry Set
;	    App is not switchable
;	Carry Clear
;	    App is switchable
; USES:
;	FLAGS
;
SwapCompatible:
	clc					; Assume switchable
	test	[pifBehavior],fKeyboard 	; if modifies keyboard...
	jnz	scx				; no problem
	test	[pifMsFlags],fCOM1+fCOM2	; if switchable COM app...
	jz	scx
sc1:	stc					; Can't switch
scx:	ret

;**
;
; CheckPartition - Make sure app partition is in and app has focus
;
; ENTRY:
;	SS = Data of desired app DS
; EXIT:
;	App partition is in
;	App has the focus
; USES:
;	FLAGS
;
CheckPartition:
	cmp	ss:[SwapPDB],0		; Partition in?
	jz	scx			; Yes, all done
	push	ax
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	es
	push	ds

	push	ss
	pop	ds			; DS = Data
	call	FlushComBufferNoYield	; make sure the screen is updated
cp1:	cmp	[fFocus],0		; do we have the focus yet?
	jnz	cp2			; yes, bring in our code
	call	ProcessMessage		; process messages
	jmp	cp1			; and wait

cp2:	call	RestorePartition	; bring in the partition
	call	HookGood		;~~tqn 050187
	call	SwapVectors		; hook up the int vectors

	sti				; Ints back on
	xor	bx,bx			; Reset flag and set right task
	xchg	bx,ss:[SwapPDB] 	; point back to original PDB
	mov	ah,50H
	int	21H			; Make this the current task

cpx:	pop	ds
	pop	es
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret

;**
;
;  RestorePartition - Restore the partitions owned by this task.
;
;	RestorePartition is called when an app goes from iconic
;	to full screen.
;
; ENTRY:
;	DS = Data of app to bring in
; EXIT:
;	Carry Clear
;	    App partition brought in
;	Carry Set
;	    Could not bring in App partition
;	    AX is sring ID of error message to print
;		Couldn't write current partition out
;		Couldn't read new partition in
; USES:
;	ALL but DS,BP
;
RestorePartition:
	inc	[fChecking]		; Indicate we are manipulating partition
;!! HP
;
;  If we are in Extended mode, make sure that we have enough swap disk space
;  left to swap Windows out.
;
;  Moreover, we can only roll the Windows kernel out if the COMM library
;  hasn't installed its COMM int handlers.

;
;~~tqn 041087
;
; [TotalSize] is only computed when the app is in extended mode, so we can use
; it as a flag for checking.
;
	mov	CX, [TotalSize] 	; Required Swap Windows size
	or	CX, CX			; Are we in the Extended mode case?
	jz	rpHP			; Nop
	shr	CX, 1			; Size/2
	push	CX
	call	GetDiskFree		; DX:AX= free paras/2 in swap disk
	pop	CX
	or	DX, DX			; No problem if over 1MB
	jnz	rpHP1
	cmp	AX, CX
	mov	AX, msgNoDisk		; Anticipate error
	jc	rpr3			; Error, not enough swap disk space
rpHP1:
	call	CheckWinCom		; Did COMM install any int handlers?
	mov	AX, msg_Swap_Win	; Anticipate error
	jc	rpr3
;******

rpHP:
	mov	ds,[Partition]		; Get the partition address to restore
rprLoop:
	mov	bx,word ptr ds:[prtOwner]	; get the current owners ID
	mov	ax,ss
	cmp	ax,bx				; Do we own it?
	jz	rpr2				; yes, check parents
    ;
    ; Swap out the current partition user
    ;
;
;~~tqn 040787
;
	xor	CX, CX			; Mark as coming from RestorePartition
	call	WPEEMS			; try to save in EEMS
	jnc	rpr1			; success, now try to restore ours
;~~
	or	bx,bx			; current user out or terminated?
	jz	rpr1			; yes, just restore
	call	WritePartition		; save it to disk
	xor	BX, BX			; Mark current partition as released
	mov	ax,msgNoDisk
	jc	rpr3			; Barfed

rpr1:
	push	word ptr ds:[prtCount]	; Read might read the wrong count
;
;~~tqn 040787
;
	push	DS:[PrtOrgOwner]
	call	RPEEMS			; try to restore from EEMS
	jnc	RPrt1
;~~
	mov	bx,ss			; New owner of partition
	call	ReadPartition
RPrt1:					;~~tqn 040787
	pop	DS:[PrtOrgOwner]	;~~tqn 042687
	pop	ax
	mov	ds:[prtCount],al	; set correct count
	mov	ax,msgNoSwap		; return possible error
	jc	rpr3			; Error
rpr2:	mov	cx,ds:[prtParent]	; Get parent
	jcxz	rpr3			; Done, carry clear
	mov	ds,cx
	jmp	rprLoop

rpr3:	push	ss
	pop	ds			; restore DS
	dec	[fChecking]		; Not doing partition, carry uneffected
rprx:	ret


;**
;
; WritePartition - Create a swap file and write the partition out
;
; ENTRY:
;	DS:0 -> partition to write out
;	BX is partition owner
; EXIT:
;	Carry Clear
;	    Partition written out
;	Carry Set
;	    Partition write failed
; USES:
;	ALL but DS,BP
;
WritePartition:
;
	mov	si,ds
	xor	bx,si		; Seed for name
	push	bx
	mov	si,3C00H	; create the file
	call	GetSwapFile	; Make file
	mov	ah,40H		; write the partition
	call	RWpartition	; Do it
	pop	bx
	jnc	rprx		; Ok, done
	call	DeleteSwapFile	; if we couldn't write it, better delete it..
	stc
	ret

;
;~~tqn 040787
;
page
;====== WPEEMS ================================================================
;
; If EEMS supported, try to swap the current partition directly to LIM space
; or to bank it out depending on what type of memory (EEMS or Conventional)
; that partition currently resides (it cannot reside on both)
;
; ENTRY:
;	DS:0 -> partition to write out
;	BX is partition owner (<> 0 if from RestorePartition)
;	CX:	<>	first time allocation
;		=  0:	called by RestorePartition
; EXIT:
;	Carry Clear
;	    Partition written out
;
;	Carry Set
;	    Partition write failed
; USES:
;	ALL but BX,DS,ES,BP
;
;=============================================================================
;
WPEEMS		Proc	NEAR

	push	BX			; Save useful context
	push	DS
	push	ES
;
	cmp	CS:[EEMSBlock].fEEMS, 1 ; EEMS supported?
	jne	WPdisk			; No, go to disk
	mov	SI, DS
	dec	SI
	mov	ES, SI			; ES: Partition header
	inc	SI			; SI: Partition start
	mov	AX, ES:[ga_size]
	cmp	DS:[PrtParent], 0	; Go to disk if Partition is not TopLevel
	jnz	WPdisk
	cmp	SI, CS:[EEMSBlock].BankLine
	jae	WPEBank 		; jump if bankable

WPLIM:					; Swap to LIM if non-bankable
	or	BX, BX			; Do not save if partition is released
	je	WPDisk0 		; return success
;
	mov	DS, BX			; Save partition owner
	ccall	AllocEEMSpool, <AX>
	inc	CX			; Mark CX flag as below the line
	jc	WPdisk			; Swap to disk if out of EEMS
;
; Get partition owner and assign EEMS PID to it
;
	mov	[EEMSSwapBlock].SaveWinPID, AX
	mov	[EEMSSwapBlock].PartitionPages, BX ; Init # pages allocated to
						   ; ..current partition
; DS= partition owner (not current instance data)
;
	mov	BX, CS:[EEMSBlock].EndFrame	; Mark LIM frame dirty
	mov	[EEMSSwapBlock].CurrentPara, BX ; So new pages are mapped starting
						; ..from [CurrentPara]
	xor	BX, BX
	mov	[EEMSSwapBlock].CurrentPage, BX ; Start from the beginning
;
	Save	<AX>
	ccall	SaveContext, <[EEMSSwapBlock].WinContextSeg>
	ccall	RWBank, <BX, SI, ES:[ga_size], AX, DS>
	ccall	RestoreContext, <[EEMSSwapBlock].WinContextSeg>
;
WPDisk0:
	clc
	jmp	WPEEMSRet
WPDisk:
	stc
	jmp	WPEEMSRet	       ; Return with CY flag

WPEBank:
	or	CX, CX
	jnz	WPEFindP		; Process call from FindPartition
;
; We have a bankable EEMS partition and it is called from RestorePartition:
; If current Partition doesn't own banks, Save it to disk and mark Partition
; as released.
;
	or	BX, BX			; Is partition already released?
	jz	WPDisk0 		; Yes, just return
	mov	DS, BX
	mov	AX, [EEMSSwapBlock].SaveWinPID
	or	AX, AX
	jz	WPDisk			; Save to disk
;
; Current SaveWinPID active:
;  If PartitionPages == 0, the current partition must be the original coming
;  from Extended mode (WinPara > ConvSize) and it doesn't own any banks for
;  its Head/Tail mapping. So save it to disk
;
	cmp	[EEMSSwapBlock].PartitionPages, 0
	jz	WPDisk
	jmp short WPDisk0

WPEFindP:
;
; We have a bankable EEMS partition and it is the first time allocation:
; - Allocate enough banks for this Winoldap instance partition
; - Allocate pages to save the Partition data residing in the
;     physical pages of the start (Head) and end (Tail) of the partition.
;
	push	SS
	pop	DS
	push	BX			; Save current partition
;
	ccall	GetBankSeg, <SI>	; AX= Head bank seg
	mov	BX, AX
	mov	CX, CS:[EEMSBlock].PageSize
	mov	DX, SI
	add	DX, ES:[ga_size]	; DX= End of partition
	sub	AX, SI			;
	or	AX, AX			; done if Head aligned with physical pages
	jz	WPEEMS1
	add	AX, CX			; AX= HeadSize
	add	BX, CX			; BX= Next physical page address
	cmp	BX, DX			; Does partition fit inside 1 physical page
	jb	WPEEMSBig		; Nop, continue
	mov	AX, ES:[ga_size]
	mov	[HeadTailSize], AX	; Set Headtail = Partition Size
	jmp short WPEEMS2		;     Head     = 0

WPEEMSBig:
	mov	[HeadSize], AX
WPEEMS1:
	push	AX			; Save it
	Save	<CX, DX>
	ccall	GetBankSeg, <DX>
	sub	AX, DX
	neg	AX
	pop	DX			; AX= Tail Size
	add	AX, DX			; AX= Head +Tail Size
	mov	[HeadTailSize], AX	; Save it
;
; if (Head+Tail) < PageSize ,HTExtraPage= 1
; else HTExtraPage += 2
;
	cmp	AX, CX
	jb	WPEEMS2
	inc	[HTExtraPage]

WPEEMS2:
;
; if current partition is the original and its SaveWinPID = 0 allocate
; enough pages to save Head&Tail.
;
	pop	BX			; Get back current partition
	or	BX, BX			; if released,
	jz	WPENotOrg
	mov	DS, BX
;
; Initialize original Head/Tail parms
;
	mov	[HeadtailSize], AX
	mov	CL, Byte Ptr SS:[HTExtraPage]
	mov Byte Ptr [HTExtraPage], CL
	mov	CX, SS:[HeadSize]
	mov	[HeadSize], CX
;
	cmp	[EEMSSwapBlock].SaveWinPID, 0
	jne	WPEEMS3
	cmp	[fOrgPartition], 0
	jz	WPDisk1 		; swap to disk if not the original
;
	ccall	AllocEEMSPool, <AX>
	jc	WPDiska 		; swap to disk if out of EEMS
	mov	[EEMSSwapBlock].SaveWinPID, AX
	mov	[EEMSSwapBlock].PartitionPages, BX
	jmp short WPENotOrg

WPEEMS3:
;
; SaveWinPID active:
;  If PartitionPages <> 0, current partition is all set up.
;  else it must be coming from extended mode with (WinPara > Convsize) and
;	it is the original;
;	we cannot allocate new banks for saving Head/tail, so we have to
;	save current partition to disk.
;
	cmp	[EEMSSwapBlock].PartitionPages, 0
	jnz	WPENotOrg

WPDisk1:
;
; The partition owner resides on Winoldap partition banks. Save it to disk
; and map ours in.
;
	mov	BX, DS			; Partition owner
	mov	DS, SI			; Partition start
	push	ES
	push	SI
	call	WritePartition
	pop	SI
	pop	ES
	jc	WPDiska 		; Abort if could not even write to disk

	mov	Word Ptr DS:[PrtOwner], 0 ; Mark partition as released

WPENotOrg:
;
; The partition owner already owns a set of banks including room for Head&Tail
; so we don't need to allocate more banks for him.
;
	push	SS
	pop	DS
	mov	AX, ES:[ga_size]	; Get back partition size
	ccall	AllocEEMSPool, <AX>
	jnc	WPE1			; CY if cannot satisfy request
;
; Out of EEMS cases:
; if Current owner is released, return;
; if Current owner doesn't own its banks, save it to disk
; else if it is the original, release its Head/Tail banks, Save it to disk
;      else Save its Head/Tail banks, map in the Winoldap banks,
;	    release original Head/Tail banks;
; grab partition;
;
	mov	BX, ES:[prtowner+10H]
	or	BX, BX
	jz	WPDiska
	mov	DS, BX
	mov	DX, [EEMSSwapBlock].SaveWinPID
	or	DX, DX			; Does Current own its banks?
	jz	WPDiska 		; No, save it to disk
	xor	AX, AX
;
; Current SaveWinPID active:
;  If PartitionPages == 0, it is the original and doesn't own partition pages.
;  It is already saved to disk at "WPDisk1". So we just return success.
;
	cmp	[EEMSSwapBlock].PartitionPages, AX
	jz	WPEEMSRet
	cmp	[fOrgPartition], AL	; Is current the original?
	jnz	WPE2			; Yes, release its Head/Tail
					; No, Save Current mapping
	mov	AX, DX
	mov	BX, [EEMSSwapBlock].PartitionPages
	mov	CX, AX			; Restore Original Head/Tail if valid
	push	SS
	pop	DS
	call	RestoreOrg		; DX= Original SaveWinPID if <> -1
	cmp	DX ,-1			; Original Head/Tail?
;
; If not, it means that the original owner has been saved to disk. So we just
; have to fake a successful return since the Winoldap partition banks are
; currently not occupied.
;
	je	WPEEMSRet		; no
	mov	AX, ES:[PrtOrgOwner+10H]; Yes, release original Head/Tail banks
	mov	BX, SP
;
; Stack currently contains:
; at SP   = Save ES
;    SP+2 = Save DS (partition start)
;    SP+4 = Save BX (partition owner)
;					; Tell WritePartition that
	mov	[BX+4], AX		; ..we have a new owner (original)
	mov	DS, AX

WPE2:
	EMScall wEMM_FREE_PID
	xor	AX, AX
	mov	[EEMSSwapBlock].SaveWinPID, AX
	mov	[EEMSSwapBlock].PartitionPages, AX

WPDiska:
	stc
	jmp short WPEEMSRet
;
;
; Fresh banks have been successfully allocated to our partition. Map them.
;
; BX= Number of banks
; AX= Newly created PID
; CurrentPage = 0
;
; Setup regs for MapPartition
;
WPE1:
	mov	[EEMSSwapBlock].SaveWinPID, AX ; PID is now initialized
	mov	[EEMSSwapBlock].PartitionPages, BX ; Init # pages allocated to
	mov	DX, AX
	mov	AX, ES:[prtowner+10H]
	or	AX, AX			; Do not save Head/Tail if released
	jz	WPEMap
	mov	DS, AX
	mov	AX, [EEMSSwapBlock].SaveWinPID ; =0: do not save Head/Tail
	mov	BX, [EEMSSwapBlock].PartitionPages

WPEMap:
	xor	CX, CX			; Save Function
	push	SS
	pop	DS
;
	call	MapPartition		; Map our newly created partition

WPEEMSRet:
;
	pop	ES			; Restore useful context
	pop	DS
	pop	BX
	ret

WPEEMS	EndP


page
;====== RPEEMS ===============================================================
;
; Restore this instance partition previously residing either in a set of
; EEMS banks owned by [SaveWinPID] (<> 0) or by Winoldap PID.
;
; Entry:
;  DS:0 	-> partition
;  BX		=  Current partition owner (=0 if released)
;
; Exit:
;  CY:		=  0: Restore successful
;		   1: fail, try to restore from disk if possible
; Regs:
;
;
;=============================================================================

RPEEMS		Proc	NEAR

	push	BX
	push	DS
;
	cmp	CS:[EEMSBlock].fEEMS, 1 ; EEMS supported?
	stc
	jne	RPEdisk0		; No, go to disk

	cmp	DS:[PrtParent], 0	; Are we a child
	stc
	jnz	RPEDisk0		; Yes, go to disk

	mov	SI, DS
	dec	SI
	mov	ES, SI			; ES: Partition header
	inc	SI			; SI: Partition start
	mov	AX, ES:[ga_size]
	push	SS
	pop	DS			; DS: Winoldap Data

	cmp	SI, CS:[EEMSBlock].Bankline
	jae	RPEBank 		; If non-bankable, try to restore from LIM

RPELIM: ; Non-Bankable partition:
	;  if this instance partition is saved in EEMS banks owned by [SaveWinPID]
	;  restore it; else try to restore from disk.

	mov	DX, [EEMSSwapBlock].SaveWinPID
	or	DX, DX
	stc
	jz	RPEDisk0

	mov	BX, CS:[EEMSBlock].EndFrame	; Mark LIM frame dirty
	mov	[EEMSSwapBlock].CurrentPara, BX ; So new pages are mapped starting
						; ..from [CurrentPara]
	xor	BX, BX
	mov	[EEMSSwapBlock].CurrentPage, BX ; Start from the beginning
	inc	BX				; BANK_READ function
;
	mov	DI, CS:[EEMSBlock].ContextSize
	mov	CL, 4
	shl	DI, CL
	sub	SP, DI
	mov	CX, SP
	push	DI
	push	AX
	push	SS
	pop	ES
	mov	DI, CX
	EMScall wEMM_CONTEXT_SAVE
	pop	AX

	ccall	RWBank, <BX,SI,AX,DX,DS>

	mov	SI, DI
	EMSCall wEMM_CONTEXT_RESTORE
	pop	DI
	add	SP, DI
;
; Free SaveWinPID
;
	xor	DX, DX
	mov	[EEMSSwapBLock].PartitionPages, DX
	xchg	[EEMSSwapBlock].SaveWinPID, DX
	EMSCall wEMM_FREE_PID
	clc
RPEDisk0:
	jmp short RPEEMSRet

RPEBank:
;
; Set up regs assuming to map our partition

	mov	DX, [EEMSSwapBlock].SaveWinPID
	mov	DI, [EEMSSwapBlock].PartitionPages
	mov	[EEMSSwapBlock].CurrentPage, 0
	mov	CX, DX			; <> 0: restore function
;
	or	BX, BX			; Is owner present?
	jnz	RPEBank1		; Yes, check our Partition PID
;
; if Current Partition doesn't own banks, it has been saved to disk by WPEEMS
; so the partition is released (BX = PtrOwner =0);
;
; if we owns banks, Restore our Head&Tail,Map our PID
; else restore us from disk
;
	or	DX, DX			; Do we owns banks?
	jz	RPEDisk 		; No, restore from disk
	or	DI, DI
	jz	RPEDisk
	xor	AX, AX			; Don't save Head/Tail
	call	MapPartition
	jmp	RPEEMSRet

RPEBank1:
;
; Current Partition owns its banks:
; If (our [SaveWinPID] <> 0) Save Current Head/tail, map back ours banks;
;
	mov	DS, BX
	mov	AX, [EEMSSwapBlock].SaveWinPID
	mov	BX, [EEMSSwapBlock].PartitionPages
	push	SS
	pop	DS
	or	DX ,DX
	jz	RPEdisk1		; Save to disk if no bank owned
	or	DI, DI
	jz	RPEDisk1

	cmp	[fOrgPartition], 0	; Are we the original owner?
	jne	RPE1			; Yes, Restore Winoldap banks
;
;
; Map our instance partition
;
	call	MapPartition
	jmp short RPEEMSRet

RPE1:	; our partition is the original one(thus belongs to Winoldap PID)
	;  So restore it.
;
	call	RestoreOrg		; CX <> -1: restore Orginial Head/Tail
	jmp short RPEEMSRet

RPEDisk1:
;
; Current Partition owns banks
; We don't.
;
; Since our partition has been saved to disk (due to a shortage of EEMS or
; if (we are the original AND WinPara > ConvSize)),
; the original partition cannot own any banks for its head/tail and was
; also swapped to disk. Thus, the WOA banks were already released.
; Action:
; - Save current Head/Tail, Map back WOA banks;
; - Restore our partition from disk;
;
	xor	CX, CX
	dec	CX			; Do not restore Head/tail
	call	RestoreOrg
					; Go to disk to restore our partition
RPEDisk:
	stc

RPEEMSRet:
	pop	DS
	pop	BX
	ret

RPEEMS		EndP


page
;====== RWHeadTail ============================================================
;
; Save/Restore the beginning (Head) and end (tail) of the partition.
; Assume partition owner(prtOwner) is always <> 0
;
; Entry:
;  AX:		Logical PID, owner of Head/Tail pages
;  BX:		= 0: Save Function
;		<>0: Restore function
;  SI:		Start of Partition
;  DI:		Start of Head/tail pages
;  ES:		Partition header
;  DS:		Winoldap Data
;  [HeadSize]
;  [HeadTailSize]
;
; Exit:
;
;
; Regs:
;  AX, BX, CX, DX
;
;==============================================================================


RWHeadTail	Proc	NEAR

	push	SI			; Save needed context
	push	DI

;
; Save LIM context before using it to RW head/tail
; Build save context on stack
;
	mov	DX, CS:[EEMSBlock].ContextSize
	mov	CL, 4
	shl	DX, CL			; DX= Context Size in bytes
	sub	SP, DX
	mov	CX, SP
	push	DX			; Save it

	push	AX
	push	ES
	push	SS
	xchg	DI, CX
	pop	ES			; ES:DI -> Save Area
	push	CX			;!! Bug in AST 061887
	EMScall wEMM_CONTEXT_SAVE
	pop	CX			;!!
	mov	DI, CX			; Get back HT page start
	pop	ES
	pop	AX

;
	push	BX			; Save function
	mov	[EEMSSwapBlock].CurrentPage, DI ; Setup current free page
	mov	DI, AX				; DI: PID
	mov	DX, CS:[EEMSBlock].EndFrame	; Mark LIM frame dirty
	mov	[EEMSSwapBlock].CurrentPara, DX ; So new pages are mapped starting
						; ..from [Startframe]
	mov	CX, [HeadSize]		; CX: Head size in paras
	or	CX, CX
	je	RWTail
	ccall	RWBank, <BX, SI, CX, DI, DS>	; RW Head
;
RWTail:
	add	SI, ES:[ga_size]	; get end of partition
	mov	AX, [HeadTailSize]
	sub	AX, [HeadSize]		; AX: Tail Size in paras
	or	AX, AX
	pop	BX			; Get back RW function
	jz	RWHTEnd
	sub	SI, AX
;
	ccall	RWBank, <BX, SI, AX, DI, DS>	; RW Tail
;
RWHTEnd:
	pop	DX			; Get back Save area size
	mov	SI, SP			; Restore EEMS context save area
	push	CX			;!! Bug in AST 061887
	EMSCall wEMM_CONTEXT_RESTORE	; DS:SI -> Context Area
	pop	CX			;!!
	add	SP, DX			; Clean up temp area
;
	pop	DI			; Restore needed context
	pop	SI
	ret

RWHeadTail	Endp
;

page
;====== MapPartition =========================================================
;
; Map the banks owned by the Partition PID to the current partition.
; The Head and Tail portion are saved in banks belonging to the current owner
; If calling from a restore operation, restore the Head&Tail data from banks
; owned by CX.
;
; Entry:
;  AX:		PID owning the Head & Tail banks
;		= 0 Skip saving Head/Tail
;  BX:		number of banks owned by AX
;  CX:		= 0:  First save function
;		<>0:  PID to use for restoring Head & Tail
;		=-1:  Skip restoring Head/Tail
;  DX:		Partition PID to be mapped
;  SI:		partition start
;  DI:		# of pages owned by CX(if <> 0)
;  ES:		partition arena header
;  DS:		our data
;  CurrentPage: Partition logical starting page to be mapped
;  [HTExtraPage]
;  [HeadSize]
;  [HeadTailSize]
;
; Exit:
;  CY		cleared
;
; Regs:
;  All but SI,ES,DS,BP
;
;=============================================================================

MapPartition	Proc	NEAR

	push word ptr ES:[PrtCount+10H] ; Save # of owners
	push	ES:[PrtOrgOwner+10H]
	push	DI			; Save for restore
	push	CX			; Save function/ PID
	push	DX			; Save Partition PID

	push	[EEMSSwapBlock].CurrentPage
;
	or	AX, AX			; Skip saving Head/Tail if PID=0
	je	MapPNotSave
	sub	BX, [HTExtraPage]
	mov	DI, BX			; DI= Start of temp logical page
	xor	BX, BX
	call	RWHeadTail

MapPNotSave:
;
; Map new banks to the wanted partition
;
	ccall	GetBankSeg, <SI>
	cmp	AX, SI
	je	MapP1			; jmp if partition is bank aligned
	add	AX, CS:[EEMSBlock].PageSize
MapP1:
	mov	BX, ES:[ga_size]
	sub	BX, [HeadTailSize]
	pop	[EEMSSwapBlock].CurrentPage
	pop	DX			; DX= Partition PID
	or	BX, BX			; Don't map zero size
	jz	MapPSkip
	cmp	DX, CS:[WOAPID] 	; special processing if WOAPID
	jne	MapPCont

	push	AX
	mov	AX, BX			; BX= PageSize aligned
	xor	DX, DX
	div	CS:[EEMSBlock].PageSize
	mov	CX, AX			; CX= Number of pages to map
	pop	AX
	mov	DS, ES:[PrtOwner +10h]
	ccall	MapWOAPID
	jmp short MapPSkip

MapPCont:
	ccall	MapBanks, <AX, BX, DX>
;
; Restore Head/Tail portions if valid
;
MapPSkip:
	pop	BX			; <> 0 for restore function
	pop	DI			; Get back # of pages
	or	BX, BX
	je	MapPend
	cmp	BX, -1			; Skip restoring if PID=-1
	je	MapPEnd
	mov	AX, BX			; PID to use for restore
	sub	DI, [HTExtraPage]
	call	RWHeadTail		; Restore Head&Tail from Partition PID

MapPend:
	pop	ES:[PrtOrgOwner+10H]
	pop	AX
	mov	ES:[PrtCount+10H], AL
	clc
	ret
;
MapPartition	Endp
;

;====== RestoreOrg ===========================================================
;
; Restore the original EEMS mapping (owned by WOAPID) to the current partition
;
; Entry:
;  AX=		PID of Head/Tail pages to be saved
;  BX=		its number of pages
;  CX=		-1: Do not restore original head/tail
;  SI=		Partition start
;  ES=		Partition header
;  DS=		Winoldap data
;
; Exit:
;  CY=		0: Success: DX= -1: Original Head/Tail banks not present
;			      <>-1: Original Head/Tail PID
;
;		1: Error:   detected in EEMS call (AX= Error code)
; Regs:
;  All but SI,ES,DS,BP
;
;=============================================================================

RestoreOrg	Proc	NEAR


; Get WOA PID and its mapping
; Assume current owner always <> 0
;
;
; DX= WOA PID

	mov	DX, CS:[WOAPID]
	cmp	CX, -1
	je	RONotRestore
;
	mov	CX, -1			; Assume Original freed
	cmp	ES:[PrtOrgOwner+10H], 0
	jz	RONotRestore		; Skip if Original Owner was terminated.
	mov	DS, ES:[PrtOrgOwner+10H]; Get the original Partition owner
	cmp	[EEMSSwapBlock].SaveWinPID, 0
	jz	RONotRestore		; Do not restore if no PID
;
	mov	DI, [EEMSSwapBlock].PartitionPages
	or	DI, DI
	jz	RONotRestore
	mov	CX, [EEMSSwapBlock].SaveWinPID ; <> 0: restore function
;
RONotRestore:
	push	SS
	pop	DS
	push	CX			; Save PID function
	call	MapPartition
	pop	DX			; Restore it
	ret

RestoreOrg	endp
;
;~~

;**
;
; RWPartition - Perform Partition I/O
;
; ENTRY:
;	AH = DOS call reg for read or write (3FH or 40H)
;	BX is handle number of swap file
;	DS:0 -> partition
; EXIT:
;	BX HANDLE CLOSED
;	Carry Clear
;	    Operation successful
;	Carry Set
;	    Operation failed
; USES:
;	ALL but DS,BP
;
RWpartition:
	push	ds
	cmp	bx,-1			; if open or create failed...
	stc
	jz	sprx2			; return carry set
	push	ax
	push	bx
	cCall	GlobalSize,<ds> 	; get size of partition
	mov	di,dx
	mov	si,ax			; DI:SI is size
	pop	bx
	pop	ax

spr1:	mov	cx,15*4096		; Do this many bytes at a crack
	or	di,di			; More than 64K left?
	jnz	spr2			; Yup, do next hunk
	cmp	si,cx			; Use SI or CX count
	jae	spr2			; Use CX
	mov	cx,si			; Last piece
spr2:	xor	dx,dx			; ds:dx points at buffer
	push	ax			; Save op code
	int	21h			; read/write those bytes
	cmp	ax,cx			; Do it all?
	pop	ax			; restore function code
	stc				; Assume error
	jnz	sprx			; Was error
	mov	dx,ds			; Bump to next hunk
	add	dx,(15*4096)/16
	mov	ds,dx
	sub	si,cx			; Sub off size of what we just did
	sbb	di,0
	mov	dx,si			; Done?
	or	dx,di
	jnz	spr1			; Not done, next hunk
sprx:	pushf				; Save success failure
	mov	ah,3eH			; Close handle
	int	21H
	popf
sprx2:	pop	ds
	ret

;**
;
; ReadPartition - Read the partition in and delete the swap file
;
; ENTRY:
;	DS:0 -> partition to write out
;	BX is partition owner
; EXIT:
;	Carry Clear
;	    Partition read in, swap file deleted
;	Carry Set
;	    Partition read failed, swap file NOT deleted
; USES:
;	ALL but DS,BP
;
ReadPartition:
	mov	si,ds
	xor	bx,si		; Seed for name
	push	bx
	mov	si,3D00H	; open for read only
	call	GetSwapFile
	mov	ah,3FH
	call	RWpartition	; read it into memory
	pop	bx
	jc	osfx		; if failure, don't delete
	errn$	DeleteSwapFile	; ReadPartition falls into DeleteSwapFile
;**
;
; DeleteSwapFile GetSwapFile - Perform swap file operations
;
; ENTRY:
;	BX is seed used to identify swap file
;	For GetSwapFile
;		SI = AX for OPEN,CREATE DOS INT 21H call
; EXIT:
;	For GetSwapFile
;		BX is file handle = -1 if OPEN or CREATE failed
; USES:
;	ALL but DS,DI,BP
;
DeleteSwapFile:
	mov	si,4100H		; Delete DOS call
GetSwapFile:
	push	ds
	push	di
	push	cs
	pop	es
	mov	di,cs:[pSwapNumber]	; fill in number field
	mov	cx,4			; Four digit #
osf:	mov	ax,bx
	and	al,00001111b		; Next hex digit
	add	al,'A'                  ; Convert to printable form
	stosb
	shr	bx,1			; Next digit
	shr	bx,1
	shr	bx,1
	shr	bx,1
	loop	osf
	push	cs
	pop	ds
	mov	dx,codeOffset Swapname ; ds:dx points at name
	xor	cx,cx		; For create attrib of 0
	mov	ax,si		; open, create, destroy
	int	21h
	jnc	osf2		; Worked
	mov	ax,-1		; Set invalid handle #
osf2:	mov	bx,ax
	pop	di
	pop	ds
osfx:	ret


;**
;
; InitSwapVars - Initialize Swap variables:  SwapDrive, SwapSize
;					     MouseVector
;					     pSwapNumber, SwapName
;
;    NOTE WARNING DANGER!!!!!! THIS CODE CAN ONLY BE CALLED ONCE!!!!
;		it overlays itself with data
;
; ENTRY:
;	INT 33 vector set to indicate mouse
;	INT 3F vector points to Windows INT 3F handler
; EXIT:
;	SwapDrive set
;	SwapSize set
;	If SwapDrive is valid
;	    Base swap file name built at SwapName
;	    pSwapNumber points into SwapName at number field
;	MouseVector set if MS mouse detected
;	Int3FVector points to Windows INT 3F handler
;	pInDOS points to DOS INDOS flag
;	pErrMode points to DOS ERRORMODE flag
;	StartUpPathname points to <initial Windows directory>\PIF\
;		(place to look for PIF files)
; USES:
;	ALL but DS,BP
;
StartUpPathname label byte	; this code is replaced by the startup path
szDrive db	"SwapDisk",0
szSize	db	"SwapSize",0


InitSwapVars:
    ;
    ; Set the mouse variables
    ;
	call	InitMouseVars

    ;
    ; Init SwapDrive from WIN.INI
    ;
isvm:	mov	di,codeOffset szPif
	mov	bx,codeOffset szDrive
	mov	cx,codeOffset SwapDrive
	mov	dx,15			;~~tqn 042487 Get Swap Drive
	regptr	csdi,cs,di
	regptr	csbx,cs,bx
	regptr	cscx,cs,cx
	cCall	GetProfileString,<csdi,csbx,cscx,cscx,dx>
    ;
    ; Build up swap file name at SwapName
    ;
	xor	ax,ax
	mov	si,codeOffset SwapName
	push	cs
	pop	ds

assumes ds,code

	regptr	dssi,ds,si
	cCall	GetTempFileName,<ax,csdi,si,dssi>

;!! HP
	and	[SwapName], 11011111b	; Convert to upper case
;****
	mov	ax,word ptr [SwapDrive]
	cmp	al,'?'          ; We to calc swap drive?
	je	isv1		; Yes
;
;~~tqn 042487
;
	ja	isvDrive
	cmp	al,'/'          ; Valid delimiter for EEMS support
	je	isv1		; We need also to calc swap drive
	jmp short isvNS 	; Swap drive is invalid

isvDrive:
;~~
	and	al,11011111b	; convert to upper case
	cmp	al,[SwapName]	; Did get temp build a name on this drive?
	jz	isv1		; Yes, we're happy
    ;
    ; Need to force a temp file name on the correct drive
    ;
	or	al,TF_FORCEDRIVE
	inc	si
	cCall	GetTempFileName,<ax,csdi,si,dssi>
	mov	ax,[si] 		; change to root directory
	mov	[si-1],ax
	mov	byte ptr [si+1],"\"

isv1:	or	dx,dx			; Is the swap drive removable?
	jnz	isv1b			; No, it is ok
;
; Here if NO SWAP DRIVE
;
isvNS:	mov	[SwapName],0
	jmp	short isv2

    ;
    ; Find out where numeric part of name is and set pSwapNumber
    ;
isv1b:	lodsb
	cmp	al,'~'          ; find out where the number is
	jnz	isv1b
	add	si,3
	mov	[pSwapNumber],si
    ;
    ; Get SwapSize parm
    ;
	xor	ax,ax
	mov	di,codeOffset szSize
	call	GetUserProCS
assumes ds,data
	mov	cl,6		; convert 1024/16
	shl	ax,cl
	mov	cs:[SwapSize],ax

isv2:
;
; Initialize the original DOS int21 vector
;
	mov	BX, -1
	call	WinoldapCall		; DS:SI -> OrgInt21Proc
	lDS	AX, [SI]
	mov word ptr CS:[OrgInt21Proc], AX
	mov word ptr CS:[OrgInt21Proc +2], DS
	mov	CS:[EEMSBlock].Bankline, BX ; -1 if no EEMS
;
;~~tqn 042487
;
; We now have to scan Swapdrive for "/E" or "/e" which selects EEMS support.
; Any character in front of "/e" is ignored. 0 indicates the end of string
;
	push	CS
	pop	DS
	lea	SI, Swapdrive
	xor	BX, BX			; Assume swapdisk=? or <drive>

isvELoop:
	lodsw
	dec	SI			; setup for next load
	or	AH, AH			; end of string
	jz	isvENext		; Yes, no match
	and	AH, 11011111b		; convert to upper case
	cmp	AX, 'E/'                ; match?
	jne	isvELoop		; no, try next char
;
; make sure that EEMS is correctly initialized
;
	inc	BX			; Set initeems input: swapdisk=/e

isvENext:
	push	SS
	pop	DS
	call	InitEEMS



;~~
;
;  Remember where INT 3FH points for those stupid BASCOM programs
;
	mov	ax,353FH
	int	21H
	mov	word ptr cs:[Int3FVector  ],bx
	mov	word ptr cs:[Int3FVector+2],es

;
;  Save Critical erro handler vector for any good apps.
;

	mov	ax,3524H
	int	21H
	mov	word ptr cs:[CritErrPtr   ],bx
	mov	word ptr cs:[CritErrPtr +2],es

;
;  Get a pointer to the INDOS, ERRMODE flag....
;
	mov	ah,34H
	int	21h
	mov	word ptr cs:[pInDOS  ],bx
	mov	word ptr cs:[pInDOS+2],es
;
;~~tqn 061687
;
; In 2.00, we have to get the DOS Errmode flag ourselves since the Windows
; kernel maintains its own local copy. The location of that flag is DOS
; version-dependent:
; - For DOS 2.X, it is the byte following InDos flag.
; - For DOS 3.X, it is the byte before the InDOS flag.
;
	mov	DX, BX
	inc	DX			; Assume DOS 2.X
	mov	AH, 30h 		; Get DOS version
	int	21h			; DX unchanged
	cmp	AL, 3			; DOS 3.X or higher?
	jb	isvDOS2 		; no, jump
	dec	DX
	dec	DX
	cmp	AH, 10
	jae	isvDOS2
	sub	DX, (311h - 168h )	; Adjust Errmode pointer for DOS 3.0
					; Pure Magic
isvDOS2:
	mov	word ptr cs:[pErrMode  ],DX
	mov	word ptr cs:[pErrMode+2],ES

    ;
    ; Set the BIOS version flag
    ;
	mov	AH,0C0H
	int	15H			; Get environ info. (only supported
					; in AT II BIOS or above)
	jc	bex10			; If failed then AT I BIOS
 	mov	AX,12FFH		; see if function 12H is supported
 	int	16H
	cmp	AX, 10FFH		; if AX=10FFh then no AT II BIOS
 	je	bex10
	mov	CS:fATIIBIOS,0FFH
bex10:
;~~
;
;  Replace previous code with startup pathname at StartupPathname
;
isv3:	mov	ax,codeOffset szKernel
	regptr	csax,cs,ax
	cCall	GetModuleHandle,<csax>
	mov	di,codeOffset StartupPathname
	mov	cx,codeOffset isv3
	sub	cx,di
	regptr	csdi,cs,di
	cCall	GetModuleFilename,<ax,csdi,cx>
	push	cs
	pop	es
	call	GetPureName
	mov	ax,"IP"                         ; tack on \PIF\
	stosw
	mov	ax,"\F"
	stosw
	xor	ax,ax
	stosb

	push	ss
	pop	ds
	ret

;**
;
; IsSwappable - Is an app swappable
;
;   An app is swappable if it does NOT have the fKeyboard, fNoSwitch,
;	fCOM1, and fCOM2 pif bits set
;
; ENTRY:
;	DS = Data of app to test
; EXIT:
;	CX == 0 partition is NOT swappable
;	CX != 0 partition is swappable
;	    DX = SwapSize from WIN.INI
; USES:
;	CX,DX,FLAGS
;
IsSwappable:
	xor	cx,cx
	test	[pifBehavior],fKeyboard+fNoSwitch
	jnz	isw1
	test	[pifMsFlags],fCOM1+fCOM2
	jnz	isw1
	mov	dx,cs:[SwapSize]
	inc	cx
isw1:	ret

;**
;
; GetDiskFree - Return converted disk free space on swap drive
;
; ENTRY:
;	SwapName contains drive letter of swap drive
; EXIT:
;	AX is free paras on swap drive / 2
; USES:
;	AX,BX,CX,DX,FLAGS
;
GetDiskFree:
	mov	dl,cs:[SwapName] ; get amount of disk space available...
	sub	dl,"A"-1
	mov	ah,36H
	int	21h
	shr	cx,1		; convert to paragraphs per sector
	shr	cx,1
	shr	cx,1
	shr	cx,1
	shr	cx,1		; extra divide to get half of free disk space
	mul	cx		; ((Para/Sector)/2) * Sectors / Cluster
	mul	bx		; * # free clusters = ax = (free paragraphs on disk)/2
	ret



sEnd	CODE
end
