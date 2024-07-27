page ,132
.xlist
include cmacros.inc
include mac.inc
.list
title EEMS support in Winoldap


;++++++  HP Adaptation +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;
;
;  $Author:   tqn  $
;  $Revision:   1.6  $
;  $Date:   04 Nov 1987 23:40:22  $
;
;
;  This module contains a set of routines which manage the allocation, mapping
;  and context saving of EEMS banks. These services are invoked in 2 cases:
;
;  1) When Winoldap prepares to go to the Extended mode ("SaveWindows"), returns
;     from the Extended mode ("RestoreWindows") or when a context switch is requested
;     between the current old application and Windows in both directions ("SaveApp",
;     "RestoreApp").
;
;  2) When a partition swapping occurs for GOOD and BAD apps. See FindPartition,
;     RestorePartition in TTYSWAP
;
;  Author:	Tuan Nguyen
;
;  History:	03/15/87	First Pass
;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


page
include	ems.inc
;
page

sBegin	DATA
	assumes	DS, data

ExternB	<fEEMSWin>			  	; (TTYEXT)
;
GlobalW EEMSSwapBlock,0,EEMSSwapInfoSize/2

sEnd	DATA

page

sBegin	CODE
	assumes	CS, CODE

ExternFP	<GetCurrentPDB, WinoldapCall>	; (KERNEL)
ExternW		TopPDB				; (TTYEXEC)
;
GlobalW EEMSBlock,0,EEMSInfoSize/2
GlobalW	WoaPID,0			; Winoldap task EEMS PID
;
EMMDevName	db	'EMMXXXX0'	; EEM ID string following EMM_API vector
;
;
	public	UnmapBanks
;


.xlist
subttl	InitEEMS: Initialize EEMS global parms
.list
page
;====== InitEEMS =============================================================
;
; Initialize EEMS global parameters in EEMSBLOCK
; This routine SHOULD be called ONLY ONCE by the first instance of Winoldap!
;
;	
; Entry:
;  BX:		0: SwapDisk= /e
;		1: SwapDisk= ? or <Drive>
; Exit:
;  CY:   	0: Success
;  		   EEMSBlock initialized, fEEMS = 1: LIM4.0
;						= 2: LIM3.2
;		1: Fail: AX= Error Code
;		   BankLine = -1, fEEMS = 0
; Regs:
;  AX,BX,CX,DX
;
;=============================================================================

cproc	InitEEMS, <PUBLIC, NEAR>

cBegin
	
	push	SI			; Save needed context
	push	DI
	push	ES
	push	DS
;
; Check to see if EMM_API is intalled
;
	xor	AX, AX
	mov 	DS, AX			; DS= Segment of interrupt table
	LES	DI, DS:[4 *EMM_API] 
;
; The 10th byte at the EMM_API segment should contain the EMM ID code.
;				 
	push	CS
	pop	DS
  	mov	SI,codeOffset EMMdevname
	mov	DI,10			; Point to device name
	mov	cx,8
	repz	cmpsb
   	jnz	InitNoEEMS		; No, error
;
	EMSCall	wEMM_STATUS		; Is EMM driver active and well?
	or	AH, AH
	jnz	InitNoEEMS		; No, error
;
; Get EEMS save context size
;
	EMSCall wEMM_CONTEXT_SIZE	; AL= size in bytes
	xor 	AH, AH
	add	AX, PARAGRAPH -1	; round up to convert to paras
	mov	CL, 4
	shr	AX, CL
	mov	CS:[EEMSBlock].ContextSize, AX
;
; Get Page frame address
;
	push	BX
	EMSCall wEMM_PAGE_FRAME		; Get LIM StartFrame
	mov	CS:[EEMSBlock].StartFrame, BX
	mov	CS:[EEMSBlock].LowestBankSeg, BX
	add	BX, FRAME_SIZE
	mov	CS:[EEMSBlock].EndFrame, BX
	pop	BX

	EMSCall wEMM_VERSION		; AL= BCD version number
	or	AH, AH
	jne	InitNoEEMS

	or	BX, BX			; Swap to disk ONLY?
	jnz	InitLIM			; No, continue
;
; SwapDisk= ? or <Drive>
; Flag that we have LIM3.2 so oldapps using EMS can be saved and restored
; correctly.
;
InitLIM32:
	mov	CS:[EEMSBlock].fEEMS, 2
	jmp short InitEEMSEnd0

;
InitnoEEMS:
InitnoLIM:

	dec	CS:[EEMSBlock].BankLine	; mark as invalid 
	stc				; mark failure

InitEEMSEnd0:
 	jmp	InitEEMSEnd		; ..and return

InitLIM:
;
; We can only fully support version 4.X and higher
;
	cmp	AL, 40H     
	jb 	InitLIM32
;
; Start initializing the global stucture EEMSBlock
;
; Get information about hardware capability
;
	mov	SI, SP			; Allocate temp buffer from stack
	sub	SP, EEMShwInfoSize
	mov 	DI, SP
	push	SS
	pop	ES			; ES:DI -> hardware info block
	EMSCall wEMM_HARDWARE_CAPS
;
	mov	BX, ES:[DI].hwi_pagesize
	mov	CS:[EEMSBlock].PageSize, BX
	xor	DX, DX
	mov	AX, FRAME_SIZE
	div	BX			; AX= # raw pages per LIM frame
	mov	CS:[EEMSBlock].PageperFrame, AX
	mov	SP, SI			; Clean up stack
;
; Get number of mappable physical pages
;
	EMSCall wEMM_MAPPABLE_ARRAY_LEN ;CX= number of mappable pages
	mov	CS:[EEMSBlock].Map_Pages, CX
;
; Get lowest mappable page
;
 	push	SS
 	pop	ES
 	mov	SI, SP			; Allocate temp buffer from stack
	shl	CX, 1			; 4 bytes per entry
	shl	CX, 1
 	sub	SP, CX
 	mov	DI, SP			; ES:DI -> physical address array
 	EMSCall wEMM_MAPPABLE_ARRAY
   	mov	AX, ES:[DI]		; get lowest segment
   	mov	CS:[EEMSBlock].LowestBankSeg, AX
 	mov	SP, SI			; Get back old stack
	save	<AX>
	ccall	GetCurrentPDB		; DX= TopPDB

	add	DX, PDB_SIZE
	sub	AX, DX
;
; Get TopPDBBank (= 0 if TopPDB +PDB_SIZE is in conventional memory)
;
	ja	Initnext
	ccall	GetBankSeg, <DX>        ; TopPDB+PDB_SIZE is in EEMS bank
	mov	CS:[EEMSBlock].TopPDBBank, AX
	xor	AX, AX

Initnext:
	mov	CS:[EEMSBlock].ConvSize, AX ; =0 if TopPDB+PDB_Size is in EMS
;
; Get EEMS BankLine
;
	xor	BX, BX
	call	WinoldapCall
;
; AX= WOA PID
; BX= BankLine
; DS:SI -> Original Int21 handler
; ES:DI -> WOA PID mapping array (logical pages owned by WOA PID)
; CX= Number of entries in array (4 bytes per entry)
;
	mov	CS:[WOAPID], AX
	mov	CS:[EEMSBlock].BankLine, BX
	or 	BX, BX
	jnz	InitE1
	jmp	InitNoEEMS		; Error if No Bankline
;
InitE1:
	mov	AX, BX
	xor	DX, DX
	div	CS:[EEMSBlock].PageSize	; Get the offset from the mapping array
 	mov 	CX, 60			; where Bankline logical page resides
	sub	CX ,AX
	mov	CS:[EEMSBlock].WOAMapWords, CX
	add	CX, 4 -1		; Round up before convert to paras
	shr	CX, 1
	shr	CX ,1
	mov	CS:[EEMSBlock].WOAContextSize, CX
;	
	sub	BX, CS:[EEMSBlock].LowestBankSeg
;
; Get number of logical raw pages owned by PID0
;
	mov	AX, BX
	div	CS:[EEMSBlock].PageSize	; Get number of pages owned by PID0
	mov	CS:[EEMSBlock].PID0Pages, AX
;
	inc	CS:[EEMSBlock].fEEMS	; mark presence of EEMS
	clc				; ret success

InitEEMSEnd:
	pop	ES			; Restore needed regs
	pop	DS
	pop	DI
	pop	SI

cEnd	; InitScreen

	
.xlist
subttl	AllocEEMSPool: Allocate new banks from EEMS pool
.list
page		 
;====== AllocEEMSpool =========================================================
;
; Call the EEMM driver to allocate new raw banks based on the request size in
; paras.
;
; Entry:
;   RequestSize: requested size in paragraphs
;   DS:		 Winoldap DATA
;
; Exit:
;   CY:	0	Success
;	1	Problem -> failure
;   AX:		new PID
;   BX:         Number of raw logical pages allocated
;   CurrentPage:Set to 0
;
; Regs:
;  AX, BX, DX
;
;==============================================================================

cproc	AllocEEMSPool, <PUBLIC, NEAR>
	ParmW	RequestSize

cBegin
	mov	AX, RequestSize
	xor	DX, DX
	div	CS:[EEMSBlock].PageSize			
	or 	DX, DX			; Round up if DX <> 0
	je	NoRoundUp
	inc	AX

NoRoundUp:
	mov	BX, AX			; Set up to get PID
	EMScall	wEMM_GET_RAW_PID   
	or	AH, AH
	je	AllocOk			; Error if AX <> 0
	stc
	jmp short AllocEnd

AllocOk:
	mov	AX, DX			; return newly allocated PID
;
; Current free page starts at 0
;
	mov	[EEMSSwapBlock].CurrentPage, 0	
	clc				; return success

AllocEnd:
cEnd		; AllocEEMSPool



;====== ReallocPID ===========================================================
;
; Reallocate size of specified PID (grow or shrink)
;
; Entry:
;  DS:		Winoldap DATA
;
; Exit:
;  CY:		0: Success
;		    BX: New # pages reallocated
;		1: Fail, AH= Error code
; Regs:
;  AX, BX, DX
;
;=============================================================================

cproc	ReallocPID, <PUBLIC, NEAR>
	parmW	EntryPID		; PID to reallocate
	parmW	NewSize			; New Size in paras

cBegin
	mov	AX, NewSize
	add	AX, CS:[EEMSBlock].PageSize ; round up before Converting to
	dec	AX			; ..number of needed logical pages
	xor	DX, DX
	div	CS:[EEMSBlock].PageSize	; AX= # pages needed
	mov	BX, AX
	EMSCall	wEMM_REALLOCATE_FOR_PID,,, EntryPID
	or	AH, AH
	je	RPIDret			; jump if OK
	stc				; Mark failure
RPIDret:
;
cEnd	; ReallocPID



.xlist
subttl	MapBanks: Map new Banks to frame
.list
page
;====== MapBanks ==============================================================
;
; This routine is called to map banks to either the LIM frame or any physical
; page address specified in StartframeSeg. If !(LIM || SaveWinPID || WOAPID) it
; must come from AppPID. AppPID is assumed to map all its banks in one call so
; that its starting logical page is always 0.
;
; Entry:
;  DS:		Winoldap Instance DATA (not necessarily the current one)
;  CurrentPage: raw page to be mapped if (LIM || SaveWinPID || WOAPID)
;
; Exit:
;  CurrentPage: Current free logical page in the page array owned by PID
;		in the LIM case or PID is SaveWinPID
;  CY:		0: Success
;		1: Failure, AH= error code
; Regs:
;
;==============================================================================

cproc	MapBanks, <PUBLIC, NEAR>
	ParmW	StartframeSeg		; Segment starting of a frame
	ParmW	FrameSize		; Resquest frame size in paras
	ParmW	EntryPID		; PID owning the logical pages
cBegin
	push	SI			; Save needed context
	push	DI
	push	ES
;
; Get number of raw logical pages per frame
;
	mov	AX, StartFrameSeg
;
; init BX as the next free logical page to be mapped
; Start with logical page 0 if not for LIM

	mov	BX, [EEMSSwapBlock].CurrentPage	; Assume LIM setup
	mov	CX, CS:[EEMSBlock].PageperFrame
	cmp	AX, CS:[EEMSBlock].StartFrame
	je 	MapB1				; Jump to handle LIM
		
;
; Handle mapping not for LIM: Get #pages needed for FrameSize
;
	xor	DX, DX
	mov	AX, FrameSize
	mov	BX, CS:[EEMSBlock].PageSize
	dec	BX
	add	AX, BX			; Round up FrameSize
	inc	BX
	div	BX			; AX= # raw logical pages to be mapped
	xor	BX, BX			; next free page always starts at 0
;					  ..if not LIM or not SaveWinPID
	mov	DX, EntryPID
	cmp	DX, [EEMSSwapBlock].AppPID
	je 	MapB1a
;
; Map starts at [CurrentPage] if it is not AppPID
;		
	mov	BX, [EEMSSwapBlock].CurrentPage

MapB1a:
	mov	CX, AX
;		
; Build map list
;
; CX= Number of pages to be mapped 
; BX= next logical free page number (0 if not LIM)
;				    ([CurrentPage] if LIM)
;
	mov	AX,StartFrameSeg  	; Get start of frame segment
MapB1:
	push	AX
	push	BX
	push	BX
	push	BX
	EMSCall	wEMM_GET_PID_PAGES,,,EntryPID ; BX= # of raw pages owned by
	pop 	AX	     		      ;     ..EntryPID
	add	AX, CX
	cmp	AX, BX			; don't map pages out-of-range
	pop	AX
	jbe	MapB1b			
	sub	BX, AX
	mov	CX, BX			; map until the last valid page
MapB1b:
	pop	BX
	pop	AX
	push	AX			; Save it
	call	MapPages
;
; Update next free logical page and reset CurrentPara if LIM
;
	pop	AX			; Get frame start
	cmp	AX, CS:[EEMSBlock].StartFrame
	jne	MapBNext
	mov	[EEMSSwapBlock].CurrentPara, AX
	mov	[EEMSSwapBlock].CurrentPage, BX	
;
MapBNext:
	pop	ES			; Restore needed context
	pop	DI
	pop	SI
;
cEnd		; MapBanks



;====== UnmapBanks ===========================================================
;
; Unmap raw pages unused in LIM space owned by SaveWinPID
;
; Entry:
;  DS:		Winoldap data
;
; Exit:
;  CurrentPage: next free pages owned by SaveWinPID
;
; Regs:
;  AX, BX. DX
;
;=============================================================================

UnmapBanks	proc	NEAR

	ccall	GetBankSeg, <[EEMSSwapBlock].CurrentPara>
;
; Determine number of unused raw pages to be unmap
; AX= physical page address where CurrentPara resides
;
	add	AX, CS:[EEMSBlock].PageSize ; Next physical page must be free
	mov	BX, CS:[EEMSBlock].Endframe
	sub	BX, AX 			; BX= total unused page size
	xor	DX, DX
	xchg	BX, AX
	div	CS:[EEMSBlock].PageSize	; AX= # unused pages
	sub	[EEMSSwapBlock].CurrentPage, AX ; Adjust index to free pages
	ret

UnmapBanks	endp



.xlist
subttl RWBank:	Read/Write to the LIM frame
.list
page
;====== RWBank  ===============================================================
;
; Given a start segment and the request size, this function reads/writes to
; the LIM frame starting from the [CurrentPara] segment. Automatic mapping
; to new logical pages is made depending on the requested size and the position
; of [CurrentPara] to the end of the LIM frame.
;
; Entry:
;
; Exit:
;   [CurrentPara]:	Next free space within LIM frame
;			= CS:[EndFrame]: LIM frame is dirty
;
; Regs:
;  AX,BX,CX
;
;============================================================================== 

cproc	RWBank, <PUBLIC,NEAR>

	ParmW	function		; fREAD or fWRITE function
	ParmW	StartSeg		; Segment to be read/written from/to
	ParmW	ReqSize			; Requested size in paras
	ParmW	EntryPID		; PID owner to logical pages to be mapped
	ParmW	InstanceData		; Segment where [EEMSSwapBlock] belongs to


cBegin
	push	SI			; Save needed context
	push	DI	
	push	DS
	push	ES
	
	mov	DS, InstanceData	; DS= Winoldap Instance data
	mov	AX, CS:[EEMSBlock].EndFrame
;
; Get number of paras to RW in Frame
;
	sub	AX, [EEMSSwapBlock].CurrentPara
                       			; Is LIM frame dirty ?
	jne	RWBankLoop		; No, go on
;
; Yes, get a fresh new LIM frame
;
	mov	AX, FRAME_SIZE
	save	<AX>
	ccall	MapBanks, <CS:[EEMSBlock].StartFrame, AX, EntryPID>
;
; Update current size
; CurrentPara index is updated by MapBanks
;
RWBankLoop:
	cmp	AX, ReqSize
	jbe	RWBank1
	mov	AX, ReqSize		; last move string

RWBank1:
;
; Write AX paras to frame
;
	call	GetWords       		; return BX,CX = size in words
	mov	ES, [EEMSSwapBlock].CurrentPara ; Assume WRITE, so setup regs
	mov	DS, StartSeg
	xor	DI, DI			
	xor	SI, SI
;
; Since AX is guaranteed to be <= 64Kb, BX is assumed to be 0
;
	cmp     function, BANK_WRITE
	je 	RWWrite
;
; Set up for READ operation
;
	mov 	BX, DS
	mov 	DX, ES			; Exchange source and destination
	mov	ES, BX
	mov	DS, DX

RWWrite:
	cld
	rep	movsw
	sub	ReqSize, AX		; get remaining paras to process
	jbe	RWBDone			; None left if sub-zero
;
	add	StartSeg, AX		; Bump up startseg
;
; Map new banks to LIM space
;
	mov	DS, InstanceData
	mov	AX, FRAME_SIZE
	save	<AX>
	ccall	MapBanks, <CS:[EEMSBlock].StartFrame, AX, EntryPID>
;
; CurrentPara is reset by MapBanks
;
; Assume that the LIM pageframe is 64Kb
;
	jmp	RWBankLoop

RWBDone:
	mov	DS, InstanceData
	add	[EEMSSwapBlock].CurrentPara, AX	; Update index to free LIM space
;
; if [CurrentPara] == [Endframe] then the LIM frame is dirty, new banks need
; to be mapped on the next call to RWBank. We cannot map it now because it
; might waste valuable pages in case RWBanks is not called again
;
	pop	ES			; Restore saved context
	pop	DS
	pop	DI
	pop	SI
cEnd 



;====== GetWords =============================================================
;
; Given a size in paras, returns it in words
;
; Entry:
;  AX:		Size in Paras
;
; Exit:
;  BX,CX:	dword size in words
;
; Regs:
;  BX,CX
;
;=============================================================================

GetWords	Proc	NEAR

	mov	CX, AX			; CX= Size in paras
	xor	BX, BX
	rcl	CX, 1
	rcl	BX, 1			; mutiply SizeParas by 8 to get size
	rcl	CX, 1			; in words
	rcl	BX, 1
	rcl	CX, 1
	rcl	BX, 1
	ret
GetWords	Endp

.xlist
Subttl	SaveContext: save the EEMS mapping context
.list
page
;====== SaveContext ==========================================================
;
; Save the EEMS mapping context in the given context array
;
; Entry:
;  ContextSeg:	Segment of the save context array
;
; Exit:
;  CY:		0: Success (AH=0)
;		1: Error   (AH= Error code)
; Regs:
;  AX
;
;=============================================================================

cproc	SaveContext, <PUBLIC, NEAR>
	parmW	ContextSeg 		; Seg ptr to the save context array
cBegin
	push	DI			; Save needed context
	push	ES
;
	xor	DI, DI
	mov	ES, ContextSeg
	push	CX			;!! bug in AST 061887
	EMSCall wEMM_CONTEXT_SAVE
	pop	CX			;!!
	or	AH, AH			; AH=0: success (CY=0)
	je	SCEnd 
	stc				; Error code in AH (CY=1)
SCEnd:
	pop	ES
	pop	DI
cEnd


.xlist
Subttl	RestoreContext: Restore the EEMS mapping context
.list
page
;====== RestoreContext =======================================================
;
; Restore the EEMS mapping context previously saved in the given context array
;
; Entry:
;  ContextSeg:	Segment of the save context array
;
; Exit:
;  CY:		0: Success (AH=0)
;		1: Error   (AH= Error code)
; Regs:
;  AX
;
;=============================================================================

cproc	RestoreContext, <PUBLIC, NEAR>
	parmW	ContextSeg 		; Seg ptr to the save context array
cBegin
	push	SI			; Save needed context
	push	DS
;
	xor	SI, SI
	mov	DS, ContextSeg
	push	CX			;!! bug in AST 061887
	EMSCall wEMM_CONTEXT_RESTORE
	pop	CX			;!!
	or	AH, AH			; AH=0: success (CY=0)
	je	RCEnd 
	stc				; Error code in AH (CY=1)
RCEnd:
	pop	DS
	pop	SI
cEnd


.xlist
subttl	GetBankSeg: Return physical page segment
.list
page
;====== GetBankSeg ===========================================================
;
; Get the physical page EEMS segment where AppSegment resides.
; AppSeg is assumed to be below 640K where EEMS segment mapping is linear
;	
; Entry:
;
;
; Exit:
;  CY:		0: Success, AX= EEMS segment
;			    BX= Physicalpage # above conventional memory (0 ref)
;		1: Fail, Cannot find any matching EEMS page segment
; Regs:
;  AX, BX, CX, DX
;
;=============================================================================

cproc	GetBankSeg, <PUBLIC, NEAR>
	ParmW	AppSegment		; Application segment
cBegin
	mov	AX, AppSegment
	mov	BX, CS:[EEMSBlock].LowestBankSeg
	cmp	AX, BX
	jb	GBEnd      		; CY set if not found
;
; Assume that EEMS segment mapping is linear below 640k
;
	xor	DX, DX
	sub	AX, BX			; AX= para offset to AppSegment from
					;     start of EEMS
	mov	CX, CS:[EEMSBlock].PageSize
	div	CX			; AX= number of pages above the
					;     conventional memory
	push	AX
	mul	CX                      ; AX= Para offset to physical page
					;     from the start of EEMS
	add	AX, BX			; AX= Physical page where Appseg resides
	pop	BX
	clc
GBEnd:
;
cEnd	; GetBankSeg


.xlist
subttl	RestorePages: Remap logical pages
.list
page
;====== RestorePages =========================================================
;
; Remap logical pages belonging either to PID0 or WoaPID (Winoldap) to the
; physical bank address derived from EntrySeg.
; 
; Process only if EEMS support for Winoldap is active (AppPID <> 0)
;
; Entry:
;  DS:		Winoldap DATA
;
; Exit:
;  CY:		0: Success: Pages mapped
;		1: Error:   EMS not supported || EntrySeg > 640kb
;  CurrentPage:	Starting logical raw page to map
;
; Regs:
;  AX,BX,CX,DX
;
;=============================================================================
;
cproc	RestorePages, <PUBLIC, NEAR>
	ParmW	EntrySeg		; Segment which bank is to be mapped
	ParmW	EntrySize		; Its size in paras
;
; Depending on the location of EntrySeg relative to the PageSize boundary,
; 2 logical pages may be needed to be mapped, assuming EntrySize <= PageSize.
;
cbegin
	push	SI			; Save useful context
	push	DI
	push	ES
;
	cmp	[EEMSSwapBlock].AppPID, 0
	stc
	je	RPEnd			; Only process if fEEMSWin active
	mov	CX, EntrySeg
	add	CX, EntrySize
	dec	CX			; CX= Last para in EntrySeg
	ccall	GetBankSeg, <CX>
	jc	RPEnd			; CY set: EntrySeg fully in Conv mem
					; ..so no processing needed
;
; Entry:
;  	AX= Physical bank containing EntrySeg last para
;	BX= Physical page number from conv memory
;
	push	BX
	push	BX
	ccall	GetBankSeg, <EntrySeg>
	pop	CX
	inc	CX			; include last page
	sub	CX, BX			; CX= number of pages to map
	xor	DX ,DX			; Assume PID0
;
	pop	DI
	cmp	DI, CS:[EEMSBlock].PID0Pages
	jb 	RPPID0  		; jump if EntrySeg belongs to PDI0  
;
; EntrySeg belongs to WOA PID. So find out its mapping
;
	ccall	MapWOAPID		; CY cleared by MapWOAPID
	jmp short RPEnd

RPPID0:	; Map the necessary logical banks
;
; Now:
;	AX= Starting physical page address
;	BX= Starting logical raw page
;	CX= Number of pages to map
;	DX= PID
;
; Setup Map array from stack
;
	call	MapPages
	clc
RPEnd:
	pop	ES			; Restore useful context
	pop	DI  
	pop	SI

cEnd	; RestorePages


page
;====== MapWOAPID ============================================================
;
; Get Winoldap mapping array and Map the relevant pages back.
;
; Entry:	
;  AX:          Physical segment to be mapped
;  CX:		number of pages to map
;  DS:		Winoldap Instance data (not necessarily current one)
;  [WOAContextSeg]
;  [WOAPID]
;  [WOAMapWords]
;
; Exit:
;  DX:		WOA PID
;  CurrentPage:	Logical page where EntrySeg resides
;
; Uses:
;  DX
;
;==============================================================================

cproc	MapWOAPID, <NEAR,PUBLIC>
	
cBegin
	push	AX			; Save needed contest
	push	BX	
	push	CX	
	push	SI
;
; Find the logical starting page in the original partition PID (WOAPID)
;

	sub	AX, CS:[EEMSBlock].BankLine
	xor	DX, DX
	div	CS:[EEMSBlock].PageSize	; AX= Offset to WOAContextSeg
	mov	DS, [EEMSSwapBlock].WOAContextSeg
	mov	SI, AX
	shl	SI, 1			; 
	shl	SI, 1			; 4 words per entry
	mov	DX, CS:[WOAPID]
;
	EMSCall	wEMM_MAP_PHYSICAL_PAGES
	push	SS			; Get back our DS
	pop	DS
;
	pop	SI
	pop	CX			; Restore context
	pop	BX	
	pop	AX	

cEnd	; MapWOAPID


page
;====== MapPages =============================================================
;
; Map physical pages by building the mapping array from stack
;
; Entry:
;  AX=		Starting physical page address
;  BX=		Starting logical raw page
;  CX=		Number of pages to map
;  DX= 		PID
;  DS:		might be <> Winoldap Data
;
; Exit:
;  BX:		Next logical page
;
; Regs:
;  AX,SI,DI,ES
;
;=============================================================================
      
MapPages	Proc	NEAR

	push	DS
;
 	mov	SI, SP
	mov	DI, CX
	shl	DI, 1			; four bytes per entry
	shl	DI, 1
	sub	SP, DI
	mov	DI, SP
	push	SI			; Save return stack
	push	CX			; Save # pages to map
;
	push	SS
	pop	ES			; ES:DI= Start of map array
	push	DI			; Save it
;
	cld
RPLoop:					; Build map array
	xchg	AX, BX			; logical page entry
	stosw
	xchg	AX, BX			; physical address entry
	stosw
	add	AX, CS:[EEMSBlock].PageSize ;Bump to next physical address and
	inc	BX			; ..logical raw page
	loop	RPLoop
;
	pop	SI			; Start of map array
	pop	CX			; # pages to map
;
	push	ES
	pop	DS
	EMSCall	wEMM_MAP_PHYSICAL_PAGES
;
	pop	SI			; Saved stack 
	mov	SP, SI
;
	pop	DS
	ret

MapPages	Endp

sEnd	CODE




END
