;------------------------------------------------------------------------
;
; MODULE TTYPAINT.ASM - Code which paints the screen 
;			Code which initializes hTTYWnd
;
;			$Author:   rmh  $
;			$Revision:   1.0  $
;			$Date:   21 Sep 1987 23:07:28  $
;
	.xlist
	include cmacros.inc
	NOVK	  = 1		; dont include virtual keys
	NOMB	  = 1		; dont include message box stuff
	NOMST	  = 1		; dont include misc structures
	NOSYSMETRICS = 1	; dont need this
	include windows.inc
	VK_SHIFT = 10H
	VK_MENU  = 12H
	include tty.inc
	include menu.inc
	.list
;
; Number of chars of old app name put in old app Icon
;
MaxIconNameLen	equ	3



;-----------------------------------------------
;
; External Windows Procedures
;
externFP	<GlobalAlloc,GlobalFree>
externFP	CreateWindow
externFP	DestroyWindow
externFP	<GetClientRect,GetWindowRect>
externFP	MoveWindow
externFP	DefWindowProc
externFP	ScrollWindow
externFP	InvalidateRect
externFP	<GetDC,ReleaseDC>
externFP	SelectObject
externFP	<TextOut,BitBlt,DrawIcon>
externFP	<FillRect,FrameRect>
externFP	<DestroyCaret,HideCaret>
externFP	GetTextMetrics
externFP	GetStockObject
externFP	SetTextColor
externFP	<SetBkColor,SetBkMode>
externFP	<BeginPaint,EndPaint>
externFP	<UpdateWindow,PostQuitMessage>
externFP	<GlobalLock, GlobalUnlock>
externFP	GetMenuItemID
externFP	GetSystemMenu
externFP	GetCurrentTask
externFP	AnsiUpper
externFP	GetWindowDC

;-----------------------------------------------
;
; External WINOLDAP Procedures
;
externNP	<TTYhScroll,TTYvscroll>				;TTYSEL
externNP	<ClearSelection>

externNP	<SetRowCol,SetCursor, MoveCursor>		;TTYFUNC
externNP	<MyYield>

externNP	<Okay, MyDisableItem, MemoryError>		;TTYWIN
externNP	<GetPureName, TTYDefault>




;-----------------------------------------------
;
; Per task data stored in app DS
;
sBegin		data

externW 	VidSegment		; Seg/handle of logical video buffer
externW 	VidCursLook		; Cursor type
externW 	RowCol			; Cursor position
externB 	VidDirty		; Flag for cursor updating
externB 	Row			; Row part of RowCol
externB 	fScroll 		; Scrolling flag
externW 	fIcon			; ICONIC flag
externW		fSelect			; Marking flag
externW 	WinWidth		; Pix width of display
externW 	WinHeight		; Pix height of display
externB 	InBuf			; Input buffer
externB		OutBuf			; Output buffer
externB 	fUserLock		; Disable user actions if set
externB 	pifTitle		; Title of program
externB		pifPathName
externB		Signature
externW		hIcon			; Downloaded Icon

;
; Scrolling variables
;
ScrollRow	label	word
globalB 	TopScrollRow,-1
globalB 	BottomScrollRow,-1

globalW 	VisiCol,0			; Display X origin in 80X25 screen
globalW 	VisiRow,0			; Display Y origin in 80X25 screen
globalW 	CharHeight,8			; Pix height of lines
globalW 	CharWidth,8			; Pix width of chars
globalW 	hDC,?				; DC handle
globalB 	FlushLock,0			; Re-entrancy flag for flush code

;
; Row Col temp vars for scroll code
;
globalW 	tRow,?
globalW 	tCol,?

;
; DO NOT DISTURB THE ORDER OF THESE TWO VARIABLES!!!!!!!
;
externW 	ExecParm,2			; Exec parametrs
externW 	hTTYWnd,0			; Window handle of app

externW 	hMenu,0 			; Handle to system menu of app
externW 	MyTopPDB,0			; Top PDB of WINOLDAP
externW 	ShowParm,0			; Arg to ShowWindow
;
; Rectangle structure for Window operations
;
Rectangle	label	word
globalW 	left,?
globalW 	top,?
globalW 	right,?
globalW 	bottom,?

globalW 	TextColor,0		; Storage for text color

;
; The following group of variables forms the Video update queue
;   which queues up scroll and text events for the display
;   on apps that run in a Window (video hooks in). What this
;   queue contains is screen events which have ALREADY been performed
;   in the Logical Video Buffer, but have NOT been made on the Window
;   display yet. In other words this queue contains those events
;   which need to be made to the Window Client Area display in order
;   to make the display and the Logical video buffer correspond.
;
;  There are two types of entries in the queue, each is 4 bytes long
;
;      Scroll Commands
;      ---------------
;      First	  - Top Row	    (hi bit set)
;      Second	  - Bottom Row
;      Third	  - Number of lines to scroll
;      Fourth	  - Fill Attribute for blank line
;
;	 There is never more than ONE scroll event in the queue.
;	 If there is a scroll event, it is always the first event in the queue.
;	 The scroll event is ALWAYS a FULL WIDTH scroll UP, Scroll Downs and
;	     not full width scrolls are not queued.
;
;      Text Commands
;      ---------------
;      First	  - Column	    (hi bit clear)
;      Second	  - Row
;      Third	  - Count  (low byte)
;      Fourth	  - Count  (hi	byte)
;
;	 Commands DO NOT span lines
;
VidBuffer	dw	2*5 dup (?)		; Room for 5 events
VidBufferEnd	label	word
globalW 	VidHead,?
globalW 	VidTail,?
VidPrev 	dw	?

globalW 	hDatax,0		; handle to data xchange
globalW 	pDatax,0		; pointer into data xchange

globalB 	lpPS,?,%(SIZE PAINTSTRUCT) ; Paint structure

externW		hBrBlack		; Brushes
externW		hBrWhite		;
externW		hFont			; Fonts

;
; Data which represents an old app ICON
;
globalW 	IconIDLen, 0
IconName	db	(MaxIconNameLen+3) dup (?)
IconID		db	MaxIconNameLen dup (ICONTICK)
IconNameLen	dw	MaxIconNameLen

sEnd		data

sBegin	code
	assumes cs,code
	assumes ds,data

	public	InsChrOutCommand,InsScrollCommand,UpdateDisplay
	public	ScrollUpdate,ScrollDownUpdate,ClearUpdate
	public	FlushComBuffer, FlushComBufferNoYield, TTYPaint, PrintText
	public	PaintIconInit, TTYPaintIcon
	public	MyCreateWindow, TTYGoodInit, ClipWindow
	externB TTYName

;**
;
; MyCreateWindow - Create a window associated with this application
;
; ENTRY:
;	IconIDLen is set
; EXIT:
;	CharWidth and CharHeight is set
;
; USES:
;	C convention
;
TEMPSIZE	equ	40
cProc	MyCreateWindow, <NEAR,PUBLIC>, <SI,DI>
   localV   szTemp, TEMPSIZE		; Tempoary storage for title
   localV   tm, %(SIZE TEXTMETRIC + 8)
cBegin
    ;
    ; Set up the App name with dots
    ;
	mov	ax, ds			; ES=DS=SS
	mov	es, ax
	mov	cx, TEMPSIZE
	lea	di, szTemp
	mov	si, dataOffset pifTitle
	rep	movsb			; Copy the title over to szTemp
	xor	al, al
	lea	di, szTemp
	mov	cx, TEMPSIZE
	repne	scasb			; Find the end
	dec	di			;   DI points to NULL at the end
	jcxz	CWaddNull		;   Truncate if overflow
	mov	cx, IconIDLen		; Add the dots
	jcxz	CWaddNull
CWdotLoop:
	mov	byte ptr [di], ICONTICK
	inc	di
	loop	CWdotLoop
CWaddNull:				; Add the null at the end
	mov	byte ptr [di], 0
    ;
    ; Create the window name
    ;
	lea	di, szTemp		; get pointer to program title
	regptr	pName,ss,di
	mov	si,codeOffset TTYName	; Class name
	regptr	pTTY,cs,si
	xor	ax,ax
	mov	dx,WS_TILEDWINDOW+WS_VSCROLL+WS_HSCROLL
	regptr	Style,dx,ax
	xor	bx,bx
	mov	cx,CW_USEDEFAULT
	cCall	CreateWindow,<pTTY,pName,style,cx,bx,cx,bx,bx,bx,ss,bx,bx>
	mov	[hTTYWnd],ax		; Save handle
	call	MemoryError		; Check for barf, no return if error
	xor	di, di
	mov	si, ax			; Save window handle
    ; 
    ; Get the system menu
    ;
	cCall	GetSystemMenu,<ax,di>	; get handle to system menu
	mov	[hMenu],ax		; Save handle
	call	MemoryError		; Check for barf, no return if error
    ;
    ; Get the font metrics
    ;
	cCall	GetDC,<si>
	mov	di, ax
	cCall	SelectObject,<di,hFont>
	lea	bx, tm
	regptr	pCharSize,ss,bx
	cCall	GetTextMetrics,<di,pCharSize>
	cCall	ReleaseDC,<si,di>
	mov	cx, tm.tmAveCharWidth
	mov	[CharWidth],cx
	mov	bx, tm.tmHeight
	mov	[CharHeight],bx
cEnd	MyCreateWindow

;**
;
; TTYGoodInit - Initialize good app Display stuff
;
; ENTRY:
;	DS = Data of good app to init
;	CharHeight set
; EXIT:
;	Cursor type and position set
;	Video update queue initialized
;	System menu modified
; USES:
;	ALL but DS,SS,BP
;
TTYGoodInit:
    ;
    ; Set the cursor to a one pix high underline at the bottom of the char
    ;
    	mov	ax, [CharHeight]
	dec	al
	mov	ah,al
	dec	ah
	mov	[VidCursLook],ax
	call	SetCursor
    ;
    ; Cursor to upper left corner
    ;
	xor	dx,dx
	call	SetRowCol
    ;
    ; Video update queue empty
    ;
	mov	ax,dataOffset VidBuffer
	mov	[VidHead],ax
	mov	[VidTail],ax
    ;
    ;disable the Copy item in menu
    ;
	mov	ax, cmCopy
	call	MyDisableItem
	ret



;** 
;
; ClipWindow - Make sure the window isn't larger than MaxLines and MaxCols
;
;	This function should be called after menus are downloaded
;	using DDEStartUp.
;
; ENTRY:	
;	DS = 
;	hTTYWnd, CharHeight, CharWidth valid
;
; EXIT:
;	Window is sized
;
cProc	ClipWindow, <NEAR>, <>
   localV   clientRect, 8
   localV   wndRect, 8
cBegin
    ;
    ; Get the client and window rectangle
    ;
    	mov	si, [hTTYWnd]
	lea	bx, clientRect
	regptr	lpRect,ss,bx
	cCall	GetClientRect, <si,lpRect>
	lea	bx, wndRect
	regptr	lpRect,ss,bx
	cCall	GetWindowRect, <si,lpRect>
   ;
   ; Calcutlate the full screen size
   ;
   	mov	cx, [CharWidth]
   	mov	ax, maxCol
   	mul	cx
   	mov	cx, ax
   	mov	bx, [CharHeight]
	mov	ax, maxLine
	mul	bx
	mov	bx, ax
   ;
   ; Add in the boarders to get:
   ;
   ;	AX,DX = Current Window size
   ;	CX,BX = Full screen size
   ;
   	mov	ax, wndRect.rcRight
   	sub	ax, wndRect.rcLeft
   	mov	dx, wndRect.rcBottom
   	sub	dx, wndRect.rcTop
   	add	cx, ax			; Use the difference between
   	add	bx, dx			;  Client and Window size for 
   	sub	cx, clientRect.rcRight	;  boarder size.
   	sub	bx, clientRect.rcBottom
   ;
   ; Make sure that the current window is no larger than the 
   ; full screen window calculated in CX,BX.
   ;
   	cmp	cx, ax
	jb	tcX
	mov	cx, ax
tcX:
   	cmp	bx, dx
	jb	tcY
	mov	bx, dx
tcY:
   ;
   ; Resize the window
   ;
   	xor	ax, ax
   	cCall	MoveWindow, <si, (wndRect.rcLeft), (wndRect.rcTop), cx, bx, ax>
cEnd
	
	
	


;**
;
; TTYPaint - Paint an old app screen
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	Window or ICON painted
;	DX = AX = 0
; USES:
;	C standard
;
TTYPaint:
    ;
    ; If any pending scroll events, paint whole screen
    ;
	mov	di,[VidHead]		; do we have pending update events?
	cmp	di,[VidTail]
	jz	pntB			; no, just continue
	test	byte ptr [di],80H	; scroll event?
	jz	pntB			; no...
	mov	[VidTail],di		; get rid of pending events
	xor	di,di
	cCall	InvalidateRect,<si,di,di,di>	; paint the whole screen
    ;
    ; Get a DC for Painting
    ;
pntB:	mov	di,dataOffset lpPS
	regptr	dsdi,ds,di
	cCall	BeginPaint,<si,dsdi>
	mov	[hDC],ax
	mov	si,ax
    ;
    ; If Bad app open, no paint
    ;
	lea	bx,[di+PSrcPaint]	; Point to paint rect
	mov	ax,[fIcon]
	or	ax,ax			; bogus open for bad apps?
	jns	pnt0			;  No, continue
	jmp	pnt5			;  Yes, just ignore
    ;
    ;  Paint a non-Iconic good app
    ;  First select OEM font
    ;
pnt0:	push	bx
	cCall	SelectObject,<si,hFont> ; select in OEM font
	pop	bx
    ;
    ; Figure Left Col of repaint
    ;
	mov	ax,[bx+rcLeft]
	mov	si,[VisiCol]
	mov	di,[CharWidth]
	add	ax,si
	cwd
	idiv	di
	mov	[tCol],ax		; first column of repaint
	mul	di
	sub	ax,si
	mov	[Left],ax		; bit position on screen
    ;
    ; Figure Top Row of repaint
    ;
	mov	ax,[bx+rcTop]
	mov	si,[VisiRow]
	mov	cx,[CharHeight]
	add	ax,si
	cwd
	idiv	cx
	mov	[tRow],ax		; First line of repaint
	mul	cx
	sub	ax,si
	mov	[Top],ax		; Bit position on screen
    ;
    ; Figure Bottom Row of repaint
    ;
	sub	ax,[bx+rcBottom]
	neg	ax
	add	ax,cx
	dec	ax
	cwd
	idiv	cx
	mov	cx,MaxLine
	sub	cx,[tRow]
	cmp	ax,cx
	jle	pnt1
	mov	ax,cx
pnt1:	mov	[Bottom],ax
	or	ax,ax
	jle	pnt5			; Bottom is <= 0 no-repaint
    ;
    ; Figure Right Col of repaint
    ;
	mov	ax,[bx+rcRight]
	sub	ax,[Left]
	add	ax,di			; add in character width
	dec	ax
	cwd
	idiv	di
	mov	bx,MaxCol
	sub	bx,[tCol]
	cmp	ax,bx
	jle	pnt2
	mov	ax,bx
pnt2:	mov	[Right],ax
    ;
    ; Figure offset in log vid buffer of first char that is re-painted
    ;
	mov	ax,[tRow]		; get first row
	mov	bx,[tCol]		; get first column
	mov	dx,ax
	mov	cl,4
	shl	ax,cl			; multiply by 16
	mov	cx,ax
	shl	ax,1
	shl	ax,1			; multiply by 64
	add	ax,cx			; ax = ax*80
	add	bx,ax
	shl	bx,1
	mov	si,bx			; SI is index into vid buffer for
					;   first re-paint char
    ;
    ; Perform the re-paint
    ;
pnt4:	mov	cx,[Right]
	push	[Left]
	push	si
	call	PrintText		; Print correct part of this line
	pop	si
	pop	[Left]
	add	si,2*MaxCol		; Next line
	mov	ax,[CharHeight]
	add	[Top],ax		; Next line
	dec	[Bottom]		; One line done
	jnz	pnt4			; More lines
pnt5:	mov	di,dataOffset lpPS
	cCall	EndPaint,<hTTYWnd,dsdi>
	jmp	Okay

;**
;
; TTYPaintIcon - Paint an old app Icon
;
;    This routine handles an old app paint when the app is ICONIC
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	App ICON painted
; USES:
;	ALL but DS,ES,SI,SS,BP
;
TTYPaintIcon:
    ;
    ; Get a DC for Painting
    ;
	mov	di,dataOffset lpPS
	regptr	dsdi,ds,di
	cCall	BeginPaint,<si,dsdi>		; Must be called
	cCall	GetWindowDC,<si>		; I want the whole window 
	mov	si, ax	

	cmp	hIcon, 0
	je	piNormal
    ;
    ; Paint a downloaded icon
    ;
    	sub	ax, ax
	cCall	DrawIcon, <si, ax, ax, hIcon>
	jmp	pix
    ;
    ; Paint a normal icon with tick marks
    ;
piNormal:
	lea	di,[di+PSrcPaint]		; Point to paint rect
	regptr	dsdi,ds,di
	cCall	GetClientRect,<hTTYWnd,dsdi>	; get size of ICON window
    ;
    ; Fill rect with white
    ;
    	cCall	FillRect, <si, dsdi, hBrWhite>	
    ;
    ; Put on border
    ;
	call	MyFrameRect			; Put on a border
	inc	[di+rcLeft]
	dec	[di+rcRight]
	call	MyFrameRect			; FAT border
    ;
    ; Set background
    ;
	mov	ax,TRANSPARENT
	cCall	SetBkMode,<si,ax>		; Set transparent background
    ;
    ; Set name of app
    ;
	mov	cx,[IconNameLen]
	mov	ax,[CharWidth]
	shr	ax,1
	mov	bx,[CharHeight]
	shr	bx,1
	shr	bx,1
	inc	bx
	mov	dx,dataOffset IconName
	call	MyTextOut
	mov	cx,[IconIDLen]
	mov	ax,[CharWidth]
	shr	ax,1			; ax contains half a character width
	mov	bx,[di+rcBottom]
	sub	bx,[di+rcTop]		; bx contains height of icon
	sub	bx,[CharHeight]
	sub	bx, 3			; subtract for the boarder
	mov	dx,dataOffset IconId	; Print Tick marks
	call	MyTextOut
    ;
    ; Release DC
    ;
pix:   	
	mov	di, hTTYWnd
	cCall	ReleaseDC, <di, si>		;
	mov	si,dataOffset lpPS
	regptr	dssi, ds, si
	cCall	EndPaint, <di, dssi>
	jmp	Okay
    	

;**
;
; MyFrameRect - Put a frame around a rectangle
;
; ENTRY:
;	DS:DI -> Rect structure
;	SI = handle to DC
; EXIT:
;	Black frame put on rectangle
; USES:
;	ALL but DS,ES,SI,DI,SS,BP (C standard)
;
MyFrameRect:
	regptr	dsdi,ds,di
	cCall	FrameRect,<si,dsdi,hBrBlack>
	ret
;**
;
; MyTextOut - Call textOut
;
; ENTRY:
;	AX = X
;	BX = Y
;	DS:DX -> Text
;	CX = Count
;	SI = handle to DC
; EXIT:
;	Text printed
; USES:
;	ALL but DS,ES,SI,DI,SS,BP (C standard)
;
MyTextOut:
	regptr	dsdx,ds,dx
	cCall	TextOut,<si,ax,bx,dsdx,cx>
	ret

;**
;
; PaintIconInit - Set up the name for an old app Icon
;
; ENTRY:
;	SS = DS = Data of app to build ICON for
;	pifPathname has name of app
; EXIT:
;	IconName IconNameLen IconIDLen set
; USES:
;	ALL but DS,SS,BP
;
PaintIconInit:
	mov	di,dataOffset pifPathname
	regptr	ssdi,ss,di
	cCall	AnsiUpper,<ssdi>		; Upper case the name
	push	ss
	pop	es
	call	GetPureName			; Point to the name part
	mov	ax,[di] 			; Get first two chars of name
	mov	word ptr [IconName],ax
	mov	ax,[di+2]			; Get second two chars
	mov	word ptr [IconName+2],ax
	mov	bx,-1
    ;
    ; Check to see how many of the above 4 chars are valid
    ;
pii1:	inc	bx
	cmp	byte ptr [di+bx],'.'            ; Found extension?
	jnz	pii1				; no, keep looking
	cmp	bx,MaxIconNameLen		; Is name without extension long enough?
	jb	pii2				; no
	mov	bx,MaxIconNameLen		; Name is long enough
pii2:	mov	word ptr [IconName+bx],0
	mov	byte ptr [IconName+2+bx],0
	mov	[IconNameLen],bx
    ;
    ; Set number of tick marks to differenciate apps with same name.
    ;	we do this by scanning the task list for other old apps with
    ;	same IconName and tick count
    ;
pii3:	cCall	GetCurrentTask
pii4:	mov	es,dx				; point at head of task list
	mov	cx,ss
	mov	dx,es:[TDB_TaskSS]		; get the task ds
	cmp	dx,cx				; is it us?
	jz	pii5				; yes, we don't count us

	mov	ds,dx
	cmp	word ptr ds:[Signature],'PC'    ; old app DS?
	jnz	pii5				; no, we don't match
    ;
    ; See if his name is the same as ours
    ;
	mov	ax,word ptr ds:[IconName]
	cmp	ax,word ptr ss:[IconName]
	jnz	pii5				; Different
	mov	ax,word ptr ds:[IconName+2]
	cmp	ax,word ptr ss:[IconName+2]
	jnz	pii5				; Diferent
	mov	ax,ds:[IconIDLen]
	cmp	ax,ss:[IconIDLen]
	jnz	pii5				; Different
    ;
    ;  duplicate name found
    ;
	cmp	ax,MaxIconNameLen	; already maximum length?
	jae	pii6			; Yes, quit
	inc	ss:[IconIDLen]		; add one more tick
	jmp	pii3			; and look for another match

pii5:	mov	dx,es:[TDB_next]
	or	dx,dx			; end of chain?
	jnz	pii4			; No, keep going
pii6:	push	ss
	pop	ds
	ret
;**
;
; PrintText - Print indicated text of indicated line
;
;   This routine is a bit tricky. We wan't to repaint the text
;	on the indicated area of the screen, BUT REMEMBER, this
;	screen may have a selection on it (a region that is inverted).
;	We want to maintain the selection in its inverted state.
;   RECALL: The inverted selection text is indicated by high bit set
;	    in the attributes of the video buffer
;   RECALL: That the video buffer represents white on black while
;	    the display is black on white.
;
; ENTRY:
;	DS = Data of app
;	SI is offset in vid buffer of first char on this line to print
;	CX = Count of chars to print on this line
;	Left, Top are display location of first char on this line
; EXIT:
;	Indicated part of indicated line is re-painted
; USES:
;	Value in Left may be destroyed
;	ALL but DS,SS,BP
;
PrintText:
	mov	es,[VidSegment] 	; ES:SI -> Vid buffer
	mov	dx,es:[si]		; Get first Char/Attrib
    ;
    ; Check if first char is "normal", if it is background and text
    ;	color are set OK, and we don't need to make initial expensive
    ;	Windows calls to set them
    ;
	cmp	dh,00010000b
	jb	pt4		; Some color on Black background (OK)
	cmp	dh,10010000b
	jae	pt4		; Something inverted, but not black background (OK)
    ;
    ; Need to re-set background and Text color according to attrib
    ;
pt3:	push	cx		; Save count
	xor	di,di
	mov	[TextColor],di	; Assume black on white
	mov	es,[VidSegment]
	mov	dx,es:[si]	; Get attrib
	cmp	dh,00010000b
	jb	pt31		; Some color on Black background (Black on White)
	cmp	dh,10010000b
	jae	pt31		; Something inverted, but not black background (Black on White)
	not	di		; Is inverted, do as White on Black
	mov	[TextColor],di
pt31:	cCall	SetTextColor,<hDC,di,di>; Set foreground
	not	di			; Background is inverse of foreground
	cCall	SetBkColor,<hDC,di,di>	; Set background
	mov	es,[VidSegment]
	mov	dx,es:[si]
	pop	cx

pt4:	jcxz	pt7			; No chars, all done
	cld
	mov	di,dataOffset OutBuf	; Build text line at OutBuf
	mov	bx,cx
pt5:	lods	word ptr es:[si]	; Next char/attrib
	mov	[di],al 		; Char to buffer
	inc	di			; Skip over attrib
	cmp	ah,dh			; Same attrib as first char?
	loopz	pt5			; Keep going if yes
	jz	pt6			; Exhausted count
    ;
    ; Attrib is different
    ;  Print line so far then Figure amount of line not done
    ;  and re-do color settings for it
    ;
	mov	di,dataOffset OutBuf
	inc	cx			; Undo last char
	sub	bx,cx			; BX number chars not done
	sub	si,2			; Undo last char
	mov	ax,[CharWidth]
	mul	bx
	add	ax,[Left]		; Next piece starts here
	push	ax
	push	cx
	regptr	pOutBuf,ds,di
	cCall	TextOut,<hDC,Left,Top,pOutBuf,bx> ; Print what so far
	pop	cx			; Count not done
	pop	[Left]			; Position of rest
	jmp	pt3			; Re-do color setting

    ;
    ; Count exhausted, print last part of line, reset to "normal" if needed
    ;
pt6:	sub	di,bx
	cCall	TextOut,<hDC,Left,Top,pOutBuf,bx> ; Print rest of line
	mov	di,[TextColor]
	or	di,di			; Need to reset "normal"?
	jz	pt7			; No, all done
    ;
    ; Reset to Black on White
    ;
	mov	si,[hDC]
	cCall	SetBkColor,<si,di,di>
	not	di
	cCall	SetTextColor,<si,di,di>
pt7:	ret

;------------------------------------------------
;
; The rest of the code in this file is the code
;   for managing the Video update queue for an app running
;   in a Window (video hooks in).
;

;**
;
; InsChrOutCommand - Insert a text event in the Video queue
;
; ENTRY:
;	DS = Data of app
;	DX = Count of chars changed
;	RowCol is location of first char changed in 80X25 screen
; EXIT:
;	Selection MAY BE cleared (as screen has been changed)
;	Event queued (may be fully processed)
;	MAY PERFORM A YIELD
; USES:
;	ALL but DS,SS,BP
;
InsChrOutCommand:
	cmp	dx,1		; One char?
	jbe	InsChrOut	; Yes, no multi line
    ;
    ; Multi char text, might be multiple lines
    ;
	mov	bx,[RowCol]	; Get start
	mov	al,MaxCol
	sub	al,bl		; al = number of chars avail on first line
	cbw
	cmp	dx,ax		; All on same line?
	jbe	InsChrOut	; Yes
    ;
    ; Text spans multiple lines
    ; Insert a queue event for the first line
    ;
	sub	dx,ax		; DX is chars left after first line
	push	dx
	mov	dx,ax		; First lines worth
	call	InsChrOut
	pop	dx
    ;
    ; Insert queue events for the rest of the lines
    ;
	mov	ah,[Row]
	xor	al,al			; increase row by one, set column zero
insc0:	inc	ah			; Next line
	sub	dx,MaxCol		; Last line (partial)?
	jb	insc1			; yes
    ;
    ; Insert text event for full line
    ;
	push	ax
	push	dx
	mov	dx,MaxCol
	call	InsChrOut2
	pop	dx
	pop	ax
	jmp	insc0

    ;
    ; Last line (partial)
    ;
insc1:	add	dx,MaxCol		; Undo above subtract
	jnz	InsChrOut2		; If anything, go insert it
	ret				; Done

;**
;
; InsChrOut InsChrOut2 - Insert Text command in queue
;
;    InsChrOut inserts chars at location RowCol
;    InsChrOut2 inserts chars at location in AX register
;
;    NOTE: It is callers responsibility to insure commands DO NOT
;	   span multiple lines.
;
; ENTRY:
;	DS = Data of app
;	DX is count of chars changed
;	InsChrOut
;	    RowCol is location of event
;	InsChrOut2
;	    AX is Row/Col location of event on 80X25 screen
; EXIT:
;	Event queued (may cause flush)
;	MAY PERFORM A YIELD
; USES:
;	ALL but DS,SS,BP
;
InsChrOut:
	mov	ax,[RowCol]
InsChrOut2:
	mov	si,[VidTail]
	cmp	si,[VidHead]		; Buffer empty?
	jz	InQ			; Yes, stick in new command
	mov	di,[VidPrev]		; point to last command
    ;
    ; If this text command is going in AFTER a scroll OK
    ; If after a text command merge this command with it
    ; if they are on the same line
    ;
	test	byte ptr [di],10000000b ; previous command scroll?
	jnz	InQ			; yes, so buffer this one
    ;
    ; On same row (line) as prev text command?
    ;
	cmp	ah,[di+1]		; same row?
	jnz	InQ			; no, generate separate entry
    ;
    ; Compute end column of prev text command
    ;
	mov	bx,[di]
	xor	bh,bh
	add	bx,[di+2]
	dec	bx			; bx = final column prev command
    ;
    ; Compute end column of this command
    ;
	mov	cx,ax
	xor	ch,ch
	add	cx,dx
	dec	cx			; cx = final column this command
    ;
    ; End column of merged command is max of prev and this
    ;
	cmp	bx,cx			; is prev maximum?
	jae	ico1			; yes
	mov	bx,cx			; this command is maximum
    ;
    ; Start column of merged command is min of prev and this
    ;
ico1:	cmp	al,[di] 		; is new min start column?
	jbe	ico2			; yes
	mov	al,[di] 		; prev = min start column
ico2:	cbw
    ;
    ; Set merged command
    ;
	sub	bx,ax
	inc	bx			; Length is inclusive
	mov	[di],al
	mov	[di+2],bx
	ret

;**
;
; NewQ - Empty queue then insert this command in queue
;
; ENTRY:
;	DS = Data of app
;	AX = First word of queue entry
;	DX = Second word of queue entry
; EXIT:
;	Command inserted
;	VidPrev points to this command just added
;	VidTail points to next queue location
;	MAY PERFORM A YIELD
; USES:
;	ALL but DS,SS,BP
;
NewQ:	call	UpdateDisplay		; Flush queue
	mov	si,[VidTail]
	errn$	InQ			; Fall into InQ
;**
;
; InQ - Insert command in queue
;
; ENTRY:
;	DS = Data of app
;	SI = Pointer of where to insert command (VidTail)
;	AX = First word of queue entry
;	DX = Second word of queue entry
; EXIT:
;	Command inserted
;	VidPrev points to this command just added
;	VidTail points to next queue location
;	MAY PERFORM A YIELD
; USES:
;	ALL but DS,SS,BP
;
InQ:	mov	di,si
	call	NextComLoc		; SI -> next queue entry
	cmp	si,[VidHead]		; Enough Room in queue?
	jnz	InQ7			; yes, insert the command
    ;
    ; Queue is full, flush it
    ;
	call	UpdateDisplay		; otherwise, flush the buffer...
	mov	si,[VidTail]
	jmp	InQ			; ...and try again

InQ7:	mov	[di],ax 		; Insert command
	mov	[di+2],dx
	mov	[VidPrev],di		; New prev
	mov	[VidTail],si		; New tail
	ret

;**
;
; InsScrollCommand - Insert a Scroll (UP) event in the Video queue
;
; ENTRY:
;	DS = Data of app
;	AL = Number of lines to scroll
;	CH = Top row of scroll
;	DH = Bottom row of scroll
;	BH = Fill attribute for blank line
; EXIT:
;	Selection MAY BE cleared (as screen has been changed)
;	Event queued (may be fully processed)
;	MAY PERFORM A YIELD
; USES:
;	SI,DI,FLAGS
;
InsScrollCommand:
	push	ax
	push	bx
	push	cx
	push	dx
    ;
    ; Build AX and DX into first and second word of scroll command
    ;
	mov	dl,al
	mov	al,ch
	mov	ah,dh
	mov	dh,bh
	or	al,80h
    ;
    ; If queue empty, add scroll event
    ;
	mov	si,[VidTail]
	mov	di,[VidHead]
	cmp	si,di			; Buffer empty?
	jz	isc4			; yes, scroll command is head of list
    ;
    ; Either add this scroll command as ONLY scroll event at head of queue, or
    ;	combine this scroll event with the one at the head of the queue
    ;
	cmp	[di],ax 		; Scroll Command at head of list
					;    with same top and bottom?
	jnz	isc3			; no, flush queue, add to head of list
	cmp	[di+3],dh		; same fill attribute?
	jnz	isc3			; no, flush queue, add to head of list
    ;
    ; This scroll event has same top and bottom and fill attribute as
    ;  the scroll event already at the head of the queue. Combine them
    ;  into one bigger scroll.
    ;
	mov	cl,[di+2]		; get old number of lines to scroll
	cmp	cl,MaxBufLines		; have we buffered to limit?
	jae	isc3			; yes, flush queue, add to head of list
	add	cl,dl			; scroll up this many rows
	cmp	cl,MaxLine		; Whole screen?
	jbe	isc2			; No
	mov	cl,MaxLine		; Limit to whole screen
isc2:	mov	[di+2],cl		; New number of lines
	mov	cx,[di] 		; cl=top row, ch = bottom row
	and	cl,01111111b		; Mask off scroll command indicator
	xchg	si,di			; si = head, di = tail
    ;
    ; Check the text commands in the queue to see if they overlap the scroll
    ;	area. Adjust their location by the change in scroll amount if they do.
    ;
isc2a:	call	NextComLoc
	cmp	si,di			; End of queue
	jz	isc5			; Yes, all done
	mov	ax,[si]
	or	al,al		; character command?
	js	isc2a		; no
	cmp	ah,ch
	ja	isc2a		; if row past last scroll row, then OK
	cmp	ah,cl
	jb	isc2a		; if row before first scroll row, then OK
    ;
    ; The Text command overlaps the scroll area, adjust its row by the
    ;	scroll amount
    ;
    ; THERE IS A BUG HERE!!!!!!!!! We should check to see if the NEW
    ;	row of the text command is ABOVE the top row of the scroll
    ;	area. If it is, this text command has been scrolled off the screen
    ;	and it should be removed from the queue
    ;
	sub	byte ptr [si+1],dl ; Adjust row of text command to new location
	jmp	isc2a

isc3:	call	NewQ		; Add this command as the first in a new queue
	jmp	short isc5

isc4:	call	InQ		; Add this command to the queue at SI
isc5:	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret

;**
;
; UpdateDisplay - Flush out the queue to the display and clear any selections
;
; ENTRY:
;	DS = Data of app
; EXIT:
;	Queue flushed, selections cleared
;	MAY PERFORM A YIELD
; USES:
;	SI,DI,FLAGS
;
UpdateDisplay:
	push	ax
	push	bx
	push	cx
	push	dx
	call	FlushComBuffer
	call	ClearSelection
	pop	dx
	pop	cx
	pop	bx
	pop	ax
udis1:	ret

;**
;
; FlushComBufferNoYield - Flush video event queue to display without yeild
;
; ENTRY:
;	DS = Data of app
; EXIT:
;	Video event queue flushed to display
;	Selections NOT cleared
;	If VidDirty indicates, cursor reset
; USES:
;	ALL but AX,DI,DS,SS,BP
;
FlushComBufferNoYield:
	push	ax
	push	di
	call	FlushComBuffer2
	pop	di
	pop	ax
fcbny:	ret

;**
;
; FlushComBuffer - Flush video event queue to display
;
;   NOTE: It is possible to get nested calls to the actual flush code
;	  so we protect it with a semaphore, don't want two threads
;	  simultaniously diddling the queue!!!!
;
; ENTRY:
;	DS = Data of app
; EXIT:
;	Video event queue flushed to display
;	Selections cleared
;	MAY DO A YEILD
;	If VidDirty indicates, cursor reset
; USES:
;	ALL but DS,SS,BP
;
FlushComBuffer:
	mov	ax,[VidHead]
	cmp	ax,[VidTail]		; queue empty?
	jz	fcbx3			; Yes, just check VidDirty

fcbxa:	call	MyYield 		; be a nice guy and clean out
	jnz	fcbxa			; pending events

	call	ClearSelection		; Clear selections (screen changed)
FlushComBuffer2:
	mov	dl,1
	xchg	[FlushLock],dl		; Test set semaphore
	or	dl,dl
	jnz	fcbny			; dont reenter ourselves

	mov	[TopScrollRow],255	; Initial limit is MAX

fcbx0:	mov	si,[VidHead]
	cmp	si,[VidTail]
	jz	fcbx2			; queue empty, clear semaphore and check
					;   VidDirty
    ;
    ; Get event and process
    ;
	mov	dx,[si]
	mov	cx,[si+2]
	call	NextComLoc
	mov	[VidHead],si
	cmp	[fIcon],SIZEICONIC	; iconic?
	jz	fcbx0			; If so, no display, so done
    ;
    ; Check command type
    ;
	test	dl,10000000b	; Scroll?
	jnz	fcbx1		; yes
    ;
    ; Text command
    ;
	cmp	dh,0		; negative row?
	jl	fcbx0		; yes, just ignore
	cmp	dh,[TopScrollRow] ; Inside scroll area?
	jbe	fcbxT		; No
	cmp	dh,[BottomScrollRow] ; Inside scroll area?
	jbe	fcbx0		; Yes, ignore...
fcbxT:	call	TextUpdate
	jmp	fcbx0

    ;
    ; Scroll command
    ;
fcbx1:	mov	ax,dx
	mov	al,dh
	sub	al,cl
	mov	[ScrollRow],ax		; set Top and bottom scroll row
	mov	al,cl
	mov	bh,ch
	xor	cx,cx
	mov	ch,dl
	and	ch,01111111b		; Mask of command ID bit
	mov	dl,Col80
	neg	[FlushLock]
	call	ScrollUpdate
	neg	[FlushLock]
	jmp	fcbx0

fcbx2:	mov	[FlushLock],0
fcbx3:	cmp	[VidDirty],0
	jz	tupx
	jmp	MoveCursor

;**
;
; TextUpdate - Update display from Logical video buffer
;
; ENTRY:
;	DS = Data of app
;	DL = Column
;	DH = Row
;	CX = Count
; EXIT:
;	Indicated region updated from logical video buffer
; USES:
;	ALL but DS,SS,BP
;
TextUpdate:
	push	dx
	push	cx
    ;
    ; Compute index into logical video buffer
    ;
	mov	bx,dx
	mov	al,bh
	cbw
	mov	cl,4
	shl	ax,cl			; multiply by 16
	mov	cx,ax
	shl	ax,1
	shl	ax,1			; multiply by 64
	add	ax,cx			; ax = ax*80
	xor	bh,bh
	add	bx,ax
	shl	bx,1			; X two bytes per char
	push	bx			; BX is index into logical video buffer
    ;
    ; Hide the caret, get a DC, and select the font
    ;
	mov	si,[hTTYWnd]
	cCall	HideCaret,<si>
	cCall	GetDC,<si>
	mov	[hDC],ax
	mov	di,ax
	cCall	SelectObject,<di,hFont>

	pop	si			; si point into video buffer
	pop	cx			; cx is update count
	pop	dx			; dh = row, dl = column
    ;
    ; Compute display limits (set Top and Left)
    ;
	mov	ax,[CharHeight]
	mul	dh
	sub	ax,[VisiRow]
	mov	[Top],ax

	mov	ax,[CharWidth]
	mul	dl
	sub	ax,[VisiCol]
	mov	[Left],ax
    ;
    ; Print the text, release the DC, and move the cursor
    ;
	call	PrintText
	cCall	ReleaseDC,<hTTYWnd,hDC>
	call	MoveCursor
tupx:	ret

;**
;
; NextComLoc - Address next queue element
;
; ENTRY:
;	SI = Current queue index
; EXIT:
;	SI = Index of next queue element
; USES:
;	SI,FLAGS
;
NextComLoc:
	add	si,4				; Next
	cmp	si,dataOffset VidBufferEnd	; Wrap?
	jb	ncl1				; no
	mov	si,dataOffset VidBuffer 	; wrap
ncl1:	ret

;**
;
; ScrollDownUpdate - Scroll Down Window Region
;
; ENTRY:
;	DS = Data of app
;	CH,CL = Row, Column of upper left corner
;	DH,DL = Row, Column of lower right corner
;	BH = Fill attribute
;	AL = Number of lines to scroll
; EXIT:
;	Indicated region of display scrolled down
;	Cursor updated
; USES:
;	SI,DI,ES,FLAGS
;
ScrollDownUpdate:
	push	ax
	push	bx
	mul	byte ptr [CharHeight]
	jmp	short ScrollCore

;**
;
; ScrollUpdate - Scroll Up Window Region
;
; ENTRY:
;	DS = Data of app
;	CH,CL = Row, Column of upper left corner
;	DH,DL = Row, Column of lower right corner
;	BH = Fill attribute
;	AL = Number of lines to scroll
; EXIT:
;	Indicated region of display scrolled Up
;	Cursor updated
; USES:
;	SI,DI,ES,FLAGS
;
ScrollUpdate:
	push	ax
	push	bx
	mul	byte ptr [CharHeight]
	neg	ax
ScrollCore:
	mov	bx,ax
	call	HideCursor
	push	cx
	push	dx
	call	CalcRectangle
	xor	cx,cx
	mov	si,[hTTYWnd]
	mov	di,dataOffset Rectangle
	regptr	pRect,ds,di
	cCall	ScrollWindow,<si,cx,bx,pRect,pRect>
	cCall	UpdateWindow,<si>
	call	MoveCursor
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret

;**
;
; ClearUpdate - Clear Window Region
;
; ENTRY:
;	DS = Data of app
;	CH,CL = Row, Column of upper left corner
;	DH,DL = Row, Column of lower right corner
;	BH = Fill attribute
; EXIT:
;	Video queue flushed
;	Indicated region of display cleared
;	Cursor updated
; USES:
;	ES,FLAGS
;
ClearUpdate:
	push	ax
	push	bx
	push	cx
	push	dx
	push	di
	push	si
	call	UpdateDisplay
	call	HideCursor
	mov	si,[hbrWhite]
	test	bh,01110000b
	jz	clr1
	mov	si,[hbrBlack]
clr1:	call	CalcRectangle
	cCall	GetDC,<hTTYWnd>
	mov	di,ax
	mov	ax,dataOffset Rectangle
	regptr	pRect,ds,ax
	cCall	FillRect,<di,pRect,si>
	cCall	ReleaseDC,<hTTYWnd,di>
	call	MoveCursor
	pop	si
	pop	di
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret

;**
;
; CalcRectangle - Convert an 80X25 rect into a client area (display) rect
;
; ENTRY:
;	DS = Data of app
;	CH,CL = Row, Column of upper left corner (80X25 coord)
;	DH,DL = Row, Column of lower right corner (80X25 coord)
; EXIT:
;	Rectangle contains display rectangle for this area
; USES:
;	AX,DX,FLAGS
;
CalcRectangle:
	mov	ax,[CharHeight]
	mul	ch
	sub	ax,[VisiRow]
	mov	[Top],ax
	mov	ax,[CharHeight]
	inc	dh
	mul	dh
	sub	ax,[VisiRow]
	mov	[Bottom],ax
	mov	ax,[CharWidth]
	mul	cl
	sub	ax,[VisiCol]
	mov	[Left],ax
	mov	ax,[CharWidth]
	inc	dl
	mul	dl
	sub	ax,[VisiCol]
	mov	[Right],ax
	ret

;**
;
; HideCursor - Hide the Window cursor
;
; ENTRY:
;	DS = Data of app to hide cursor of
; EXIT:
;	Cursor hidden
; USES:
;	FLAGS
;
HideCursor:
	push	ax
	push	bx
	push	cx
	push	dx
	cCall	HideCaret,<hTTYWnd>
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret

sEnd	code
	end
