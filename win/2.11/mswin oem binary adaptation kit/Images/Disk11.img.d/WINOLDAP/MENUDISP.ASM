title	**	Menu Display operations for SPUD
;-------------------- MENU -------------------------------------------
;
;CONTENTS:
;
;   1)	Code to handle displaying menus for level 2 applications.
;
;PROGRAMMER:
;	Ralph Peterson
;	Mouse code added by Richard Hawes
;
;	$Revision:   1.40  $
;	$Date:   28 Oct 1987 10:05:34  $
;
;---------------------------------------------------------------------
page
	.xlist
	include cmacros.inc
	include menu.inc
	include grabber.inc
	include tty.inc
	include mouse.inc
	.list


;-------------------------------------------------------------------
;
; External WINOLDAP functions
;
externNP	<GoGrab, StringLength, GetMenuTitle, GetMenuItem>
externNP	<ExecuteMacro, ProcessHotKey>
externNP	<GetMouseStatus, SetMouse>
externNP	<PrepMouse, ReleaseMouse>
externNP	<ROMKeyboard, CheckInts>
externNP	<SaveKeyTrans,RestoreKeyTrans>
lowchars	equ	32
midchars	equ	128
highchars	equ	160

GRABINFOLEN	= size GrabInfo
GRABREQLEN	= size GrabRequest


sBegin	data
externW 	MenuSeg
externB 	pifBehavior
externW 	pWindowTitle
externW 	TitleLen

externW 	oldapstate

externW 	MenuTop 	;points to the top of the menu list

externW 	CopySeg
externW 	CopyPara
GlobalW 	CopySize,0

externW 	BufSeg
externW 	BufSize



GlobalB 	GrabStatus,0		;grabber is originally not installed

GlobalB 	GrabData,0,GRABINFOLEN
GlobalB 	KeyMapping,0,6		;area to save key mapping while in
					;menu mode (Vectra only)

sEnd	data


sBegin	code

	assumes cs,code
	assumes ds,data


GlobalB 	fControlMenu,0		;flag set if control menu should
					;be automatically invoked
externB 	fWOAGrab		; Does WOA int9 handle screen grabs?
externB 	fVectra
externW 	GrabVersion
externB 	fHPGrab
externB 	fSwitch
externW 	fMark
externW 	fEndBlock

externW 	fPasteActive
externW 	fHotKey

externB 	AltSet

CopyOffset	dw	0
TitleOffset	dw	0
MenuOffset	dw	0
MenuBarOffset	dw	0	;offset into CopySeg where Menu screen
				;area can be saved.  This is directly
				;following the menubar screen data

MenuSelect	dw	NO_ITEM ;offset of current menu selected
				; -1 if no current selection
MenuItemSelect	dw	NO_ITEM ;offset of current menu item selected
				; This item could be on the menu bar
				; in which case this value would be the
				; same as MenuSelect.
				; -1 if no current selection
MenuBarCoord	label	word	;starting coordinates of menu bar item
MenuBarPos	db	0	;current alpha char position in menu bar
MenuBarRow	db	0	;current line of menu bar we are on

MenuCoord	label	word	;starting coordinates of menu item
MenuPos 	db	0	;col of menu
MenuRow 	db	0	;row of menu

MenuOrig	dw	0	;Upper left corner of menu

RealMenuSize	dw	0	;size of actual menu (used for clipping)
MenuSize	dw	0	;size of menu
MenuBarSize	dw	0	;temporary storage of menu bar size

GlobalW 	fPasteOK,0
GlobalW 	fMenuActive,0
GlobalW 	fMenu,0
GlobalB 	GrabReq,0,GRABREQLEN
globalB 	MouseButton, 0

PUBLIC		DoMenu, UpdateAppTitle
PUBLIC		EnableGrabber, DisableGrabber, DoBeep
PUBLIC		MenuBarDimension, PrepMenuBar, RestoreMenuArea, SetSelectState
PUBLIC		DisplayMenuBar, PrepMenu, MenuDimension
PUBLIC		MenuPosition, DisplayMenu, ProcessMenus, GetNextItemDown
PUBLIC		GetNextItemUp, GetScreenDimension, PutGoGrab, SetMenuTitle
PUBLIC		GetMTByLetter, GetMIByLetter, SaveDataArea
PUBLIC		CheckSize, GoGetGrab, DisplayTitleBar
PUBLIC		MenuBarOffset, TitleOffset, CopyOffset, DisplaySystemIcon
PUBLIC		DoMenuRestore, MenuHandler, HotKeyHandler, InitMenuVars
PUBLIC		BadBeginPaint, BadEndPaint
PUBLIC		GetMouseAction, FindMenu, FindMenuBar
PUBLIC		GetKeyboardAction, GoRealizeColor, CanWeDoMenus


;====== MAIN ROUTINES ====================================================
;
;  Main routines which get and process all user actions.
;
;=========================================================================


;--------------- DoMenu -----------------------------------------------
;
;	DESCRIPTION
;		This routine is the supervisor for handling Menus and
;		the menu bar.  This routine checks the validity of
;		the menus and proceeds only if OK.  It then processes
;		the menu bar and then any menu or macro required.
;
;
;	ENTRY
;		SS: = Winoldap Data Segment
;
;
;	EXIT
;
;	MODIFIES
;		All but BP
;
;---------------------------------------------------------------------------
DoMenu	PROC	NEAR

	push	BP
	push	SS			;get the data segment
	pop	DS
	call	CheckInts		; are there ints pending
	jnz	DMRet			; if so reurn without resetting flag
	call	InitMenuVars
	jc	DMRet
	cmp	CS:[fHotKey],0
	je	DM5
	call	HotKeyHandler
	stc				;enable grabber flag
	jmp	DM7
DM5:
	call	MenuHandler
DM7:
	mov	AX,0			;must preserve flags (mov not xor)
	jnc	DM10
	mov	AX,0FFFFH
DM10:
	push	SI
	push	AX
	call	DoMenuRestore
	pop	AX
	pop	SI
	or	AX,AX			;execute a macro?
	jnz	DMRet
	cCall	ExecuteMacro,ES:[SI].item
DMRet:
	mov	word ptr CS:fMenuActive,0 ;reset the menu active flag
	pop	BP
	ret

DoMenu	ENDP



;------------- InitMenuVars -----------------------------------------------
;
;	DESCRIPTION
;		This routine checks for the feasability of displaying
;		menus and sets flags appropriately.
;
;	ENTRY
;
;	EXIT
;		MenuVars are initialized
;		CF: = 0 if capable of menuing
;		CS: = 1 if no menuing allowed
;		ES: = MenuSeg
;
;	MODIFIES
;		All but BP and Seg regs
;
;---------------------------------------------------------------------------
InitMenuVars	PROC	NEAR

	mov	CS:fMenu,0		;disable menu pending flag
	cmp	CS:[fMenuActive],0	; Menus allready up?
	jne	IMVBad
	call	CanWeDoMenus
	mov	CS:fMenuActive,1
	jc	IMVBadBell
	call	BadBeginPaint
	mov	CS:CopyOffset,0 	;initialize the screen buffer area
	mov	AX,CopyPara
	shl	AX,1
	shl	AX,1
	shl	AX,1
	shl	AX,1
	mov	CopySize,AX
	call	DisableGrabber
   ;
   ; Initialize variables
   ;
	mov	CS:MenuBarCoord, 0
	mov	cs:MenuSize, 0
	mov	cs:MenuSelect, NO_ITEM
	mov	cs:MenuItemSelect, NO_ITEM
	mov	ES,MenuSeg
	clc				;show it is OK
	jmp	IMVRet
IMVBadBell:
IMVBad:
	stc
IMVRet:
	ret

InitMenuVars	ENDP


;============= CanWeDoMenus =============================================
;
;	DESCRIPTION
;		This routine checks to see if all of the hooks are set
;		to allow menuing in bad apps. This routine does not beep.
;
;	ENTRY
;		DS = Winoldap DS
;
;	EXIT
;		CF = 1 if no menus
;		CF = 0 if menus available
;
;	MODIFIES
;		All but BP and seg regs
;
;===========================================================================
CanWeDoMenus	PROC	NEAR
	push	ES
	cmp	CS:fSwitch, SW_CLEAR	;is a switch pending
	jnz	CWDMBad
	cmp	MenuSeg,0		 ;is there a Menu buffer?
	jz	CWDMBad
	mov	AX,GRAB_GETVERSION
	mov	SI,InquireGrab
	call	GoGrab			;get the grabber version
	cmp	AH,2
	jl	CWDMBad
	call	GetScreenDimension
	or	AX,AX			;can we use grabber extensions
	jz	CWDMBad
	cmp	CS:[fPasteActive],0	; Are we pasting?
	jnz	CWDMBad
	test	[pifBehavior], UGLYBITS ; Are we ugly?
	jz	CWDMRet
	errn$	CWDMBad
CWDMBad:
	stc
CWDMRet:
	pop	ES
	ret
CanWeDoMenus	ENDP

;---------  MenuHandler ---------------------------------------------------
;
;	DESCRIPTION
;		This routine handles Menu Mode operations.
;
;	ENTRY
;		MenuVars initialized
;		Grabber Disabled
;
;	EXIT
;		CF: = 1 means re enable the grabber
;		CF: = 0 do not re enable the grabber
;		      and execute Macro in structure pointed to by SI
;
;	MODIFIES
;		All
;
;--------------------------------------------------------------------------

MenuHandler	PROC	NEAR

	call	DisplayTitleBar 	;show the title bar
	jc	MHRet			;abort if it won't come up
	call	DisplayMenuBar
	call	HighlightFirstMenu
	call	SetCursorKeys
	call	PrepMouse
	call	GetMouseStatus
	mov	CS:MouseButton,BL
MHSetControl:
	cmp	CS:[fControlMenu],0	;do we want control menu immediately?
	jz	MH10
	mov	CS:[fControlMenu],0	;reset the flag
	xor	ax, ax
	mov	si, MenuTop
	call	DisplayMenu
	call	HiliteFirstMenuItem
MH10:
	call	ProcessMenus		;process the menus
MHRet:
	pushf				;Restore the mouse		RMH
	call	ReleaseMouse
	call	RestoreCursorKeys
MHNores:
	popf				;				RMH
	ret				;carry will be set for abort

MenuHandler	ENDP


;---------  SetCursorKeys -------------------------------------------------
;
;	DESCRIPTION
;		This routine will save the state of Vectra CCP and set it
;		to CCPCUR mapping.
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;		AX, BX
;
;--------------------------------------------------------------------------
SetCursorKeys:
	test	fVectra,1
	jz	SCKRet
	push	ES
	push	DI
	lea	DI,KeyMapping
	push	DS
	pop	ES
	call	SaveKeyTrans
	mov	AX,F16_SET_XLATORS
	xor	BL,BL			;CCP to CCPCUR
	int	16H			;set the translators
	pop	DI
	pop	ES
SCKRet:
	ret


;---------  RestoreCursorKeys ---------------------------------------------
;
;	DESCRIPTION
;		This routine will restore the state of the saved key
;		mappings.
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;		AX, BX
;
;--------------------------------------------------------------------------
RestoreCursorKeys:
	test	fVectra,1
	jz	RCKRet
	push	ES
	push	DI
	lea	DI,KeyMapping
	push	DS
	pop	ES
	call	RestoreKeyTrans
	pop	DI
	pop	ES
RCKRet:
	ret



;---------------- HotKeyHandler --------------------------------------------
;
;	DESCRIPTION
;		This routine handles all of HotKey mode.
;
;	ENTRY
;		MenuVars initialized
;		Grabber Disabled
;
;	EXIT
;
;	MODIFIES
;		All
;
;
;---------------------------------------------------------------------------
HotKeyHandler	PROC	NEAR
	stc
	cmp	OldApState, SF_TERMINATED
	je	HHRet
	call	DisplayTitleBar 	;show the title bar
	jc	HHRet			;abort if it won't come up
	mov	DI,codeOffset GrabReq	;must display system menu icon
	push	CS
	pop	ES
	call	DisplaySystemIcon
	call	ProcessHotKey
HHRet:
	ret

HotKeyHandler	ENDP


;--------------- ProcessMenu ------------------------------------------
;
;	DESCRIPTION
;		This gets and processes all user actions in menu mode.
;
;	ENTRY
;		ES: = MenuSeg
;		DI: = Grab Request Offset
;		CS: = Grab Request Segment
;		Menu Bar is displayed in initial state
;		MenuSelect has valid data
;		MenuBarPos has valid data
;
;	EXIT
;		CF: = 1 Abort menu bar
;		CF: = 0 Process data in MenuSelect
;		SI: = pointer to Menu Info structure in which the macro
;		     need be executed
;
;	MODIFIES
;		AX, BX, CX, DX, SI
;
;--------------------------------------------------------------------------
ProcessMenus	PROC	NEAR

PMLoop:
    ;
    ; Get a Keyboard Action
    ;
	call	GetKeyboardAction
    ;
    ; Process it
    ;
	or	bx, bx
	jnz	PMProcessAction
    ;
    ; Get a Mouse Action
    ;
	call	GetMouseAction
    ;
    ; Process it
    ;
	or	bx, bx
	jz	PMLoop
	errn$	PMProcessAction

  ;------
  ;
  ; Process the user action according to BX.
  ;
  ; The flags in BX are tested in sequence. If a flag is set,
  ; the corresponding action is taken. After all the flags have
  ; been tested the loop is restarted.
  ;
  ; SI - New menu offset
  ; AX - New menu row and col
  ; BX - User action
  ;
PMProcessAction:

    ;
    ; Optimization: Don't display a menu if it is allready  pulled down
    ;
	test	bl, AF_DISPLAY
	jz	PMNoO
	cmp	cs:MenuSize, 0
	je	PMNoO
	cmp	si, cs:MenuSelect
	jne	PMNoO
	and	bl, not( AF_DISPLAY or AF_RELEASE )
	or	bl, AF_UNSELECT or AF_SELECTTITLE
PMNoO:

    ;
    ; Release Menus?
    ;
	test	bl, AF_RELEASE
	je	PMNoRel
	call	ReleaseMenu
PMNoRel:

    ;
    ; Unselect? Unselect the current item
    ;
	test	bl, AF_UNSELECT
	jz	PMNoU
	call	Unselect
PMNoU:

    ;
    ; Execute? Execute only non-popup enabled items
    ;
	test	bl, AF_EXEC
	jz	PMNoE
	test	es:[si].flags, MF_GRAYED or MF_DISABLED
	jnz	PMAbort
	test	es:[si].flags, MF_POPUP
	jnz	PMExecPopup
	mov	cs:AltSet, 0
	clc
	ret
PMExecPopup:
	or	bl, AF_DISPLAY or AF_HILITEFIRST
PMNoE:

    ;
    ; Abort?
    ;
	test	bl, AF_ABORT
	jz	PMNoAbort
PMAbort:
	mov	CS:AltSet,0		;clear Alt Key flag to be safe
	stc				;set abort flag
	ret
PMNoAbort:

    ;
    ; Display? If a popup display the menu, otherwise
    ;	       select it.
	test	bl, AF_DISPLAY
	jz	PMNoD
	test	es:[si].flags, MF_POPUP
	jz	PMNoMenuToDisplay
	test	es:[si].flags, MF_GRAYED or MF_DISABLED
	jnz	PMNoMenuToDisplay
	push	ax
	push	bx
	call	DisplayMenu
	pop	bx
	pop	ax
	jmp	PMNoD
PMNoMenuToDisplay:
	mov	bl, AF_SELECTTITLE
PMNoD:

    ;
    ; Select title?
    ;
	test	bl, AF_SELECTTITLE
	jz	PMNoST
	push	bx
	mov	bx, MT_SELECT
	call	SetMenuTitle
	pop	bx
PMNoST:

    ;
    ; Select Item?
    ;
	test	bl, AF_SELECTITEM
	jz	PMNoSI
	call	SelectMenuItem
PMNoSI:

    ;
    ; Hilite first item ?
    ;
	test	bl, AF_HILITEFIRST
	jz	PMNoH
	call	HiliteFirstMenuItem
PMNoH:

    ;
    ; All done: loop again
    ;
	jmp	PMLoop

ProcessMenus	ENDP

;------------------- GetKeyboardAction ----------------------------------
;
;	DESCRIPTION
;		This routine polls the keyboard and determines what
;		actions if any are appropriate.
;
;	ENTRY
;
;	EXIT
;		BX - Action
;
;		If a menu field is specified:
;			AL - Menu col
;			AH - Menu row
;			SI - Menu pointer
;
;	USES
;		CX, AX, SI, BX
;
;---------------------------------------------------------------------
GetKeyboardAction PROC	NEAR
    ;
    ; Check for AltSet for a key press
    ;
	mov	AL,CS:AltSet		;has the alt key been triggered
	or	AL,AL
	jz	KARead			;abort if so
	mov	bx, AF_ESCAPE
	ret
    ;
    ; Read the keyboard
    ;
KARead:
	mov	AH,1
	call	ROMKeyboard
	jz	KAnop
	xor	AH,AH
	call	ROMKeyboard
    ;
    ; Decide which letter it is
    ;
	push	di
	push	es			;Setup for a SCAS
	push	cs
	pop	es
	lea	di, cs:KAKey		;KAKey contains the keys
	mov	dx, ax
	mov	al, ah
	mov	cs:KAlast, al		;Gardian
	mov	cl, -1
	repne	scasb
	sub	di, codeOffset KAKey + 1
	pop	es
    ;
    ; DI contains an key index
    ;
    ; Is a menu pulled down?
    ;
	mov	si, cs:MenuSelect
	mov	ax, cs:MenuBarCoord
	cmp	cs:MenuSize, 0
	je	KADoit
    ;
    ; Yes, MenuSelectItem and MenuCoord should be used for
    ;	   all cases except ones which call GetNextMenuTitle
    ;
KAMenuDown:
	add	di, 7
	cmp	di, 12			; Left and Right need Bar values
	je	KADoit			;  Right, OK
	cmp	di, 11
	je	KADoit			;  Left, OK
	mov	si, cs:MenuItemSelect
	mov	ax, cs:MenuCoord
	errn$	KADoit
    ;
    ; Call the appropriate handler and load BX
    ;
KADoit:
	mov	bx, di
	add	bx, bx
	call	cs:KAJmp[bx]
	mov	bl, AF_NOP
	jcxz	KAActionSet
	mov	bl, cs:KATbl[di]
	xor	bh, bh
KAActionSet:
	pop	di
	jmp	KARet
    ;
    ; Nop
    ;
KAnop:
	mov	bx, AF_NOP
KARet:
	ret

    ;
    ; Key code handlers
    ;
					; Menu Bar
KAJmp	dw	KANop			;   ESC
	dw	KANop			;   ENTER
	dw	KANop			;   Up
	dw	KANop			;   Down
	dw	GetPrevMenuTitle	;   Left
	dw	GetNextMenuTitle	;   Right
	dw	GetMTByLetter		;   Letter
					; Menu Down
	dw	KANop			;   ESC
	dw	KANop			;   ENTER
	dw	GetNextItemUp		;   Up
	dw	GetNextItemDown 	;   Down
	dw	GetPrevMenuTitle	;   Left
	dw	GetNextMenuTitle	;   Right
	dw	GetMIByLetter		;   Letter

   ;
   ; Action Codes
   ;
					; Menu Bar
KATbl	db	AF_ESCAPE		;   ESC
	db	AF_EXECUTE		;   ENTER
	db	AF_CYCLEMENU		;   Up
	db	AF_CYCLEMENU		;   Down
	db	AF_CYCLETITLE		;   Left
	db	AF_CYCLETITLE		;   Right
	db	AF_EXECUTE		;   Letter
					; Menu
	db	AF_ESCAPE		;   ESC
	db	AF_EXECUTE		;   ENTER
	db	AF_CYCLEITEM		;   Up
	db	AF_CYCLEITEM		;   Down
	db	AF_CYCLEMENU		;   Left
	db	AF_CYCLEMENU		;   Right
	db	AF_EXECUTE		;   Letter

    ;
    ; Keyboard table: the keys we are interested in the proper order.
    ;
KAKey	db	01			; Esc
	db	28			; Enter
	db	72			; Up
	db	80			; Down
	db	75			; Left
	db	77			; Right
KAlast	db	?			; Letter

GetKeyboardAction ENDP


;------------------- GetMouseAction ----------------------------------
;
;	DESCRIPTION
;		This routine polls the mouse and determines what
;		actions if any are appropriate.
;
;	ENTRY
;
;	EXIT
;		BX - Action
;
;		If a menu field is specified:
;			AL - Menu col
;			AH - Menu row
;			SI - Menu pointer
;
;	USES
;		CX, AX, SI, BX
;
;---------------------------------------------------------------------
GetMouseAction	PROC	NEAR
	push	di
    ;
    ; Get the mouse status
    ;
	call	GetMouseStatus
    ;
    ; Find out where we are. Put the result in DI.
    ;
	xor	di, di
	call	FindMenu	; Yes, Is it in a menu item?
	jc	MATestBar
	inc	di		;  Yes, test for an field
	jmp	MATestSI

MATestBar:
	call	FindMenuBar	; Is it in a menu bar?
	jc	MATestButton	;  No, test buttons. No need to test SI.
	add	di, 3		;  Yes, test for an field
	errn$	MATestSI

MATestSI:
	cmp	si, NO_ITEM	; Is an field selected ?
	jne	MATestButton	;  No, test buttons
	inc	di		;  Yes, note it.
	errn$	MATestButton
    ;
    ; DI = 0 - Mouse not in any menu item or title
    ;	   1 - Mouse in menu item
    ;	   2 - Mouse in menu but not in a item
    ;	   3 - Mouse in menu title
    ;	   4 - Mouse in menu bar but not in a title
    ;
    ; AX = Menu field position
    ; BX = Mouse button status
    ; SI = Menu field pointer
    ;
    ; Find out how the left button changed.
    ;
MATestButton:
	and	bl, 01h 	; only look at the first button
	mov	bh, bl
	xchg	MouseButton, bh
	xor	bh, bl
	cmp	bx, 0000h	; no mouse action
	je	MANothing
	cmp	bx, 0001h	; Mouse move
	je	MAMouseMove
	cmp	bx, 0101h	; Mouse press
	je	MAPrsRel
	add	di, 5		; Mouse release
MAPrsRel:
    ;
    ; Mouse press or release: DI points to a entry in MAPRTbl.
    ;
	mov	bl, cs:MAPRTbl[di]
	xor	bh, bh
	jmp	MARet
						; Mouse Press
MAPRTbl db	AF_ABORT or AF_RELEASE		;    Not in anything
	db	AF_SELECTITEM or AF_UNSELECT	;    In menu item
	db	AF_UNSELECT			;    In menu but not item
	db	AF_PULLMENU			;    In menu title
	db	AF_UNSELECT			;    In bar but not title
						; Mouse Release
	db	AF_ABORT or AF_RELEASE		;    Not in anything
	db	AF_EXECUTE			;    In menu item
	db	AF_ABORT or AF_RELEASE		;    In menu but not item
	db	AF_EXECUTE			;    In menu title
	db	AF_ABORT or AF_RELEASE		;    In bar but not title

    ;
    ; Mouse Button Down and Move: DI point to a entry in MAMTbl.
    ;
MAMouseMove:
	mov	cx, cs:MenuItemSelect
	cmp	si, cx		; Any change?
	je	MANothing	;   No, don't do anything
	cmp	cx, cs:MenuSelect
	jne	MANoBar
	add	di, 5
MANoBar:
	mov	bl, cs:MAMTbl[di]
	xor	bh, bh
	jmp	MARet
						; Mouse Move table
MAMTbl	db	AF_UNSELECT			;   In nothing
	db	AF_SELECTITEM or AF_UNSELECT	;   In menu item
	db	AF_UNSELECT			;   In menu but not item
	db	AF_PULLMENU			;   In menu title
	db	AF_UNSELECT			;   In bar but not title
						; Was in Bar
	db	AF_RELEASE			;   In nothing
	db	AF_SELECTITEM or AF_UNSELECT	;   In menu item
	db	AF_UNSELECT			;   In menu but not item
	db	AF_PULLMENU			;   In menu title
	db	AF_RELEASE			;   In bar but not title

    ;
    ; Do nothing
    ;
MANothing:
	mov	bx, AF_NOP
	errn$	MARet

MARet:
	pop	di
	ret
GetMouseAction	ENDP





;====== MENU SEARCHING ROUTINES =====================================
;
;  These routines search for a specific item on a menu or the menu bar
;
;======================================================================



;--------------- FindMenuBar ----------------------------------------
;
;	DESCRIPTION
;		Finds the menu title on the menu bar associated with
;		the passed position.
;
;	ENTRY
;		DL - column
;		DH - row
;		ES - Valid Menu Seg
;
;	EXIT
;		SI - pointer to the menu title structure
;		      NO_ITEM if not in menu bar
;		AL - Menu title column
;		AH - Menu title row
;		CF - set if not in menu bar
;
;	USES
;		None
;
;---------------------------------------------------------------------
cProc	FindMenuBar, <NEAR,PUBLIC>
cBegin	NoGen
    ;
    ; Register Usage
    ;	AL - Current column
    ;	AH - Current row
    ;	SI - Current Menu offset
    ;	CL - Find col
    ;	DL - Find row
    ;	BL - Temp register
    ;
	push	bx
    ;
    ; Is the mouse on the menu bar?
    ;
	mov	ax, cs:[MenuBarSize]
	cmp	dh, ah
	ja	FMBRetNo
    ;
    ; Yes, Start at the Top menu
    ;
	xor	ax, ax			; Clear current row and column
	mov	si, [MenuTop]		; Start at the top
	test	es:[si].flags, MF_REMOVE; Is it removed?
	jnz	FMBNoMatch		;   Yes, get the next one.
    ;
    ; Check for a hit.
    ;
FMBLoop:
	cmp	ah, dh
	jne	FMBNoMatch
	cmp	al, dl
	ja	FMBNoMatch
	mov	bl, es:[si].twidth
	add	bl, al
	inc	bl
	inc	bl
	cmp	bl, dl
	jbe	FMBNoMatch
    ;
    ; We have a hit: setup AX and BX are ready, just clear carry
    ;
	test	es:[si].flags, MF_GRAYED or MF_DISABLED
	jnz	FMBRetNoItem
	clc
	jmp	FMBRet
    ;
    ; No Hit: try the next menu
    ;
FMBNoMatch:
	cmp	es:[si].next, 0 	; Is there another menu?
	je	FMBRetNoItem		;   No, finish
	test	es:[si].flags, MF_REMOVE;   There might be: Is it removed?
	jz	FMBNext 		;     No: get it
	mov	si, es:[si]		;     Yes: try again
	jmp	FMBNoMatch
FMBNext:
	call	GetNextMenuTitle
	jmp	FMBLoop

FMBRetNoItem:
	mov	si, NO_ITEM
	clc
	jmp	FMBRet

FMBRetNo:
	mov	si, NO_ITEM
	stc
	errn$	FMBRet
FMBRet:
	pop	bx
	ret
cEnd	NoGen

;--------------- FindMenu ----------------------------------------
;
;	DESCRIPTION
;		Finds the menu item on current menu associated with
;		the passed position.
;
;	ENTRY
;		DL - column
;		DH - row
;		ES - Valid Menu Seg
;
;	EXIT
;		SI - pointer to the menu title structure
;		     NO_ITEM if not in any item.
;		AL - Menu title column
;		AH - Menu title row
;		CF - set if not in the menu.
;
;	USES
;		CX
;
;---------------------------------------------------------------------
cProc	FindMenu, <NEAR,PUBLIC>
cBegin	NoGen
    ;
    ; Register Usage
    ;	AL - Current column
    ;	AH - Current row
    ;	DI - Current Menu offset
    ;	CL - Find col
    ;	DL - Find row
    ;	BL - Temp register
    ;
	push	bx
    ;
    ; Is a menu pulled down?
    ;
	cmp	cs:[MenuSize], 0
	je	FMRetNo
	mov	cx, cs:RealMenuSize
	jcxz	FMRetNo
    ;
    ; Yes, is the mouse on the menu?
    ;
	mov	ax, cs:MenuOrig ; There is a menu pulled down
	cmp	dl, al
	jb	FMRetNo
	cmp	dh, ah
	jb	FMRetNo
	add	al, cl
	add	ah, ch
	cmp	dl, al
	jae	FMRetNo
	cmp	dh, ah
	jae	FMRetNo
    ;
    ; Yes: start at the top of the menu and search down for a hit
    ;
	mov	si, cs:MenuSelect	; Start at the top
	mov	si, es:[si].item
	mov	ax, cs:MenuOrig
	inc	al			; Make room for the boarders
	inc	ah
    ;
    ; Check for a hit.
    ;
FMLoop:
	test	es:[si].flags, MF_REMOVE; Is it removed?
	jnz	FMGetNext		;   Yes, get the next one
	cmp	ah, dh
	jne	FMNoMatch
	cmp	al, dl
	ja	FMNoMatch
	mov	bx, cs:[MenuSize]
	add	bl, al
	dec	bl
	dec	bl
	cmp	bl, dl
	jbe	FMNoMatch
    ;
    ; We have a hit: setup AX and BX are ready, just clear carry
    ;
	test	es:[si].flags, MF_SEPARATOR
	jnz	FMRetNoItem
	mov	si, si
	inc	al
	inc	al
	clc
	jmp	FMRet
    ;
    ; No Hit: try the next menu item
    ;
FMNoMatch:
	inc	ah
FMGetNext:
	cmp	es:[si].next, 0 	; Is there another menu?
	je	FMRetNoItem		;   No, finish

	mov	si, es:[si].next
	jmp	FMLoop
;
; Not on the menu
;
FMRetNo:
	stc
	mov	si, NO_ITEM
	jmp	FMRet
;
; On the menu but not on a enabled item
;
FMRetNoItem:
	clc
	mov	si, NO_ITEM
	errn$	FMRet

FMRet:
	pop	bx
	ret
cEnd	NoGen

;------------------- GetPrevMenuTitle ----------------------------------
;
;	DESCRIPTION
;		Gets the previous menu title in the menu bar.
;
;	ENTRY
;		AX - current menu poistion (col,row)
;		ES:SI - current menu
;
;	EXIT
;		AX - previous menu position
;		ES:SI - previous menu
;
;	USES
;		None
;
;-----------------------------------------------------------------------
cProc	GetPrevMenuTitle, <NEAR>, <DI>
localW	lastPointer
localW	lastPos
cBegin
	mov	di, si
GPTLoop:					;get pointer to search for
	mov	lastPointer, si 		;A save si, di
	mov	lastPos, ax
	call	GetNextMenuTitle
	jcxz	GPTRet
	cmp	si, di
	jne	GPTLoop
	mov	si, lastPointer
	mov	ax, lastPos
GPTRet:
cEnd

;------------------- GetNextMenuTitle ----------------------------------
;
;	DESCRIPTION
;		Gets the next menu title in the menu bar.
;
;	ENTRY
;		AX - current menu poistion (col,row)
;		ES:SI - current menu
;
;	EXIT
;		AX - next menu position
;		ES:SI - next menu
;		CX - Is zero if only one menu title
;
;	USES
;		None
;
;-----------------------------------------------------------------------
GetNextMenuTitle PROC NEAR
	push	bx
	push	di
    ;
    ; Set the pointer and CX
    ;
	xor	cx, cx
	mov	di, si
GNMNext:
	mov	si, es:[si].next
	or	si, si
	jnz	GNMCheck
	cmp	di,MenuTop
	je	GNMDone
	mov	si,MenuTop		;get pointer to the system menu
	errn$	GNMCheck

GNMCheck:
	test	es:[si].flags, MF_REMOVE
	jnz	GNMNext
	inc	cx
   ;
   ; Pointer and CX are set: Adjust position
   ;
   ; First, check for top
   ;
	cmp	si, MenuTop
	jne	GNMblah
	xor	ax, ax
	jmp	GNMDone
GNMBlah:
	cmp	di, MenuTop
	jne	GNMNextCol
	mov	ax, 0100h
	jmp	GNMDone
   ;
   ; Check next position on the row
   ;
GNMNextCol:
	add	al,ES:[di].twidth	 ;add the width of the current item
	inc	al			;account for padding
	inc	al
	mov	bl,al			;save possible new pos
	add	bl,ES:[si].twidth	 ;add new width
	inc	bl			;add padding
	inc	bl
	cmp	bl,byte ptr grabdata.giCharsX ;is this past the end
	jl	GNMDone 		;No,
	xor	al,al			;beginning of new row
	inc	ah			;new row

GNMDone:
	pop	di
	pop	bx
	ret
GetNextMenuTitle ENDP


;--------------- GetNextItemUp ---------------------------------------------
;
;	DESCRIPTION
;		This routine is updated to point to the next logical
;		item if an up arrow is struck.
;
;	ENTRY
;		ES:SI: = MenuItemSelect
;		AX: = MenuCoords
;
;	EXIT
;		SI: = MenuItemSelect which is updated
;		AX: = MenuCoords
;
;	MODIFIES
;		AX, CX, DX, SI, DI
;
;----------------------------------------------------------------------------
cProc	GetNextItemUp, <NEAR>, <DI>
localW	lastPointer
localW	lastPos
cBegin
	mov	di, si
GUILoop:					;get pointer to search for
	mov	lastPointer, si 		;A save si, di
	mov	lastPos, ax
	call	GetNextItemDown
	jcxz	GUIRet
	cmp	si, di
	jne	GUILoop
	mov	si, lastPointer
	mov	ax, lastPos
GUIRet:
cEnd


;--------------- GetNextItemDown ------------------------------------------
;
;	DESCRIPTION
;		This routine is updated to point to the next logical
;		item if a down arrow is struck.
;
;	ENTRY
;		AX:    = MenuCoord
;		ES:SI: = MenuItemSelect
;		MenuSelect must be valid
;
;	EXIT
;		SI: = MenuItemSelect which is updated
;		MenuCoords are updated
;		CX: 0 if no other items
;
;	MODIFIES
;		AX, BX, SI
;
;----------------------------------------------------------------------------
GetNextItemDown PROC	NEAR
	push	DI
   ;
   ; Set the pointer
   ;
	xor	cx, cx
	mov	DI, CS:MenuSelect		;get the menu title struc
	mov	DI, ES:[DI].item		;get the first item
GNDNext:
	mov	SI, ES:[SI].next
	or	SI,SI				;is this the last one
	jnz	GNDCheck
	cmp	SI, DI
	je	GNDDone
	mov	SI, DI
GNDCheck:
	inc	cx
	test	ES:[SI].flags, MF_REMOVE	; is it valid
	jnz	GNDNext
   ;
   ; Pointer set: Set Position
   ;
	cmp	si, di
	jne	GNDInc
	mov	AX,CS:MenuOrig			;get the origin
	add	AL,3				;border + padding
GNDInc:
	inc	ah
   ;
   ; Check for seporators
   ;
	test	es:[si].flags, MF_SEPARATOR
	jnz	GNDNext

GNDDone:
	pop	DI
	ret

GetNextItemDown ENDP



;------------- GetMTByLetter -----------------------------------------------
;
;	DESCRIPTION
;		This routine finds the next menu title in the list
;		whose first character matches the letter given.  It sets
;		a flag if this items first letter is not unique.  It also
;		returns the menu bar position of this item.
;
;	ENTRY
;		DL: = Letter to search for (ASCII)
;		ES: = MenuSeg
;		MenuSelect must be valid
;		MenuBarCoord must be valid
;
;	EXIT
;		SI: = pointer to structure of item or the current
;		      item if no match is found
;		AX: = position of item in SI
;		CX: = 1 if match is found, 0 if not
;
;	MODIFIES
;		AX, BX, CX
;
;----------------------------------------------------------------------------

GetMTByLetter	PROC	NEAR

	mov	AL,DL
	cmp	AL,SYSTEM_CHAR
	jnz	GMTBL10
	xor	AX,AX
	mov	SI,MenuTop
	mov	CX,1
	jmp	GMTBLRet
GMTBL10:
	push	DI
	push	DX
	mov	DI,0FFFFH		;search for mnemonic flag
	call	MyUpper 		;get uppercase of char to search for
	mov	CL,AL			;copy char to search for
GMTBL11:
	mov	SI,CS:MenuSelect
	mov	BX,CS:MenuBarCoord
GMTBL12:
	cmp	SI,MenuTop		;is this the system menu?
	jz	GMTBL12a
	cmp	ES:[SI].next,0
	jnz	GMTBL15
	mov	SI,MenuTop
GMTBL12a:
	cmp	ES:[SI].next,0
	jnz	GMTBL13
	jmp	GMTBLDefault
GMTBL13:
	mov	BX,0100H		;coordinates of start of menu bar
	jmp	GMTBL17
GMTBL15:
	add	BL,ES:[SI].twidth
	inc	BL
	inc	BL
	cmp	BL,GrabData.giCharsX
	jle	GMTBL17
	inc	BH			;next row
	xor	BL,BL
GMTBL17:
	mov	SI,ES:[SI].next
	test	ES:[SI].flags,MF_REMOVE
	jnz	GMTBL20
	mov	AL,ES:[SI].mnem 	;get the mnemonic char
	call	MyUpper 		;convert to upper case
	cmp	AL,CL			;is it what we are looking for
	jnz	GMTBL20
	or	DI,DI			;looking for mnemonics
	jz	GMTBLFound
	cmp	ES:[SI].mnempos,0FFH	;is it a mnemonic
	jnz	GMTBLFound
GMTBL20:
	cmp	SI,CS:MenuSelect	;no match in list
	jz	GMTBLDefault
	jmp	GMTBL12

GMTBLFound:
	mov	CH,BL
	add	CH,ES:[SI].twidth
	inc	CH
	inc	CH
	cmp	CH,GrabData.giCharsX	;will it fit
	jle	GMTBL25
	inc	BH				;next row
	xor	BL,BL
GMTBL25:					;position is correct here
	mov	DX,SI				;get pointer in DX
						;and check for duplicates
	cmp	SI,CS:MenuSelect		;is cursor already there
	jz	GMTBLNoDup
GMTBL28:
	cmp	ES:[SI].next,0
	jnz	GMTBL30
	mov	SI,MenuTop
	cmp	ES:[SI].next,0
	jz	GMTBLNoDup
GMTBL30:
	mov	SI,ES:[SI].next 	;we alredy checked system menu
	cmp	SI,DX
	jz	GMTBLNoDup
	test	ES:[SI].flags,MF_REMOVE
	jnz	GMTBL35
	mov	AL,ES:[SI].mnem 	;get the mnemonic
	call	MyUpper 		;convert to upper case
	cmp	AL,CL			;is it what we are looking for
	jnz	GMTBL35
	or	DI,DI			;looking for mnemonics
	jz	GMTBLDup
	cmp	ES:[SI].mnempos,0FFH	;is it mnemonic
	jnz	GMTBLDup
GMTBL35:
	jmp	GMTBL28

GMTBLDefault:
	or	DI,DI
	jz	GMTBLDef10
	xor	DI,DI			;if no mnemonics look for first chars
	jmp	GMTBL11
GMTBLDef10:
	mov	DX,CS:MenuSelect
	mov	BX,CS:MenuBarCoord
	xor	CX,CX
	jmp	GMTBLDone
GMTBLDup:
GMTBLNoDup:
	mov	CX,1
GMTBLDone:
	mov	SI,DX
	mov	AX,BX
	pop	DX
	pop	DI

GMTBLRet:
	ret
GetMTByLetter	ENDP

;--------------GetMIByLetter--------------------------------------------
;
;	DESCRIPTION
;		This routine gets the menu item in the currently dropped
;		down menu that corresponds to the letter passed (if one
;		exists)
;
;	ENTRY
;		DL: = letter to match
;		ES: = MenuSeg
;		MenuItemSelect is valid
;		MenuSelect is valid
;		MenuOrig is valid
;		MenuCoord is valid
;
;	EXIT
;		SI: = pointer to the appropriate item (current item
;			if none match)
;		AX: = position of item in SI
;		CX: = 1 if match found, 0 otherwise.
;
;	MODIFIES
;		AX, BX, CX
;
;----------------------------------------------------------------------------
GetMIByLetter	PROC	NEAR

	push	DI
	push	DX
	mov	AL, DL
	mov	DI,0FFFFH		;search for mnemonics first
	call	MyUpper 		;convet char to upper case
	mov	CL,AL			;save the char
GMIBL3:
	mov	SI,CS:MenuItemSelect
	mov	BX,CS:MenuCoord
	or	SI,SI			;are there any items to select
	jnz	GMIBL5
	jmp	GMIBLDefault
GMIBL5:
	cmp	ES:[SI].next,0		;is this the last one
	jnz	GMIBL10
	mov	SI,CS:MenuSelect
	mov	BX,CS:MenuOrig
	add	BL,3			;padding
	inc	BH			;border
	mov	SI,ES:[SI].item
	jmp	GMIBL15
GMIBL10:
	mov	SI,ES:[SI].next
	test	ES:[SI].flags,MF_REMOVE
	jnz	GMIBL15a
	inc	BH
GMIBL15:
	test	ES:[SI].flags,MF_SEPARATOR+MF_DISABLED
	jnz	GMIBL15a
	mov	AL,ES:[SI].mnem 	;get mnemonic
	call	MyUpper
	cmp	AL,CL
	jnz	GMIBL15a
	or	DI,DI
	jz	GMIBLFound
	cmp	ES:[SI].mnempos,0FFH
	jnz	GMIBLFound
GMIBL15a:
	cmp	SI,CS:MenuItemSelect
	jnz	GMIBL5
	jmp	GMIBLDefault		 ;no match

GMIBLFound:
	mov	DX,SI			;save the pointer
	cmp	SI,CS:MenuItemSelect	;are there already
	jz	GMIBLNoDup




GMIBL20:
	cmp	ES:[SI].next,0		;is this the last one
	jnz	GMIBL25
	mov	SI,CS:MenuSelect
	mov	SI,ES:[SI].item
	jmp	GMIBL30
GMIBL25:
	mov	SI,ES:[SI].next
GMIBL30:
	test	ES:[SI].flags,MF_SEPARATOR+MF_GRAYED+MF_DISABLED+MF_REMOVE
	jnz	GMIBL32
	mov	AL,ES:[SI].mnem
	call	MyUpper
	cmp	AL,CL
	jnz	GMIBL32
	or	DI,DI
	jz	GMIBLDup
	cmp	ES:[SI].mnempos,0FFH
	jnz	GMIBLDup
GMIBL32:
	cmp	SI,CS:MenuItemSelect
	jnz	GMIBL20
	jmp	GMIBLNoDup		;no duplicates

GMIBLDefault:
	or	DI,DI
	jz	GMIBLDef10
	xor	DI,DI			;no mnemonics, look for first chars
	jmp	GMIBL3
GMIBLDef10:
	mov	DX,CS:MenuItemSelect
	mov	BX,CS:MenuCoord
	xor	CX,CX
	jmp	GMIBLDone
GMIBLDup:
GMIBLNoDup:
	mov	CX,1
GMIBLDone:
	mov	SI, DX			 ;get the result
	mov	AX, BX
	pop	DX
	pop	DI
	ret

GetMIByLetter	ENDP





;===== MENU SELECTION CODE ============================================
;
; The following routines select and unselect menu items and titles.
; These are the only routines which set MenuItemSelect and MenuSelect.
;
;======================================================================


;---------------- SetMenuTitle ----------------------------------------
;
;	DESCRIPTION
;		A menu title can be in there states: Clear (Normal),
;		Bar (|MenuTitle|), and Selected (Inverted). This
;		routine is used change the state of a menu title. Note:
;		Selection can be used loosely to mean either selected or
;		hilited.
;
;	ENTRY
;		BX := 0 to clear it, 2 to bar it, 4 to hilite it.
;		if BX is not 0
;		    SI := offset of menu title
;		    AX := coordinates of the menu
;
;	EXIT
;		MenuItemSelect is updated.
;		MenuSelect is updated.
;		MenuBarCoord is updated.
;
;	USES
;		None.
;
;-------------------------------------------------------------------------
cProc	SetMenuTitle, <NEAR>, <es,di,si,ax,bx>
localW	mode
localW	coord
cBegin
	mov	mode, bx
    ;
    ; Save the state
    ;
	jmp	cs:SMTJmp[bx]
SMTJmp	dw	SMTClear
	dw	SMTBar
	dw	SMTSelect
SMTBar:
	mov	ax, cs:[MenuBarCoord]	; The current selction is OK
	mov	si, cs:[MenuSelect]
	mov	cs:MenuItemSelect, NO_ITEM
	jmp	SMTStateSaved

SMTSelect:
	mov	cs:MenuSelect, si	; Used the passed one
	mov	cs:MenuBarCoord, ax
	mov	cs:MenuItemSelect, si
	jmp	SMTStateSaved

SMTClear:
	mov	ax, cs:[MenuBarCoord]	; Use and clear the current menu
	mov	si, cs:[MenuSelect]
	mov	cs:MenuBarCoord, 0
	mov	cs:MenuSelect, NO_ITEM
	mov	cs:MenuItemSelect, NO_ITEM
SMTStateSaved:
	mov	coord, ax

    ;
    ; What video attribute should you put the title to?
    ;
	mov	di, codeOffset GrabReq
	cmp	mode, MT_SELECT
	je	SMTHilite
    ;
    ;	Set to normal color
    ;
	mov	bh, LC_SYSTEMUNSELECT	; Test for gray, and system menu
	cmp	SI, [MenuTop]
	je	SMTCallSelect
	mov	BH, LC_GRAYED
	test	es:[SI].flags, MF_GRAYED
	jnz	SMTCallSelect
	mov	BH, LC_UNSELECTED
	jmp	SMTCallSelect
    ;
    ;	Set to the hilite color
    ;
SMTHilite:
	mov	BH, LC_SYSTEMSELECT	; Test for gray and system menu
	cmp	SI, [MenuTop]
	je	SMTCallSelect
	mov	BH, LC_SELECTGRAY
	test	es:[SI].flags, MF_GRAYED
	jnz	SMTCallSelect
	mov	BH, LC_SELECTED
	errn$	SMTCallSelect

SMTCallSelect:
	mov	BL,01			;menu bar flag
	call	SetSelectState
    ;
    ; Set Boarders
    ;
SMTSetBoarders:
	mov	AX, coord
	mov	ES,MenuSeg
	mov	CL,ES:[SI].twidth
	add	CL,AL
	inc	CL				;calc right edge
	mov	CS:[DI].grStyle,F_CHAR+FMT_OTHER
	mov	CS:[DI].grXorg,AL
	mov	CS:[DI].grYorg,AH
	mov	CS:[DI].grXext,1
	mov	CS:[DI].grYext,1
	mov	CS:[DI].grChar,' '              ;vertical bar
	cmp	mode, MT_BAR			;are we adding the bar?
	jne	SMT10
	cmp	si, [MenuTop]			; Yes
	je	SMT10
	test	es:[si].flags, MF_DISABLED or MF_GRAYED
	jnz	SMT10				; Don't touch dead menus
	mov	CS:[DI].grChar,VERT_BAR 	  ;blank
SMT10:
	mov	ax, cs
	mov	es, ax
	call	PutGoGrab
	mov	CS:[DI].grXorg,CL
	call	PutGoGrab
   ;
   ; Update

cEnd

;------------ Unselect ---------------------------------------------
;
; DESCRIPTION
;		Unselect the current selected item
;
; ENTRY
;		MenuItemSelect is valid
;		MenuSelect is valid
; EXIT
;		MenuItemSelect is updated
;
; USES
;		None
;
;-------------------------------------------------------------------------
Unselect:
	push	ax
	push	bx
	push	si
      ;
      ; Is there an Item selected ?
      ;
	mov	si, cs:MenuItemSelect
	cmp	si, NO_ITEM
	je	PMUPop			;If No menuitem is select do nothing
      ;
      ; Yes, act according to its type
      ;
	cmp	si, cs:MenuSelect
	je	PMUMenuTitle
	call	UnselectMenuItem
	jmp	PMUPop
PMUMenuTitle:
	mov	bx, MT_CLEAR
	cmp	cs:MenuSize, 0
	je	PMUSet
	mov	bx, MT_BAR
PMUSet:
	call	SetMenuTitle
PMUPop:
	pop	si
	pop	bx
	pop	ax
	ret


;------------------- UnselectMenuItem  ---------------------------------
;
;	DESCRIPTION
;		Unselect the current selected menu item
;
;	ENTRY
;		MenuItemSelect is valid
;		MenuCoord is valid
;
;	EXIT
;		MenuItemSelect is set to NO_ITEM
;		MenuCoord is cleared
;
;	USES
;		BX
;
;-----------------------------------------------------------------------
UnselectMenuItem PROC	NEAR
	push	ax
	push	si
	mov	si, cs:MenuItemSelect
	cmp	si, NO_ITEM
	je	UMIExit
	test	ES:[SI].Flags,MF_SEPARATOR
	jnz	UMIExit
	mov	BH,LC_UNSELECTED
	test	ES:[SI].Flags,MF_GRAYED
	jz	UM5
	mov	BH,LC_GRAYED
UM5:
	xor	BL,BL			;set menu flag
	mov	AX,CS:MenuCoord 	;get coordinates in AX
	dec	AL			;mark whole line
	dec	AL
	call	SetSelectState		;un select the current item
	mov	cs:MenuItemSelect, NO_ITEM
	mov	cs:MenuCoord, 0
UMIExit:
	pop	si
	pop	ax
	ret
UnselectMenuItem ENDP


;------------------- SelectMenuItem  ---------------------------------
;
;	DESCRIPTION
;		Selects the passed menu item
;
;	ENTRY
;		AX - the new menu item position
;		ES:SI - the new menu item
;
;	EXIT
;		MenuCoord and MenuItemSelect are updated.
;
;	USES
;		None
;
;-----------------------------------------------------------------------
SelectMenuItem	PROC	NEAR
	push	ax
	push	bx
	mov	CS:MenuCoord, AX
	mov	CS:MenuItemSelect, SI
	test	ES:[SI].Flags, MF_DISABLED or MF_SEPARATOR
	jnz	SMexit
	mov	BH,LC_SELECTED
	test	ES:[SI].Flags, MF_GRAYED
	jz	SM20
	mov	BH,LC_SELECTGRAY
SM20:
	xor	BL,BL			;set menu flag
	dec	AL			;mark whole line
	dec	AL
	call	SetSelectState		;select the new item
SMexit:
	pop	bx
	pop	ax
	ret
SelectMenuItem	ENDP


;---------------- SetSelectState ---------------------------------------
;
;	DESCRIPTION
;		This routine either sets the the menu bar item or
;		menu item to the passed attribute
;
;	ENTRY
;		BH: = logical color
;		BL: = 0 if MenuItem, <> 0 if MenuTitle
;		SI: = points to menuInfo structure to be set
;		AL: = Screen col to start at
;		AH: = Screen row to start at
;		DI: = pointer to grab request structure
;
;	EXIT
;
;
;	MODIFIES
;		AX, BX
;
;------------------------------------------------------------------------

SetSelectState	PROC	NEAR

	push	SI
	push	ES
	push	CX
	mov	CX,BX			;get flag in CL
	call	GoRealizeColor
	mov	CS:[DI].grAttr,BH	  ;put in structure
	mov	ES,MenuSeg
	mov	CS:[DI].grXorg,AL
	mov	CS:[DI].grYorg,AH
	mov	CS:[DI].grYext,1	  ;only one row high
	or	CL,CL			;MenuTitle?
	jz	SSS10
	mov	AL,ES:[SI].twidth	 ;get item width
	inc	AL
	inc	AL
	jmp	SSS20
SSS10:
	mov	AX,CS:MenuSize
	dec	AL			;account for border
	dec	AL
SSS20:
	mov	CS:[DI].grXext,AL
	mov	AL,F_ATTR+FMT_OTHER	;set style flags
	mov	CS:[DI].grStyle,AL
	push	ES
	mov	AX,CS
	mov	ES,AX			;ES:DI muust point to grab request struc
	call	PutGoGrab
	pop	ES
	cmp	CH,LC_UNSELECTED	;are we unselecting it
	jne	SSS30
	mov	AL,CS:[DI].grXorg
	mov	AH,CS:[DI].grYorg
	inc	AL
	or	CL,CL
	jnz	SSS35
	inc	AL
SSS35:
	call	SetMnemonic
SSS30:
	pop	CX
	pop	ES
	pop	SI
	ret

SetSelectState	ENDP






;====== DISPLAY CODE =====================================================
;
;  These routines display the menus and the menu bar
;
;=========================================================================


;------------- DisplayTitleBar -----------------------------------------
;
;	DESCRIPTION
;		This routine displays the title bar at the top of the
;		screen.
;
;	ENTRY
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		ES:DI: = GrabReq structure
;
;	MODIFIES
;
;--------------------------------------------------------------------------
DisplayTitleBar PROC	NEAR


	push	CS
	pop	ES
	lea	DI,GrabReq
	push	SI
	mov	BH,LC_TITLEBAR
	mov	AH,1
	mov	AL,GrabData.giCharsX
	xor	CX,CX				;origin
	mov	SI,codeOffset TitleOffset
	call	SaveDataArea
	jc	DTB80
	xor	AX,AX
	mov	SI, pWindowTitle
	call	UpdateAppTitle
	clc
	jmp	DTB90
DTB80:
	stc
DTB90:
	pop	SI
	ret

DisplayTitleBar     ENDP

;------------------- DisplaySystemIcon ----------------------------------
;
;	DESCRIPTION
;		This routine displays the system menu icon in the
;		left edge of the title bar.  This is a destructive
;		write to the screen.  No screen data is saved.
;
;	ENTRY
;		ES:DI: = pointer to grab request structure
;
;	EXIT
;
;	MODIFIES
;		AX,BX
;
;-----------------------------------------------------------------------
DisplaySystemIcon	PROC	NEAR

	push	DS
	push	SI
	mov	SI,MenuTop
	mov	DS,MenuSeg
	mov	word ptr ES:[DI].grLpData+2,DS
	mov	ES:[DI].grXorg,1
	mov	ES:[DI].grYorg,0
	mov	ES:[DI].grYext,1
	mov	AL,[SI].twidth
	mov	ES:[DI].grXext,AL
	push	SI
	add	SI,text
	mov	word ptr ES:[DI].grLpData,SI
	mov	ES:[DI].grStyle,FMT_OTHER+C_CHAR
	call	PutGoGrab
	mov	BH,LC_SYSTEMUNSELECT
	mov	BL,0FFH 			;menutitle
	xor	AX,AX				;start pos
	pop	SI
	call	SetSelectState
	pop	SI
	pop	DS
	ret

DisplaySystemIcon	ENDP



;---------------- DisplayMenuBar --------------------------------------
;
;	DESCRIPTION
;		This routine displays the menu bar at the top of
;		the screen.
;
;	ENTRY
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if failure
;		SI: = pointer to selected menu (same as MenuSelect)
;		MenuSelect is updated
;		MenuBarPos is updated
;		MenuBarRow is updated
;		ES: = MenuSeg
;
;	MODIFIES
;		All but DS
;
;-----------------------------------------------------------------------
DisplayMenuBar	PROC	NEAR


	mov	AX,CS			;set up for call
	mov	ES,AX
	mov	DI,codeOffset GrabReq
	call	DisplaySystemIcon
	call	PrepMenuBar
	jnc	DMB3
	mov	ES,MenuSeg
	jmp	DMB80			;return if error
DMB3:
	mov	SI,MenuTop
	mov	AX,MenuSeg
	mov	ES,AX			;get the menu segment in ES
	mov	SI,ES:[SI].next 	;start at first menu bar menu
	mov	BH,LC_GRAYED		;Gray just in case
	call	GoRealizeColor
	mov	CS:[DI].grAttr,BH
	mov	word ptr CS:[DI].grLpData+2,AX	;get segment in grLpData
	mov	BH,01H			;row counter initialized
	mov	BL,grabdata.giCharsX	;clear position
	mov	CS:MenuBarPos,0 	;start at pos 0
DMB5:
	or	SI,SI			;is there an item
	jnz	DMB6
	jmp	DMB70			;jump if not
DMB6:
	test	ES:[SI].flags,MF_REMOVE
	jnz	DMB30
	xor	DX,DX
	mov	DL,ES:[SI].twidth
	dec	BL			;account for padding
	dec	BL			;
	cmp	DL,BL
	jle	DMB10
	inc	BH			;next row
	mov	CS:MenuBarPos,0 	;on left edge
	mov	BL,grabdata.giCharsX	;get new row starting count
	dec	BL
	dec	BL


DMB10:
	sub	BL,DL			;decrement the number of chars
	inc	CS:MenuBarPos		;one char pos pad
	mov	CS:[DI].grXext,DL	  ;get length in Grb req struc
	mov	CS:[DI].grYorg,BH
	mov	AL,CS:MenuBarPos
	mov	CS:[DI].grXorg,AL
	add	CS:MenuBarPos,DL	;update current position
	inc	CS:MenuBarPos		;add 1 for pad on other end
	mov	CS:[DI].grYext,1
	mov	CS:[DI].grStyle,FMT_OTHER+C_CHAR
	test	ES:[SI].flags,MF_GRAYED
	jz	DMB12
	mov	CS:[DI].grStyle,C_CHAR_F_ATTR+FMT_OTHER
DMB12:
	push	SI
	add	SI,text
	mov	word ptr CS:[DI].grLpData,SI ;address of string
	pop	SI
	push	AX
	push	ES
	push	CS			;ES:DI must point to grab req for call
	pop	ES
	call	PutGoGrab
	pop	ES			;get menu seg back
	pop	AX
	mov	AH,BH
	push	BX
	call	SetMnemonic
	pop	BX
DMB30:
	mov	SI,ES:[SI].next 	;get the next one
	jmp	DMB5			;process it
DMB70:					;menu bar is up here

	clc
	jmp	DMB90

DMB80:
	stc
DMB90:
	ret

DisplayMenuBar	ENDP


;---------------------DisplayMenu ----------------------------------------
;
;	DESCRIPTION
;		This routine displays the initial menu on the screen and
;		updates all valid data items relating to the menu.
;
;	ENTRY
;		AX: = MenuPosition
;		ES:SI -> Menu Title structure
;
;	EXIT
;		CF: = 1 if error and no menu
;		CF: = 0 if OK
;		MenuItemSelect updated
;		MenuBarCoord Updated.
;		if no item is selectable MenuItemSelect = NO_ITEM
;
;	MODIFIES
;
;--------------------------------------------------------------------------
DisplayMenu	PROC	NEAR
	mov	cs:MenuBarCoord, ax
	cmp	ES:[SI].ID,SystemMenuID
	jnz	DMenu2
	call	InitializeSystemMenu
DMenu2:
	push	SI
	call	PrepMenu		;prepare the screen area
	jnc	DMenu4			;need long error jump
	jmp	DMenu80 		;abort if error
DMenu4:
	mov	AX,CS:MenuOrig		;get the origin
	inc	AH			;add a row for the border
	add	AL,3			;add border and padding
	mov	SI,ES:[SI].item 	;get first item
	mov	DI,codeOffset GrabReq
	mov	CS:[DI].grYext,1	  ;always working one row at a time
	mov	CS:[DI].grChar,196	  ;get separator char in case we need it
DMenu5:
	test	ES:[SI].flags,MF_REMOVE
	jz	DMenu6
	jmp	DMenu23
DMenu6:
	mov	CS:[DI].grYorg,AH	  ;set the row to be updated
	test	ES:[SI].flags,MF_SEPARATOR ; is this a separator
	jz	DMenu10 		;jump if not

					;set up for a separator here
	dec	AL			;adjust the column
	dec	AL
	mov	CS:[DI].grXorg,AL	  ;set the grXorg
	inc	AL			;restore the column
	inc	AL
	mov	BX,CS:MenuSize
	dec	BL			;adjust width
	dec	BL
	mov	CS:[DI].grXext,BL	  ;set the grXext
	mov	CS:[DI].grStyle,F_CHAR+FMT_OTHER  ; set to fill with -'s
	jmp	DMenu20

DMenu10:				;here to set up for normal string
	mov	CS:[DI].grXorg,AL
	mov	CL,ES:[SI].twidth
	mov	CS:[DI].grXext,CL	  ;set the grXext
	test	ES:[SI].flags,MF_GRAYED ;is the item to be grayed
	jz	DMenu12
	push	BX
	mov	BH,LC_GRAYED
	call	GoRealizeColor
	mov	CS:[DI].grAttr,BH
	pop	BX
	mov	CL,C_CHAR_F_ATTR+FMT_OTHER
	jmp	DMenu14
DMenu12:
	mov	CL,C_CHAR+FMT_OTHER
DMenu14:
	mov	CS:[DI].grStyle,CL
	mov	word ptr CS:[DI].grLpData+2,ES	  ;get segment of data
	push	SI			;save SI
	add	SI,text 		;get offset to string
	mov	word ptr CS:[DI].grLpData,SI
	pop	SI			;restore pointer

DMenu20:
	push	AX			;save the world
	push	ES
	push	CS			;get CS in ES
	pop	ES
	call	PutGoGrab
	pop	ES
	pop	AX
	push	AX
	push	BX
	call	SetMnemonic
	pop	BX
	pop	AX
	test	ES:[SI].flags,MF_CHECKED
	jz	DMenu22
	call	CheckMenuItem
DMenu22:
	inc	AH			;add a row
DMenu23:
	mov	SI,ES:[SI].next 	;get the next item
	or	SI,SI			;is there another item?
	jz	DMenu70
	jmp	DMenu5			;loop if there is

DMenu40:
	mov	CS:MenuItemSelect,NO_ITEM;show no item selected
	mov	CS:MenuCoord,0		;same here

DMenu70:
	clc
	jmp	DMenu90
DMenu80:
	stc				;show an error
	mov	CS:MenuItemSelect,NO_ITEM;show that no item can be selected
					;in this menu
DMenu90:
	pop	SI
	ret

DisplayMenu	ENDP

;--------------- ReleaseMenu ------------------------------------------
;
;	DECRIPTION
;		Release the pulled down menu
;
;	ENTRY
;		MenuSelect is valid
;
;	EXIT
;		MenuSelect is updated
;
;----------------------------------------------------------------------
ReleaseMenu	PROC	NEAR
	push	ax
	push	bx
	push	si
	mov	si, cs:MenuSelect
	cmp	si, NO_ITEM
	je	RMExit
    ;
    ; Clear the title
    ;
	mov	ax, cs:MenuBarCoord
	mov	bx, MT_CLEAR
	call	SetMenuTitle
    ;
    ; Restore the menu area
    ;
	mov	ax, cs:MenuOrig
	mov	bx, cs:MenuSize
	mov	si, cs:MenuOffset
	call	RestoreMenuArea
    ;
    ; Reset the menu variables
    ;
	xor	ax, ax
	mov	cs:MenuSize, ax
	mov	cs:MenuOrig, ax

RMExit:
	pop	si
	pop	bx
	pop	ax
	ret
ReleaseMenu	ENDP




;---------- DoMenuRestore ------------------------------------------------
;
;	DESCRIPTION
;		This routine restores what ever has to be restored for
;		DoMenu.
;
;	ENTRY
;		AX: = 0 if grabber need not be restored
;		AX: <> 0 if grabber needs restoring
;
;	EXIT
;
;	MODIFIES
;	       All but BP and seg regs
;
;---------------------------------------------------------------------------
DoMenuRestore	PROC	NEAR

	push	AX			;save the flag
	cmp	CS:MenuBarSize,0	;was there a menu bar
	jz	DMR10
	mov	AX,0100H		;origin of menu bar
	mov	BX,CS:MenuBarSize
	mov	SI,MenuBarOffset
	call	RestoreMenuArea 	;restore the screen menu bar area
DMR10:
	xor	AX,AX			;title bar in upper left corner
	mov	BH,1			;1 row high
	mov	BL,GrabData.giCharsX	;width of screen
	mov	SI,CS:TitleOffset
	call	RestoreMenuArea
	call	BadEndPaint
	pop	AX
	or	AX,AX			;restore grabber?
	jz	DMRRet
	call	EnableGrabber
DMRRet:
	ret

DoMenuRestore	ENDP



;-------------- InitializeSystemMenu --------------------------------------
;
;	DESCRIPTION
;		This routine sets the initial states of the system menu
;		items.
;
;	ENTRY
;
;	EXIT
;		Flag fields of system menu items is updated.
;		ES: = MenuSeg
;
;	MODIFIES
;
;
;---------------------------------------------------------------------------
InitializeSystemMenu	PROC	NEAR

	push	SI
	push	DI
	mov	ES,BufSeg		;get grabber buffer segment
	xor	SI,SI			;point to beginning
	cmp	OldApState, SF_TERMINATED
	je	PGray
	mov	AX,ES:[SI]
	and	AH, 01111111B
	mov	CX,MF_ENABLED		;assume good
	cmp	AX,4			; copy / paste format
	jz	PCheck
	cmp	AX,1			; text grab format
	jz	PCheck
	cmp	AX,5			;Clipboard text format
	jz	PCheck
PGray:
	mov	CX,MF_GRAYED
PCheck:
	mov	ES,MenuSeg
	mov	BX,0			;search by ID
	mov	AX,SystemMenuID
	call	GetMenuTitle
	mov	SI,AX			;SI points to system menu structure
	mov	AX,cmPaste		; find paste
	call	GetMenuItem
	jc	MinCheck
	mov	DI,AX
	mov	ES:[DI].Flags,CX
MinCheck:
	mov	DX,MF_GRAYED
	cmp	OldApState, SF_TERMINATED
	je	SetMin
	cmp	CS:fEndBlock,0
	je	SetMin
	mov	DX,MF_ENABLED
SetMin:
	mov	AX,cmbMinimize
	call	GetMenuItem
	jc	CloseCheck
	mov	DI,AX
	mov	ES:[DI].Flags,DX
CopyCheck:
	mov	DX,MF_GRAYED
	cmp	CS:fMark,0
	jz	SetCopy
	mov	DX,MF_ENABLED
SetCopy:
	mov	AX,cmCopy
	call	GetMenuItem
	jc	CloseCheck
	mov	DI,AX
	mov	ES:[DI].Flags,DX
CloseCheck:
	mov	CX,MF_GRAYED
	cmp	CS:fEndBlock,0				;close not enabled
	je	SetClose				;while marking
	cmp	oldapstate,SF_TERMINATED
	jne	SetClose
	mov	CX,MF_ENABLED
SetClose:
	mov	AX,cmbClose
	call	GetMenuItem
	jc	ISMDone
	mov	DI,AX
	mov	ES:[DI].Flags,CX
ISMDone:
	pop	DI
	pop	SI
	ret

InitializeSystemMenu	ENDP



;--------------- MenuBarDimension -------------------------------------------
;
;	DESCRIPTION
;		This routine sequences through the menu bar items and
;		determines the the screen dimensions required to hold
;		all of the items.
;
;	ENTRY
;
;	EXIT
;		AH: = Height of menu bar
;		AL: = Width of menu bar (should be screen width)
;
;	MODIFIES
;		AX, BX, CX, DX
;
;----------------------------------------------------------------------------

MenuBarDimension	PROC	NEAR

	push	SI
	push	DI
	push	ES
	mov	AX,MenuSeg
	mov	ES,AX			;get Menu Segment in ES
	mov	DI,MenuTop		;get pointer to first menu (system)
	xor	AX,AX			;in case of error
	cmp	DI,0FFFFH		;is there a system menu
	je	MBD80
	xor	CX,CX			;
	xor	BX,BX			; # cols left in line
	xor	DX,DX			; position of this item in line
	mov	DI,ES:[DI].next 	;get first menu bar item
	or	DI,DI
	jz	MBD70
MBD5:
	test	ES:[DI].flags,MF_REMOVE ;has the item been removed?
	jnz	MBD20			;then don't count it
	xor	AX,AX
	mov	AL,ES:[DI].twidth
	inc	AX
	inc	AX			;add two for cushion on either end
	cmp	AX,BX
	jg	MBD10
	sub	BX,AX			;decrement the number of cols avail
	jmp	MBD20
MBD10:
	inc	CX			;add a row
	xor	BX,BX
	mov	BL,grabdata.giCharsX	;get the space in a row
	sub	BX,AX			;decrement
MBD20:
	mov	DI,ES:[DI].next 	;get the next item
	or	DI,DI			;is this the end
	jz	MBD70
	jmp	MBD5

MBD70:
	xor	AX,AX			;assume no menus
	or	CL,CL			;any menus?
	jz	MBD80
	mov	AH,CL			;get number of rows
	mov	AL,grabdata.giCharsX	;menu bar is always screen width
MBD80:
	pop	ES
	pop	DI
	pop	SI
	ret

MenuBarDimension	ENDP

;---------------- GetScreenDimension ----------------------------------
;
;	DESCRIPTION
;		This routine calls the grabber to retrieve the current
;		text dimensions of the screen.	This is needed to display
;		the menu bar correctly.
;
;	ENTRY
;
;	EXIT
;		AH: = Height of current text screen
;		AL: = Width of current text screen
;		AX: = 0 if menus can not be used
;
;	MODIFIES
;		All but DS
;
;-----------------------------------------------------------------------
GetScreenDimension	PROC	NEAR
	mov	AX,DS
	mov	ES,AX
	lea	DI, GrabData
	push	DI
	push	ES
	mov	AX,GRAB_GETINFO
	mov	SI,InquireGrab
	call	GoGrab			;make call to the grabber
	pop	ES
	pop	DI
	or	AX,AX
	jz	GSD90			;return zeros if no menus
	mov	AL,ES:[DI].giCharsX	;get the number of columns
	mov	AH,ES:[DI].giCharsY	;put the rows in AH
GSD90:
	ret

GetScreenDimension	ENDP

;---------------- SaveDataArea --------------------------------------
;
;	DESCRIPTION
;		This routine saves the area defined into the copyseg.
;		It updates the appropriate pointer to show where
;		the data was saved.
;
;	ENTRY
;		AH: = 1 based height
;		AL: = 1 based width
;		CH: = 0 based Y origin
;		CL: = 0 based X origin
;		BH: = logical color to fill area with
;		CS:SI: = points to data location to save pointer if
;		      successful
;
;	EXIT
;		CF: = success
;		data area at SI updated
;
;	MODIFIES
;		AX,CX
;
;-----------------------------------------------------------------------
SaveDataArea	PROC	NEAR

	push	DI
	push	ES
	push	CS
	pop	ES
	lea	DI,GrabReq
	xor	DX,DX
	mov	word ptr ES:[DI].grLpData,DX	  ;get size first
	mov	word ptr ES:[DI].grLpData+2,DX
	mov	ES:[DI].grXorg,CL
	mov	ES:[DI].grYorg,CH
	mov	ES:[DI].grXext,AL
	mov	ES:[DI].grYext,AH
	mov	ES:[DI].grStyle,FMT_NATIVE
	call	GoGetGrab
	call	CheckSize
	jg	SDA80				;not big enough, error
	mov	DX,CopySeg
	mov	word ptr ES:[DI].grLpData+2,DX
	mov	DX,CS:CopyOffset
	mov	CS:[SI],DX
	mov	word ptr ES:[DI].grLpData,DX
	call	GoGetGrab			;save the data
	add	DX,AX				;find end
	mov	CS:CopyOffset,DX
	call	GoRealizeColor			;get the color
	mov	ES:[DI].grAttr,BH
	mov	ES:[DI].grChar,' '                ;fill with spaces
	mov	ES:[DI].grStyle,FMT_OTHER+F_BOTH
	call	PutGoGrab
	clc
	jmp	SDA90
SDA80:
	stc
SDA90:
	pop	ES
	pop	DI
	ret

SaveDataArea	ENDP


;--------------- CheckSize ---------------------------------------
;
;	DESCRIPTION
;		This routine checks to see if the Copybuffer is large
;		enough to hold the data to be copied.
;
;	ENTRY
;		AX: = Size needed in bytes
;		CS:CopyOffset: = Offset into CopySeg that data can be
;				 copied into
;		CS:CopySize: = total size of Copy segment
;
;	EXIT
;		Flags set to show if size is above or below
;
;	MODIFIES
;
;-----------------------------------------------------------------------
CheckSize	PROC	NEAR

	push	BX
	mov	BX,CopySize
	sub	BX,CS:CopyOffset
	cmp	AX,BX
	pop	BX
	ret

CheckSize	ENDP

;---------------- PrepMenuBar ---------------------------------------
;
;	DESCRIPTION
;		This routine preps the menu bar at the top of the
;		screen.  The data under the menu bar is saved prior
;		to writing to this area.
;
;	ENTRY
;
;
;	EXIT
;		CF: = 0 if successful
;		CF: = 1 if unsuccessful
;		ES:DI -> Grab Request Structure
;		MenuBarSize updated
;
;	MODIFIES
;		ES, AX, BX, CX, DX, DI
;
;------------------------------------------------------------------------

PrepMenuBar  PROC    NEAR


	push	SI
					;first save the menu bar area

	call	MenuBarDimension	;get the menu bar dimensions
	mov	CS:MenuBarSize,AX
	or	AX,AX			;uis there a menu bar
	jz	PMB90
	mov	BH,LC_UNSELECTED
	mov	CX,0100H		;menubar position
	mov	SI,codeOffset MenuBarOffset
	call	SaveDataArea
	clc
	jmp	PMB95
PMB90:
	stc
PMB95:
	pop	SI
	ret				;carry flag alredy correct

PrepMenuBar	ENDP


;--------------UpdateAppTitle--------------------------------------------
;
;	DESCRIPTION
;		This routine displays the application title on the
;		 in the center of the title bar
;
;	ENTRY
;		AX: = 0 means display the title passed
;		AX: <> 0 means clear the area the title would occupy
;		DS:SI: = pointer to title string (null terminated)
;		ES:DI: = pointer to GrabRequest data structure
;		GrabData values are valid
;
;	EXIT
;		Application Title Displayed
;
;	MODIFIES
;		AX, BX
;
;-------------------------------------------------------------------------
UpdateAppTitle PROC    NEAR

	push	SI
	mov	ES:[DI].grChar,' '
	mov	BH,LC_APPTITLE			;assume copy characters
	mov	ES:[DI].grStyle,FMT_OTHER+C_CHAR_F_ATTR
	or	AX,AX
	jz	UAT10
	mov	BH,LC_TITLEBAR
	mov	ES:[DI].grStyle,FMT_OTHER+F_BOTH
UAT10:
	call	GoRealizeColor
	mov	ES:[DI].grAttr,BH
	pop	SI
	push	ES
	push	DS
	pop	ES
	call	StringLength
	pop	ES
	or	AX,AX
	jz	UATRet
	mov	BL,GrabData.giCharsX
	shr	BL,1				;half the screen
	mov	BH,AL
	shr	BH,1				;half the width of the string
	sub	BL,BH
	mov	ES:[DI].grXorg,BL
	mov	ES:[DI].grYorg,0
	mov	ES:[DI].grYext,1
	mov	ES:[DI].grXext,AL
	mov	word ptr ES:[DI].grLpData+2,DS
	mov	word ptr ES:[DI].grLpData,SI
	call	PutGoGrab
UATRet:
	ret

UpdateAppTitle ENDP

;------------------ RestoreMenuArea ----------------------------------------
;
;	DESCRIPTION
;		This routine restores the screen under the menu Area
;		to it's premenu state.
;
;	ENTRY
;		DI: = pointer to Grab Request structure
;		CS: = Grab request segment
;		SI: = offset into CopySeg to start from
;		AL: = X origin
;		AH: = Y origin
;		BL: = width of area
;		BH: = height of area
;	EXIT
;
;	MODIFIES
;		AX, BX
;
;---------------------------------------------------------------------------

RestoreMenuArea 	PROC	NEAR

	or	BX,BX			;is there anything to restore?
	jz	RMARet
	push	SI
	push	ES
	mov	CS:[DI].grXorg,AL	  ;get origin
	mov	CS:[DI].grYorg,AH
	mov	AX,CopySeg		;get data pointer
	mov	word ptr CS:[DI].grLpData+2,AX
	mov	word ptr CS:[DI].grLpData,SI
	xor	AL,AL
	or	AL,FMT_NATIVE		;native mode
	mov	CS:[DI].grStyle,AL
	mov	CS:[DI].grYext,BH	;get extents
	mov	CS:[DI].grXext,BL
	push	CS
	pop	ES
	call	PutGoGrab
	pop	ES
	pop	SI
	mov	CS:CopyOffset,SI	;update to new offset for next save
RMARet:
	ret

RestoreMenuArea 	ENDP


;--------------- SetMnemonic ---------------------------------------------
;
;	DESCRIPTION
;		This routine sets the correct attribute to display the
;		mnemonic.
;
;	ENTRY
;		AX: = position of first char of item text
;		ES:SI: = pointer to menu info structure
;		CS:DI: = pointer to grabrequest structure
;
;	EXIT
;		Mnemonic is marked if there is one
;
;	MODIFIES
;		AX, BX
;
;-------------------------------------------------------------------------
SetMnemonic	PROC	NEAR

	test	ES:[SI].flags,MF_GRAYED or MF_DISABLED or MF_REMOVE or MF_SEPARATOR
	jnz	SMRet
	cmp	ES:[SI].mnempos,0FFH	;is there a mnemonic
	je	SMRet
	add	AL,ES:[SI].mnempos
	mov	CS:[DI].grXorg,AL
	mov	BH,LC_MNEMONIC
	call	GoRealizeColor
	mov	CS:[DI].grAttr,BH
	mov	CS:[DI].grXext,1
	mov	CS:[DI].grStyle,FMT_OTHER+F_ATTR
	push	ES
	mov	AX,CS
	mov	ES,AX
	call	PutGoGrab
	pop	ES

SMRet:
	ret

SetMnemonic	ENDP
;------------------- HighlightFirstMenu ---------------------------------
;
;	DESCRIPTION
;		This routine highlights the first item on the menubar.
;		It also selects this item as the Selected menu and sets
;		its coordinates.
;
;	ENTRY
;		ES: = Valid MenuSeg
;
;	EXIT
;		MenuSelect updated
;		MenuBarCoord updated
;
;	MODIFIES
;		AX, BX, SI
;
;---------------------------------------------------------------------------
HighlightFirstMenu	PROC	NEAR
	mov	SI,MenuTop
	xor	AX,AX			;coordinates of system menu
	call	GetNextMenuTitle
	mov	bx, MT_SELECT
	call	SetMenuTitle
HFMExit:
	ret
HighlightFirstMenu	ENDP


;------------ HiliteFirstMenuItem --------------------------
;
;	DESCRIPTION
;		High lights the first item in the current menu.
;
;	ENTRY
;		MenuSelect is valid
;		MenuOrig is valid
;
;	EXIT
;		ES:SI - First menu item
;		MenuItemSelect and MenuCoord are updated
;
;	USES
;		None
;
;-----------------------------------------------------------
HiliteFirstMenuItem PROC NEAR
	push	ax
	push	bx
	mov	bx, MT_BAR		; The title must be bared
	call	SetMenuTitle
					;now we highlight first avail item
	mov	AX,CS:MenuOrig		;get pointer to top
	inc	AH			;allow for border
	add	AL,3			;allow for border and padding
	mov	SI,CS:MenuSelect	;get the menu title structure
	mov	SI,ES:[SI].item 	;get the first item
HFILoop:
	test	ES:[SI].flags, MF_REMOVE
	jnz	HFINext
	test	ES:[SI].flags, MF_SEPARATOR
	jz	HFI30			;OK, found the first avail item
	inc	AH			;add a row
HFINext:
	mov	SI,ES:[SI].next
	or	SI,SI			;is there an item here
	jz	HFIRetError		;no, go report
	jmp	HFILoop
HFI30:
	call	SelectMenuItem
	jmp	HFIRet
HFIRetError:
	mov	CS:MenuItemSelect, NO_ITEM
	mov	CS:MenuCoord, 0
HFIRet:
	pop	bx
	pop	ax
	ret
HiliteFirstMenuItem ENDP



;--------------- PrepMenu --------------------------------------------
;
;	DESCRIPTION
;		This routine is similar to PrepMenuBar in that it prepares
;		the screen area for a drop down menu.
;
;	ENTRY
;		CS:DI -> Grab request structure
;		ES:SI -> Menu title structure to be dropped down
;
;	EXIT
;		MenuSize updated
;
;	MODIFIES
;		AX, BX, CX
;
;-----------------------------------------------------------------------

PrepMenu	PROC	NEAR

	push	SI
	push	ES
	call	MenuDimension		;get the dimensions of the menu
	or	AX,AX
	jz	PMenu80 		;error if menu of zero size
PMenu5:
	add	AL,5			;add padding + border
	add	AH,2			;add borders
	mov	CS:RealMenuSize,AX	;set original size (needed for DrawBord)
	mov	CS:MenuSize,AX		;save the size
	push	AX
	mov	BX,MT_SELECT		;select it
	mov	AX,CS:MenuBarCoord	;get coordinates
	call	SetMenuTitle
	pop	AX
	push	CS
	pop	ES			;get CS in ES
	call	MenuPosition		;get the origin of the menu
	mov	CS:MenuOrig,BX		;save the location
	mov	CH,BH			;make sure it fits
	add	CH,AH
	cmp	CH,GrabData.giCharsY
	jle	PMenu10
	mov	CH,GrabData.giCharsY
	sub	CH,BH
	mov	AH,CH
	mov	CS:MenuSize,AX
PMenu10:
	mov	CX,BX
	mov	BH,LC_UNSELECTED
	mov	SI,codeOffset MenuOffset
	call	SaveDataArea
	jc	PMenu80 		;error if carry set
	call	DrawMenuBorder
	clc				;show success
	jmp	PMenu90

pMenu80:
	stc				;an error took place
pMenu90:
	pop	ES
	pop	SI
	ret

PrepMenu	ENDP


;--------------- DrawMenuBorder ----------------------------------------
;
;	DESCRIPTION
;		This routine draws a box around the menu after it has been
;		prepared on the screen
;
;	ENTRY
;		MenuOrig is valid
;		MenuSize is valid
;
;	EXIT
;
;	MODIFIES
;
;-------------------------------------------------------------------------
DrawMenuBorder	PROC	NEAR

	push	SI
	push	DI
	push	ES
	push	BX
	push	CX
	push	DX
	mov	DI,codeOffset GrabReq
	push	CS
	pop	ES			;get CS in ES
	mov	ES:[DI].grStyle,FMT_OTHER+F_CHAR
	mov	BX,CS:MenuOrig		;get the origin
	mov	CX,CS:MenuSize		;get the size
	cmp	CX,CS:RealMenuSize	;Is the menu too long for the screen?
	jz	DrawBord10		;no, then skip this next line.
	inc	CH			;adding one to CH causes all PUTBLOCK
					;calls for the bottom row to fail since
					;it will be out of range. It will also
					;extend the vertical bars to the bottom
					;of the screen
DrawBord10:

					;draw corners
	mov	ES:[DI].grChar,UL_CORNER  ;upper left
	mov	ES:[DI].grXorg,BL
	mov	ES:[DI].grYorg,BH
	mov	ES:[DI].grXext,1
	mov	CS:[DI].grYext,1
	call	PutGoGrab
	mov	ES:[DI].grChar,UR_CORNER  ;upper right
	mov	DL,BL
	add	DL,CL
	dec	DL
	mov	ES:[DI].grXorg,DL
	call	PutGoGrab
	mov	ES:[DI].grChar,LR_CORNER    ;lower right
	mov	DH,BH
	add	DH,CH
	dec	DH
	mov	ES:[DI].grYorg,DH
	call	PutGoGrab
	mov	ES:[DI].grChar,LL_CORNER    ;lower left
	mov	ES:[DI].grXorg,BL
	call	PutGoGrab

					;draw horizontal lines
	mov	ES:[DI].grChar,HORZ_BAR   ;horizontal bar
	mov	DL,BL
	mov	DH,CL
	inc	DL			;start pos
	dec	DH			;length
	dec	DH
	mov	ES:[DI].grXorg,DL
	mov	ES:[DI].grXext,DH
	mov	ES:[DI].grYorg,BH
	call	PutGoGrab
	mov	DL,BH
	add	DL,CH
	dec	DL
	mov	ES:[DI].grYorg,DL
	call	PutGoGrab
					;draw vertical lines
	mov	ES:[DI].grChar,VERT_BAR   ;vertical bar
	mov	DL,BH
	mov	DH,CH
	inc	DL			;start pos
	dec	DH			;length
	dec	DH
	mov	ES:[DI].grXorg,BL
	mov	ES:[DI].grYorg,DL
	mov	ES:[DI].grXext,1
	mov	ES:[DI].grYext,DH
	call	PutGoGrab
	mov	DL,BL
	add	DL,CL
	dec	DL
	mov	ES:[DI].grXorg,DL
	call	PutGoGrab


	pop	DX
	pop	CX
	pop	BX
	pop	ES
	pop	DI
	pop	SI
	ret

DrawMenuBorder	ENDP



;--------------- MenuDimension ------------------------------------------
;
;	DESCRIPTION
;		This routine returns the dimensions of the popup menu
;		pointed to by the menu title structure given.
;
;	ENTRY
;		ES:SI -> Menu Title structure of popup
;
;	EXIT
;		AH: = number of rows (no border)
;		AL: = number of cols (no border)
;
;	MODIFIES
;		AX
;
;------------------------------------------------------------------------

MenuDimension	PROC	NEAR

	push	SI
	mov	SI,ES:[SI].item 	;get pointer to first item
	xor	AX,AX			;init counters
MDim10:
	or	SI,SI			;is this the end?
	jz	MDim50			;jump if so
	test	ES:[SI].flags,MF_REMOVE
	jnz	MDim30
	cmp	ES:[SI].twidth,AL	 ;cmp to current max
	jle	MDim20
	mov	AL,ES:[SI].twidth	 ;get new width
MDim20:
	inc	AH			;add a row per menu item
MDim30:
	mov	SI,ES:[SI].next
	jmp	MDim10

MDim50:
	pop	SI
	ret

MenuDimension	ENDP



;--------------- MenuPosition ---------------------------------------
;
;	DESCRIPTION
;		This routine calculates the position (upper left)
;		of the menu to be displayed.
;
;	ENTRY
;		AX: = Menu Dimensions
;		MenuBarCoord must be valid for the popup
;		ScreenCols must be valid
;
;	EXIT
;		BH: = row
;		BL: = col
;
;	MODIFIES
;		BX
;
;----------------------------------------------------------------------

MenuPosition	PROC	NEAR

	push	CX
	mov	BX,CS:MenuBarCoord	;get position of the menu title
	inc	BH			;start one row lower
	mov	CL,grabdata.giCharsX	;get number of avail cols
	push	BX			;save pos
	add	BL,AL			;add the rows needed
	cmp	CL,BL			;is there room
	jge	MPos20
	sub	CL,BL			;get the difference
	pop	BX
	sub	BL,CL
	jmp	MPos30

MPos20:
	pop	BX			;OK so get stack right and result

MPos30:
	pop	CX
	ret

MenuPosition	ENDP

;--------------------CheckMenuItem--------------------------------------
;
;	DESCRIPTION
;		This routine displays a check amrk (of some sort) next
;		to the item specified.
;
;	ENTRY
;		AX: = location of the start of the item text
;		ES:DI = GrabRequest structure
;
;	EXIT
;		Item is checked
;
;	MODIFIES
;		GrabRequest structure
;
;--------------------------------------------------------------------------
CheckMenuItem	PROC	NEAR

	push	AX
	dec	AL			;position for check
	dec	AL
	mov	ES:[DI].grXorg,AL
	mov	ES:[DI].grYorg,AH
	mov	ES:[DI].grXext,1
	mov	ES:[DI].grYext,1
	mov	ES:[DI].grStyle,FMT_OTHER+F_CHAR
	mov	ES:[DI].grChar,CHECK_CHAR
	call	PutGoGrab
	pop	AX
	ret

CheckMenuItem	ENDP



;--------------- BadBeginPaint --------------------------------------
;
;	DESCRIPTION
;		This routine simply calls the grabber with the
;		Begin Paint code
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;		C Standard
;
;--------------------------------------------------------------------
BadBeginPaint	PROC	NEAR

	push	SI
	mov	SI,InquireGrab
	mov	AX,GRAB_BEGINPAINT
	call	GoGrab
	pop	SI
	ret

BadBeginPaint	ENDP




;--------------- BadEndPaint -----------------------------------------
;
;	DESCRIPTION
;		This routine simply calls the grabber with the
;		End Paint code
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;		C Standard
;
;--------------------------------------------------------------------
BadEndPaint   PROC    NEAR

	push	SI
	mov	SI,InquireGrab
	mov	AX,GRAB_ENDPAINT
	call	GoGrab
	pop	SI
	ret

BadEndPaint   ENDP

;-------------- PutGoGrab -------------------------------------------------
;
;	DESCRIPTION
;	    This routine simply calls the PutBlock routine in the grabber.  The
;	    code is here due to the frequency of this call.
;
;	ENTRY
;		ES:DI -> filled grabrequest structure
;
;	EXIT
;		Put Block executed (error codes in AX)
;
;	MODIFIES
;		AX
;
;---------------------------------------------------------------------------
PutGoGrab	PROC	NEAR
	push	SI
	mov	AX,SM_HIDE_CURSOR
	call	SetMouse
	mov	SI,InquireGrab
	mov	AX,GRAB_PUTBLOCK
	push	ES
	push	DI
	call	GoGrab
	pop	DI
	pop	ES
	push	AX
	mov	AX,SM_SHOW_CURSOR
	call	SetMouse
	pop	AX
	pop	SI
	ret
PutGoGrab	ENDP



;--------------- GoGetGrab ----------------------------------------
;
;	DESCRIPTION
;		Calls GoGrab with GetBlock call.
;
;	ENTRY
;		ES:DI: = pouinter to filled GrabRequest strcture
;
;	EXIT
;		AX: = size of data obtained
;
;	MODIFIES
;		AX
;
;------------------------------------------------------------------
GoGetGrab	PROC	NEAR

	push	SI
	push	DI
	push	ES
	mov	AX,SM_HIDE_CURSOR
	call	SetMouse
	mov	SI,InquireGrab
	mov	AX,GRAB_GETBLOCK
	call	GoGrab
	push	AX
	mov	AX,SM_SHOW_CURSOR
	call	SetMouse
	pop	AX
	pop	ES
	pop	DI
	pop	SI
	ret

GoGetGrab	ENDP



;---------------- GoRealizeColor -----------------------------------
;
;	DESCRIPTION
;		Calls GoGrab with a realizecolor call
;
;	ENTRY
;		BH: = logical Color
;
;	EXIT
;		BH: = Physical Color
;
;	MODIFIES
;		BX
;
;-----------------------------------------------------------------------
GoRealizeColor	PROC	NEAR

	push	AX
	push	SI
	mov	AX,GRAB_REALIZECOLOR
	mov	SI,InquireGrab
	call	GoGrab
	pop	SI
	pop	AX
	ret

GoRealizeColor	ENDP


;--------------- EnableGrabber ----------------------------------------
;
;	DESCRIPTION
;		This routine calls the grabber to re enable itself
;		if it has been previously disabled
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;		AX, BX, DX
;
;-----------------------------------------------------------------------

EnableGrabber	PROC	NEAR
	push	SI
	push	DI
	cmp	GrabStatus,0		;is grabber enabled
	jne	EGRet

	inc	GrabStatus		;inc grabber install count
	xor	AX,AX
	mov	ES,BufSeg		;set buffer segment in case
	cmp	[GrabVersion],0201H
	jb	DoOldGrab
	test	[pifBehavior], UGLYBITS ; Are we ugly?
	jnz	DoOldGrab
	mov	ES,AX			;call enable with address = 0
	inc	CS:fWOAGrab		;set the int 9 flag

DoOldGrab:
	mov	SI,EnableGrab		;restore grabber
	xor	DI,DI
	mov	AX,word ptr BufSize
	mov	DX,word ptr BufSize+2
	call	GoGrab
EGRet:
	pop	DI
	pop	SI
	ret

EnableGrabber	ENDP


;-------------- DisableGrabber ---------------------------------------------
;
;	DESCRIPTION
;		This routine disables the grabber if needed.
;
;	ENTRY
;
;	EXIT
;
;	MODIFIES
;		AX, DX
;
;-----------------------------------------------------------------------------
DisableGrabber	PROC	NEAR

	push	SI
	cmp	GrabStatus,1		;is grabber enabled
	jnz	DGRet

	dec	GrabStatus		;decrement the grab install count
	cmp	[GrabVersion],0201H
	jb	DoOldDisable
	test	[pifBehavior], UGLYBITS ; Are we ugly?
	jnz	DoOldDisable
	mov	CS:fWOAGrab,0		 ;clear the int 9 flag

DoOldDisable:
	mov	SI,DisableGrab
	call	GoGrab			;disable the grabber
DGRet:
	pop	SI
	ret

DisableGrabber	ENDP


;------------------- MyUpper ------------------------------------------
;
;	DESCRIPTION
;		This routine converts the character passed to it's upper
;		case equivalent if needed.
;
;	ENTRY
;		AL: = char to be converted
;
;	EXIT
;		AL: = converted char
;
;	MODIFIES
;		AX
;
;-----------------------------------------------------------------------
MyUpper PROC	NEAR

	cmp	AL,97			;lower case a
	jb	MURet			;nothing to convert below this
	cmp	AL,122
	ja	MUSpecial		;check special (local) chars
	sub	AL,32			;adjust char
	jmp	MURet

MUSpecial:
	cmp	AL,129			;lower umlaut u
	jnz	MUS1
	add	AL,25
	jmp	MURet
MUS1:
	cmp	AL,130			;lower e accent
	jnz	MUS2
	add	AL,14
	jmp	MURet
MUS2:
	cmp	AL,132			;lower umlaut a
	jnz	MUS3
	add	AL,10
	jmp	MURet
MUS3:
	cmp	AL,148			;lower umlaut o
	jnz	MUS4
	add	AL,5
	jmp	MURet
MUS4:
	cmp	AL,164			;lower tilde n
	jnz	MURet
	inc	AL
MURet:
	ret

MyUpper ENDP



;------------------ DoBeep ----------------------------------------------
;
;	DESCRIPTION
;	    Generate a two tone 'blip' sound The classic Microsoft error tone
;
;	ENTRY
;	     none
;	EXIT
;	     none
;	MODIFIES
;	     AX, CX
;
;---------------------------------------------------------------------------
DoBeep		proc	near
	mov	al,0B6H 		;select timer 2
	out	43h,al
	mov	ax,0533H		;divisor for 1000 hz
	out	42h,al			;write timer 2 count - lsb
	mov	al,ah
	out	42h,al			;write timer 2 count - msb
	jmp	short beep1
beep1:	in	al,61h			;get current setting of port
	mov	ah,al			;save setting
	or	al,00000011b		;turn speaker on
	out	61h,al
	mov	cx,2400H		;wait awhile
beep2:	loop	beep2
	mov	cx,0633H		;new tone
	mov	al,cl
	out	42H,al
	mov	al,ch
	out	42H,al
	mov	cx,3400H		;wait again
beep3:	loop	beep3
	mov	al,ah
	out	61h,al
	ret
DoBeep		endp

sEnd	code
	END

