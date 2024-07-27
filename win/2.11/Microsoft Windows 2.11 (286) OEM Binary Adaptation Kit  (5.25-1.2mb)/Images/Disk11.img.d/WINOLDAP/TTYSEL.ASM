;------------------------------------------------------------------------
;
; MODULE TTYSEL.ASM
;
;	Keyboard Handlers for Windowed apps
;	Mouse Handlers for Windowed apps
;	Marking code for Windowed apps
;	selecting contents of apps that run in a Window
;	Code for scrolling old apps that run in a Window
;	Code for performing old app data exchange
;	Code for handling old app title changes
;	Code for handling various Windows Messages
;
;	$Author:   rcp  $
;	$Revision:   1.37  $
;	$Date:   10 Nov 1987 14:05:22  $
;

; MICROSOFT TRIP FIXES
;
; Removed setting MenuSeg to Dx in MyEnableItem
; Called LockMenuSeg and UnLockMenuSeg instead of locking and unlocking
; the hMenuSeg directly here in MyEnableItem
; Added declarations for new external functions
; Addeed call to UnFixText in MyEnableItem to get good app text back to OEM
;   char set

	.xlist
	include cmacros.inc
	NOMST	= 1		; dont include misc structures
	NOTEXT	= 1		; don't include TextMetric struc & text drawing modes & stock objs.
	NOSYSMETRICS = 1	; don't include system metrics equates
	include windows.inc
	include vkoem.inc
	include tty.inc
	include menu.inc
	.list

;
; Background mode we use
;
TRANSPARENT	equ	1
;
; Number of chars of old app name put in old app Icon
;
MaxIconNameLen	equ	3
;
; Selection structure for selecting regions of old app Windows screens
;
SelectStruc	struc
SelectCol	db	-1	; Selection endpoint
SelectRow	db	-1	; Selection endpoint
RightCol	db	?	; Right side of selection
BottomRow	db	?	; Bottom of selection
LeftCol 	db	?	; Left side of selection
TopRow		db	?	; Top of selection
;
; The following four values form a RECT structure which is the screen coord
;   representation of the above Row/Col rectangle
;
Left		dw	?
Top		dw	?
Right		dw	?
Bottom		dw	?
SelectStruc	ends

;-----------------------------------------------
;
; External Windows Procedures
;
externFP	<GlobalAlloc,GlobalLock,GlobalUnlock,GlobalRealloc,GlobalFree,GlobalSize>
externFP	<GetClientRect,GetWindowRect,ClipCursor,CreateDialog>
externFP	<GetDC,ReleaseDC,GetStockObject>
externFP	IntersectRect
externFP	<InvertRect,FillRect,SetBkMode,SetSysModalWindow>
externFP	ExcludeClipRect
externFP	<OpenClipboard,GetClipboardData,CloseClipboard,ShowWindow>
externFP	<EmptyClipboard,SetClipboardData,SetDlgItemText>
externFP	<MakeProcInstance,DialogBox,GetDlgItemText,EndDialog,FreeProcInstance>
externFP	<EnableMenuItem,LoadString,TextOut,AnsiUpper>
externFP	<SetScrollPos,SetScrollRange>
externFP	<GetWindowText,SetWindowText,GetKeyState,PostMessage>
externFP	<UpdateWindow,ScrollWindow>
externFP	<AnsiToOem,OemToAnsi>
externFP	<SetCapture,ReleaseCapture,SetFocus>

;-----------------------------------------------
;
; External WINOLDAP Procedures
;
externNP	<MoveCursor,TTYdefault>
externNP	<FlushComBufferNoYield>
externNP	<MulCol,SetCount, TranslateNumpad>
externNP	<TranslateKey, AddKey, Beep>
externNP	<UpdateMenu, GetItem>

; MSF added external declarations

externNP	<LockMenuSeg, UnlockMenuSeg, UnFixText>

;MSF Done


;-----------------------------------------------
;
; Per task data stored in app DS
;
sBegin	data

externB 	InBuf			; Input buffer
externB 	OutBuf			; Output buffer
externB 	pifTitle		; App title
;
; Preloaded title strings
;
externB 	szInactive		; "Inactive"
externB 	InactiveLen
externB 	szMark			; "Mark"
externB 	MarkLen
externB 	szScroll		; "Scroll"
externB 	ScrollLen
;
; Title varibles
externW 	pWindowTitle
externB 	TitleLen
globalB 	bParen, 0
staticB 	,?
;
; Variables for ASCII messages generated from keyboard
;
globalW 	AsciiValue,?
globalW 	AsciiCount,?


;
; Brushes
;
externW 	hbrWhite
externW 	hBrBlack		; Brush handle

;
; Font values
;
externW 	CharHeight
externW 	CharWidth
;
; Visible screen values (location (in pix) in 80X25 screen of display 0,0)
;
externW 	VisiRow
externW 	VisiCol

externW 	hTTYWnd 		; Window handle for apps in a Window
externW 	VidSegment		; Location of logical video buffer
;
; Video update queue pointers
;
externW 	VidHead
externW 	VidTail
;
; Data exchange values
;
externW 	hDatax
externW 	pDatax

externW 	fIcon			; ICONic flag
externW 	prevFicon		; Storage for prev ICON state
externW 	fFocus			; Focus state
globalW 	fShow, ?		; Window state flags
;
; Pif behavior bits
;
externB 	pifBehavior
externB 	pifPathname

externB 	Signature		; DS signature
externW 	hMenu			; Menu handle

globalW 	DialogID,?		; Storage for ID of current dialog
globalW 	HandleSave,0		; Storage for handles to lock/unlock
globalB 	fScroll,0		; non zero if in scroll lock
externB 	fUserLock		; Disable user actions if set
externW 	OldApState		; State of the old application

;
; Select variables
;
globalW 	Anchor, 0		; Flag used in selection logic
globalB 	fSelect, 0		; Flag for selection logic

;
; Re-entrancy variable used in the scroll bar code
;
globalB 	fVert,0


;
; Selection structures
;
Selection	SelectStruc	<>
prevSelection	SelectStruc	<>

;
; Rectangle structure for intersection computations
;
InterRect	label	word
InterLeft	dw	?
InterTop	dw	?
InterRight	dw	?
InterBottom	dw	?

globalW 	WinRect,?,4			; Rectangle structure for main
						;  Window

;
; Masks for attribute info in vid buffer
;
AndMask 	db	01111111b
OrMask		db	10000000b

; Menu structure needed to update menus

externB 	PokeMenu

;Menu data area values
externW 	hMenuSeg
externW 	MenuSeg


;
; Various Window values
;
globalW 	WinHeight,0		; Window height
globalW 	WinWidth,0		; Window width
globalW 	ScrHeight,0		; Screen height
globalW 	ScrWidth,0		; Screen width
globalW 	LastRow,MaxLine 	; Last line on display

sEnd	data

sBegin	code
	assumes cs,code
	assumes ds,data

	public	TTYhscroll,TTYvscroll, TTYSelect
	public	TTYSysKeyup, TTYSysKeyboard, TTYKeyboard, TTYKeyup
	public	TTYAscii, TTYChar, TTYSysChar
	public	TTYMouseDown, TTYMouseUp, TTYMouseMove
	public	TTYPaste, TTYCopy, MySetFocus, TTYsize
	public	StartSelection, ClearSelection, ExtendSelection
	public	CancelSelection, RenderSelection, GetSelection
	public	GetDatax,PeekDatax,DoDataX
	public	GetCommandLine,TrackRowCol,PostWarning,PostHalted
	public	MyAlloc,MyUnlock,MyEnableItem,MyDisableItem
	public	SetScrollText, SetSelectText, SetInactiveText, ConvertToAnsi
	public	PrependWinText, SetWinTitle, GetWinTitle
	public	ResetWinText
	public	AddANSIToClipboard		;KMF 041587
	public	SelectRect			;KMF 042887

;-----------------------------------------------
;
; Global task data stored in WINOLDAP code segment
;
    ;
    ; Pointer to DOS ERRORMODE variable
    ;
	externD pErrMode

;--------
;
; Keyboard Handlers for windowed applications
;
;	Normally, the keyboard messages end up as entries into
;	the keyboard buffer for the old applications. However,
;	if in Scroll or Mark mode, they drive WinOldAp.
;
;	WinOldAp's keyboard buffer emulating the IBM BIOS's keyboard buffer
;	contains a PC8 and sconcode entry. Windows on the other hand
;	sends the character and the scancode in seperate messages. Thus,
;	WinOldAp must wait until it gets both character and scancode
;	before it can put a entry into the keyboard buffer. It does this
;	by posting a USER+1 message to itself when a WM_KEYDOWN message
;	comes. If a WM_CHAR message is comming, it will arrive before
;	the special USER+1 message comes. When USER+1 message arrives,
;	both character and scancode are present. The USER+1 handler, TTYascii,
;	will then call TranslateKey to convert the charater and scancode
;	from Windows to OEM convention.
;
;

;**
;
; TTYSysChar - Handler for SYS key messages
;
;	Execept for Alt-Space, Alt combinations are passed
;	onto the old application
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;
; EXIT:
;	Key handled
; USES:
;	C standard
;
TTYSysChar:
	push	ax
	mov	ax,VK_MENU
	call	CheckKeyState	; is ALT key down?
	pop	ax
	js	SCspace 	; if ALT key not down, then do default
	jmp	TTYdefault
SCSpace:
	cmp	cl,' '          ; if ALT space, do default
	jne	TTYChar
	jmp	TTYdefault
;**
;
; TTYChar - Handler for character messages messages
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	If in Scroll or Select mode, handle approp to that mode
;	Else generate key data for TTYascii message to follow
;	DX = AX = 0
; USES:
;	C standard
;
TTYChar:
	cmp	[fScroll],0	; are we in scroll mode?
	jz	chr1		; no
    ;
    ; Scroll mode, Cancel mode if CR, ^C, or ESC
    ;
	call	CheckChar	; CR, ^C, ESC cancel scroll mode
	jnz	chr4		; Other keys ignored
	mov	[fScroll],0	; Cancel scroll
	call	ResetWinText	; Remove caption diddle
	jmp	short chr4	; and done

chr1:	test	[fSelect],FS_ACTIVE; are we in Select mode?
	jz	chr3		; no, give to the application
    ;
    ; Have selection in progress
    ;	^C or ESC cancels selection
    ;	CR renders the selection
    ;	Other keys passed through
    ;
	call	CheckChar	; ENTER, ^C, or ESC?
	jnz	chr3		; nope, ignore
	cmp	cl,13		; was the key ENTER
	jz	chr1a		; yes, render selection
	call	CancelSelection ;
	jmp	short chr4	; clear selection and return

    ;
    ; Render the current selection
    ;
chr1a:	call	RenderSelection ; Render the selection
	jmp	short chr4	; and return

    ;
    ; Set Key data for app sucked up on next TTYascii message
    ;	if app is running
    ;
chr3:	cmp	byte ptr [OldApState],SF_RUNNING; Is the task running?
	jb	chr4			;  No, ignore keys
	cmp	[AsciiCount],0		; Is a character already there?
	jne	chr4			;  Yes, ignore key
	mov	[AsciiValue],cx 	; save ascii value
	mov	[AsciiCount],di 	; and count
chr4:	jmp	Okay

;**
;
; CheckChar CheckChar2 - Check for interesting chars
;
; ENTRY:
;	CL is ascii char
; EXIT:
;	CheckChar
;	    Zero set if CL is ESC, CR or ^C
;	CheckChar2
;	    Zero set if CL is ESC, or ^C
; USES:
;	FLAGS
;
CheckChar:
	cmp	cl,13
	jz	cc1
CheckChar2:
	cmp	cl,3
	jz	cc1
	cmp	cl, cEsc
cc1:	ret

;**
;
; TTYAscii - This is the handler for the special KEY message generated
;		by the key down code in TTYSEL.ASM
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	Key data placed in input buffer for app
; USES:
;	C standard
;
TTYAscii:
	mov	ax,-1			; Assume arg to Translate key is No trans
	cmp	byte ptr [AsciiCount],0 ; Key translation from TTYchar?
	jz	tos1			; No
	mov	ax,[AsciiValue] 	; Yes, get translation as call to Translate
tos1:	mov	dx,es			; Modifiers here (ALT CTRL SHIFT state)
	call	TranslateKey		; Translate the key
	cmp	ax,-1			; -1 means no translation
	jz	tos3			; ignore key
	push	ax
	push	di
	call	CancelSelection 	; Remove any selections
	pop	di
	pop	ax
	test	fUserLock, UL_ON	; UserLock?
	jnz	tos3			;  yes, ignore the key.
	call	AddKey			; Put in key(s)
	or	di,di			; did we fit them all in?
	jz	tos3			; yes, done
	call	Beep			; beep!
tos3:
	mov	[AsciiCount], 0
	mov	[AsciiValue], 0
	jmp	Okay

;**
;
; TTYSysKeyboard - Handler for WM_SYSKEYDOWN message
;
; ENTRY:
;	SS = DS = Data of app key is directed to
;	CX = wParam from Windows Key message (Windows Virtual key that went down)
;		(SYS key in this case)
;	ES:DI = lParam from Windows Key message (ES = Scan code for the key)
; EXIT:
;	DX = AX = 0 if we processed message
;	Else through DefWindowProc
; USES:
;	ALL but DS,SS,BP
;
TTYSysKeyboard:
	mov	bx, es
	cmp	CX,VK_F10		; Bogus Menu Key?
	jz	TTYKey			; Yes, Process as normal key
	test	bh, 20h 		; Alt down?
	jz	tsk1			; NO, default handler unless at INT 24
	cmp	cx,VK_SPACE		; ALT space?
	jz	tsk1			; Yes, default handler unless at INT 24
	cmp	cx,VK_ESCAPE		; ALT esc?
	jz	tsk1			; Yes, default handler unless at INT 24
	cmp	cx,VK_TAB		; ALT tab?
	jz	tsk1			; Yes, default handler unless at INT 24
	cmp	cx,VK_MENU		; ALT by itself?
	jz	tsk1			; Yes, default handler unless at INT 24
    ;
    ; Alt-Numpad and Alt-0-Numpad handler
    ;
	push	ax
	mov	ax, AsciiValue
	mov	dx, AsciiCount
	call	TranslateNumPad
	mov	AsciiValue, ax
	mov	AsciiCount, dx
	or	ax,dx			;was there a translation
	pop	ax
	jnz	TTYDef
TTYKey:
	jmp	TTYKeyboard
TTYdef:
	jmp	TTYDefault
;**
;
; TTYSysKeyup - Handler for WM_SYSKEYUP message
;
;    A SYS key is going up, default handler unless at INT 24.
;
; ENTRY:
;	SS = DS = Data of app key is directed to
;	CX = wParam from Windows Key message (Windows Virtual key that went up)
;		(SYS key in this case)
;	ES:DI = lParam from Windows Key message (ES = Scan code for the key)
; EXIT:
;	DX = AX = 0 if we processed message
;	Else through DefWindowProc
; USES:
;	ALL but DS,SS,BP
;
TTYSysKeyup:
    ;
    ; If in INT 24 (errormode set), then use normal Keyboard handler
    ;	else use DefWindowProc. We come here with ALT-TAB, ALT-RETURN
    ;	ALT-SPACE and other SYS keys. If we are at INT 24, the
    ;	TTYKeyboard routine will not process keys we do not want
    ;	to deal with.
    ;
tsk1:
	lds	bx,cs:[pErrMode]	; get pointer to DOS err mode
	cmp	byte ptr ds:[bx],0	; if inside INT 24 routine...
	push	ss
	pop	ds
	jnz	TTYKeyboard		; ...dont process ALT-TAB, ENTER, SPACE
	jmp	TTYdefault

;**
;
; TTYKeyboard - Handler for WM_KEYDOWN message
;
; ENTRY:
;	SS = DS = Data of app key is directed to
;	CX = wParam from Windows Key message (Windows Virtual key that went down)
;	ES:DI = lParam from Windows Key message (ES = Scan code for the key)
; EXIT:
;	DX = AX = 0
; USES:
;	ALL but DS,SS,BP
;
TTYKeyboard:
	cmp	[fScroll],0	; Scroll going?
	jz	key0		; No
	jmp	DoScrollCommand ; Perform scroll

key0:	test	[fSelect], FS_ACTIVE; are we in select mode?
	jz	keyPrtSc	; no, just post message
    ;
    ; We are doing a Selection:
    ;  Move the end point according to the Virtual key we have.
    ;
	mov	ax,word ptr [Selection+SelectCol]

	inc	al		; Assume to right
	cmp	cx,VK_RIGHT	; Is it?
	je	key6		;  Yes, set new selection
	dec	al		;  no, assumption wrong
	dec	al		; Assume, left
	cmp	cx,VK_LEFT	; Is it?
	je	key6		;  Yes, set new selection
	inc	al		;  No, wrong assumption
	inc	ah		; Assume down
	cmp	cx,VK_DOWN	; Is it?
	jz	key6		;  Yes, set new selection
	dec	ah		;  no, assumption wrong
	dec	ah		; Assume up
	cmp	cx,VK_UP	; Is Left?
	jne	keyPrtSc	;  no, just post
	errn$	key6		;  yes, assume new selection
    ;
    ; AX is new selection endpoint
    ;
key6:	call	CoordRangeCheck ; Make sure it is on the screen
	call	SetScreenCoords ; Get in screen coords
	mov	di,ax		; Make it look like mouse
	mov	es,dx
    ;
    ; Is Shift down?
    ;	Yes, extend selection
    ;	No, start a new selection
    ;
	call	CheckShift	 ; Flags are set
	jns	key7		; Yes, handle via StartSelection
	call	ExtendSelection ; Extend the selection to new endpoint
	call	TrackRowCol	; Scroll if necessary
	jmp	Okay		; All done
key7:
	call	StartSelection
	jmp	Okay

    ;
    ; Look for ALT-PrtSc = total screen copy
    ;
keyPrtSc:
	mov	ax,VK_MENU
	call	CheckKeyState	; is ALT key down?
	jns	key8		; No
	cmp	cx,VK_MULTIPLY	; print key?
	jnz	key8no		; no
    ;
    ; ALT-PrtSc - "select" whole screen into clip board
    ;
	push	DI
	push	CX
	push	AX
	call	GrabText
	pop	AX
	pop	CX
	pop	DI
   ;
   ;  Post message to ourselves so keyboard message comes after char message
   ;
key8:
key8no: mov	ax,es
	mov	ch,al		; ch = scan code, cl = virtual key
    ;
    ; CX is wParam for message
    ; DI is low word of lParam for message (key count)
    ;
key8a:
	mov	al,VK_MENU
	call	CheckKeyState		; Get state of ALT key
	mov	dh,ah			; In high word of lParam
	mov	al,VK_CONTROL
	call	CheckKeyState		; Get state of control key
	shr	ah,1
	or	dh,ah			; In high word of lParam
	mov	al,VK_SHIFT
	call	CheckKeyState		; Get state of SHIFT key
	shr	ah,1
	shr	ah,1
	or	dh,ah			; In high word of lParam
	mov	ax,WM_KEYFIRST-1
    ;
    ; Message is WM_KEYFIRST-1
    ; wParam is input CX (typically KEY)
    ; Low word of lParam is input DI (typically low word of lParam of Windows message)
    ; High word of lParam is key states:
    ;	    Bit 15 - State of ALT key
    ;	    Bit 14 - State of Control key
    ;	    Bit 13 - State of Shift key
    ;
	cCall	PostMessage,<hTTYwnd,ax,cx,dx,di>
key9:	jmp	Okay

;**
;
; TTYKeyup - Handler for WM_KEYUP message
;
;	We are only interested in the Alt going up after a Alt-Numpad
;	sequence. In this case, we need to post a VK_KEYFIRST-1 message
;	for TTYascii.
;
; ENTRY:
;	SS = DS = Data of app key is directed to
;	CX = wParam from Windows Key message (Windows Virtual key that went down)
;	ES:DI = lParam from Windows Key message (ES = Scan code for the key)
; EXIT:
;	DX = AX = 0
; USES:
;	ALL but DS,SS,BP
;
TTYKeyup:
	cmp	cx, VK_MENU		;Is it an Alt key?
	jne	kuExit			; No
	cmp	AsciiCount, 0
	je	kuExit
	xor	cx, cx
	mov	di, AsciiCount
	jmp	key8a
kuExit:
	jmp	Okay

;**
;
; CheckShift - Return state of shift key
;
; EXIT:
;	BX is return from GetKeyState for SHIFT key
;	Sign bit set
;	    Shift Key is down
;	Sign bit re-set
;	    Shift Key is NOT down
; USES:
;	BX,DX,FLAGS
;
CheckShift:
	mov	ax,VK_SHIFT
	errn$	CheckKeyState	; Fall into CheckKeyState
;**
;
; CheckKeyState - Get Key State of indicated key
;
;  This routine is used to get the Up/Down state of keys. Typically
;	the CTRL ALT or SHIFT keys.
;
; ENTRY:
;	AX is key to get state of
; EXIT:
;	High bit of AX is its up/down state
;	 bits 14-8 are 0
;	 bits 7-0 are ???????
; USES:
;	AX,FLAGS
;
CheckKeyState:
	push	bx
	push	cx
	push	dx
	push	es
	cbw
	cCall	GetKeyState,<ax>
	and	ah,10000000b		; Mask to up-down state
	pop	es
	pop	dx
	pop	cx
	pop	bx
	ret

;---------
;
; Mouse Handlers for Windowed Applications
;
;	The mouse handlers have to worry about marking of text.
;

;**
;
; TTYMouseMove Handler for Mouse move message (selection logic)
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	Selection extended to new mouse position if selection in progress
; USES:
;	C standard
;
TTYMouseMove:
	test	cl,MK_LBUTTON		; Is this a button message?
	jz	mmv1			; Yes, ignore it
MouseMove:
	call	ExtendSelection 	; Set new selection if approp
	call	TrackRowCol		; Track if necessary
mmv1:	jmp	Okay

;**
;
; TTYMouseUp - Handler for mouse up (selection end)
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	Selection flags reset (but selection NOT rendered)
;	Mouse capture released
; USES:
;	C standard
;
TTYMouseUp:
	cCall	ReleaseCapture		; Not captured anymore
	jmp	Okay

;**
;
; TTYMouseDown - Handler for mouse down (start selection)
;
;   This routine contains the implementation of a nice user feature.
;	If the user ends a selection (mouse up) but then goes OOPS,
;	I messed up. He can "continue" the selection by doing
;	SHIFT - Mouse Down.
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	Mouse captured to app
;	App gets input focus UNLESS SHIFT-Mouse Down
;	Selection started (prev cleared) UNLESS SHIFT-Mouse Down
;	DX = AX = 0
; USES:
;	C standard
;
TTYMouseDown:
    ;
    ; If we are iconic don't bother
    ;
	cmp	[fIcon], SIZEICONIC
	je	MSFend
    ;
    ; Capture the mouse so we get events outside the client area
    ;
	push	es
	cCall	SetCapture,<si> 		; Capture mouse to Window
	pop	es
    ;
    ; Check for SHIFT-Mouse Down "continue previous selection" command
    ;
	call	CheckShift	; SHift key down?
	js	MouseMove	; Yes, this is a continue
mdn1:	call	StartSelection
	errn$	MySetFocus	 ; Fall into MySetFocus
;**
;
; MySetFocus - Set focus to this app
;
; ENTRY:
;	DS = Data of app that wants focus
; EXIT:
;	App given focus
;	AX = DX = 0
; USES:
;	C standard
;
MySetFocus:
	cmp	[fFocus], 0	; If already have focus we don't need to
				; do anything
	jne	MSFend
	cCall	SetFocus,<hTTYWnd>; get the input focus
MSFend: jmp	Okay


;--------
;
; Clipboard routines for windowed apps
;
;

;**
;
; TTYCopy - Perform Copy operation from system menu
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	Any selection rendered into clipboard and cleared
;	DX = AX = 0
; USES:
;	C standard
;
TTYCopy:
	call	GetSelection		; Render selection
	or	ax,ax			; Any selection?
	jz	JmpOkay 		; No, done
	mov	di,ax			; Handle in DI
	mov	si, CF_OEMTEXT		;!! HP KMF 040987
	call	DoDataX 		; Selection into the clipboard
	call	AddANSIToClipboard	;!! HP KMF 040987
	call	CancelSelection 	; Clear the selection
JmpOkay:jmp	Okay			; Jump into Okay

;**
;
; DoDataX - Put data in the clipboard
;
; ENTRY:
;	DS = Data of app placing data (hTTYWnd must be valid)
;	SI = Format of clipboard data
;	DI = Handle of global object containing data
; EXIT:
;	Indicated data placed in clipboard
;	Previous clipboard contents destroyed
; USES:
;	ALL but DS,ES,SS,BP,SI,DI (C standard)
;
DoDataX:
	cCall	OpenClipboard,<hTTYWnd> 	; Open the clipboard
	cCall	EmptyClipboard			; Empty it
	cCall	SetClipboardData,<si,di>	; Place our data
	cCall	CloseClipboard			; Close clipboard
	ret

;**
;
; GetDatax - Remove data from data exchange buffer if present
;
; ENTRY:
;	DS = Data of app removing data
;	hDatax is handle of data buffer (== 0 if none)
;	pDatax is pointer into hDatax of next byte to get
; EXIT:
;	Zero Set
;	    No data available
;	    If hDatax was valid, it is freed
;	Zero re-set
;	    DL is next byte in buffer, data removed (pDatax advanced)
; USES:
;	ALL but DS,SS,BP,DI
;
GetDatax:
	call	PeekDatax	; Any data
	jz	gdx2		; no, done
	cmp	dx,0A0Dh	; Is peeked data CR LF?
	jz	gdx1		; Yes, remove both and return CR as char
	dec	si		; no, only remove one char from buffer
gdx1:	mov	[pDatax],si
	or	si,si		; Re-set zero
gdx2:	ret

;**
;
; PeekDatax - See if there is data exchange data to get
;
; ENTRY:
;	DS = Data of app removing data
;	hData is handle of data buffer (== 0 if none)
;	pDatax is pointer into hDatax of next byte to get
; EXIT:
;	Zero Set
;	    No data available
;	    If hDatax was valid, it is freed
;	Zero re-set
;	    DX is next two bytes in buffer, data not removed
;	    SI is pointer to next byte of buffer (byte after two bytes read)
; USES:
;	ALL but DS,SS,BP,DI
;
PeekDatax:
	mov	ax,[hDatax]
	or	ax,ax				; Any data?
	jz	pdx2				; No, done, zero set
    ;
    ; Lock the data and look
    ;
	push	di
	call	MyLockObject
	mov	es,dx
	mov	si,[pDatax]
	lods	word ptr es:[si]
	push	ax
	call	MyUnlock
	pop	dx		; Data in DX
	or	dl,dl		; End of data
	jnz	pdx1		; no
	xor	dx,dx
	xchg	[hDatax],dx	; clear hDatax
	cCall	GlobalFree,<dx>
	xor	dx,dx		; return zero flag
pdx1:	pop	di
pdx2:	ret

;**
;
; TTYPaste - Perform paste request into old app by sucking out clipboard
;		contents and setting up app data exchange buffer
;
; ENTRY:
;	DS = Data of old app pasting to
;	SI = Window handle of this app
; EXIT:
;	hDatax and pDatax set up
;	If paste data found in clipboard it is transfered out of the clipboard
;	    into the global object pointed to by hDatax and rendered in OEM
;	    form.
;	DX:AX = 0
; USES:
;	ALL but DS,SS,BP
;
;! HP KMF 040987  Modified to check to see if clipboard contains CF_OEMTEXT,
;		  if so, this is what is pasted.
TTYPaste:
	xor	di,di
	cmp	[hDataX],di		; already pasting?
	jz	tps1
	jmp	tps3			; yes, ignore this paste
tps1:
    ;
    ; Get paste data from clipboard
    ;
	cCall	OpenClipboard,<si>
	mov	[pDataX],di		; zero clipboard handle index

TryCF_OEMTEXT:
	mov	di, CF_OEMTEXT
	cCall	GetClipboardData, <di>
	or	ax, ax			;OEM Text data?
	jz	TryCF_Text		;No, try ANSI
	push	di
	jmp	GotOne
TryCF_TEXT:
	mov	di, CF_TEXT
	cCall	GetClipboardData, <di>
	or	ax, ax			;ANSI Text data?
	jz	tps2			;No, nothing to paste
	push	di
GotOne:
	mov	di, ax
	cCall	GlobalSize,<di> 	; Get size of clipboard data

    ;
    ; Allocate an object to hold data
    ;
	mov	bx,GMEM_MOVEABLE+GMEM_ZEROINIT
	regptr	ddSize,dx,ax
	mov	si,ax
	cCall	GlobalAlloc,<bx,ddSize>
	jcxz	tps2			; Can't get clipboard data
    ;
    ; Set hDatax, transfer data, and render it into OEM form
    ;
	push	si			;low order word of size of hMem
	mov	[hDataX],ax
	cCall	GlobalLock,<ax> 	;dx:ax -> hMem
	push	dx
	cCall	GlobalLock,<di> 	;dx:ax -> hClipData
	pop	es			;es = hMem segment
	pop	cx			;cx = size of hMem == size of hClipData
	push	di			;Argument to GlobalUnlock below
	push	ds			;need to use ds, so save contents
	mov	ds,dx			;ds = hClipData segment
	xor	di,di			;ds:si -> hClipData
	xor	si,si			;es:di -> hMem
	rep	movsb			; Copy data

	pop	ds			;restore ds
	pop	di
	pop	ax			;ax = CF_OEMTEXT | CF_TEXT
	push	di

	cmp	ax, CF_OEMTEXT		;if it's OEMText, then do not convert
	je	tpsNoConvert
	xor	di,di			;otherwise convert to ANSI
	regptr	esdi,es,di
	cCall	AnsiToOem,<esdi,esdi>	; Render it to OEM form

tpsNoConvert:

    ;
    ; Unlock stuff
    ;
	cCall	GlobalUnlock ; ,<ax>	; argument already pushed above
	cCall	GlobalUnlock,<hDataX>
tps2:	cCall	CloseClipboard		; just close it then
tps3:	xor	ax,ax
	cwd
	ret


;**
;
; GetSelection - Store selection (if present) into global memory
;
; ENTRY:
;	DS = SS = Data of app to look for selection in
; EXIT:
;	AX = Handle of global object containing ANSI rendering of selection
;	   == 0 if there was no selection, or error getting it
; USES:
;	ALL but DS,ES,SI,BP,SS
;
GetSelection:
	xor	ax,ax
	test	[fSelect], FS_MARK; Is there a selection?
	jz	gsl3		; No
	call	SetUpSelection	; Get info about the selection
    ;
    ; Compute size of selection and try to alloc
    ;
	xor	dh,dh
	push	bx
	push	dx
	mov	ax,bx
	add	ax,2		; add in cr,lf
	mul	dx
	inc	ax		; room for a null at the end
	call	MyAlloc
	mov	si,di		; si points to screen (DS set in a minute)
	mov	es,dx
	mov	di,ax		; es:di points at save area (mem just alloced)
	pop	dx
	pop	bx
	mov	ax,0		; If error, AX is 0. DO NOT XOR!!!!!!!!!!!!
	jz	gsl3		; Couldn't alloc memory, forget it
	mov	ds,[VidSegment]
    ;
    ; Transfer screen data of selection into object
    ;
gsl1:	mov	cx,bx		; cx = word count of one line
	push	si
gsl2:	lodsw
	stosb
	loop	gsl2
	mov	ax,0A0Dh	; CR LF at end of line
	stosw
	pop	si
	add	si,2*MaxCol	; Next line
	dec	dx		; One line completed
	jnz	gsl1		; more lines
	dec	di		; Don't want CR-LF at end of selection
	dec	di
	xor	ax,ax		; NULL terminate
	stosb
	push	ss		; Restore ds, leave data in OEM format
	pop	ds
	errn$	MyUnlock	; Fall into MyUnlock
;**
;
; MyUnlock - Unlock the object allocated by prev call to MyAlloc
;
; ENTRY:
;	DS = Data of app that owns object
;	Handle is its handle
; EXIT:
;	AX, DI = Handle
; USES:
;	ALL but DS,ES,SI,BP,SS
;
MyUnlock:
	mov	di,[HandleSave]
	cCall	GlobalUnLock,<di>
	mov	ax,di
gsl3:	ret

;**
;
; ConvertToAnsi - Convert buffer from OEM to ANSI format
;
; ENTRY:
;	ES:0 -> NUL terminated buffer to convert
; EXIT:
;	Buffer converted
;	DS = Input SS
; USES:
;	ALL but SS,BP
;
ConvertToAnsi:
	push	ss
	pop	ds
	xor	di,di
	regptr	esdi,es,di
	cCall	OemToAnsi,<esdi,esdi>
	ret

;**
;
; MyAlloc - Allocate a global object of indicated size and lock it
;
; ENTRY:
;	DS = Data of app for which alloc is being performed
;	AX is size in bytes to alloc WARNING DANGER: HIGH BIT MUST BE CLEAR!!!!
; EXIT:
;	Zero Set
;	    Memory could not be allocated
;	Zero re-Set
;	    Object allocated and LOCKED
;	    DX:AX is its address
;	    Handle is its global handle
;	    DIRECTION FLAG CLEARED!!!!!
; USES:
;	ALL but DS,ES,SS,BP,SI,DI	(C standard)
;
MyAlloc:
	cwd
	mov	bx,GMEM_MOVEABLE+GMEM_ZEROINIT+GMEM_SHARE
	regptr	ddSize,dx,ax
	cCall	GlobalAlloc,<bx,ddSize>
	or	ax,ax
	jz	mlo1
	errn$	MyLockObject		; Fall into MyLockObject
;**
;
; MyLockObject - Lock the handle in AX
;
; ENTRY:
;	DS = Data of app which owns object
;	AX is handle of object to lock
; EXIT:
;	Zero re-set
;	DX:AX is address of locked object
;	Handle = Input AX
; USES:
;	ALL but DS,ES,SS,DI,SI,BP (C standard)
;
MyLockObject:
	mov	[HandleSave],ax
	cCall	GlobalLock,<ax>
	cld
	or	dx,dx			; re-set zero
mlo1:	ret




;**
;
; AddANSIToClipboard - Add ANSI text to the clipboard
;
; ENTRY:
;	Clipboard contains data in CF_OEMTEXT format
;
; EXIT:
;	Clipboard contains both CF_OEMTEXT and CF_TEXT
;
; USES:
;	All but DS, SS, BP
;
AddANSIToClipboard:
	cCall	OpenClipboard, [hTTYWnd]
	mov	ax, CF_OEMTEXT
	cCall	GetClipboardData, <ax>
	or	ax, ax
	jz	EndAddANSI

	mov	di, ax			;di = hOEMText
	cCall	GlobalSize, <di>
	mov	si, ax			;si = sizeof OEM text

	mov	bx, GMEM_MOVEABLE+GMEM_ZEROINIT+GMEM_SHARE
	RegPtr	dwBytes, dx, ax
	cCall	GlobalAlloc, <bx, dwBytes>
	or	ax, ax
	jnz	MemOkAATC
	jmp	EndAddANSI
MemOkAATC:
	push	ax
	push	si

	cCall	GlobalLock, <ax>	;lock both OEMn
	push	dx
	cCall	GlobalLock, <di>
	pop	es
	pop	cx			;loop counter = size of text
	push	di
	push	ds
	mov	ds, dx
	xor	di, di			;perform the copy
	xor	si, si
	rep	movsb
	pop	ds

	xor	di, di
	RegPtr	lpANSIText, es, di
	cCall	OemToAnsi, <lpANSIText, lpANSIText>
	pop	di
	cCall	GlobalUnlock, <di>

	pop	di
	mov	ax, CF_TEXT
	cCall	SetClipboardData, <ax, di>
	cCall	GlobalUnlock, <di>
EndAddANSI:
	cCall	CloseClipboard

	ret



;-----
;
; Marking code for both mouse and keyboard marks
;
;	Keyboard and mouse marks behave in similar manner and
; can be used interchangeably.
;
;	The user interface for 2.0 marking is as close to the
;	style guidelines for windows applications as possible
;	However, to support a keyboard only interface we
;	had to introduce a "Mark" mode. When in mark mode, "Mark"
;	is prepened to the window title.
;
;	Mark mode is entered by:
;	   1) Selecting Mark in the control menu
;	   2) A mouse down event in the Window client area
;
;	While in mark mode, the user can select with the mouse
;	and with cursor control pad. Shift holding down shift
;	will extend the mark.
;
;	Mark mode is exited by:
;	   1) Pressing ESC or Ctrl-C
;	   2) Selecting Copy in the control menu
;	   3) Pressing Enter in which case Render Mode is entered.
;	   4) Pressing another key. These keys will be feed to the app.
;
;	In Render mode, the mark from the mark mode is left on the
;	screen but the "Mark" is removed from the window title. While
;	in render mode, the mark cannot be extended or moved. Render mode
;	was kept for compatability with Windows 1.0.
;
;	Render mode left by:
;
;	   1) Pressing any key
;	   2) Entering mark mode
;	   3) Selecting Copy in the control menu
;
;

;**
;
; TTYSelect - Handle Select from system menu
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	Selection started at 0,0 of display
; USES:
;	C standard
;
TTYSelect:
	xor	di,di
	mov	es,di			; pretend mouse went down at display org
	call	StartSelection		; Fall into StartSelection
	jmp	Okay

;**
;
; StartSelection - Handle Select at indicated origin
;
; ENTRY:
;	DS = Data of app
;	ES:DI = Display coords of start of new selection
; EXIT:
;	Prev Selection cleared, new selection started at ES:DI of display
;	Copy and Scroll menu items disabled.
;	Anchor set a ES:DI
;	fSelect set to FS_ACTIVE | FS_MARK
; USES:
;	C standard
;
StartSelection:
	push	es			; Save coord
	push	di
	call	SetSelectText		; Change Window text
	call	ClearSelection		; Clear any prev selection
	mov	[fSelect], FS_ACTIVE	;
	pop	di			; Recover start coord
	pop	es
	call	GetMouseCoords		; Get screen Row/Col position of mouse
	mov	[Anchor],ax		; This is the anchor
	inc	di			; pretend it moved a little bit
	call	ExtendSelection
	call	TrackRowCol		; Scroll if necessary
	ret

;**
;
; FlipSelection - Invert selection in video buffer and on display
;
; ENTRY:
;	DS = Data of app selection is in
;	DS:DI -> Selection structure
;	AL is Col value for selection endpoint
;	AH is Row value for selection endpoint
;	AndMask OrMask define the attribute operation performed in vid buffer
; EXIT:
;	Data in indicated selection is inverted on screen and in logical vid buffer
;	Selection structure updated for Input endpoint
; USES:
;	ALL but DS,SS,BP
;
FlipSelection:
	push	di
	call	MakeRect		; Set values in selection structure
	call	MyGetDC 		; Get a DC, and Window handle
	pop	bx
	add	bx,Left 		; Point at Screen coord RECT structure
	errn$	InvertSelection 	; Fall into InvertSelection
;**
;
; InvertSelection - Invert a selection in the display and logical vid buffer
;
; ENTRY:
;	DS = Data of app with selection
;	DS:BX -> RECT structure in screen coords of region to invert
;	Selection represents this same RECT
;	DI = Handle to DC to invert in
;	SI = Window handle of owner of the DC
;	AndMask OrMask define the attribute operation performed in vid buffer
; EXIT:
;	Indicated region inverted in display and logical vid buffer
; USES:
;	ALL but DS,SS,BP
;
InvertSelection:
	call	MyInvert		; Invert display region
	errn$	InvertVidSelection	; Fall into InvertVidSelection
;**
;
; InvertVidSelection - Invert a selection in the logical video buffer
;
; ENTRY:
;	DS = Data of app with selection
;	Selection represents region to invert
;	AndMask OrMask define the attribute operation performed
; EXIT:
;	Indicated region inverted in logical vid buffer
; USES:
;	ALL but DS,SS,BP
;
InvertVidSelection:
	call	SetUpSelection		; Point into buffer and set limits
	inc	di			; Point at first attrib byte
	mov	ah,[AndMask]
	mov	dh,[OrMask]
flv0:	mov	cx,bx			; # of words selected on each line
	push	di
flv1:	mov	al,es:[di]		; Get attrib
	and	al,ah			; Perform operation
	or	al,dh
	stosb				; New attrib
	inc	di			; Skip over char
	loop	flv1			; Next attrib on line
	pop	di
	add	di,2*MaxCol		; Next line
	dec	dl			; One more line done
	jnz	flv0			; More lines to do
	ret

;**
;
; ClearSelection - Clear any selections in progress
;
; ENTRY:
;	DS = Data of app to clear selection for
; EXIT:
;	Any selections cleared
; USES:
;	FLAGS
;
ClearSelection:
	test	[fSelect], FS_MARK	; selection started?
	jz	csl1			; no, all done
	push	ax
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	es
	mov	[OrMask], 00000000b	; Set operation to "turn off reverse
	mov	[AndMask],01111111b	;    video"
    ;
    ; "Un" invert the inverted selection
    ;
	mov	di,dataOffset Selection
	mov	ax,[di] 		; Selection endpoint is what it is
	call	FlipSelection
	and	[fSelect], not FS_MARK	; No selction going
    ;
    ; Gray the copy menu item
    ;
	mov	ax, cmCopy
	call	MyDisableItem
	pop	es
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
csl1:	ret


;**
;
; ExtendSelection - Set new selection endpoint
;
; ENTRY:
;	DS = Data of old app being selected
;	ES = Mouse Y coord of current selection endpoint (client area coords)
;	DI = Mouse X coord of current selection endpoint (client area coords)
; EXIT:
;	Selection structure and display and vid buffer updated for selection
;	IF Mouse is no longer down, Selection is stopped
;	AX = new endpoint in ROW/COL
; USES:
;	ALL but DS,SS,BP
;
ExtendSelection:
	test	[fSelect], FS_MARK		; Is there a mark up there
	jnz	xds1
    ;
    ; This is the FIRST extension of the selection
    ;
	or	[fSelect], FS_MARK		; Don't do this again
	push	es				; Save mouse location
	push	di
    ;
    ; Set the operation on the video buffer to "turn on reverse video"
    ;
	mov	[OrMask], 10000000b
	mov	[AndMask],11111111b
	mov	ax,[Anchor]
	mov	di,dataOffset Selection
	call	FlipSelection			; Flip the selection (which
						;   for now is the single char
						;   at Anchor)
	mov	bx,MF_ENABLED
	mov	ax,cmCopy
	call	MyEnableItem			; Enable the copy menu item
	pop	di
	pop	es
    ;
    ; Compute the new selection endpoint and update display etc.
    ;
xds1:	call	GetMouseCoords			; Convert mouse coords
	cmp	ax,word ptr [Selection+SelectCol] ; New endpoint?
	jz	esEnd				; no, just track
	push	ax				; Save new endpoint
    ;
    ; UN-invert previous selection in video buffer
    ;
	mov	[OrMask], 00000000b
	mov	[AndMask],01111111b
	call	InvertVidSelection
    ;
    ; Take the intersection of the new and old selections to set area
    ;	of Window display that needs updating
    ;
	mov	ax,word ptr [Selection+SelectCol]
	mov	di,dataOffset prevSelection	; Old selection at prevSelection
	call	MakeRect
	pop	ax
	mov	di,dataOffset Selection 	; New selection at Selection
	call	MakeRect
    ;
    ; Take intersection of new and old selection
    ;
	mov	bx,dataOffset Selection+Left
	mov	cx,dataOffset prevSelection+Left
	mov	dx,dataOffset InterRect
	regptr	lpSelect,ds,bx
	regptr	lpPrevSelect,ds,cx
	regptr	lpInterRect,ds,dx
	cCall	IntersectRect,<lpInterRect,lpSelect,lpPrevSelect>
	call	MyGetDC
    ;
    ; Exclude the intersection of the two selections from any inversion
    ;
	cCall	ExcludeClipRect,<di,InterLeft,InterTop,InterRight,InterBottom>
    ;
    ; Un-invert the region of the Windows display that needs it
    ;
	mov	cx,dataOffset prevSelection+Left
	cCall	InvertRect,<di,lpPrevSelect>
    ;
    ; Invert the NEW selection in the video buffer and on the display
    ;
	mov	[OrMask], 10000000b
	mov	[AndMask],11111111b
	mov	bx,dataOffset Selection+Left
	call	InvertSelection
	mov	ax,word ptr [Selection+SelectCol] ; Selection endpoint on screen
esEnd:
	ret

;**
;
; TrackRowCol - Update display and scroll bars for current video position
;
; ENTRY:
;	DS = Data of old app being tracked
;		MUST BE AN APP RUNNING IN A WINDOW!!!!!!
;	AX is Row/Col of new track position (position of "currency" of display)
; EXIT:
;	Display and scroll bars updated so indicated position is displayed
;		in app Window
; USES:
;	ALL but DS,SS,BP
;
TrackRowCol:
	call	SetScreenCoords
    ;
    ; Check screen X position to see if it is on screen
    ;
	or	ax,ax
	jl	trc1		; Pos is off screen to left
	add	ax,[CharWidth]
	sub	ax,[ScrWidth]
	jg	trc1		; Pos is off screen to right
	xor	ax,ax		; Pos is on screen, no adjust needed
    ;
    ; Check screen Y position to see if it is on screen
    ;
trc1:	or	dx,dx
	jl	trc2		; Pos is off screen to top
	add	dx,[CharHeight]
	sub	dx,[ScrHeight]
	jle	trc3		; Pos is on screen, no vert adjust needed
    ;
    ; DX is signed amount pos is off screen (neg off to top, pos off to bottom)
    ;
trc2:	push	ax		; Save horiz adjust value
	add	dx,[VisiRow]	; Need to scroll window to this location
	mov	di,dx
	mov	cl,SB_THUMBPOSITION
	call	TTYVscroll	; Scroll vertical
	pop	ax

trc3:	or	ax,ax		; Any horiz adjust needed?
	jz	trcx		; Nope, done
	add	ax,[VisiCol]	; Need to scroll window to this location
	mov	di,ax
	mov	cl,SB_THUMBPOSITION
	jmp	TTYHscroll	; Scroll horizontal
trcx:	ret

;**
;
; CancelSelection - Stop the selection.
;
;	Reset the Window and menu to normal manner. This function
;	can be called when the there is no selection taking place.
;
; ENTRY:
;	DS = Data of app to stop selection of
;	hMenu is menu handle of windows system menu
; EXIT:
;	cmCopy system menu item disabled
;	Window
; USES:
;	ALL but DS,SS,BP
;
CancelSelection:
	call	ClearSelection		; ClearSelection tests for FS_MARK
	errn$	RenderSelection
;**
;
; RenderSelection - Render the current selection
;
;	Reset the Window title but don't remove the mark
;
; ENTRY:
;	DS = Data of app to stop selection of
;	hMenu is menu handle of windows system menu
; EXIT:
;	Window title reset
;	Scroll item is enabled
;
; USES:
;	ALL but DS,SS,BP
;
RenderSelection:
	test	[fSelect], FS_ACTIVE		; Are we in selecting?
	jz	rsExit				;   NO, don't do anything.
	and	[fSelect], not FS_ACTIVE
	call	ResetWinText			; Remove "Mark"
rsExit: ret


;**
;
; SelectRect - Make a rectangle the selection
;
;	This function is called when a [MARK] opcode has coordinates.
;
; ENTRY:
;	CL = XOrg
;	CH = YOrg
;	DL = XExt
;	DH = YExt
;	DS = Data of app selection is in
; EXIT:
;	Data in rectangle is inverted on screen and in logical vid buffer.
;	XExt and YExt are the selection endpoints.
; USES:
;	AX, BX, CX, DX, ES
;	Flags
;
SelectRect:
   ;
   ; Clear anything that is up there
   ;
	call	ClearSelection
   ;
   ; Set the anchor
   ;
	mov	ax, cx
	call	CoordRangeCheck
	mov	[Anchor],ax
   ;
   ; Mark the screen. Convert to ClientCoord
   ;
	mov	ax, dx
	call	CoordRangeCheck
	call	SetScreenCoords
	mov	di, ax
	mov	es, dx
	call	ExtendSelection
	ret

;**
;
; MyDisableItem -
;
; ENTRY:
;	DS = Data of app to stop selection of
;	hMenu is menu handle of windows system menu
;	AX is menu item ID of menu item in system menu to disable
; EXIT:
;	Menu item grayed (disabled)
; USES:
;	ALL but DS,SS,BP
;
MyDisableItem:
	mov	bx,MF_GRAYED
	errn$	MyEnableItem			; Fall into MyEnableItem
;**
;
; MyEnableItem - Modify system menu item
;
; ENTRY:
;	DS = Data of app to stop selection of
;	hMenu is menu handle of windows system menu
;	AX is menu item ID of menu item in system menu to modify
;	BX is wEnable item parameter to EnableMenuItem
; EXIT:
;	Indicated action performed on menu item
; USES:
;	ALL but DS,SS,BP
;
MyEnableItem:
	cmp	AH,SC_CLOSE/256
	jb	MEI2
	cCall	EnableMenuItem,<hMenu,ax,bx>
	ret
MEI2:
	push	ES

; MSF  Changed call to GlobalLock to be a call to LockMenuSeg
;
	call	LockMenuSeg

; MSF Done
	or	DX,DX
	jz	MEI95			;no menu segment
	push	BX
	push	AX
	mov	ES,DX			;get menuseg in ES
	pop	AX
	call	GetItem 		;get the menu item structure
	pop	BX
	jc	MEI90			;do nothing if it doesn't exist
	mov	SI,AX
	lea	DI,PokeMenu		;get UpdateMenu structure
	mov	CX,ES:[SI].ID
	mov	[DI].wIDChangeItem,CX
	mov	[DI].wIDNewItem,CX
	mov	CX,ES:[SI].flags	;get the flags
	and	CX,NOT STYLE_MASK	;clear all style bits
	or	CX,BX			;or in the correct style
	or	CX,MF_BYCOMMAND+MF_CHANGE ;change by command (ID)
	mov	[DI].wChange,CX
	mov	CX,ES:[SI].item
	mov	[DI].wMacro,CX
	mov	CX,ES:[SI].handle
	mov	[DI].wMenuID,CX
	mov	CX,MaxMenuWidth+1
	mov	DX,ES			;exchange seg regs
	mov	BX,DS
	mov	ES,BX
	mov	DS,DX
	push	SI			;save indexes
	push	DI
	add	SI,text
	add	DI,wItem
	rep	movsb			;copy the string
	pop	DI

; MSF  ned to convert back to OEM text
	mov	SI,DI
	add	SI,wItem
	call	UnFixText		; Good apps need text back to OEM set
; MSF Done

	pop	SI
	mov	ES,DX			;restore seg regs
	mov	DS,BX
	regptr	dsdi,ds,di
	cCall	UpdateMenu, dsdi	;change the menu
MEI90:
;MSF Changed call to GlobalUnlock to be call to UnlockMenuSeg
;    and added label for error condition above to jump to

	call	UnLockMenuSeg

MEI95:
; MSF Done

	pop	ES			;get original ES back
	ret
;***




;**
;
; MakeRect - Set new selection rectangle
;
; ENTRY:
;	DS = Data of app selection is in
;	DS:DI -> Selection structure
;	AL is Col value for selection endpoint
;	AH is Row value for selection endpoint
;	Anchor is selection start endpoint
; EXIT:
;	DIRECTION FLAG CLEARED!!!!!!!
;	Selection structure Values for this indicated selection set
; USES:
;	AX,BX,DX,DI,ES,FLAGS
;
MakeRect:
	cld
	push	ds
	pop	es
	stosw			; Store Select Row/Col in selection structure
    ;
    ; Now we figure which way selection is going
    ;
	mov	bx,[Anchor]	; get start of selection
	cmp	bl,al		; Anchor Col > New SelectCol?
	jbe	mr1		; no, RightCol = New SelectCol, LeftCol = Anchor col
	xchg	bl,al		; RightCol = Anchor Col, LeftCol = New SelectCol
mr1:	cmp	bh,ah		; Anchor Row > New SelectRow?
	jbe	mr2		; no, BottomRow = New SelectRow, TopRow = Anchor Row
	xchg	bh,ah		; BottomRow = Anchor Row, TopRow = New SelectRow
    ;
    ; AX is RightCol and BottomRow
    ; BX is LeftCol and TopRow
    ;
mr2:	stosw			; Set RightCol and BottomRow in selection struc
	push	ax
	mov	ax,bx
	stosw			; Set LeftCol and TopRow in selection struc
	call	SetScreenCoords ; Convert Left and Top to screen coords
	stosw			; Set Left in selection structure
	xchg	ax,dx
	stosw			; Set Top in selection structure
	pop	ax		; AX = RightCol and BottomRow
	call	SetScreenCoords ; Convert Right and Bottom to screen coords
	add	ax,[CharWidth]	; Add one char to right as char on right is included
	stosw			; Set Right in selection structure
	xchg	ax,dx
	add	ax,[CharHeight] ; Add one line to bottom as bottom line is included
	stosw			; Set Bottom in selection structure
	ret

;**
;
; MyGetDC - Get a DC to the old app Window
;
; ENTRY:
;	DS = Data of old app to get a DC for
; EXIT:
;	DI is the handle to a DC
;	SI is Window handle of the owner of the DC
; USES:
;	ALL But DS,ES,SS,BP
;
MyGetDC:
	mov	si,[hTTYWnd]
	cCall	GetDC,<si>
	mov	di,ax		; DC handle to DI
	ret


;**
;
; CoordRangeCheck - Convert a coordinate to within range if necessary.
;
; ENTRY:
;	AH = Row of 80X25 client relative mouse position
;	AL = Col of 80X25 client relative mouse position
; EXIT:
;	AH = Row of 80X25 screen relative mouse position
;	AL = Col of 80X25 screen relative mouse position
; USES:
;	AX,BX,CX,DX
;	Flags
;
CoordRangeCheck:
    ;
    ; Make sure not below 0, if so, make 0
    ;
	cmp	al, 0
	jge	crc1
	mov	al, 0
crc1:
	cmp	ah, 0
	jge	crc2
	mov	ah, 0
    ;
    ; Make sure not above max, if so, set to max
    ;
crc2:
	mov	bx, Line25*256+Col80
	cmp	al, bl
	jle	crc3
	mov	al, bl
crc3:
	cmp	ah, bh
	jle	crc4
	mov	ah, bh
crc4:
	ret


;**
;
; GetMouseCoords - Convert mouse coords into Row/Col window coords
;
;    This routine converts the mouse coord into a position
;	on the 25X80 logical app screen
;
; ENTRY:
;	DS = Data of old app where mouse is
;	ES = Mouse Y coord of mouse position to convert (client area coords)
;	DI = Mouse X coord of mouse position to convert (client area coords)
; EXIT:
;	AH = Row of 80X25 screen relative mouse position
;	AL = Col of 80X25 screen relative mouse position
; USES:
;	AX,BX,CX,DX,FLAGS
;
GetMouseCoords:
	mov	ax,di			; get mouse x co-ord
	add	ax,[VisiCol]		; AX is 80X25 Windows coord
	cwd
	mov	bx,[CharWidth]		; 80186 cant idiv from memory
	idiv	bx			; div by width/char for COL
	mov	cx,ax			; Save in CX
	mov	ax,es			; get mouse y co-ord
	add	ax,[VisiRow]		; AX is 80X25 Windows coord
	cwd
	mov	bx,[CharHeight]
	idiv	bx			; Div by height/line for Row
	mov	ch,al
	mov	ax,cx			; ax returns row, col
	call	CoordRangeCheck 	;HP KMF 042887
	ret

;**
;
; SetScreenCoords - Convert 80X25 screen position into client Window
;			display position
;
;    This routine converts a position in the 80X25 logical screen into
;	a position on the displayed window
;
; ENTRY:
;	DS = Data of app conversion is for
;	AL = 80X25 Col
;	AH = 80X25 Row
; EXIT:
;	AX = ScreenX coord WILL BE NEGATIVE IF OFF SCREEN TO LEFT
;		NOT CHECKED FOR OFF SCREEN TO RIGHT
;	DX = ScreenY coord WILL BE NEGATIVE IF OFF SCREEN TO TOP
;		NOT CHECKED FOR OFF SCREEN TO BOTTOM
; USES:
;	AX,DX,FLAGS
;
SetScreenCoords:
	mov	dx,ax
	imul	byte ptr [CharWidth]	; Col X width of a char
	sub	ax,[VisiCol]		; Biased by visible origin
	xchg	dx,ax
	mov	al,ah
	imul	byte ptr [CharHeight]	; Row X height of a char
	sub	ax,[VisiRow]		; Biased by Visible origin
	xchg	ax,dx			; ScreenX = AX, ScreenY = DX
	ret

;**
;
; SetupSelection - Set up Vid buffer parms for a selection to be selected
;
; ENTRY:
;	DS = Data of old app with selection
;	Selection contains data indicating region
; EXIT:
;	ES:DI -> First char of selection in vid buffer (upper left corner)
;	DL is number of Lines selected (Height)
;	BX is number of words (chars) on each line selected (width)
; USES:
;	ALL but DS,SS,BP
;
SetupSelection:
	mov	bx,word ptr [Selection+LeftCol] ; BL LeftCol, BH TopRow
	push	bx
	call	MulCol				; Get vid buffer location
	mov	di,bx
	mov	es,[VidSegment] 		; es:di points at Screen bytes
	pop	cx
	mov	bx,word ptr [Selection+RightCol]; BL RightCol, BH BottomRow
	sub	bx,cx				; BL is width, BH is height
	inc	bh				; Col is inclusive
	inc	bl				; Row is inclusive
	mov	dl,bh				; DL is # rows
	xor	bh,bh				; BX is word count on each row
	ret


; **
;
; PostHalted - Display bad memory dialog box and halt
;
;     This routine is called when a bad memory modifier app is detected.
;	This is an app which has done a TSR exit (INT 27, DOS CALL 31H)
;	but does not have the memory bit of its pif file checked.
;
;     Display the bad memory dialog box amd halt.
;
; ENTRY:
;	DS = SS = Data
;	Windows turned on (enabled)
; EXIT:
;	NONE. DOESN'T EXIT, HALTS
; USES:
;	ALL BUT DS,SS,BP
;
PostHalted:
	mov	di,3		; ID of bad memory modifier dialog
	call	MyMakeProc	; Set up the Dialog box
	regptr	pProc,dx,ax
	regptr	pTemp,cx,di
	cCall	CreateDialog,<ds,pTemp,hTTYwnd,pProc>	; Create the dialog
	mov	si,ax
	mov	ax,SHOW_OPENWINDOW
	cCall	ShowWindow,<si,ax>	; Show the dialog
	cCall	UpdateWindow,<si>	; Make sure display is current
Dead:	jmp	Dead		; Hang

;**
;
; PostWarning - Put up cannot find PIF file Dialog
;
; ENTRY:
;	DS = Data of app dialoging
; EXIT:
;	DI is response code from dialog
; USES:
;	ALL but DS,ES,SI,SS,BP
;
PostWarning:			; this routine should preserve SI
	mov	di,2		; Cannot find PIF file Dialog
	jmp	short dodlg

;**
;
; GetCommandLine - Put up Parameters dialog
;
; ENTRY:
;	DS = Data of app dialoging
; EXIT:
;	DI is response code from dialog
; USES:
;	ALL but DS,ES,SI,SS,BP
;
GetCommandLine:
	mov	di,1		; App parameters Dialog
dodlg:	call	MyMakeProc
	push	dx				; push args for...
	push	ax				; ..freeproc instance
	regptr	pProc,dx,ax
	regptr	pTemp,cx,di
	cCall	DialogBox,<ds,pTemp,hTTYwnd,pProc>
	mov	di,ax					; save return value
	cCall	FreeProcInstance ; ,<pProc>		; already pushed
	ret

;**
;
; MyMakeProc - Set up a WINOLDAP dialog box
;
; ENTRY:
;	DS = Data of app responsible for dialog
;	DI is WINOLDAP.RC doalog ID
; EXIT:
;	DialogID = Input DI
;	DX:AX is makeprocinstanced address of DialogProc
;	CX = 0
; USES:
;	ALL but DS,ES,SI,DI,SS,BP (C standard)
;
MyMakeProc:
	mov	[DialogID],di
	mov	bx,codeOffset DialogProc
	regptr	csbx,cs,bx
	cCall	MakeProcInstance,<csbx,ds>
	xor	cx,cx
	ret

;**
;
; DialogProc - Windows proc for all of the WINOLDAP dialogs
;
; ENTRY:
;	DS = Data of app Dialoging
; EXIT:
;	Standard Windows dialog proc return
;	Dialog values set depending on the particular dialog
; USES:
;	C standard
;
cProc	DialogProc,<PUBLIC,FAR>,<si,di>
	ParmW	dlgHwindow
	ParmW	dlgMsg
	ParmW	dlgWPARAM
	ParmD	dlgLPARAM
cBegin
	mov	si,dlghWindow
	mov	ax,dlgMsg
	cmp	ax,WM_INITDIALOG
	jnz	dp1
    ;
    ; WM_INITDIALOG
    ;
	mov	cx,dataOffset pifTitle
	cmp	byte ptr [DialogID],1	; Parameter dialog?
	jnz	dpW			; No
      ;
      ; Parameter dialog
      ;
	regptr	pText,ds,cx
	cCall	SetWindowText,<si,pText> ; Title is app name
	jmp	short dp2

dpW:	cmp	byte ptr [DialogID],2
	jnz	dpH
      ;
      ; No PIF file dialog
      ;
	mov	ax,10
	cCall	SetDlgItemText,<si,ax,pText>
	cCall	SetSysModalWindow,<si>
	jmp	short dp2

      ;
      ; Bad memory modifier dialog
      ;
dpH:	mov	di,dataOffset WinRect
	regptr	dsdi,ds,di
	cCall	GetWindowRect,<si,dsdi>
	cCall	ClipCursor,<dsdi>
	jmp	short dp2

dp1:	cmp	ax,WM_COMMAND
	mov	bx,dlgWPARAM
    ;
    ; Messages other than WM_INITDIALOG and WM_COMMAND are not handled
    ;
	mov	ax,FALSE
	jnz	dp3
    ;
    ; WM_COMMAND
    ;
	or	bh,bh
	jnz	dp3			; Commands > 255 not handled
	cmp	bl,IDOK
	jz	dp1a
	cmp	bl,IDCANCEL
	jz	dp1a
	cmp	bl,7
	jnz	dp3			; Commands other than OK CANCEL and 7
					;    are not handled
dp1a:	mov	ax,bx			; set return code
	cmp	byte ptr [DialogID],1	; GetCommandLine?
	jnz	dp1b			; no, just return button code
    ;
    ; Suck out parameters from parameters dialog
    ;
	mov	ax,10			 ; ID for dialog item
	mov	bx,dataOffset InBuf+2
	mov	byte ptr [bx-1],' '
	mov	cx,126
	regptr	pBuf,ds,bx
	cCall	GetDlgItemText,<si,ax,pBuf,cx>
	call	SetCount
	mov	ax,1			 ; return ok
dp1b:	cCall	EndDialog,<si,ax>
dp2:	mov	al,TRUE
dp3:	cwd
cEnd

;**
;
; SetInactiveText - Prepend Inactive to Window caption and sets
;		    the Parentheses mode.
;
; ENTRY:
;	DS = Data of app to set select mode of
; EXIT:
;	Select prepended to caption name if not already done
; USES:
;	ALL but DS,SS,BP
;
SetInactiveText:
    ;
    ; Add () to the window title
    ;
	mov	di, dataOFFSET OutBuf
	push	ds
	pop	es
	mov	al, '('
	stosb
	call	GetWinTitle		; append the current title
	add	di, ax
	mov	al, ')'
	stosb
	xor	al, al
	stosb
	mov	di, dataOFFSET OutBuf
	call	SetWinTitle
	mov	bParen, 1
    ;
    ; Now, add Inactive
    ;
	lea	si, szInactive
	mov	cl, InactiveLen
	jmp	short PrependWinText

;**
;
; SetSelectText - Prepend Select to Window caption and set select mode
;
; ENTRY:
;	DS = Data of app to set select mode of
; EXIT:
;	Select prepended to caption name if not already done
; USES:
;	ALL but DS,SS,BP
;
SetSelectText:
	test	[fSelect], FS_ACTIVE	; Are we in Select mode?
	jnz	sst5			;  Yes, don't change title again

	lea	si, szMark		; Prepend select to caption title
	mov	cl, MarkLen
	jmp	short PrependWinText

;**
;
; SetScrollText - Prepend Scroll to Window caption and set scroll mode
;
; ENTRY:
;	DS = Data of app to set scroll mode of
; EXIT:
;	fScroll = 1
;	Scroll Prepended to caption name if not already done
; USES:
;	ALL but DS,SS,BP
;
SetScrollText:
	mov	al,1			; Set fScroll
	xchg	[fScroll],al
	or	al,al			; Already set?
	jnz	sst5			; Yes, don't change caption again

	lea	si, szScroll		; Set scroll
	mov	cl, ScrollLen
	errn$	PrependWinText		; Fall into PrependWinText

;**
;
; PrependWinText - Prepends the passed string to the window title
;
;	Takes in account for the parenthesis
;
; ENTRY
;	si - Pointer the passed string
;	cl - string length
;
; EXIT
;
; USES
;	C convention + OutBuf
;
;
PrependWinText:
	cld
	push	ds
	pop	es
	mov	di,dataOffset OutBuf	; Build new title here

	cmp	bParen, 0		; Check for ()
	je	pwtNoParen1
	mov	al, '('                 ;  Found: Add (
	stosb
pwtNoParen1:

	xor	ch, ch			; Copy over the passed string
	rep	movsb

	mov	al, ' '                 ; Add a space
	stosb

	cmp	bParen, 0		; Check for ()
	je	pwtNoParen2
	dec	di			;  Found: remove space
pwtNoParen2:

	call	GetWinTitle		; Tack on the old title

	cmp	bParen, 0		; Check for ()
	je	pwtNoParen3
	mov	byte ptr [di], ' '      ;  Found: space over (
pwtNoParen3:

	add	di, ax			; AX is set by GetWinTitle
	xor	al, al			; Add null
	stosb
	mov	di,dataOffset OutBuf	; Build new title here
	call	SetWinTitle
sst5:	ret

;**
;
; ResetWinText - Remove prependage to Window caption name
;
; ENTRY:
;	DS = Data of app to reset caption of
; EXIT:
;	prependage of caption removed
; USES:
;	ALL but DS,SS,BP
;
ResetWinText:
	cld
	mov	di,dataOffset OutBuf	; Build new title here
	call	GetWinTitle		; Get current title
    ;
    ; Look for ' ' char which seperates prependage to remove
    ;
rwtLoop:
	inc	di
	cmp	byte ptr [di-1],' '
	jnz	rwtLoop
    ;
    ; Are there any parentheses
	cmp	bParen, 0
	je	rwtFall
	dec	di
	mov	byte ptr [di], '('
    ;
    ; Set caption title to prev title without prependage
    ;
rwtFall:
	errn$	SetWinTitle

;**
;
; SetWinTitle - Alter the Window title to the passed string
;
;	Works for bad (ie non-windowed) applications as well
;	as good apps.
;
; ENTRY:
;	DI = passed string
;	DS = Data of app to change caption of
; EXIT:
;	Indicated string prepended to caption title
; USES:
;	C Convention
;
SetWinTitle:
	push	di
	push	si
	mov	si, di
    ;
    ; Good or Bad?
    ;
	test	pifBehavior, BADBITS
	jnz	swtBad
	regptr	dssi, ds, si
	mov	ax, hTTYWnd
	save	<es>
	cCall	SetWindowText,<ax,dssi>
	jmp	swtExit
    ;
    ; Bad: copy over by hand
    ;
swtBad:
	cld
	push	ds			; Copy over the title
	pop	es
	mov	di, pWindowTitle
swtLoop:
	cld
	lodsb
	stosb
	or	al, al
	jnz	swtLoop
	dec	di
	sub	di, pWindowTitle
	mov	ax, di
	mov	TitleLen, al
swtExit:
	pop	si
	pop	di
	ret

;**
;
; GetWindowTitle - Get the current caption title
;
;	Works with good and bad applications
;
; ENTRY:
;	DS = Data of app to get caption title of
;	DS:DI -> Buffer to place caption in
; EXIT:
;	AX = length of the Window caption
;	Caption Title copied into buffer
; USES:
;	C Convention less ES
;
GetWinTitle:
    ;
    ; Good or Bad?
    ;
	push	di
	push	si
	test	pifBehavior, BADBITS
	jnz	gwtBad
    ;
    ; Good: Window title is stored in the window data structure
    ;
	mov	bx, maxCol		; BX is max size can handle
	regptr	dsdi, ds, di
	mov	ax,[hTTYwnd]
	save	<es>
	cCall	GetWindowText,<ax,dsdi,bx>
	jmp	gwtExit
    ;
    ; Bad: Window title is stored in WindowTiltleList
    ;
gwtBad:
	cld
	push	ds			; Copy over the title
	pop	es
	mov	si, pWindowTitle
	mov	cl, TitleLen
	xor	ch, ch
	rep	movsw
	xor	al, al
	stosb
	mov	al, TitleLen
	xor	ah, ah
gwtExit:
	pop	si
	pop	di
	ret

;**
;
; SetPos - Figure new VisiXXX value
;
;    What this routine does is maintain a ratio.
;
;	  Old VisiXXX		  New VisiXXX
;	  --------------------- = ---------------------
;	  Old pix not on screen   New pix not on screen
;
;    This keeps the user looking at about the same piece of his screen
;    when he re-sizes the Window. The inputs to this routine is all of
;    the values except New VisiXXX, so what this routine does is solve the
;    above equation for New VisiXXX.
;
; ENTRY:
;	DI is previous (to new size) value of VisiXXX
;	BX is number of pix (width or height) that would not fit on PREV screen
;	   == 0 if screen was large enough for WHOLE logical screen
;	AX is number of pix (width or height) that will not fit on NEW screen
;	   == 0 if screen is large enough for WHOLE logical screen
;	CX = CharWidth
; EXIT:
;	CX is new value for VisiXXX
; USES:
;	CX,DX,FLAGS
;
SetPos: or	bx,bx		; Was prev display full screen?
	jz	sp1		; Yes, put him at 0 (top or full left) since
				;     we can't divide by zero.
	cmp	ax,bx		; any change?
	jz	sp2		; no, return no change
	push	ax
	mul	di		; Mul Old VisiXXX by New Not on screen and...
	div	bx		; ... divide by Old not on screen, then...
	xor	dx,dx
	div	cx		; ...round down to even mulitple of chars
	mul	cx
	mov	cx,ax
	pop	ax
	ret

sp1:	xor	cx,cx		; Display org is 0
	ret

sp2:	mov	cx,di		; assume no change
	ret


;**
;
; TTYSize - Handler for WM_SIZE message, re-size and figure out scroll bars
;
; ENTRY:
;	SS = DS = Data of app size is for
;	CX = wParam from Windows Size message (type of resizing)
;	ES:DI = lParam from Windows Size message (DI width, ES height of new
;				client area)
; EXIT:
;	DX = AX = 0
; USES:
;	ALL but DS,SS,BP
;
TTYSize:
	mov	ax,SIZEICONIC
	cmp	cx,SIZEZOOMHIDE 	; Somebody covering us?
	jnz	ttysb			; No
    ;
    ; If somebody just zoomed, remember our current state,
    ; then pretend we're iconic
    ;
	xchg	ax,[fIcon]
	mov	[prevFicon],ax
ttysa:	mov	ax,[VidTail]		; clean out any pending updates
	mov	[VidHead],ax
OkayV:	jmp	Okay			; All done

ttysb:	cmp	cx,SIZEZOOMSHOW 	; Somebody un-covered us?
	jnz	ttysc			; No
    ;
    ; If somebody just UnZoomed, remember our previous state
    ;		(iconic, not iconic)
    ;
	mov	ax,[prevFicon]
	mov	[fIcon],ax
	jmp	ttysa			; All done

ttysc:	mov	[fIcon],cx	; remember our new state
	cmp	cx,ax		; is it iconic?
	jz	ttysa		; yes, clear pending events and done

	test	[pifBehavior],BadBits	; Bad App?
	jnz	OkayV			; Yes, ignore size stuff, no Window
    ;
    ; Either SIZENORMAL or SIZEFULLSCREEN to an app in a Window
    ;
	mov	si,es			; si = height, di = width in pix
  ;
  ; The following code figures out about the scroll bars.
  ;   it computes the number of pix that WON'T fit on the screen,
  ;   and then sets up a scroll bar with a range equal to that number.
  ;   If range is 0, scroll bar is removed.
  ;
  ; NOTE: Since this code is called due to WM_SIZE messages we must
  ;	watch out for re-entrancy. We deal with the
  ;	HORIZ scroll bar followed by the VERT scroll bar. If our
  ;	call to SetRange for the HORIZ scroll bar is changing the
  ;	presence of the scroll bar, it is possible we will get
  ;	a nested WM_SIZE message and re-enter this code. The fVert
  ;	variable is used to detect this.
  ;
    ;
    ; First figure number of pix for MaxCol char line
    ;
	mov	ax,MaxCol
	mov	cx,[CharWidth]
	mul	cx
	mov	[ScrWidth],ax
    ;
    ; Now figure number of pix that won't fit on screen
    ;
	sub	ax,di		; This number MAY be negative (all chars fit)
	add	ax,cx		; Round up to next even char boundary
	dec	ax
	cwd
	idiv	cx		; ax = number of chars across that don't fit
	imul	cx		; turn it back into pix
	or	ax,ax
	jge	ttys0
	xor	ax,ax		; Screen wide enough for all of line

ttys0:	sub	[ScrWidth],ax	; Set true screen width
	mov	bx,[WinWidth]	; Get prev scroll range
	mov	[WinWidth],ax	; Set new scroll range
	mov	di,[VisiCol]	; Get prev column
	call	SetPos		; Compute new column of display org
	mov	[VisiCol],cx	; Set it

	or	ax,ax		; AX = 0 means NO SCROLL BAR. We need to call
	jz	ttys0a		;   SetRange to make sure bar is deleted.
	cmp	ax,bx		; Change in scroll range?
	jz	ttys1		; No, skip expensive Windows calls
ttys0a:
	mov	[fVert],0	; no recursive call to vertical portion
	mov	bx,SB_HORZ
	call	SetRange
	cmp	[fVert],0
	jnz	Okay		; already processed vertical stuff

ttys1:	inc	[fVert]
    ;
    ; Compute hight in pix of MaxLine lines
    ;
	mov	ax,MaxLine
	mov	cx,[CharHeight]
	mul	cx
	mov	[ScrHeight],ax
    ;
    ; Now figure number of pix that won't fit on screen
    ;
	sub	ax,si		; This number MAY be negative (all lines fit)
	add	ax,cx		; Round up to next even line boundary
	dec	ax
	cwd
	idiv	cx		; ax = number of lines that don't fit
	imul	cx		; turn it back into pix
	or	ax,ax
	jge	ttys2
	xor	ax,ax		; Screen high enough for all lines

ttys2:	sub	[ScrHeight],ax	; Set true screen hight
	mov	bx,[WinHeight]	; Get prev scroll range
	mov	[WinHeight],ax	; Set new scroll range
	mov	di,[VisiRow]	; Get prev line
	call	SetPos		; Compute new origin
	mov	[VisiRow],cx	; Set it
	push	ax
	push	bx
	push	cx
	call	SetLastRow	; Set cursor posit and LastRow variable
	pop	cx
	pop	bx
	pop	ax
	or	ax,ax		; AX = 0 means NO SCROLL BAR. We need to call
	jz	ttys0b		;   SetRange to make sure bar is deleted.
	cmp	ax,bx		; Change in scroll range?
	jz	Okay		; No, skip expensive Windows calls

ttys0b:
	mov	bx,SB_VERT
	call	SetRange
	jmp	short Okay

;**
;
; SetLastRow - Set the LastRow variable to the bottom line displayed
;
; ENTRY:
;	DS = Data of app to set
; EXIT:
;	Zero set
;	DX = AX = 0
;	LastRow set
;	Cursor set to current location
; USES:
;	ALL but DS,SS,BP
;
SetLastRow:
	mov	ax,[WinHeight]		; Height of display in pix
	sub	ax,[VisiRow]		; - pix not displayed on top
	cwd
	div	[CharHeight]		; In lines
	mov	dx,MaxLine
	sub	dx,ax			; subtracted from Max height
	mov	[LastRow],dx		; is the last row
	call	MoveCursor
	errn$	Okay			; Fall into Okay
;**
;
; Okay - Return DWORD 0
;
; ENTRY:
;	None
; EXIT:
;	Zero set
;	DX = AX = 0
; USES:
;	AX,DX,FLAGS
;
Okay:	xor	ax,ax
	cwd
	ret

;-----------------------------------------------------
;
;  Scroll table
;
;	This defines a mapping of virtual keys to scroll commands
;
;	ENTRIES:
;	    WORD key identifier == 0 indicates end of table
;	    BYTE scroll command mapped to this key
;	    BYTE == 0 if Vertical scroll
;		 == 1 if Horizontal scroll
;
ScrollTable:
	dw	VK_UP
	db	SB_LINEUP,0
	dw	VK_DOWN
	db	SB_LINEDOWN,0
	dw	VK_LEFT
	db	SB_LINEUP,1
	dw	VK_RIGHT
	db	SB_LINEDOWN,1
	dw	VK_NEXT
	db	SB_PAGEDOWN,0
	dw	VK_PRIOR
	db	SB_PAGEUP,0
	dw	VK_END
	db	SB_PAGEDOWN,1
	dw	VK_HOME
	db	SB_PAGEUP,1
	dw	0

;**
;
; DoScrollCommand - Perform a KEY originated scroll command (if key indicates)
;
; ENTRY:
;	DS = Data of app being scrolled
;	CX is virtual key
; EXIT:
;	If key is a scroll key, scroll action is processed
;	else routine is a no-op
; USES:
;	All but DS,SS,BP
;
DoScrollCommand:
	push	cs
	pop	es
	mov	si,codeOffset ScrollTable-2

dsc1:	lodsw				; Skip past last opcode (add two to SI)
	lods	word ptr es:[si]	; Get table key
	cmp	ax,cx			; Found it?
	jz	dsc2			; Yes
	or	ax,ax			; End of table?
	jnz	dsc1			; no
	ret				; no key, so do nothing

    ;
    ; Key is a scroll key
    ;
dsc2:	lods	word ptr es:[si]	; Get command and modifier
	mov	cx,ax			; CX is command
	or	ah,ah			; Horiz or vert?
	jnz	TTYhScroll
	errn$	TTYVScroll		; Fall into TTYVScroll
;**
;
; TTYVScroll -	Handle Vertical scrolls
;
; ENTRY:
;	DS = Data of app being scrolled
;	CL is scroll command, one of
;		SB_LINEUP
;		SB_LINEDOWN
;		SB_PAGEUP
;		SB_PAGEDOWN
; EXIT:
;	Display scrolled, new VisiRow LastRow set
; USES:
;	All but DS,SS,BP
;
TTYVScroll:
	mov	bx,[VisiRow]
	mov	si,[CharHeight]
	mov	dx,[WinHeight]
	mov	ax,[ScrHeight]
	call	ScrollShare
	jz	Okay			; No change
	call	FlushComBufferNoYield	; preverves ax and di
	mov	[VisiRow],ax
	mov	bx,SB_VERT
	xor	dx,dx
	xchg	di,dx			; For VScroll, Xamount is 0, Yamount set
	jmp	short hscr1

;**
;
; TTYHScroll - Handle Horizontal scrolls
;
; ENTRY:
;	DS = Data of app being scrolled
;	CL is scroll command, one of
;		SB_LINEUP
;		SB_LINEDOWN
;		SB_PAGEUP
;		SB_PAGEDOWN
; EXIT:
;	Display scrolled, new VisiCol set
; USES:
;	All but DS,SS,BP
;
TTYHScroll:
	mov	bx,[VisiCol]
	mov	si,[CharWidth]
	mov	dx,[WinWidth]
	mov	ax,[ScrWidth]
	call	ScrollShare
	jz	Okay				; No change
	call	FlushComBufferNoYield		; preverves ax and di
	mov	[VisiCol],ax
	mov	bx,SB_HORZ
	xor	dx,dx				; Yamount is 0, Xamount set
hscr1:	mov	cx,TRUE
	mov	si,[hTTYWnd]
	push	dx
	cCall	SetScrollPos,<si,bx,ax,cx>
	pop	dx
	xor	cx,cx
	cCall	ScrollWindow,<si,di,dx,cx,cx,cx,cx>
	cCall	UpdateWindow,<si>
	jmp	SetLastRow

;**
;
; ScrollShare - Code shared by both horv and vert scroll
;		Compute new VisiXXX value after scroll and scroll amount
;
; ENTRY:
;	BX = VisiRow or VisiCol
;	SI = CharHeight or CharWidth
;	DX = WinHeight or WinWidth
;	AX = ScrHeight or ScrWidth
;	CL is scroll command, one of
;		SB_LINEUP
;		SB_LINEDOWN
;		SB_PAGEUP
;		SB_PAGEDOWN
;		SB_THUMBPOSITION
; EXIT:
;	Zero set
;	    No change to VisiXXX value
;	Zero re-set
;	    DI is change to VisiXXX value (positive or negative)
;	    AX is new VisiXXX value
; USES:
;	AX,BX,DX,DI,ES,FLAGS
;
ScrollShare:
	mov	es,bx
	cmp	cl,SB_LINEUP		; Line up?
	jnz	wsc1			; No
    ;
    ; SB_LINEUP
    ;
	sub	bx,si			; Back one line or char
	jmp	short wscg

wsc1:	cmp	cl,SB_LINEDOWN		; Line down?
	jnz	wsc2			; No
    ;
    ; SB_LINEDOWN
    ;
	add	bx,si			; Forward one line or char
	jmp	wscg

wsc2:	cmp	cl,SB_PAGEUP
	jnz	wsc3
    ;
    ; SB_PAGEUP
    ;
	sub	bx,ax			; Up one screen worth
	jmp	short wscg

wsc3:	cmp	cl,SB_PAGEDOWN
	jnz	wsc4
    ;
    ; SB_PAGEDOWN
    ;
	add	bx,ax			; Down one screen worth
	jmp	short wscg

    ;
    ; SB_THUMBPOSITION
    ;
wsc4:	mov	bx,si
	shr	bx,1
	add	bx,di
	cmp	cl,SB_THUMBPOSITION
	jz	wscg
    ;
    ; Not recognized scroll command
    ;
	xor	ax,ax		; Zero is new value, change is what it was before
	ret

wscg:	mov	di,es		; get previous VisiXXX value
	sub	di,bx		; anything changed?
	jz	wscg4		; no
	or	bx,bx		; Negative?
	jg	wscg1		; No
	xor	ax,ax		; Adjust is 0
	jmp	short wscg3

wscg1:	cmp	bx,dx		; > max of Window?
	jle	wscg2		; No
	mov	bx,dx		; Limit to max
wscg2:	mov	ax,bx		; New value
	cwd
	div	si		; round down to nearest character or line
	mul	si
wscg3:	mov	di,es		; Get prev value
	sub	di,ax		; difference between old and new is change
wscg4:	ret

;**
;
; SetRange - Set scroll range and position.
;
; ENTRY:
;	BX = Scroll bar ID (either SB_VERT or SB_HORZ)
;	AX = Equal to MAX scroll range. A scroll bar
;		of range 0 to AX is created. If AX = 0
;		then the scroll bar is removed.
;	CX = Scroll position. Location in range 0 - AX
;		to place scroll thumb. This value IS NOT
;		checked to make sure it is <= AX.
;	DS = SS = Data of app being scrolled
; EXIT:
;	Indicated scroll bar set up
; USES:
;	ALL but DS,SS,BP,SI,DI (standard C)
;
SetRange:
	mov	dx,[hTTYWnd]
    ;
    ; Set up stack frame for call to SetScrollPos
    ;
	push	dx		; window handle
	push	bx		; sb
	push	cx		; pos
	push	dx		; fRedraw (TRUE, something non-zero)

    ;
    ; Set up stack frame for call to SetScrollRange
    ;
	xor	cx,cx
	push	dx		; window handle
	push	bx		; sb
	push	cx		; posMin (always 0)
	push	ax		; posMax
	or	ax,ax		; Removing scroll bar?
	jnz	sr0		; No, fRedraw = FALSE
	inc	cx		; Yes, fRedraw = TRUE
sr0:	push	cx		; fRedraw

	cCall	SetScrollRange
	cCall	SetScrollPos
	ret

;**
;
; GrabText - Grap the entire screen into the clipboard (select whole screen)
;
;    This is what happens when ALT-PrtSc is hit for an app in a Window
;
; ENTRY:
;	DS = Data of app to grap screen of MUST BE APP RUNNING IN A WINDOW.
; EXIT:
;	Text Data on screen placed in ClipBoard
; USES:
;	ALL but DS,SS,BP
;
GrabText:
	call	FlipScreen		; Invert the display
	mov	ax,(MaxCol+2)*MaxLine+3 ; Need mem for whole screen
	call	MyAlloc
	jz	FlipScreen		; no memory, just re-flip and exit
    ;
    ; Transfer screen data into meory object
    ;
	mov	di,ax
	mov	es,dx			; ES:DI points at memory object
	mov	ds,[VidSegment]
	xor	si,si			; set row and col
	xor	dx,dx
gt1:	mov	ax,0A0Dh		; save a cr,lf
	stosw
gt2:	lodsw
	cmp	al,-1			; FF and 00 map to a SPACE
	jz	gt2a
	or	al,al
	jnz	gt2b
gt2a:	mov	al,' '
gt2b:	stosb				; save the character
	inc	dl
	cmp	dl,MaxCol		; end of line?
	jb	gt2			; No
;    ;
;    ; Remove trailing spaces from lines
;    ;
;gt3:	 cmp	 byte ptr es:[di-1],' '  ; is it just a space
;	 jnz	 gt4			 ; no, so don't delete it
;	 dec	 di			 ; back up one character...
;	 jmp	 gt3			 ; ...and keep scanning backwards

gt4:	inc	dh			; move to next row
	xor	dl,dl			; set cursor to beginning of line
	cmp	dh,MaxLine		; More line?
	jb	gt1			; Tes
    ;
    ; Remove trailing empty lines
    ;
	mov	ax,0A0Dh		; terminate with cr,lf
;gt5:	 sub	 di,2
;	 cmp	 es:[di],ax
;	 jz	 gt5
;	 add	 di,2
	stosw
	xor	ax,ax
	stosb				; zero terminate
;	 mov	 si,di			 ; remember length of object
	push	ss			; restore ds, leave data in
	pop	ds			;  OEM format
	call	MyUnlock		; Unlock the object
    ;
    ; Re-size the object to true size (not including nul lines and trailing
    ;		spaces on lines)
    ;
;	xor	ax,ax
;	regptr	axsi,ax,si
;	cCall	GlobalReAlloc,<di,axsi,ax>	; flags not used if shrinking...
	mov	si,CF_OEMTEXT
	call	DoDataX 		; Put it in clipboard
	call	AddANSIToClipboard
    ;
    ; Flip the display screen
    ;
FlipScreen:
	call	MyGetDC
	mov	bx,dataOffset WinRect
	push	bx
	regptr	dsbx,ds,bx
	cCall	GetClientRect,<si,dsbx>
	pop	bx
	errn$	MyInvert		; Fall into MyInvert
;**
;
; MyInvert - Invert region in Window display
;
; ENTRY:
;	DS = Data of app being inverted
;	DS:BX -> RECT structure in screen coords of region to invert
;	DI = Handle to DC to invert in
;	SI = Window handle of owner of the DC
; EXIT:
;	Region inverted, DC released
; USES:
;	ALL but DS,ES,SS,BP,SI,DI (C standard)
;
MyInvert:
	cCall	InvertRect,<di,dsbx>
	cCall	ReleaseDC,<si,di>
	ret

sEnd	code
	end
