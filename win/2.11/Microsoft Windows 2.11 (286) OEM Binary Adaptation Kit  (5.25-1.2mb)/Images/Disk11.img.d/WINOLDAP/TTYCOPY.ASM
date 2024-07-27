page  , 132
title	**	COPY operation for Old Applications: TTYHP
	.xlist
	include cmacros.inc
	include tty.inc
	include pdb.inc
	include copyeq.inc
	include grabber.inc
ifdef MOUSE
	include mouse.inc
endif

	.list


;!!HP	 MCP

;------  COPY  ---------------------------------------------------------------
;
;  This module contains:
;
;  1)  Code to install and process a COPY operation.
;      The COPY operation can be only invoked by a BAD old Application.
;      It uses its own keyboard hardware interrupt service routine (INT 09)
;
;  2)  The code to manage the screen area marked by a COPY operation
;
;  Programmer:	Tuan Nguyen, Ralph Peterson
;
;  Date:	05/01/86
;
;------------------------------------------------------------------------------

page

;-------  Cursor movement equates  ---------------------------------------------
;
UpArrowEQU	equ	1
LeftArrowEQU	equ	2
RightArrowEQU	equ	4
DownArrowEQU	equ	8

UP_ARROW	equ	048H
DOWN_ARROW	equ	050H
RIGHT_ARROW	equ	04DH
LEFT_ARROW	equ	04BH

;
;  The COPY area shares the same memory allocation with the Buffer segment
;  Both data structures are compatible and they are distinct by the Type field
;  The COPY area will only be moved to the WINDOWS Clipboard following a
;  context switch or an exit of the current BAD application.
;
CopyStruc	Struc		; Data structure of COPY saved area

CopyType	dw	?	;
CopySize	dw	?	; Size of Saved area in bytes
CopyStart	dw	?	; COPY area starts here

CopyStruc	Ends
;
CF_TEXT 	equ	1	; Clipboard contents is in text mode
;
;------------------------------------------------------------------------------


externFP	<GetCurrentTask>
externNP	<SwallowKey, DoBeep, BadBeginPaint, BadEndPaint>
ifdef MOUSE
externNP	<SaveMouse, RestoreMouse>
ExternNP	<GetMouseStatus, SetMouse>
externNP	<PrepMouse, ReleaseMouse>
externNP	<SetSelectText, ResetWinText,GoGetGrab>
endif
page

externB 	fSelect

sBegin	data
externW 	BufSeg
externD 	BufSize

; ~~RCP 041587
EXTERNB 	GrabData

sEnd	data


sBegin		code
		assumes cs,code
		assumes ds,data

	public	EnableMark, DisableMark, ProcessMark
	public	SetCCPMode, RestoreCCPMode
	public	RestoreMark, DoCopy, DoMark
	public	CheckInput, GetInput
	public	UpdateMark, AdjustMark, ProcessKey, CheckGrabVersion
	public	GetGrabInfo, GoMarkGrab, CheckShiftState
	public	SwitchMark, PositiveRect, GetExtents, CheckOpposites
	public	ExclusiveHorz, ExclusiveVert, ExclusiveCorner
	public	CheckForCorner, MarkArea



	ifdef MOUSE
	public	GetMarkMouseAction
	public	ButtonChange, MouseMove
	endif
ExternB 	fHPGrab
externB 	fVectra 	;1 = Vectra PC


ExternNP	GoGrab

EXTERNB 	GrabReq

GLOBALD 	OldapInt09,	?
GLOBALW 	fBeginBlock,	0	; Block Started  (Anchored)
GLOBALW 	fEndBlock,	1	; Block Terminated or No Block
GLOBALW 	fMark,		0	; =1: Area is marked
GLOBALW 	fMarkAbort,	0	; =1: Mark is to be aborted
OldCCP		dw	0		; save old CCP mapping here
OldHPENTRY	dd	0		; HPENTRY ISR address
BeginBlock	Label	word		; <> 0: marks the screen coordinates
GlobalB 	BeginCol,0		; where the COPY area starts
GlobalB 	BeginRow,0
EndBlock	Label	word		; <> 0: Marks the screen coordinates
GlobalB 	EndCol,0		; where the COPY area ends
GlobalB 	EndRow,0

ifdef	MOUSE
MousePos	label	word
GlobalB 	MouseCol,0
GlobalB 	MouseRow,0
externB 	MouseButton
externW 	MouseInit
endif


;--------------------------------------------------------------------------------

.xlist
page
.list
;======== DoMark =============================================================
;
;	Description:
;		This routine supervises the copy operation.
;
;	Entry:
;
;	Exit:
;		Screen area marked any variables updated
;		CF: = 1 if error
;	Modifies:
;		All but DS
;
;=============================================================================
cProc	DoMark,<NEAR, PUBLIC>
	parmW	MarkFlags
	parmW	UpperLeft
	parmW	MarkExtents

cBegin	DoMark

	cmp	CS:[fMark],0		 ;are we already marking or a mark area
	je	DoNormalMark
	cmp	CS:fEndBlock,0
	je	WeAreInteractive
	call	RestoreMark		;clear current marked area
	mov	CS:[fMark],0
	jmp	DoNormalMark


WeAreInterActive:
	mov	AX,MarkFlags
	test	AX,1			;is it interactive
	jz	InterActiveToRect
	call	RestoreMark
	mov	CS:fBeginBlock,1
	mov	CS:EndBlock,0
	mov	CS:BeginBlock,0
	call	RestoreMark		;Mark the home position
	jmp	DoMarkEnd

InterActiveToRect:
	call	RestoreMark		;clear current selection
	call	DisableMark
	jmp	DoMarkAuto

DoNormalMark:
	jc	DoMarkEnd
	mov	AX,MarkFlags
	test	AX,1			;is it interactive
	jz	DoMarkAuto
	call	EnableMark
	jc	DoMarkEnd
	call	ProcessMark
	call	DisableMark
	jmp	DoMarkEnd
DoMarkAuto:
	call	CheckGrabVersion
	jc	DoMarkEnd
	call	BadBeginPaint
	call	GetGrabInfo
	jc	DoMarkEnd
	mov	AX,UpperLeft
	mov	CX,MarkExtents
	or	CX,CX			;is whole screen
	jnz	DoMarkPart
	mov	CL,GrabData.giCharsX
	mov	CH,GrabData.giCharsY
	xor	AX,AX
DoMarkPart:
	lea	DI,GrabReq
	push	CS
	pop	ES
	mov	ES:[DI].grXorg,AL
	mov	ES:[DI].grYorg,AH
	mov	ES:[DI].grXext,CL
	mov	ES:[DI].grYext,CH
	push	AX
	call	GoMarkGrab		;mark the area
	pushf				;need to save carry flag
	call	BadEndPaint
	popf
	pop	AX
	jc	DoMarkEnd
	mov	CS:[BeginBlock],AX
	add	AL,CL
	add	AH,CH
	dec	AL			;make zero based
	dec	AH
	mov	CS:[EndBlock],AX
	mov	CS:fBeginBlock,1
	mov	CS:fEndBlock,1
	mov	CS:fMark,1
DoMarkEnd:

cEnd	DoMark


;================= EnableMark ===========================================
;
;	DESCRIPTION
;		This routine does all initialization for the marking
;		process.
;
;	ENTRY
;
;	EXIT
;		CF: = 0 if Mark is enabled
;		CF: = 1 if Mark not enabled (error)
;
;	MODIFIES
;		Standard C
;========================================================================
EnableMark	PROC	NEAR

	call	CheckGrabVersion
	jc	EMRet			;if error carry already set
	call	BadBeginPaint		;notify the grabber
	call	GetGrabInfo		;Update display variables
	jc	EMRet
	cmp	CS:fVectra,0
	jz	EM10
	call	SetCCPMode

EM10:

	ifdef MOUSE

	call	PrepMouse
	test	CS:MouseInit,0FFH
	jz	EM20
	call	GetMouseStatus
	mov	CS:MousePos,DX
	mov	CS:MouseButton,BL
EM20:
	endif
	mov	CS:fMark,1
	mov	CS:fMarkAbort,0
	mov	CS:fBeginBlock,1
	mov	CS:fEndBlock,0
	mov	CS:BeginBlock,0
	mov	CS:EndBlock,0
	call	RestoreMark			;mark the initial position
	call	SetSelectText
	mov	[fSelect], FS_ACTIVE	;
EMRet:
	ret
EnableMark	ENDP

;================== DisableMark ==========================================
;
;	DESCRIPTION
;		This routine resets any conditions set while marking to their
;		Original States
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;		Standard C
;==========================================================================
DisableMark	PROC	NEAR

	mov	CS:fEndBlock,1
	call	BadEndPaint
	cmp	CS:fVectra,0
	jz	DM10
	call	RestoreCCPMode
DM10:

	ifdef MOUSE

	call	ReleaseMouse
DM20:

	endif
	push	si
	push	di

	call	ResetSelectText
	pop	di
	pop	si

	ret

DisableMark	ENDP

;================= ResetSelectText ======================================
;
;	DESCRIPTION
;		This routine resets the select text flag and resets
;		the applications text if necessary.
;
;	ENTRY
;
;	EXIT
;		fSelect = 0
;
;	MODIFIES
;		All but DS,SS,BP
;
;=========================================================================
ResetSelectText:
	xor	AL,AL
	xchg	AL,[fSelect]
	or	AL,AL
	jz	RSTRet
	call	ResetWinText
RSTRet:
	ret


;================= ProcessMark ===========================================
;
;	DESCRIPTION
;		This routine takes Mouse and Keyboard movements and
;		processes them by updating the appropriate variables
;		and updating the screen.
;
;	ENTRY
;		System is set up for Marking (i.e. EnableMark
;		has been called)
;
;	EXIT
;
;	MODIFIES
;
;=========================================================================
ProcessMark	PROC	NEAR

PMLoop:
	call	GetInput			;get any Key if any
	jz	PML1
	call	ProcessKey
PML1:
	cmp	CS:fMarkAbort,0
	jnz	PMAbort
	cmp	CS:fEndBlock,0
	jnz	PMDone


	ifdef	MOUSE
	mov	CX,CS:MousePos
	mov	BL,CS:MouseButton		;get current mouse button
	call	GetMarkMouseAction
	jc	PMLoop				; No mouse
	mov	AX,CS:MousePos
	xor	BL,CS:MouseButton		;has button changed states
	jnz	ButtonChange
	mov	BL,CS:MouseButton
	cmp	CX,AX				;has mouse moved
	je	PMLoop

MouseMove:
	call	UpdateMark			;process the movement
	jmp	PMLoop

ButtonChange:
	mov	BL,CS:MouseButton
	or	BL,BL				;has a button been pressed
	jz	MouseMove			;just track
	mov	CX,CS:fBeginBlock		;get old state
	mov	CS:fBeginBlock,1
	push	AX
	call	CheckShiftState
	pop	AX
	jnz	MouseMove
	or	CX,CX				;Were we anchored
	jz	PMLoop

BeginNewMark:
	mov	BX,AX
	call	AdjustMark

	endif

	jmp	PMLoop				;here if no MOUSE defined


PMAbort:
	call	RestoreMark
	mov	CS:fMark,0
	mov	CS:fMarkAbort,0
PMDone:
	ret

ProcessMark	ENDP



;=============== UpdateMark =============================================
;
;	DESCRIPTION
;		This routine updates the position of the cursor on
;		the screen.
;
;	ENTRY
;		AX: = Position of new cursor
;		BL: = Virtual button status
;
;	EXIT
;
;	MODIFIES
;		BX
;==========================================================================
UpdateMark	PROC	NEAR
	or	BL,BL				;is the mouse down
	jz	UMDone
	cmp	CS:fBeginBlock,0
	jz	UMDone				;no action
	mov	BX,CS:BeginBlock		;get anchor
	call	AdjustMark
UMDone:
	ret

UpdateMark	ENDP



;================= SwitchMark ============================================
;
;	DESCRIPTION
;		This routine adjusts the current mark on the screen to
;		be equavalent to the new coordinates passed.
;
;	ENTRY
;		AH: = New Row EndPoint
;		AL: = New Col EndPoint
;		BH: = New Row Anchor
;		BL: = New Col Anchor
;		BeginBlock and EndBlock are valid
;
;	EXIT
;		BeginBlock: = New Anchor position
;		EndBlock: = New End Position
;
;	MODIFIES
;		none
;
;	NOTES
;		This routine is currnetly a quick non-optimized
;		routine which will be optimized after functionality
;		of the module is completed.
;
;========================================================================
SwitchMark	PROC	NEAR

	call	RestoreMark		;UnMark current area
	mov	CS:BeginBlock,BX
	mov	CS:EndBlock,AX
	call	RestoreMark		;MarkNewArea
	ret

SwitchMark	ENDP



;================= RestoreMark ============================================
;
;	Description:
;		This routine rstores the marked area of the screen to
;		it's original state.
;
;	Entry:
;
;	Exit:
;
;	Modifies:
;		none
;
;==========================================================================
RestoreMark  PROC    NEAR

	push	ES
	push	DI
	push	AX
	push	CX
	mov	AX,CS
	mov	ES,AX
	lea	DI,GrabReq
	mov	CX,CS:[EndBlock]       ;ending coord
	test	CS:[fBeginBlock],0FFFFH
	jnz	RS0
	mov	CS:[BeginBlock],CX
RS0:
	mov	AX,CS:[BeginBlock]	;starting coord
	cmp	AX,CX
	jbe	RS1
	xchg	CX,AX
RS1:					;AX is upper left CX is lower right
	cmp	CL,AL
	ja	RS2
	xchg	CL,AL
RS2:
	sub	CL,AL
	inc	CL
	sub	CH,AH
	inc	CH			;AX is origin, CX is extents
	mov	ES:[DI].grXorg,AL
	mov	ES:[DI].grYorg,AH
	mov	ES:[DI].grXext,CL
	mov	ES:[DI].grYext,CH
	call	GoMarkGrab		;restore the attributes
	pop	CX
	pop	AX
	pop	DI
	pop	ES
	ret

RestoreMark	ENDP

	ifdef	MOUSE

;================= GetMarkMouseAction ==================================
;
;	DESCRIPTION
;		This routine gets any mouse action that is pertinent
;		to marking.
;
;	ENTRY
;
;	EXIT
;		CF: = 0 if Mouse available
;		CF: = 1 if no mouse
;		CS:MousePos updated
;		CS:MouseButton updated
;	MODIFIES
;
;========================================================================
GetMarkMouseAction	PROC	NEAR

	push	AX
	push	BX
	push	CX
	push	DX
	test	CS:MouseInit,0FFH
	jz	GMMAError
	call	GetMouseStatus
	and	BX,01H
	mov	CS:MouseButton,BL
	mov	CS:MousePos,DX
	clc
	jmp	GMMARet
GMMAError:
	stc
GMMARet:
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	ret

GetMarkMouseAction	ENDP
	endif



;==================== CheckShiftState ===================================
;
;	DESCRIPTION
;		This routine gets the current shift state.
;
;	ENTRY
;
;	EXIT
;		AX: = Shift State
;		ZF: = 1 if no shift keys down
;		ZF: = 0 if a shift key is down
;
;	MODIFIES
;		AX
;
;=========================================================================
CheckShiftState PROC	NEAR
	mov	AH,2
	int	16H
	and	AX,03				;only look at shift keys
	ret

CheckShiftState ENDP


;==================== GetInput ==========================================
;
;	DESCRIPTION
;		This routine gets keystrokes for the mark operation
;		It returns if no keys are pending.
;
;	ENTRY
;
;
;	EXIT
;		ZF: = 1 if no key
;		ZF: = 0 if there is a key
;		AX: = ScanCode, ASCII value of key if ZF: = 0
;
;	MODIFIES
;		AX
;
;=========================================================================
GetInput	PROC	NEAR
	mov	AH,1
	int	16H
	jz	GIRet
	mov	AH,0
	int	16H
GIRet:
	ret

GetInput	ENDP

;================CheckInput================================================
;
;	DESCRIPTION
;		Calls Int 16 to see if there is a key.
;
;===========================================================================
CheckInput	PROC	NEAR

	mov	AH,1
	int	16H
	ret

CheckInput	ENDP


;================== ProcessKey ===========================================
;
;	DESCRIPTION
;		This routine accepts a keycode and sets the mark variables
;		accordingly.
;
;	ENTRY
;		AX: = Int 16H version of key
;
;	EXIT
;		Mark variables updated.
;
;	MODIFIES
;
;=========================================================================
ProcessKey	PROC	NEAR


	mov	CX,CS:EndBlock
	mov	DH,GrabData.giCharsY
	mov	DL,GrabData.giCharsX
	dec	DH				;zero based
	dec	DL
	push	AX
	call	CheckShiftState
	mov	BX,AX
	pop	AX
	cmp	AH,01H
	jz	PKEsc
	cmp	AH,1CH
	jz	PKEnter
	cmp	AH,48H
	jz	PKUp
	cmp	AH,4BH
	jz	PKLeft
	cmp	AH,4DH
	jz	PKRight
	cmp	AH,50H
	jz	PKDown
	cmp	AH,60H
	jz	PKUp
	cmp	AH,61H
	jz	PKLeft
	cmp	AH,62H
	jz	PKDown
	cmp	AH,63H
	jz	PKRight
	jmp	PKRet

PKEsc:
	mov	CS:fMarkAbort,1
	jmp	PKRet

PKEnter:
	mov	CS:fEndBlock,1
	jmp	PKRet

PKUp:
	cmp	CH,0
	jle	PKRet
	dec	CH
	jmp	PKUpdate

PKDown:
	cmp	CH,DH
	jae	PKRet
	inc	CH
	jmp	PKUpdate

PKLeft:
	cmp	CL,0
	jle	PKRet
	dec	CL
	jmp	PKUpdate


PKRight:
	cmp	CL,DL
	jae	PKRet
	inc	CL
	jmp	PKUpdate


PKUpdate:
	or	BX,BX				;shift key down
	jnz	PKU10
	mov	AX,CX
	mov	BX,CX
	jmp	PKU20
PKU10:
	mov	AX,CX
	mov	BX,CS:BeginBlock
PKU20:
	call	AdjustMark
PKRet:
	ret

ProcessKey	ENDP


;==================== CheckGrabVersion ===================================
;
;	DESCRIPTION
;		This routine checks to see if the grabber is an HP
;		grabber with a version greater than 2.0.
;
;	ENTRY
;
;	EXIT
;		CF: = 0 if Grabber is OK
;		CF: = 1 if Grabber cannot support Marking
;
;	MODIFIES
;		AX
;
;==========================================================================
CheckGrabVersion	PROC	NEAR
	push	SI
	cmp	CS:fHPGrab,0
	jz	CGVAbort
	mov	SI,InquireGrab
	mov	AX,GRAB_GETVERSION
	call	GoGrab
	cmp	AH,2
	jl	CGVAbort
	cmp	AL,1
	jl	CGVAbort
	clc
	jmp	CGVRet
CGVABORT:
	stc
CGVRet:
	pop	SI
	ret

CheckGrabVersion	ENDP


;==================== GetGrabInfo ========================================
;
;	DESCRIPTION
;		This routine calls the grabber to get screen info.
;
;	ENTRY
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if failed
;
;	MODIFIES
;		AX, BX, ES
;
;=========================================================================
GetGrabInfo	PROC	NEAR
	push	SI
	push	DI
	mov	AX,DS
	mov	ES,AX
	lea	DI, GrabData
	push	DI
	push	ES
	mov	AX,GRAB_GETINFO
	mov	SI,InquireGrab
	call	GoGrab			;make call to the grabber
	pop	ES			;to get what we know about display
	pop	DI
	or	AX,AX			;fail in graphics mode
	jz	GGIFail
	clc
	jmp	GGIRet
GGIFail:
	stc
GGIRet:
	pop	DI
	pop	SI
	ret

GetGrabInfo	ENDP


;==============GoMarkGrab===========================================
;
;	Description:
;		Calls go grab to mark an area on the screen
;
;	Entry:
;		ES:DI -> Valid GrabRequest structure
;
;	Exit:
;		AX: = error code if CF: = 1
;
;	Modifies:
;		AX
;
;==================================================================
GoMarkGrab	PROC	NEAR
	push	ES
	push	DI
	push	SI

	ifdef MOUSE
	test	CS:MouseInit,0FFH
	jz	GMG10
	mov	AX,SM_HIDE_CURSOR
	call	SetMouse
GMG10:
	endif

	mov	AX,GRAB_MARKBLOCK
	mov	SI,InquireGrab
	call	GoGrab

	ifdef MOUSE
	test	CS:MouseInit,0FFH
	jz	GMG20
	mov	AX,SM_SHOW_CURSOR
	call	SetMouse
GMG20:
	endif

	pop	SI
	pop	DI
	pop	ES
	ret

GoMarkGrab	ENDP



page
;============= DoCopy ===================================================
;
;	Description:
;		Captures data in copy buffer between marks and copies
;		it to the grabber buffer
;
;	Entry:
;		DS = winoldap data segment
;
;	Exit:
;		Screen restored
;		Grabber buffer updated
;
;	Modifies:
;		All except DS
;
;=========================================================================

cProc	DoCopy,<NEAR, PUBLIC>

cBegin	DoCopy
	cmp	CS:fMark,0
	jnz	RB0
	call	DoBeep
	jmp	RBRet
RB0:
	mov	AX,CS
	mov	ES,AX
	lea	DI,GrabReq
	xor	AX,AX			;get a zero
	mov	word ptr ES:[DI].grLpData+2,AX
	mov	word ptr ES:[DI].grLpData,AX
	mov	CX,CS:[EndBlock]       ;ending coord
	test	CS:[fBeginBlock],0FFFFH
	jnz	RB0a
	mov	CS:[BeginBlock],CX
RB0a:
	mov	AX,CS:[BeginBlock]	;starting coord
	cmp	AX,CX
	jbe	RB1
	xchg	CX,AX
RB1:					;AX is upper left CX is lower right
	cmp	CL,AL
	ja	RB2
	xchg	CL,AL
RB2:
	sub	CL,AL
	inc	CL
	sub	CH,AH
	inc	CH			;AX is origin, CX is extents
	mov	ES:[DI].grXorg,AL
	mov	ES:[DI].grYorg,AH
	mov	ES:[DI].grXext,CL
	mov	ES:[DI].grYext,CH
	mov	ES:[DI].grStyle,FMT_OTHER+C_CHAR
	call	GoGetGrab		   ;get the size (includes header)

	mov	BX,word ptr [BufSize+2]
	or	BX,BX
	jnz	BufOK			; OK if greater than 64K
	mov	BX,word ptr [BufSize]
	cmp	BX,AX
	jb	RBErr
BufOK:
	mov	AX,[BufSeg]
	mov	word ptr ES:[DI].grLpData+2,AX
	mov	AX,CopyStart
	mov	word ptr ES:[DI].grLpData,0
	call	GoGetGrab
	mov	ES,[BufSeg]

  ; We don't need to do the next three instructions since the grabber does
  ;   them for us.

;	xor	DI,DI
;	mov	word ptr ES:[DI].CopyType,1	;text type
;	mov	ES:[DI].CopySize,AX

	cmp	CS:[fEndBlock],0		;are we still in mark mode
	jnz	RB10
	mov	CS:[fEndBlock],1
	mov	CS:[fMarkAbort],1
	jmp	RBRet
RB10:
	mov	CS:fMark,0			;show that nothing is marked
	push	AX				;save byte count
	call	RestoreMark
	call	BadEndPaint			;release screen
	pop	AX
	jmp	RBRet
RBErr:
	xor	AX,AX			;return 0 chars copied
RBRet:

cEnd	DoCopy



;============SetCCPMode====================================================
;
;	Description:
;		This routine saves the old CCP mode and sets the new mode
;		to emulate the num - cursor pad.
;
;	Entry:
;
;	Exit:
;
;	Modified:
;		none
;
;===========================================================================

SetCCPMode	PROC	NEAR

	push	DS
	push	DI
	push	AX
	xor	AX,AX
	mov	DS,AX
	mov	DI,HPENTRY*4+2
	mov	AX,DS:[DI]		;get the code segment
	mov	DS,AX
	mov	DI,V_SPCCP		;get offset to num pad vectors
	add	DI,4			;point to DS
	mov	AX,DS:[DI]
	mov	DS,AX
	mov	DI,DH_V_PARENT		;point to parent
	mov	AX,DS:[DI]
	mov	CS:[OldCCP],AX
	mov	AX,V_LCCP_NUMPAD	;get vector for num pad xlator
	mov	word ptr DS:[DI],AX	;save it
	pop	AX
	pop	DI
	pop	DS
	ret

SetCCPMode	ENDP


;================RestoreCCPMode=======================================
;
;	Description:
;		Restores the CCP to it's original translator mode
;
;	Entry:
;
;	Exit:
;
;	Modified:
;		none
;
;=======================================================================

RestoreCCPMode	PROC	NEAR

	push	DS
	push	DI
	push	AX
	xor	AX,AX
	mov	DS,AX
	mov	DI,HPENTRY*4+2
	mov	AX,DS:[DI]		;get the code segment
	mov	DS,AX
	mov	DI,V_SPCCP		;get offset to num pad vectors
	add	DI,4			;point to DS
	mov	AX,DS:[DI]
	mov	DS,AX
	mov	DI,DH_V_PARENT		;point to parent
	mov	AX,CS:[OldCCP]		;get vector to replace
	mov	word ptr DS:[DI],AX	;save it
	pop	AX
	pop	DI
	pop	DS
	ret

RestoreCCPMode	ENDP

;=========================================================================
;=========================================================================
;
; The following routines are used for determining the delta of an old
; marked area and a new mark area.  It can be used by both keyboard
; and mouse routines.
;
;=========================================================================
;=========================================================================


;==============AdjustMark=================================================
;
;	DESCRIPTION
;		This routine adjusts the current mark on the screen to
;		be equavalent to the new coordinates passed.
;
;	ENTRY
;		AH: = New Row EndPoint
;		AL: = New Col EndPoint
;		BH: = New Row Anchor
;		BL: = New Col Anchor
;		BeginBlock and EndBlock are valid
;
;	EXIT
;		BeginBlock: = New Anchor position
;		EndBlock: = New End Position
;
;	MODIFIES
;		none
;
;========================================================================
AdjustMark     PROC    NEAR

	push	AX
	push	BX
	push	CX
	push	DX
	cmp	BX,CS:BeginBlock
	jz	AdjM10
	call	SwitchMark
	jmp	AdjMDone
AdjM10: 				;Anchor is the same at this point
	mov	DX,AX			;save this just in case
	call	GetExtents
	mov	CX,AX			;save the extents
	mov	AX,CS:EndBlock
	call	GetExtents
	push	BX			;save the Anchor
	mov	BX,CX
	call	CheckOpposites		;Did the Mark change directions
	pop	BX
	jz	AdjM20			;jump if no opposing extents
	mov	AX,DX			;get the coordinates back
	call	SwitchMark
	jmp	AdjMDone
AdjM20:
	;
	; At this point we have Anchor in BX, New extents in CX and
	; old extents in AX.
	;
	call	ExclusiveHorz
	call	ExclusiveVert
	call	CheckForCorner
	jnz	AdjM30
	call	ExclusiveCorner
AdjM30:
	mov	CS:EndBlock,DX		;update the endblock
AdjMDone:
	pop	DX
	pop	CX
	pop	BX
	pop	AX
	ret



AdjustMark     ENDP

;===========MarkArea======================================================
;
;	DESCROPTION
;		This routine calls MarkBlock with the coordinates given.
;		The coordinates and extents are adjusted if need be.
;
;	ENTRY
;		BX: = Anchor
;		AX: = Extents
;
;	EXIT
;
;	MODIFIES
;
;=========================================================================
MarkArea	PROC	NEAR

	push	AX
	push	BX
	push	CX
	push	DI
	push	ES
	call	Positiverect		;convert to positive extents
	mov	CX,CS
	mov	ES,CX
	lea	DI,GrabReq
	mov	ES:[DI].grXorg,BL
	mov	ES:[DI].grYorg,BH
	mov	ES:[DI].grXext,AL
	mov	ES:[DI].grYext,AH
	call	GoMarkGrab
	pop	ES
	pop	DI
	pop	CX
	pop	BX
	pop	AX
	ret
MarkArea	ENDP




;===========ExclusiveHorz=================================================
;
;	DESCRIPTION
;		This routine marks or unmarks as the case may be
;		any area beyond the first horizontal extent up to
;		the second horizontal extent.
;
;	ENTRY
;		AX: = Extents of first rect
;		BX: = Anchor for both rects
;		CX: = Extents of second rect
;
;	EXIT
;
;	MODIFIES
;		none
;
;	NOTES
;		This routine assumes that both extents are in the same
;		direction.  i.e. both have the same sign.
;
;==========================================================================
ExclusiveHorz	PROC	NEAR

	push	AX
	push	BX
	push	CX
	cmp	AL,CL
	jz	ExHzDone
	;
	; See exclusiveVert for an explanation of the following three
	; lines of code.
	;
	test	AL,80H
	jz	ExHz10
	inc	BL
ExHz10:
	add	BL,AL
	sub	CL,AL
	;
	; We now have Horizontal extents and new horz origin
	;
	test	AH,80H			;is it negative (up)
	jnz	ExHz30
	cmp	CH,AH
	jle	ExHz40
	xchg	CH,AH
	jmp	ExHz40
ExHz30:
	inc	BH			;Same reason as above
	cmp	CH,AH
	jge	ExHz40
	xchg	CH,AH
ExHz40:
	;
	; We now have an Anchor and extents (in CX) of the ExHorz rectangle
	;
	mov	AX,CX			;get extents in AX
	call	MarkArea
ExHzDone:
	pop	CX
	pop	BX
	pop	AX
	ret
ExclusiveHorz	ENDP


;==============ExclusiveVert=============================================
;
;	DESCRIPTION
;		This routine marks or unmarks as the case may be
;		any area beyond the first vertical extent up to
;		the second vertical extent.
;
;	ENTRY
;		AX: = Extents of first rect
;		BX: = Anchor for both rects
;		CX: = Extents of second rect
;
;	EXIT
;
;	MODIFIES
;		none
;
;	NOTES
;		This routine assumes that both extents are in the same
;		direction.  i.e. both have the same sign.
;
;==========================================================================
ExclusiveVert	PROC	NEAR

	push	AX
	push	BX
	push	CX
	cmp	AH,CH
	jz	ExVtDone

	;
	; The following lines determine if we are above or below the anchor
	; This is important since there is no Zero extent.
	; Example:
	;   In the following  A=Anchor	 X=Marked cell
	;
	;   AXXXXX	     has extents 6,1
	;
	;   AXXXXX
	;   XXXXXX	     has extents 6,2
	;
	;   XXXXXX
	;   AXXXXX	     Has extents 6,-1
	;
	;   This means anything with a negative extent will be modified
	;   incorrectly.  Example:
	;
	;   Current rect has extents   5,-2
	;   New rect has extents       5,-3
	;   Anchor is 10,10
	;
	;   Just looking in the negative extents, The line to be marked
	;   would be	10 + (-2) = 8 with extent of  -3 - (-2) = -1
	;   This would mark the line above the one we want since these
	;   coordinates are the same as marking line 7 with extent of 1.
	;
	; In this case the Anchor is adjusted so the correct rect will be
	; marked
	;
	test	AH,80H			;are we above the Anchor
	jz	ExVt10
	inc	BH
ExVt10:
	add	BH,AH
	sub	CH,AH
	;
	; We now have Vertical extents and new Vert origin
	;
	test	AL,80H			;is it negative (left)
	jnz	ExVt30
	cmp	CL,AL
	jle	ExVt40
	xchg	CL,AL
	jmp	ExVt40
ExVt30:
	inc	BL			;same reason as above
	cmp	CL,AL
	jge	ExVt40
	xchg	CL,AL
ExVt40:
	;
	; We now have an Anchor and extents (in CX) of the ExVert rectangle
	;
	mov	AX,CX			;get extents in AX
	call	MarkArea
ExVtDone:
	pop	CX
	pop	BX
	pop	AX
	ret
ExclusiveVert	ENDP



;==============ExclusiveCorner===========================================
;
;	DESCRIPTION
;		This routine is similar ExclusiveHorz and ExclusiveVert
;		except that it obtains the intersection of those
;		results.
;
;	ENTRY
;		AX: = Extents of first rect
;		BX: = Anchor for both rects
;		CX: = Extents of second rect
;
;	EXIT
;
;	MODIFIES
;		none
;
;	NOTES
;		This routine assumes that both extents are in the same
;		direction.  i.e. both have the same sign.
;
;==========================================================================
ExclusiveCorner PROC	NEAR

	push	AX
	push	BX
	push	CX
	test	AH,80H			;negative
	jz	ExCr10
	inc	BH
ExCr10:
	test	AL,80H
	jz	ExCr20
	inc	BL
ExCr20:
	add	BH,AH
	sub	CH,AH
	add	BL,AL
	sub	CL,AL
	mov	AX,CX
	call	MarkArea
	pop	CX
	pop	BX
	pop	AX
	ret
ExclusiveCorner        ENDP


;===============CheckForCorner===========================================
;
;	DESCRIPTION
;		This routine checks to see if the exclusive corner must
;		be remarked.  This is true only if both extents have
;		grown or both extents have shrunk.
;
;	ENTRY
;		AX: = extents of rect1
;		CX: = extents of rect2
;		rects are assumed to have same origin
;
;	EXIT
;		ZF: = 1 if needs remarking
;		ZF: = 0 if does not need remarking
;
;	MODIFIES
;		none
;
;========================================================================
CheckForCorner	PROC	NEAR
	push	AX
	push	CX
	cmp	AH,CH
	jz	CFCNo
	cmp	AL,CL
	jz	CFCNo
	sub	AH,CH
	test	CH,80H			;negative
	jz	CFC10
	neg	AH
CFC10:
	sub	AL,CL
	test	CL,80H
	jz	CFC20
	neg	AL
CFC20:
	and	AX,8080H
	xor	AH,AL
	jmp	CFCDone
CFCNo:
	mov	AX,1
	or	AX,AX
CFCDone:
	pop	CX
	pop	AX
	ret
CheckForCorner	ENDP


;==============PositiveRect===============================================
;
;	DESCRIPTION
;		This routine takes a rectangle that may contain
;		positive and/or negative extents and converts it
;		to positive extents.  This may be done by changing
;		the given corner coordinates.
;
;	ENTRY
;		AX: = Extents  (BH = Vert, BL = Horz)
;		BX: = Corner coordinates  (AH = Vert, AL = Horz)
;
;	EXIT
;		AX: = Extents (positivve)
;		BX: = New Corner coordinates
;
;	MODIFIES
;		AX, BX
;
;==========================================================================
PositiveRect	PROC	NEAR

	cmp	AH,0
	jge	heightOK
	add	BH,AH
	neg	AH
heightOK:
	cmp	AL,0
	jge	widthOK
	add	BL,AL
	neg	AL
widthOK:
	ret

PositiveRect	ENDP

;===============GetExtents================================================
;
;	DESCRIPTION
;		This routine converts a rectangle defined by two points
;		to a rectangle defined by one point and positive/negative
;		extents.
;
;	ENTRY
;		AX: = Point two
;		BX: = Point one (Anchor)
;
;	EXIT
;		AX: = extents
;		BX: = Point one (unchanged)
;
;	MODIFIES
;		AX
;
;=========================================================================
GetExtents	PROC	NEAR

	push	CX
	xor	CX,CX
	inc	CH
	inc	CL
	sub	AH,BH
	jns	GE10
	neg	CH
GE10:
	sub	AL,BL
	jns	GE20
	neg	CL
GE20:
	add	AH,CH
	add	AL,CL
	pop	CX
	ret

GetExtents	ENDP


;============CheckOpposites=============================================
;
;	DESCRIPTION
;		This routine checks the given rectangles extents
;		too see if the extents are sign opposing in either
;		direction.  These rectangles must have the same Anchor point.
;
;	ENTRY
;		AX: = Extents of rectangle 1
;		BX: = Extents of rectangle 2
;
;	EXIT
;		ZF: = 1 if no extents are opposing
;		ZF: = 0 if at least one pair of extents are opposing
;
;	MODIFIES
;
;========================================================================
CheckOpposites	PROC	NEAR

	push	AX
	push	BX
	and	AX,8080H
	and	BX,8080H
	xor	AX,BX
	pop	BX
	pop	AX
	ret

CheckOpposites	ENDP



sEnd	code
	end




