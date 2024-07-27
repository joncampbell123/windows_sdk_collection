page	,132
title	**	Execute Macro operations

	.xlist
	include cmacros.inc
	NOTEXT		equ	1
	NORASTEROPS	equ	1
	NOMB		equ	1
	NOSYSMETRICS	equ	1
	include windows.inc
	include execmac.inc
	include tty.inc
	include error.inc
	include vkoem.inc
	.list

	NOMACROINFO	equ	1
	include macro.inc

; Windows functions

	externFP	PostMessage
	externFP	SendMessage
	externFP	GetFocus

; WinOldAp functions

	externNP	ActivateNextWindow
	externNP	AddKey
	externNP	CheckInMacroString
	externNP	CheckOutMacroString
	externNP	DDEFinishMacro
	externNP	FinishSwitchMacro
	externNP	FinishExitMacro
	externNP	DoCopy
	externNP	DoMark
	externNP	EnableGrabber
	externNP	LockMacroSeg
	externNP	MacroUpdateMenu
	externNP	StartPaste
	externNP	ResetWinText
	externNP	RestoreMark
	externNP	SelectRect
	externNP	SetScrollText
	externNP	TranslateKey
	externNP	TTYCopy
	externNP	TTYPaste
	externNP	TTYSelect
	externNP	UnLockMacroSeg

; WinOldAp data

	externB 	PIFBehavior		;tells if good or bad app
	externW 	hTTYWnd 		;handle to apps window
	externW 	hDataX			;good app clipboard data
	externB 	fScroll 		;good app scroll flag
	externB 	fSwitch 		;bad app switch flag
	externW 	fMark
	externW 	fEndBlock
	externW 	fMenu
	externB 	fMarkAbort
	externW 	OldApState		;State of the application

sBegin	data

WOA_Ops 	db	5,	'ABORT',        OPABORT
		db	5,	'CLOSE',        OPCLOSE
		db	4,	'COPY',         OPCOPY
		db	5,	'MACRO',        OPMACRO
		db	4,	'MARK',         OPMARK
		db	8,	'MAXIMIZE',     OPMAXIMIZE
		db	4,	'MENU',         OPMENU
		db	8,	'MINIMIZE',     OPMINIMIZE
		db	4,	'NULL',         OPNULL
		db	5,	'PASTE',        OPPASTE
		db	7,	'RESTORE',      OPRESTORE
		db	8,	'SCANCODE',     OPSCANCODE
		db	6,	'SCROLL',       OPSCROLL
		db	9,	'SHOWTITLE',    OPSHOWTITLE
		db	6,	'SWITCH',       OPSWITCH
		db	0

WOA_Op_Params	db	6,	'SCREEN',       PRSCREEN
		db	2,	'ON',           PR_ON
		db	3,	'OFF',          PR_OFF
		db	0

WOA_Op_MF_Params db	 9,	 'MF_GRAYED',    PRMF_GRAYED
		db	10,	'MF_ENABLED',   PRMF_ENABLED
		db	11,	'MF_DISABLED',  PRMF_DISABLED
		db	10,	'MF_CHECKED',   PRMF_CHECKED
		db	12,	'MF_UNCHECKED', PRMF_UNCHECKED
		db	0

Key_Equivalents db	5,	'BREAK',        KYBREAK
		db	4,	'DOWN',         KYDOWN
		db	3,	'END',          KYEND
		db	5,	'ENTER',        KYENTER
		db	3,	'ESC',          KYESC
		db	3,	'F10',          KYF10
		db	3,	'F11',          KYF11
		db	3,	'F12',          KYF12
		db	2,	'F1',           KYF1
		db	2,	'F2',           KYF2
		db	2,	'F3',           KYF3
		db	2,	'F4',           KYF4
		db	2,	'F5',           KYF5
		db	2,	'F6',           KYF6
		db	2,	'F7',           KYF7
		db	2,	'F8',           KYF8
		db	2,	'F9',           KYF9
		db	4,	'HOME',         KYHOME
		db	4,	'LEFT',         KYLEFT
		db	3,	'NUM',          KYNUM
		db	4,	'PGDN',         KYPGDN
		db	4,	'PGUP',         KYPGUP
		db	5,	'PRTSC',        KYPRTSC
		db	5,	'RIGHT',        KYRIGHT
		db	6,	'SCROLL',       KYSCROLL
		db	3,	'SYS',          KYSYS
		db	3,	'TAB',          KYTAB
		db	2,	'UP',           KYUP
		db	0

;IBM PC ScanCodes for ASCII chars
;  index into this table is the ASCII value for a character - 033H (='!')
ASCIItoPCScan	db	02H	;!
		db	29H	;"
		db	04H	;#
		db	05H	;$
		db	06H	;%
		db	08H	;&
		db	30H	;'
		db	0AH	;(
		db	0BH	;)
		db	09H	;*
		db	0DH	;+
		db	33H	;,
		db	0CH	;-
		db	34H	;.
		db	35H	;/
		db	0BH	;0
		db	02H	;1
		db	03H	;2
		db	04H	;3
		db	05H	;4
		db	06H	;5
		db	07H	;6
		db	08H	;7
		db	09H	;8
		db	0AH	;9
		db	27H	;:
		db	27H	;;
		db	33H	;<
		db	0DH	;=
		db	34H	;>
		db	35H	;?
		db	03H	;@
		db	1EH	;A
		db	30H	;B
		db	2EH	;C
		db	20H	;D
		db	12H	;E
		db	21H	;F
		db	22H	;G
		db	23H	;H
		db	17H	;I
		db	24H	;J
		db	25H	;K
		db	26H	;L
		db	32H	;M
		db	31H	;N
		db	18H	;O
		db	19H	;P
		db	10H	;Q
		db	13H	;R
		db	1FH	;S
		db	14H	;T
		db	16H	;U
		db	2FH	;V
		db	11H	;W
		db	2DH	;X
		db	15H	;Y
		db	2CH	;Z
		db	1AH	;[
		db	28H	;\
		db	1BH	;]
		db	07H	;^
		db	0CH	;_
		db	29H	;`
		db	1EH	;a
		db	30H	;b
		db	2EH	;c
		db	20H	;d
		db	12H	;e
		db	21H	;f
		db	22H	;g
		db	23H	;h
		db	17H	;i
		db	24H	;j
		db	25H	;k
		db	26H	;l
		db	32H	;m
		db	31H	;n
		db	18H	;o
		db	19H	;p
		db	10H	;q
		db	13H	;r
		db	1FH	;s
		db	14H	;t
		db	16H	;u
		db	2FH	;v
		db	11H	;w
		db	2DH	;x
		db	15H	;y
		db	2CH	;z
		db	1AH	;{
		db	28H	;|
		db	1BH	;}
		db	29H	;~

;index into this table is an IBM PC ScanCode
;result is a Windows Virtual KeyCode

PCtoWinVK	db	VK_ESCAPE	;esc
		db	'1'             ;1 !
		db	'2'             ;2 @
		db	'3'             ;3 #
		db	'4'             ;4 $
		db	'5'             ;5 %
		db	'6'             ;6 ^
		db	'7'             ;7 &
		db	'8'             ;8 *
		db	'9'             ;9 (
		db	'0'             ;0 )
		db	VK_OEM_MINUS	;- _
		db	VK_OEM_PLUS	;= +
		db	VK_BACK 	;bksp
		db	VK_TAB		;tab
		db	'Q'             ;q
		db	'W'             ;w
		db	'E'             ;e
		db	'R'             ;r
		db	'T'             ;t
		db	'Y'             ;y
		db	'U'             ;u
		db	'I'             ;i
		db	'O'             ;o
		db	'P'             ;p
		db	VK_OEM_4	;[ {
		db	VK_OEM_6	;] }
		db	VK_RETURN	;enter
		db	VK_CONTROL	;ctrl
		db	'A'             ;a
		db	'S'             ;s
		db	'D'             ;d
		db	'F'             ;f
		db	'G'             ;g
		db	'H'             ;h
		db	'J'             ;j
		db	'K'             ;k
		db	'L'             ;l
		db	VK_OEM_1	;; :
		db	VK_OEM_7	;' "
		db	VK_OEM_3	;` ~
		db	VK_SHIFT	;lshift
		db	VK_OEM_5	;\ |
		db	'Z'             ;z
		db	'X'             ;x
		db	'C'             ;c
		db	'V'             ;v
		db	'B'             ;b
		db	'N'             ;n
		db	'M'             ;m
		db	VK_OEM_COMMA	;, <
		db	VK_OEM_PERIOD	;. >
		db	VK_OEM_2	;/ ?
		db	VK_SHIFT	;rshift
		db	VK_PRINT	;* prtsc
		db	VK_MENU 	;alt
		db	VK_SPACE	;space
		db	VK_CAPITAL	;caps lk
		db	VK_F1		;f1
		db	VK_F2		;f2
		db	VK_F3		;f3
		db	VK_F4		;f4
		db	VK_F5		;f5
		db	VK_F6		;f6
		db	VK_F7		;f7
		db	VK_F8		;f8
		db	VK_F9		;f9
		db	VK_F10		;f10
		db	VK_OEM_NUMBER	;num lk
		db	VK_OEM_SCROLL	;scrl lk
		db	VK_HOME 	;home
		db	VK_UP		;up ar
		db	VK_PRIOR	;pg up
		db	VK_SUBTRACT	;numpd -
		db	VK_LEFT 	;left ar
		db	VK_CLEAR	;numpd 5
		db	VK_RIGHT	;rght ar
		db	VK_ADD		;numpd +
		db	VK_END		;end
		db	VK_DOWN 	;down ar
		db	VK_NEXT 	;pg dn
		db	VK_INSERT	;ins
		db	VK_DELETE	;del



;keep these two together
GlobalW CurMacOff,	0	;offset of the macro currently executing
GlobalW CurMacID,	0	;Macro ID of the macro currently executing
GlobalW CurMacPrevOff,	0	;offset of last op in current macro
GlobalD EMStk,		0, 21	;macro stack
GlobalW EMStkCount,	0	;number of macros awaiting execution
GlobalB fEMPaste,	0	;TRUE if we are pasting
GlobalB fCBM,		0	;TRUE if have cleared bad mark before paste
GlobalB fExecuting,	0	;TRUE if we are executing

sEnd	data



sBegin	code
	assumes cs, code
	assumes ds, data

	public	EMClearStack
	public	EMCheckInCurMac
	public	EMMain
	public	EMError
	public	EMFinalExit
	public	EMgKeystroke
	public	EMgAbort
	public	EMgCopy
	public	EMbCopy
	public	EMMacro
	public	EMgMark
	public	EMbMark
	public	EMMenu
	public	EMgPaste
	public	EMbPaste
	public	ResumeExecMacro
	public	EMScroll
	public	EMgSwitch
	public	EMbSwitch
	public	EMExit
	public	EMPush
	public	EMPop
	public	EMgProc
	public	EMbProc

	public	MCLParse
	public	SkipNPC, GetChar, MatchStr, GetNum
	public	mcl0, mcl1, mcl2, mcl3, mcl4, mcl5, mcl6, mcl7, mcl8, mcl9,
	public	mcl10, mcl11
	public	mcl12, mcl13, mcl15, mcl16, mcl17, mcl19, mcl20, mcl21

;routines for good apps

EMgProc dw	EMError 	;error
	dw	EMgKeystroke	;keystroke
	dw	EMgAbort	;abort
	dw	EMgClose	;close
	dw	EMgCopy 	;copy
	dw	EMMacro 	;macro
	dw	EMgMark 	;mark
	dw	EMgMaximize	;maximize
	dw	EMMenu		;menu
	dw	EMgMinimize	;minimize
	dw	EMNull		;null
	dw	EMgPaste	;paste
	dw	EMgRestore	;restore
	dw	EMScancode	;scancode
	dw	EMScroll	;scroll
	dw	EMError 	;showtitle
	dw	EMgSwitch	;switch

;routines for bad apps

EMbProc dw	EMError 	;error
	dw	EMbKeystroke	;keystroke
	dw	EMbAbort	;abort
	dw	EMbClose	;close
	dw	EMbCopy 	;copy
	dw	EMMacro 	;macro
	dw	EMbMark 	;mark
	dw	EMbMaximize	;maximize
	dw	EMMenu		;menu
	dw	EMbMinimize	;minimize
	dw	EMNull		;null
	dw	EMbPaste	;paste
	dw	EMbRestore	;restore
	dw	EMScancode	;scancode
	dw	EMError 	;scroll
	dw	EMShowTitle	;showtitle
	dw	EMbSwitch	;switch

;============================================================================
;				 ExecuteMacro
;============================================================================
;
; Purpose	Executes a WinOldAp DDL execute string.
;
; Parameters	MacroID = an integer identifying the macro to execute.
;
; Return Values ax = 0 if macro id valid
;		     ERR_PARAMETERS if macro id invalid
;
; Notes 	Uses ax, bx, cx, dx
;
;============================================================================
cProc	ExecuteMacro, <NEAR, PUBLIC>, <si, di, es>
	ParmW	MacroID

cBegin	ExecuteMacro

	mov	al, [fExecuting]	;if coming in for the first time
	or	al, al			;then we don't have to worry about
	jz	EMFirstEntrance 	;saving any previous macro
	call	EMPush			;else save the one previously running
EMFirstEntrance:
	mov	[fExecuting], 1
	mov	ax, MacroID
EMGetMacro:
	mov	[CurMacID], ax
	cCall	CheckOutMacroString, <ax>
	or	dx, ax			;did we get a macro string?
	jz	EMErrCheckOut		;no, execute any that are waiting
	mov	[CurMacOff], ax
EMTempExit:
	mov	[fEMPaste], 1		;exit and resume later
	jmp	EMExit
EMMain:
	cCall	LockMacroSeg		;get macro segment
	mov	es, ax
	mov	di, [CurMacOff] 	;get macro offset
	cmp	byte ptr es:[di], 0	;is exec str exhausted?
	je	EMDone			;yes, exit if no more waiting
	mov	[CurMacPrevOff], di
	call	MCLParse		;get next instruc
	cCall	UnLockMacroSeg
	mov	[CurMacOff], di 	;save pointer to next instruc
	shl	bx, 1			;convert opcode to jump table offset
	test	[PIFBehavior], BADBITS	;get either good or bad app table
	jz	EMGoodProc
	jmp	[bx + EMbProc]		;execute approp bad app routine
EMGoodProc:
	jmp	[bx + EMgProc]		;execute approp good app routine
EMDone:
	cCall	UnLockMacroSeg
	call	EMCheckInCurMac
EMStackCheck:
	cmp	[EMStkCount], 0 	;any macros waiting on stack?
	je	EMFinalExit
	call	EMPop			;yes, get next one and exec it
	jmp	EMMain
EMErrCheckOut:
	mov	AX, ERR_PARAMETERS	;return error code
	jmp	EMReturn
EMFinalExit:
	mov	dl, 0			;clear shift states
	call	SetShiftStates
	mov	[fExecuting], 0 	;mark as no more macros left
	test	[PIFBehavior], BADBITS	;if a bad app then make sure to
	jz	EMExit			;enable grabber
	call	EnableGrabber
EMExit: 				;call this only if no errs
	xor	ax, ax
EMReturn:

cEnd	ExecuteMacro

;============================================================================
;			   ResumeExecMacro
;============================================================================
;
; Purpose	Provides an entry point to ExecuteMacro.
;
; Notes 	For certain instructions, such as pasting keystrokes,
;		ExecuteMacro has to 'quit'.  When the paste is complete,
;		control returns to here, and macro execution resumes
;		where it was left off.
;
;============================================================================
ResumeExecMacro:
	pop	ax		;ret address
	push	ax		;random value to simulate cProc param on stack
	push	ax		;restore ret address
	push	bp		;duplicate cProc behaviour
	mov	bp, sp
	push	si
	push	di
	push	es
	mov	[fEMPaste], 0	;set flag to FALSE
	jmp	EMMain


;============================================================================
;				 EMError
;============================================================================
;
; Purpose	Handles syntax errors from parser.
;
; Notes 	Not defined yet, beep?
;
;============================================================================
EMError:
	jmp	EMMain


SetShiftStates	proc	near
	push	bx
	push	es
	xor	bx, bx
	mov	es, bx
	mov	bl, es:417H	;read current settings
	and	bl, 11110000b	;keep lock states
	or	bl, dl		;and set new shift states
	mov	byte ptr es:417H, bl
	pop	es
	pop	bx
	ret
SetShiftStates	endp


;============================================================================
;				EMKeystroke
;============================================================================
;
; Purpose	Paste a keystroke into the application.
;
; Parameters	al = main byte
;		ah = auxiliary byte
;		dl = shift state flags
;
;============================================================================
EMgKeystroke:
	cmp	[fScroll], 0		;use windows msgs for scrolling
	jz	EMbKeystroke		;otherwise put in app buffer
	mov	si, ax
	mov	al, ah			;convert pc scancode to win virt key
	xor	ah, ah
	sub	al, 1
	mov	bx, dataOFFSET PCtoWinVK
	xlat	PCtoWinVK
	mov	cx, ax
	mov	ax, WM_KEYDOWN
	mov	dl, dh
	xor	dh, dh
	mov	es, dx
	mov	di, 1
	RegPtr	lParam, es, di
	cCall	SendMessage, <hTTYWnd, ax, cx, lParam>
	mov	cx, si
	mov	ax, WM_CHAR
	RegPtr	lParam, es, di
	cCall	SendMessage, <hTTYWnd, ax, cx, lParam>
	mov	[fEMPaste], 1
	jmp	EMExit

EMbKeystroke:
	call	SetShiftStates
	mov	dh, dl		;dh = modifiers
	mov	cl, 4		;bit 8 = alt, bit 7 = ctrl, bit 6 = shift
	shl	dh, cl
	mov	ch, ah
	call	TranslateKey
	mov	di, 1
	call	AddKey
	mov	[fEMPaste], 1
	jmp	EMExit			;returns to ResumeExecMacro

;============================================================================
;				EMAbort
;============================================================================
;
; Purpose	Set switch controlling allowance of user abort of macro.
;
; Parameters	al = PR_ON or PR_OFF
;
;============================================================================
EMbAbort:
EMgAbort:
	jmp	EMMain

;============================================================================
;				EMClose
;============================================================================
;
; Purpose	Close application.
;
; Parameters	None.
;
;============================================================================
EMgClose:
	mov	bx, WM_CLOSE
	cCall	PostMessage, <[hTTYWnd], bx, bx, bx, bx>
EMbClose:
	cmp	[OldApState], SF_TERMINATED
	jne	EMbClose1
	mov	[OldApState], SF_CLOSED
EMbClose1:
	call	EMClearStack
	jmp	EMFinalExit


;============================================================================
;				EMCopy
;============================================================================
;
; Purpose	Copy marked region into the clipboard.
;
; Parameters	None.
;
;============================================================================
EMgCopy:
	call	TTYCopy
	jmp	EMMain

EMbCopy:
	cCall	DoCopy
	jmp	EMMain



;============================================================================
;				EMMacro
;============================================================================
;
; Purpose	Begins execution of a different macro.
;
; Parameters	cx = Macro Id
;
; Notes 	Uses ax, di.
;
;============================================================================
EMMacro:
	call	EMPush
	mov	ax, cx
	jmp	EMGetMacro


;============================================================================
;				EMMaximize
;============================================================================
;
; Purpose	Performs maximize function of system menu.
;
; Parameters	none
;
;============================================================================
EMgMaximize:
	mov	ax, WM_SYSCOMMAND
	mov	bx, SC_MAXIMIZE
	cCall	PostMessage, <hTTYWnd, ax, bx, bx, bx>
EMbMaximize:
	jmp	EMMain


;============================================================================
;				EMMark
;============================================================================
;
; Purpose	Performs designated Mark.
;
; Parameters	al  = type ( PRSCREEN | PR_NONE | PR_COORDINATES )
;		cl  = XOrg
;		ch  = YOrg
;		dl  = XExt
;		dh  = YExt
;
;============================================================================
EMgMark:
	cmp	al, PR_NONE
	je	EMgMarkInteractive
	cmp	al, PR_COORDINATES
	je	EMgMarkSelect
	xor	cx, cx			;(XOrg, YOrg) = (0, 0)
	mov	dx, Line25*256+Col80	;(XExt, YExt) = (79, 24)
EMgMarkSelect:
	call	SelectRect
	jmp	EMMain
EMgMarkInteractive:
	call	EMClearStack
	call	TTYSelect
	jmp	EMFinalExit

EMbMark:
	cmp	al, PR_NONE
	je	EMbMarkInteractive
	xor	bx, bx
	cmp	al, PR_COORDINATES
	je	EMbMarkSelect
	xor	cx, cx			;(0,0) (0,0) means mark the screen
	xor	dx, dx
EMbMarkSelect:
	cCall	DoMark, <bx, cx, dx>
	jmp	EMMain
EMbMarkInteractive:
	call	EMClearStack
	mov	[fExecuting], 0 	;signal we're done in case re-entrant
	mov	bx, 1
	cCall	DoMark, <bx, bx, bx>
	jmp	EMFinalExit


;============================================================================
;				EMMenu
;============================================================================
;
; Purpose	Forwards Menu command to menu routines.
;
; Parameters	cx = Menu Id
;		dx = Menu Flags
;		ax = Macro Id
;
;============================================================================
EMMenu:
	cCall	MacroUpdateMenu, <cx, dx, ax>
	jmp	EMMain

;============================================================================
;				 EMMinimize
;============================================================================
;
; Purpose	Performs Minimize operation.
;
; Parameters	None.
;
;============================================================================
EMgMinimize:
	mov	ax, WM_SYSCOMMAND
	mov	bx, SC_MINIMIZE
	cCall	PostMessage, <hTTYWnd, ax, bx, bx, bx>
	jmp	EMMain
EMbMinimize:
	jmp	EMbSwitch


;============================================================================
;				EMNull
;============================================================================
;
; Purpose	Sets the number of times int 16's from the app should fail.
;
; Parameters	cl = Number of times to fail, 1 to 255
;
;============================================================================
EMNull:
	or	cl, cl		;cl must be at least 1
	jnz	EMNull1
	mov	cl, 1
EMNull1:
	mov	[fEMPaste], cl
	jmp	EMExit


;============================================================================
;				EMPaste
;============================================================================
;
; Purpose	Performs Paste from clipboard to app.
;
; Parameters	None.
;
; Notes 	Uses si.
;
;============================================================================
EMgPaste:
	mov	si, [hDataX]
	call	TTYPaste
	mov	[fEMPaste], 1
	jmp	EMExit		;exit so that app may run and accept keys

EMbPaste:
	cmp	[fCBM], 0	;is this our second pass through here?
	jne	EMbP1		;yes, we can now paste
	mov	[fCBM], 1	;no, clear the current mark and quit
	mov	[fEMPaste], 1
	mov	si, [CurMacPrevOff]
;	sub	[CurMacOff], 7	;set pointer back at [paste], NOT RELIABLE!
	mov	[CurMacOff], si
	call	EMClearBadMark
	jmp	EMExit
EMbP1:
	mov	[fCBM], 0
	call	StartPaste
	mov	[fEMPaste], 1
	jmp	EMExit

;============================================================================
;				EMRestore
;============================================================================
;
; Purpose	Performs restore system menu function.
;
; Parameters	none
;
;============================================================================
EMgRestore:
	mov	ax, WM_SYSCOMMAND
	mov	bx, SC_RESTORE
	cCall	PostMessage, <hTTYWnd, ax, bx, bx, bx>
EMbRestore:
	jmp	EMMain

;============================================================================
;			    EMScancode
;============================================================================
;
; Purpose	Puts raw scancode into keystroke buffer
;
; Parameters	cx = scancode (base 10)
;
;============================================================================
EMScancode:
	mov	di, 1
	mov	ax, cx
	call	Addkey
	mov	[fEMPaste], 1
	jmp	EMExit

;============================================================================
;				EMScroll
;============================================================================
;
; Purpose	Puts system in Scroll mode.
;
; Parameters	None.
;
;============================================================================
EMScroll:
	call	SetScrollText
	jmp	EMMain

;============================================================================
;				EMShowTitle
;============================================================================
;
; Purpose	Displays the title bar and menu bar (if any).
;
; Parameters	None.
;
;============================================================================
EMShowTitle:
	mov	cs:[fMenu], 1
	jmp	EMMain

;============================================================================
;				EMSwitch
;============================================================================
;
; Purpose	Performs a context-switch.
;
; Parameters	None.
;
; Notes 	Any remaining macro commands will not be executed.
;
;============================================================================
EMgSwitch:
	call	EMClearStack
	mov	ax, WM_SYSCOMMAND
	mov	bx, SC_NEXTWINDOW
	cCall	PostMessage, <hTTYWnd, ax, bx, bx, bx>
	jmp	EMFinalExit

EMbSwitch:
	call	EMClearStack
	mov	[fExecuting],0	;Finish up
	mov	ax,SwitchOutID	;Execute switch macro
	cCall	ExecuteMacro, <ax>
	or	ax,ax
	jz	EmbSwExit
	mov	ax, SwitchOutID ; NO: Finish up manually
	call	FinishSwitchMacro
	jmp	EMFinalExit
EMbSwExit:
	jmp	EMExit

;============================================================================
;			      EMClearBadMark
;============================================================================
;
; Purpose	Clears an mark operation in bad apps
;
; Parameters	None
;
; Notes 	Uses no registers
;
;============================================================================
EMClearBadMark	proc	near
	cmp	cs:[fMark], 0
	je	EMCBMEnd
	cmp	cs:[fEndBlock], 0
	jne	EMCBM1
	mov	cs:[fEndBlock], 1
	mov	cs:[fMarkAbort], 1
	jmp	EMCBMEnd
EMCBM1:
	call	RestoreMark
	mov	cs:[fMark], 0
EMCBMEnd:
	ret
EMClearBadMark	endp


;============================================================================
;			  EMCheckInCurMac
;============================================================================
;
; Purpose	Checks in the current macro.
;
; Notes 	Uses di.
;
;============================================================================
EMCheckInCurMac proc	near
	mov	di, [CurMacID]
	cCall	CheckInMacroString, <di>
	cCall	DDEFinishMacro, <di>		; Tell DDE we have finished
	mov	ax, di
	call	FinishSwitchMacro		; Tell the switcher we have finished
	call	FinishExitMacro 		; Tell ShowTerminate we have finished
	mov	[CurMacID], 0
	mov	[CurMacOff], 0
	ret
EMCheckInCurMac endp


;============================================================================
;				EMPush
;============================================================================
;
; Purpose	Places current macro data onto the stack.
;
; Notes 	Physically, the top of the stack are the values for
;		CurMacID and CurMacPtr.  The stack we maintain here
;		keeps all of the macros that are awaiting completion.
;
;		Uses di, ax.
;
;============================================================================
EMPush	proc near
	cmp	[EMStkCount], NESTINGDEPTH	;let's not overflow, o-tay?
	jge	EMPushDone
	push	ds
	pop	es
	mov	di, [EMStkCount]
	shl	di, 1
	shl	di, 1
	add	di, dataOFFSET [EMStk]
	mov	ax, [CurMacID]			;push macro id
	stosw
	mov	ax, [CurMacOff] 		;and macro pointer
	stosw
	inc	[EMStkCount]
EMPushDone:
	ret
EMPush	endp


;============================================================================
;				EMPop
;============================================================================
;
; Purpose	Makes the next macro on the stack the current macro.
;
; Notes 	Uses es, di, ax.
;
;============================================================================
EMPop	proc near
	cmp	[EMStkCount], 0 	;and especially don't underflow
	jle	EMPopDone
	dec	[EMStkCount]
	mov	si, [EMStkCount]
	shl	si, 1
	shl	si, 1
	add	si, dataOFFSET [EMStk]
	lodsw				;get the macro id
	mov	[CurMacID], ax
	lodsw
	mov	[CurMacOff], ax
EMPopDone:
	ret
EMPop	endp

;============================================================================
;			     EMClearStack
;============================================================================
;
; Purpose	Checks in all the current macro and all waiting macros.
;
; Notes 	Uses es, di, ax
;
;============================================================================
EMClearStack	proc	near
EMClrStk1:
	call	EMCheckInCurMac
	cmp	[EMStkCount], 0 		;and check in all macros
	je	EMClrStk2			;that are waiting, if any
	call	EMPop
	jmp	EMClrStk1
EMClrStk2:
	ret
EMClearStack	endp

;============================================================================
;			     ASCIICtrlCorrect
;============================================================================
;
; Purpose	Corrects the value of ASCII chars if Ctrl modifier set
;
; Parameters	ah = PC Scan code
;		al = ASCII value
;		dl = shift state flags
;
; Return Values al = ASCII value corrected for Ctrl
;
; Notes 	Example: if Macro string '^A', input is al= 65 ( == 'A')
;					       output is al= 1 ( == ctrl A)
;
;		This code also allows lower case alpha chars, so ^c == ^C
;
;============================================================================
ASCIICtrlCorrect	proc	near
	test	dl, CTRLMASK
	jz	ACCEnd
	cmp	al, 'A'
	jl	ACCEnd
	cmp	al, '_'
	jg	ACCLowerCase
	sub	al, 'A' - 1
	jmp	ACCEnd
ACCLowerCase:
	cmp	al, 'a'
	jl	ACCEnd
	cmp	al, 'z'
	jg	ACCEnd
	sub	al, 'a' - 1
ACCEnd:
	ret
ASCIICtrlCorrect	endp


SkipNPC proc	near
NPCChk: cmp	byte ptr es:[di], 32
	jg	DoneNPC
	inc	di
	jmp	NPCChk
DoneNPC:ret
SkipNPC endp

GetChar proc	near
	inc	di
	call	SkipNPC
	ret
GetChar endp

;============================================================================
;				  MatchStr
;============================================================================
;
; Purpose	Determines if a string matches one in a table.
;
; Parameters
;
;   di: Points to string that's trying to be matched.
;   si: Points at a table of reserved word strings that possibly contain
;	a match.  This list of reserved word strings is in the following
;	format:
;
;	[<str1 name>]	db    <str1 len>, 'str1', <str1 return value>
;	[<str2 name>]	db    <str2 len>, 'str2', <str2 return value>
;			      . . .
;	[<strN name>]	db    <strN len>, 'strN', <strN return value>
;			db    0
;
; Return Values di: pointing at where we left off
;		si: pointing at return value, 0 if no match
;
; Notes 	Uses ax, si, di
;
;============================================================================
	public	MatchLoop
MatchStr proc	near
	push	bx		;save opcode
	push	cx		;save (possible) number param
	xor	ch, ch		;only use cl
	mov	bx, di		;save pointer to match string
Match:	lodsb			;al=length of str
	mov	cl, al		;
	lodsb			;al=first char
MatchLoop:
	cmp	al, byte ptr es:[di]
	je	SoFarSoGood
	add	al, 'a' - 'A'
	cmp	al, byte ptr es:[di]
	je	SoFarSoGood
	jmp	WhoKnows
SoFarSoGood:
	lodsb
	inc	di
	loop	MatchLoop
WhoKnows:
	jcxz	MatchEnd	;if we matched, then exit with success
	mov	di, bx		;  else reset di -> match string
	add	si, cx		;  and set si -> next string
	cmp	byte ptr [si], 0;if we exhausted the list
	je	NoMatch 	;  then exit without success
	jmp	Match		;  else try with the next string
NoMatch:
	stc
MatchEnd:
	dec	si
	pop	cx
	pop	bx
	ret
MatchStr endp


;============================================================================
;				  GetNum
;============================================================================
;
; Purpose
;
;   Returns the numerical value of a string of digits.
;
; Parameters
;
;   di = pointer to beginning of string of digits representing a decimal number
;
; Return Values
;
;   di = where we left off
;   ax = number
;
; Notes
;
;   Uses ax, si
;
;============================================================================

GetNum	proc	near
	cmp	byte ptr es:[di], '0'
	jl	NotANum
	cmp	byte ptr es:[di], '9'
	jg	NotANum
	xor	ax, ax		;initialize accumulator to 0
	push	cx
	mov	cx, 10		;assume input is base 10
	push	dx
	mov	dh, 0		;use only dl
more:
	mov	dl, byte ptr es:[di]
	add	ax, dx
	sub	ax, '0'         ;convert ASCII representation to a number
	inc	di
	cmp	byte ptr es:[di], '0'
	jl	done
	cmp	byte ptr es:[di], '9'
	jg	done
	mul	cx		;yes, so mult accumulator by 10
	jmp	more		;and get next digit
NotANum:
	stc
	ret
done:
	clc
	pop	dx
	pop	cx
	ret
GetNum	endp


;============================================================================
;				 MCLParse
;============================================================================
;
; Purpose
;
;   Returns a WinOldAp Macro Command Language instruction with assoc data.
;
; Parameters
;
;   es:di = pointer to execute string of characters
;
; Return Values
;
;   es:di = pointer to one character past the last character in the token we
;	    just returned (i.e., where we left off)
;
;   bx = opcode
;
;   Other return values vary based on bx as follows:
;
;     bx = OPKEYSTROKE		 al = main byte
;				 ah = auxiliary byte
;				 dl = shiftstate flags
;
;     bx = OPABORT		 ax = PR_ON or PR_OFF
;
;     bx = OPCOPY		 no additional data
;
;     bx = OPMARK		 ax = PR_NONE or PR_SCREEN or PR_COORDINATES
;				 if PR_COORDINATES then
;				   cl = Xorg
;				   ch = Yorg
;				   dl = Xext
;				   dh = Yext
;
;     bx = OPMENU		 cx = menu id
;				 ax = macro id
;				 dx = menu flags
;
;     bx = OPMACRO		 cx = macro id
;
;     bx = OPCLOSE,		 no additional data
;	   OPPASTE,
;	   OPSWITCH,
;	   OPSCROLL,
;
; Notes
;
;     Uses all registers except cs, ds, es, ss
;
;============================================================================

MCLParse proc	near

	xor	bx, bx		;used for opcode
	xor	dx, dx		;used for flags
mcl0:
	mov	al, es:[di]	;al = current char, di -> next char
	inc	di
	cmp	al, '+'
	je	mcl1
mcl0b:
	cmp	al, '^'
	je	mcl3
mcl0c:
	cmp	al, '~'
	je	mcl5
mcl0d:
	cmp	al, '{'
	jne	mcl0e
	call	SkipNPC
	jmp	mcl7
mcl0e:
	cmp	al, '['
	jne	mcl0f
	call	SkipNPC
	jmp	mcl11
mcl0f:
	cmp	al, '}'
	jne	mcl0g
	call	SkipNPC
	jmp	mcl22
mcl0g:
	cmp	al, ']'
	jne	mcl0h
	call	SkipNPC
	jmp	mcl23
mcl0h:
	dec	di			;reset di to point to current char
mcl0i:
	mov	bx, dataOFFSET ASCIItoPCScan
	mov	ah, al			;ah = ascii code
	sub	al, '!'                 ;al = offset in table
	xlat	ASCIItoPCScan
	xchg	ah, al
	call	ASCIICtrlCorrect
	jmp	mcl24
mcl1:
	cmp	byte ptr es:[di], '+'
	je	mcl2
	or	dl, SHIFTMASK
	mov	al, byte ptr es:[di]
	inc	di
	jmp	mcl0b
mcl2:
	jmp	mcl24
mcl3:
	cmp	byte ptr es:[di], '^'
	je	mcl4
	or	dl, CTRLMASK
	mov	al, byte ptr es:[di]
	inc	di
	jmp	mcl0c
mcl4:
	jmp	mcl24
mcl5:
	cmp	byte ptr es:[di], '~'
	je	mcl6
	or	dl, ALTMASK
	mov	al, byte ptr es:[di]
	inc	di
	jmp	mcl0d
mcl6:
	jmp	mcl24
mcl7:
	cmp	byte ptr es:[di], '{'
	je	mcl8
	mov	si, dataOFFSET Key_Equivalents
	call	MatchStr
	jnc	mcl7a
	jmp	mcl_Error_Exit
mcl7a:
	mov	ah, [si]		;return value (auxiliary byte)
	;
	;the following hacked up lines deal with keywords that also have
	;ASCII values
	;
	cmp	ah, KYENTER		;some keys have ASCII values
	jne	mcl7_TAB
	mov	al, 13			;ASCII CR
	jmp	mcl9
mcl7_TAB:
	cmp	ah, KYTAB
	jne	mcl7_ESC
	mov	al, 9			;ASCII TAB
	jmp	mcl9
mcl7_ESC:
	cmp	ah, KYESC
	jne	mcl7b
	mov	al, 27			;ASCII ESC
	jmp	mcl9
mcl7b:
	mov	al, 0			;for all other special keys,
	jmp	mcl9			;main byte = 0
mcl8:
	jmp	mcl24
mcl9:
	cmp	byte ptr es:[di], '}'
	je	mcl10
	inc	di
	jmp	mcl9
mcl10:
	jmp	mcl24
mcl11:
	cmp	byte ptr es:[di], '['      ;is it '[['
	je	mcl12
	mov	si, dataOFFSET WOA_Ops	    ;no, compare to op strings
	call	MatchStr
	jnc	mcl11b
	jmp	mcl_Error_Exit
mcl11b:
	mov	bl, [si]
	jmp	mcl13
mcl12:
	jmp	mcl24
mcl13:
	call	SkipNPC
	cmp	byte ptr es:[di], ']'    ; no parameters?
	jne	mcl13b
	mov	ax, PR_NONE
	jmp	mcl_Exit
mcl13b:
	mov	si, dataOFFSET WOA_Op_Params
	call	MatchStr
	jc	mcl13c
	mov	al, [si]
	jmp	mcl15
mcl13c:
	call	GetNum
	mov	cx, ax
	jmp	mcl16
mcl15:
	cmp	byte ptr es:[di], ']'
	je	mcl15b
	inc	di
	jmp	mcl15
mcl15b:
	jmp	mcl_Exit
mcl16:
	call	SkipNPC
	cmp	byte ptr es:[di], ']'   ;is it ]]?
	jne	mcl16b
	jmp	mcl_Exit
mcl16b:
	cmp	byte ptr es:[di], 'M';could it be MF_...?
	jne	mcl16c		     ;not a chance
	mov	si, dataOFFSET WOA_Op_MF_Params
	call	MatchStr	     ;match with MF_ parameters
	jc	mcl_Error_Exit
	or	dl, byte ptr [si]    ;update the menu flags
	call	GetChar
	jmp	mcl17
mcl16c:
	call	GetNum		     ;try numbers
	jc	mcl_Error_Exit	     ;no, we've tried everything
	mov	ch, al		     ;yes, ch = Yorg
	call	GetChar
	jmp	mcl20
mcl17:
	cmp	byte ptr es:[di], '|'
	jne	mcl17b
	call	GetChar 	; get next MF_*
	jmp	mcl19
mcl17b: 			;it better be a number
	call	GetNum
	jmp	mcl15
mcl19:
	mov	si, dataOFFSET WOA_Op_MF_Params
	call	MatchStr
	jc	mcl_Error_Exit
	or	dl, byte ptr [si]	;update flags
	call	GetChar
	jmp	mcl17
mcl20:
	call	GetNum
	mov	dl, al
	call	GetChar
mcl21:
	call	GetNum
	mov	dh, al
mcl21b:
	cmp	byte ptr es:[di], ']'
	je	mcl21c
	inc	di
	jmp	mcl21b
mcl21c:
	mov	ax, PR_COORDINATES
	jmp	mcl15
mcl22:
	cmp	byte ptr es:[di], '}'
	jne	mcl_Error_Exit
	jmp	mcl24
mcl23:
	cmp	byte ptr es:[di], ']'
	jne	mcl_Error_Exit
mcl24:
	mov	bx, OPKEYSTROKE
	jmp	mcl_Exit
mcl_Error_Exit:
	mov	bx, OP_ERROR
mcl_Exit:
	inc	di		;point di at next character
	ret

MCLParse endp



sEnd	code
	end

