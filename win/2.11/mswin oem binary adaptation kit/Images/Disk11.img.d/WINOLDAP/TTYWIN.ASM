;------------------------------------------------------------------------
;
; MODULE TTYWIN.ASM - Main Windows Code
;		      WinMain and handlers for various messages
;		      Code for managing the Video update queue
;
;			$Author:   rcp  $
;			$Revision:   1.50  $
;			$Date:   11 Nov 1987 15:03:36  $
;
	.xlist
	include cmacros.inc
	NORASTOPS = 1		; dont include raster ops
	NOVK	  = 1		; dont include virtual keys
	NOMB	  = 1		; dont include message box stuff
	NOMST	  = 1		; dont include misc structures
	NOSYSMETRICS = 1	; dont need this
	include windows.inc
	VK_SHIFT = 10H
	VK_MENU  = 12H
	include tty.inc
	include dde.inc
	include menu.inc
	include macro.inc
	include galloc.inc
	.list

;
; You will not find this referenced anywhere BUT IT HAS TO BE HERE!!!!!!
;    it gets the correct runtime loaded
;
externB 	__acrtused

;-----------------------------------------------
;
; External Windows Procedures
;
externFP	<GlobalAlloc,GlobalFree>
externFP	CreateWindow
externFP	DestroyWindow
externFP	<GetClientRect,GetWindowRect>
externFP	DefWindowProc
externFP	InvalidateRect
externFP	<DestroyCaret,HideCaret>
externFP	SetFocus
externFP	GetTextMetrics
externFP	GetStockObject
externFP	<LoadCursor,LoadIcon,LoadString>
externFP	RegisterClass
externFP	<GetWindowText,SetWindowText>
externFP	<GetInstanceData,GetCurrentPDB>
externFP	<UpdateWindow,PostQuitMessage>
externFP	<GetKeyState>
externFP	<AnsiUpper>
externFP	IsClipboardFormatAvailable
externFP	<GlobalLock, GlobalUnlock>
externFP	GetMenuItemID
externFP	ChangeMenu

;-----------------------------------------------
;
; External WINOLDAP Procedures
;
externNP	<TTYOpen,TTYhScroll,TTYvscroll,TTYquery,TTYPaste>
externNP	<GetPifData,ParseComLine,ShowError,InitSwapVars>
externNP	GetCommandLine

externNP	<SetRowCol,SetCursor, MoveCursor>		;TTYFUNC
externNP	<ProcessMessage,MyYield>
externNP	<Func16,AddKey,GoFunc, Beep>

externNP	<SetMenuMode>					;TTYOEM

externNP	<GetPureName>					;TTYEXEC

externNP	<AddANSIToClipboard>				;TTYSEL
externNP	<GetSelection, CancelSelection, ClearSelection>
externNP	<PaintIconInit, TTYPaintIcon>
externNP	<TTYkeyboard, TTYSysKeyboard>
externNP	<TTYSysKeyup, TTYKeyup>
externNP	<TTYAscii, TTYChar, TTYSysChar>
externNP	<TTYMouseUp, TTYMouseDown, TTYMouseMove>
externNP	<SetInactiveText, ResetWinText, TTYsize>
externNP	<MyEnableItem,MyDisableItem>

externNP	<DDEStartUp, DDEExit, DDESwitchIn>		;DDE
externNP	<DDESwitchOut, DDEModuleInit>

externNP	<UpdateMenu, InitMenuMem, LockMenuSeg, FixText> ;MENU
externNP	<UnlockMenuSeg,  GetItem>
externNP	<CanWeDoMenus, PrepMenusForClose>

externNP	<ExecuteMacro>					;EXECMAC

externNP	<MacroInitMem>					;MACRO

externNP	GetAllDeviceCaps				;HP KMF 052287

externNP	<IsVectra, SaveLinkMapping, RestoreLinkMapping> ;TTYSYS

externNP	<TTYPaint, MyCreateWindow, TTYGoodInit >	;TTYPAINT
externNP	< ClipWindow, FlushComBufferNoYield>

externNP	< DisableGrabber>				;MENUDISP

;-----------------------------------------------
;
; Per task data stored in app DS
;
sBegin		data

externW 	RowCol			; Cursor position
externB 	Row			; Row part of RowCol
externB 	fScroll 		; Scrolling flag
externW 	fIcon			; ICONIC flag
externW 	fSelect 		; Marking flag
externW 	WinWidth		; Pix width of display
externW 	WinHeight		; Pix height of display
externB 	InBuf			; Input buffer
externB 	fUserLock		; Disable user actions if set
externW 	DDEMessageHandlers	; Used by TTYDDE
externW 	VidCursLook		; Cursor type
externW 	VidSegment
externW 	hMenuSeg		; Global menu segment
externW 	hMacroSeg		; Global macro segment
externB 	ShowWindowKludge	; TTYTASK.ASM

globalB Signature,'CP'          ; indicates old app DS  ;**  Must be first two
globalW Partition,0		; Partition address	;**  variables in DS

globalW 	OldApState, SF_NOT_STARTED	; {Not Started, Running, Terminated}
globalW 	wWoaExitCode,0			; ExitCode of WinExec
globalW 	wReturnCode,-1			; Return Code of the app
globalB 	fExitMacro,0			; Executing the exit macro
globalB 	fFocus,0			; Flag for whether we have focus
staticB 	fFocus2,0
globalB 	TTYClass,?,%(SIZE WNDCLASS)	; Class structure
;
; Menu variables
;
globalB 	PokeMenu,0,MenuPokeSize 	;menu update structure
externW 	hMenuSeg
externW 	MenuSeg
externW 	hWinMenu			;window menu for good app

;
; DO NOT DISTURB THE ORDER OF THESE TWO VARIABLES!!!!!!!
;
globalW 	ExecParm,2			; Exec parametrs
globalW 	hTTYWnd,0			; Window handle of app

globalW 	hMenu,0 			; Handle to system menu of app
globalW 	MyTopPDB,0			; Top PDB of WINOLDAP
globalW 	ShowParm,0			; Arg to ShowWindow

;
; WARNING....DANGER.....WARNING
;
;    BEFORE CHANGING THE RELATIVE ORDER OF ANY OF THESE PIF
;    VARIABLES, CAREFULLY CHECK THE CODE IN TTYEXEC.ASM
;    AROUND THE gpf8b: LABEL. MANY OF THESE VARIABLES ARE SET
;    OUT OF THE PIF FILE IN HUNKS (many variables set simultaniously
;    using string move instructions).
;
globalW 	pifDrive,0		; Drive of program
globalB 	pifPathname,?,65	; Name of program
	 public pifInts 		; INT range of program
		pifInts label word
globalB 	pifIntLow,?
globalB 	pifIntHigh,?
globalB 	pifTitle,?,31		; Title of program
globalB 	pifInitialDir,?,65	; Initial dir
globalW 	pifMaxMem,?		; Max memory
globalW 	pifMinMem,?		; Min memory
globalW 	pifSysMem,?		; Behavior
labelW		<PUBLIC, pifFB> 	; Behavior
globalB 	pifBehavior,?		; Behavior
globalB 	pifFlags,?		; Behavior
globalB 	pifMsFlags,?		; Behavior

;!! HP RMH 040387
;
; Pointer to the PureName of the App
;
globalW 	szAppName,?

; Instance handle
;
globalW 	hWoaInst,?

; Set When the window is enabled
;
globalB 	fEnabled,1

;
; DOS FCB do-dads for EXEC calls
;
globalB 	FCB1,?,12
globalB 	FCB2,?,12

;
; Char output buffer
;
globalB 	OutBuf,?,MaxCol

;
; EXEC parameter block for EXEC function call 0
;
		public	MyParmBlock
MyParmBlock	label word
globalW 	EnviroSeg,0
		dd	InBuf
		dd	FCB1
		dd	FCB2

;---------------------------------------
;
; SwapBlock - Define areas of memory to be maintained on a per task basis
;
;    This is a table, and what it does is define the areas of memory maintained
;	by the SwapVectors, SaveVectors, and RestoreVectors calls in TTYSWAP.ASM
;
;    Each entry in the table is of this form:
;
;		WORD  size in words of the area
;		WORD  offset in low memory of first byte of area.
;			0:This Word is the address of this area
;		DWORD address in WINOLDAP DS of save area for this region
;			This area must of course be AT LEAST as big as the
;			first word indicates.
;
;    The table is terminated by an entry with a size of 0
;
		public	SwapBlock
SwapBlock	label	word
		dw	(SIZE ComArea)/2	; IBM IPC Area
		dw	4F0H			; At 0:4F0
		dd	ComArea

cntSwap 	dw	0			; Int vectors from PIF file
srcSwap 	dw	0
dstSwap 	dd	0

		dw	(8*4)/2 		; INT Vectors 2-9
		dw	2*4
		dd	Int2$9

		dw	(SIZE RS232area)/2	; RS-232 PORT addrs
		dw	RS232_base
		dd	RS232area

		dw	0		; zero means end of table

		public	RS232area,int$8
RS232area	dw	2 dup (0)	; save area 0:400H -> 0:403H
ComArea 	db	50 dup (?)	; save area 0:4F0H -> 0:521H
;
; WARNING!!!!!! DO NOT DISTURB THE FOLLOWING AREA!!!!!!
;
int2$9		dd	6 dup (?)	; save for INT 2 - INT 7
int$8		dd	?		; Save for INT 8 vector (timer)
		dd	?		; Save for INT 9 vector

globalD 	pFileSpec,?		; Pointer to program name



globalW 	__psp,0 	    ; psp:0 (paragraph #)

;
; Global instance data imported from any previous WINOLDAP instance
;
	public	__cpackitin,__csetup
__cpackitin	label	word
__csetup	label	word

GlobalInstanceData	label	byte
globalW 	hbrWhite,?		; Brushes
globalW 	hbrBlack,?
globalW 	hFont,? 		; Font handle
globalW 	wBinaryFormat,? 	; "Binary" clipboard format
globalD 	prevParms,?		; Parms
globalB 	NoMemMsg,?,msgMax	; Pre-load no memory message
globalB 	MarkLen,?
globalB 	szMark,?,maxTS		; Pre-load "Mark"
globalB 	ScrollLen,?
globalB 	szScroll,?,maxTS	; Pre-load "Scroll"
globalB 	InactiveLen,?
globalB 	szInactive,?,maxTS	; Pre-load "Inactive" for bad apps
GlobalInstanceSize	equ	$-GlobalInstanceData

sEnd		data

sBegin	code
	assumes cs,code
	assumes ds,data
;
; New SLIBW.LIB 051187 requires these 2 C variables to be declared
;
	public	__setargv, __setenvp,_exit,__cinit

	public	DestroyExit, TTYClose
	public	ShowTerminate, FinishExitMacro
	public	TTYdefault, Okay
	public	TTYInitMenu
	public	MyChangeMenu, TTYCommand
	public	TTYSetFocus, TTYKillFocus
	public	TTYDDE, TTYDestroy, MemoryError

;
; String for class registration
;
	globalB TTYName, <'tty',0>

;**
;
; WinMain - Windows start up proceedure
;
; ENTRY:
;	DS = Data for new instance of WINOLDAP
; EXIT:
;	Indicated old app started, run, and exited
; USES:
;	ALL but DS,ES,SS,SI,DI,BP (C standard)
;
cProc	WinMain,<NEAR,PUBLIC>,<si,di>
	ParmW	hInstance
	ParmW	hPrev
	ParmD	lpParms
	ParmW	cmdShow
cBegin
    ;
    ; Save the instance handle
    ;
	mov	ax, hInstance
	mov	hWoaInst, ax
;
;~~tqn 092187
;
; Mark our instance local heap as static so it will not be able to grow
; when the local heap default size (set in Winoldap.def) run out of space.
; This is a must because in extended mode (i.e. Windows swapped), the
; instance data segment has to be relocated to low memory and the old app
; partition is alocated after it. The undocumented GAH_DON'T_GROW flag
; is only supported by Kernels dated 09/18/87 or later.
;
	mov	AX, DS
	dec	AX
	mov	ES, AX			; ES: Instance arena header
	or	ES:[ga_client0], GAH_DONT_GROW	; Set the flag
;~~
	cld
	mov	si,hPrev
	or	si,si				; very first time?
	jz	ttyld0				; yes, skip this stuff
    ;
    ; Import the global data from prev instance
    ;
	mov	cx,GlobalInstanceSize
	mov	di,dataOffset GlobalInstanceData
	cCall	GetInstanceData,<si,di,cx>
	mov	es,SEG_lpParms
	cmp	byte ptr es:[80h],0	; do we have a command line?
	jnz	ttyldx			; yes..continue
    ;
    ; No command line, import it from prev instance
    ;
	les	bx,[prevParms]
	mov	OFF_lpParms,bx
	mov	SEG_lpParms,es
ttyldx: jmp	ttyld1

    ;
    ; This is the very first instance, we must register and set everything
    ;
ttyld0:
    ;
    ; Init the swap drive stuff in the code segment. We only need to do this
    ;	ONCE because all other instances share the same code.
    ;
	call	InitSwapVars			; Init the swap drive stuff
    ;
    ; Init the DDE stuff
    ;
	call	DDEModuleInit
    ;
    ; Get the white brush and set our class background to it
    ;
	mov	ax,WHITE_BRUSH
	cCall	GetStockObject,<ax>
	mov	[hbrWhite],ax			; Set white brush handle
	mov	si,dataOffset TTYCLASS
	mov	[si+clsHbrBackground],ax	; White background
    ;
    ; Get the black brush
    ;
	mov	ax,BLACK_BRUSH
	cCall	GetStockObject,<ax>
	mov	[hBrBlack],ax
    ;
    ; Get the system font
    ;
	mov	ax,OEM_FIXED_FONT
	cCall	GetStockObject,<ax>
	mov	[hFont],ax
    ;
    ; Set my instance handle in the class structure
    ;
	mov	[si+clsHInstance],ds
    ;
    ;  Set the class cursor to the arrow
    ;
	xor	di,di
	mov	ax,IDC_ARROW
	regptr	pCurs,di,ax
	cCall	LoadCursor,<di,pCurs>
	mov	[si+clsHCursor],ax
    ;
    ; Set class icon to get WM_PAINTICON messages
    ;
	mov	ax, IDI_APPLICATION		; Rectangle
	sub	bx, bx
	regptr	lpIconName, bx, ax
	cCall	LoadIcon, <bx, lpIconName>
	mov	[si].clsHIcon, ax
    ;
    ; Preload "Not enough Memory" error message
    ;
	regptr	pMsg,ds,di
	mov	di, dataOffset NoMemMsg
	mov	bx, msgNoMem
	mov	cx, msgMax
	cCall	LoadString,<ds,bx,pMsg,cx>
    ;
    ; Preload title strings
    ;
	regptr	pMsg, ds,di
	mov	di, dataOffset szMark
	mov	bx, tsMark
	mov	cx, maxTS
wmPreloadLoop:
	save	<bx,cx>
	cCall	LoadString, <ds,bx,pMsg,cx>
	mov	[di-1], al
	cmp	bx, tsInactive
	je	wmSetClassName
	add	di, cx
	inc	di
	inc	bx
	jmp	wmPreloadLoop
    ;
    ; Set class name
    ;
wmSetClassName:
	mov	ax,codeOffset TTYName
	mov	word ptr [si+clsLpSzClassName],ax
	mov	word ptr [si+2+clsLpSzClassName],cs
    ;
    ; Set class WndProc
    ;
	mov	word ptr [si+clsLpFnWndProc],codeOffset TTYWndProc
	mov	word ptr [si+2+clsLpFnWndProc],cs
    ;
    ; Register the class with Windows
    ;
	mov	[si+clsStyle],CS_VREDRAW+CS_HREDRAW+CS_BYTEALIGNCLIENT
	regptr	pTTYClass,ds,si
	cCall	RegisterClass,<pTTYClass>
    ;
    ; Process the command line
    ;
ttyld1: les	si,lpParms
	mov	word ptr [prevParms],si 	; Save our command line here
	mov	word ptr [prevParms+2],es
    ;
    ; Parse the command line, and find the PIF info for the app given
    ;
	call	ParseComLine			; Sets pFileSpec
	cCall	GetPIFData,<pFileSpec>		; Get PIF info
	or	ax,ax				; did we succeed?
	jnz	cn2				; Yes
	mov	ax, -1				; No, nothing more to do
	jmp	DestroyExit			;  AX=-1 to be consistent with WinExec

    ;
    ; Init the ICON painting for this app and set the top PDB
    ;	 for this old app "group"
    ;
cn2:	call	PaintIconInit
	call	FixWinText			; Fix win strings if need be
	cCall	GetCurrentPDB
	mov	[MyTopPDB],ax			; remember where our pdb is...
    ;
    ; Create the Window for the app.
    ;	NOTE: We ALWAYS create a Window, even for apps that don't run in a
    ;	      window. The reason is that when an old app that doesn't run
    ;	      in a Window is ICONIC, it DOES have a Window (the ICON).
    ;	      You might think that we do not need a Window for an app that
    ;	      is not switchable, BUT REMEMBER we can start such an app in
    ;	      the ICONIC state!!!!!! Thus we do need a Window even for these
    ;	      guys.
    ;
	cCall	MyCreateWindow

	call	GetAllDeviceCaps
    ;
    ; Allocate and fill with defaults the macro buffer. This macro buffer
    ; may still be around when we exit. In which case, DestroyExit will
    ; release it
    ;
	call	MacroInitMem		;initialize macro memory
    ;
    ; Set up the menus for good and bad apps
    ;
	call	InitMenuMem


tgi10:
	lea	DI,PokeMenu
	mov	[DI].wMenuID,SystemMenuID	;get system menu ID


	push	DI
	add	DI,wItem			;need string space
	call	RemoveAccelerators
	pop	DI

	test	[pifbehavior],BADBITS		;bad app
	jz	tgi15

	mov	BX,1030 		;Dummy Restore
	mov	AX,cmbRestore
	mov	[DI].wChange,MF_GRAYED
	call	MyChangeMenu


	mov	BX,1030 		;Dummy Move
	mov	AX,cmbMove
	mov	[DI].wChange,MF_GRAYED
	call	MyChangeMenu


	mov	BX,1030 		;Dummy Size
	mov	AX,cmbSize
	mov	[DI].wChange,MF_GRAYED
	call	MyChangeMenu


	mov	BX,1028 		;Minimize (Switch)
	mov	AX,cmbMinimize
	mov	[DI].wChange,MF_ENABLED
	call	MyChangeMenu

	mov	BX,1030
	mov	AX,cmbMaximize
	mov	[DI].wChange,MF_GRAYED
	call	MyChangeMenu



	mov	[DI].wIDNewItem,2059		;bogus ID
	mov	[DI].wChange,MF_APPEND+MF_SEPARATOR
	regptr	dsdi,ds,di
	cCall	UpdateMenu, dsdi


	mov	BX,1029 		;Close
	mov	AX,cmbClose
	mov	[DI].wChange,MF_GRAYED
	call	MyChangeMenu

tgi15:

;	put in separator

	mov	[DI].wIDNewItem,2049		;bogus ID
	mov	[DI].wChange,MF_APPEND+MF_SEPARATOR
	regptr	dsdi,ds,di
	cCall	UpdateMenu, dsdi

tgi20:
;	Make all of these disabled, they will be changed when the menu
;	is popped up.

;	put in mark

	mov	BX,1024 		;mark macro
	mov	AX,cmSelect
	mov	[DI].wChange,MF_ENABLED
	call	MyChangeMenu

;	put in copy

	mov	BX,1025
	mov	AX,cmCopy
	mov	[DI].wChange,MF_GRAYED
	call	MyChangeMenu

;	put in paste

	mov	BX,1026
	mov	AX,cmPaste
	mov	[DI].wChange,MF_GRAYED
	call	MyChangeMenu



;	put in scroll

	mov	BX,1027 		;scroll macro
	mov	AX,cmScroll
	call	MyChangeMenu
	mov	[DI].wChange,MF_GRAYED
tgi30:

;***


    ;
    ; Set the show parameter to show the window
    ;
	mov	ax,cmdShow
	mov	[ShowParm],ax
    ;
    ; Set the PIF interrupt area in the SwapBlock table (except for swap location)
    ;
    ; We set low bound to 16
    ;
;
;~~tqn 072987
;
; We have to save at least all the DOS vectors (int 20H - int 29H) since they
; will be restored back by "RestoreVectors" upon exit of an extended mode
; BAD oldapp. Here we fo a clean save of vectors from int16 to int 7FH even
; if the app PIF said that it doesn't use all of those vectors.
;
;~~
	mov	AX, [PifInts]		; See what Pif settings has for us
	mov	al,16			; start at int 16
	cmp	ah,77H			; max >= last hardware vector on AT?
	jae	cn4			; yes, ok
	mov	ah,77H			; always swap at least INT 16 - INT 77H
cn4:	xor	cx,cx
	sub	ah,al			; AH = number to swap starting at AL
	mov	cl,ah
	inc	cx
	shl	cx,1			; cx has word count for that many
	mov	[cntSwap],cx		; Count here
	mov	di,cx
	shl	di,1			; di has byte count
	xor	ah,ah
	shl	ax,1
	shl	ax,1			; ax has loaction on low mem (0:AX)
	mov	[srcSwap],ax		; Addr in low mem here
	push	di			; Save size in bytes of int save area
    ;
    ; Allocate the logical screen (if good app) and the area for PIF
    ;	int vector saves
    ;
	xor	bx,bx			; Assume no screen
	test	[pifBehavior],BADBITS	; Bad app?
	jnz	cn5			; Yes, no logical screen
	call	TTYGoodInit		; if good guy initialize screen
	mov	bx,MaxCol*MaxLine*2	; if good guy, allocate this much for screen
cn5:	pop	di
	xor	ax,ax
	mov	word ptr [dstSwap],bx	; Pointer to vector save right after screen
	add	di,bx			; Add on size of vector area
	jz	cn6			; No memory needed
    ;
    ; Allocate the area for the logical screen and vector save
    ;
	errnz	<GMEM_FIXED>
	regptr	dsize,ax,di
	cCall	GlobalAlloc,<ax,dsize>
	call	MemoryError		; Check for barf, no return if error
    ;
    ; Initialize the video buffer to all spaces
    ;	NOTE: What we actually do is init the video buffer AND the vector
    ;	      save area to spaces. This is OK as it doesn't matter what
    ;	      is in the vector save area right now. In the event there
    ;	      is no screen buffer, this is not needed, but so what if
    ;	      we do it anyway.
    ;
cn6:	mov	[VidSegment],ax 	; Save video seg
	mov	word ptr [dstSwap+2],ax ; And vector save seg
	mov	es,ax
	mov	cx,di			; Size of block
	shr	cx,1			; In words
	mov	ax,00000111b*256+' '    ; Set to char/attrib for space
	xor	di,di			; Start at 0 in block
	rep	stosw			; Init it
    ;
    ; Fill up Inbuf with parameters if approp
    ;
	cmp	byte ptr [InBuf],-1	; Stuff already there?
	jnz	cn7			; Yes, don't disturb it
	call	GetCommandLine		; Set command line buffer up


    ;
    ; Get the AppName
    ;
cn7:
	mov	di,dataOffset pifPathname
	regptr	ssdi,ss,di
	cCall	AnsiUpper,<ssdi>		; Upper case the name
	push	ss
	pop	es
	call	GetPureName			; Point to the name part
	mov	[szAppName],di
    ;
    ; Enter the startup state where we try to identify identify a
    ; a DDE shell out there
    ;
	cCall	DDEStartUp
	cmp	OldApState, SF_CLOSED		; Were we closed?
	mov	ax, msgNoExec			;
	je	DestroyExit			;  Yes, terminate without EXEC
    ;
    ; if Windowed app size the window
    ;
	test	[pifBehavior], BADBITS
	jnz	wmNotGood
	call	ClipWindow			; Should be called after DDEStartUp
wmNotGood:
    ;
    ; Parse the FCBs for EXEC from the command line
    ;
	mov	si,dataOffset Inbuf+1
	mov	di,dataOffset FCB1
	push	ds
	pop	es
	mov	ax,2901h
	int	21h
	call	FindNextArg			;get the next command line arg
	jnz	wmExec				;Don't fill FCB if no second
						;arg
	mov	di,dataOffset FCB2
	mov	ax,2901h
	int	21h
    ;
    ; EXEC the old app
    ;
wmExec:
	mov	cx,ds
	les	dx,[pFileSpec]
	mov	bx,dataOffset MyParmBlock
	mov	al,0080h	; hi bit in AL means I'm not a windows app,
				;  I'm the top level guy, with work done.
	call	Func16		; start up the guy
	errn$	DestroyExit	; Fall into Destroyexit
;**
;
; DestroyExit - Clean up and Exit
;
;	All paths lead here. This has the responsibility of making
;	sure all global resources which were allocated in WinMain
;	are freed.
;
; ENTRY:
;	AL = Exit code
;	DS = Data of app ending (hTTYwnd MUST BE VALID!!!!!!)
; EXIT:
;	App Window destroyed
;	DOES NOT RETURN, issues DOS EXIT system call with AL error code.
; USES:
;	ALL but DS,ES,SI,DI,SS,BP
;
DestroyExit:
   ;
   ; Save the exit code for latter
   ;
	mov	byte ptr wWoaExitCode, al; Set the exit code
	mov	OldApState, SF_CLOSED	;
   ;
   ; Let the module clean up it resources and terminate all conversations
   ;
	call	DDEExit
   ;
   ; Free any Macro and Menu segments which were not allready cleaned up
   ;
	mov	ax, hMenuSeg		; hMacroSeg is set to 0 when
	or	ax, ax
	jz	deNoMenu		; freed
	cCall	GlobalFree, ax
	mov	hMenuSeg, 0
deNoMenu:
	mov	ax, hMacroSeg		; hMenuSet is set to 0 when
	or	ax, ax
	jz	deNoMacro		; freed
	cCall	GlobalFree, ax
	mov	hMacroSeg, 0
deNoMacro:
   ;
   ; Destroy hTTYWnd including the video buffer associated with it
   ;
	mov	ax, VidSegment		; Was there a buffer?
	or	ax, ax
	jz	deNoVideo		;  No: continue
	cCall	GlobalFree, ax		;  Yes: Free its video buffer
	mov	VidSegment, 0
deNoVideo:
	mov	ax, hTTYWnd
	or	ax, ax
	jz	deNoWnd
	cCall	DestroyWindow,<ax>	; Destroy the window (Only one)
	mov	hTTYWnd, 0
   ;
   ; Wait for quit here and process messages
   ;
   ; The termination path gets a little windy. The above DestroyWindow
   ; call will send a message for TTYDestroy. TTYDestroy will post a
   ; WM_QUIT message which will cause ProcessMessage to jump about
   ; and come back here at _exit:
   ;
deSpin:
	call	ProcessMessage
	jmp	deSpin
_exit:					; Needs to be defined for Runtime
deNoWnd:
	mov	al, byte ptr wWoaExitCode;
	mov	ah,4CH
	int	21h
cEnd	noGen

;**
;
; MemoryError - Check for and handle memory error
;
; ENTRY:
;	Entry of DestroyExit
;	AX is return from a memory call, AX == 0 if no memory error
; EXIT:
;	If it returns, no error
;	Else it DOES NOT return, but jumps to DestroyWindow to end the app
; USES:
;	FLAGS
;
MemoryError:
	or	ax,ax		; Ok?
	jnz	me1		; Yes
	mov	ax,msgNoMem	; ID for no memory error
	call	ShowError	; Show it AX is preserved
	jmp	DestroyExit	; and bomb out

me1:	ret


__CINIT:				; Stubbed out for runtime
__setargv:				;~~tqn 052687
__setenvp:				;~~tqn 052687
	ret


;----------------------------------------
;
; Windows message handler table
;
;	These two tables define which routine handles
;	which Windows message.
;
WndFunc dw	WM_KEYDOWN
	dw	WM_KEYUP
	dw	WM_CHAR
	dw	WM_KEYFIRST-1
	dw	WM_MOUSEMOVE
	dw	WM_LBUTTONDOWN
	dw	WM_LBUTTONUP
	dw	WM_PAINT
	dw	WM_PAINTICON
	dw	WM_SYSCHAR
	dw	WM_SYSKEYDOWN
	dw	WM_SYSKEYUP
	dw	WM_SETFOCUS
	dw	WM_KILLFOCUS
	dw	WM_VSCROLL
	dw	WM_HSCROLL
	dw	WM_SYSCOMMAND
	dw	WM_INITMENU
	dw	WM_SIZE
	dw	WM_USER+1
	dw	WM_CLOSE
	dw	WM_QUERYOPEN
	dw	WM_QUERYQUIT
	dw	WM_ENABLE
	dw	WM_DESTROY
	dw	WM_COMMAND
	dw	WM_DDE_INITIATE
	dw	WM_DDE_TERMINATE
	dw	WM_DDE_ADVISE
	dw	WM_DDE_UNADVISE
	dw	WM_DDE_ACK
	dw	WM_DDE_DATA
	dw	WM_DDE_REQUEST
	dw	WM_DDE_POKE
	dw	WM_DDE_EXECUTE
DefMsg	dw	?
FuncLen equ    $-WndFunc

WndProc dw	TTYKeyboard		; WM_KEYDOWN
	dw	TTYKeyUp		; WM_KEYUP
	dw	TTYChar 		; WM_CHAR
	dw	TTYAscii		; WM_KEYFIRST-1
	dw	TTYMouseMove		; WM_MOUSEMOVE
	dw	TTYMouseDown		; WM_LBUTTONDOWN
	dw	TTYMouseUp		; WM_LBUTTONUP
	dw	TTYpaint		; WM_PAINT
	dw	TTYPaintIcon		; WM_PAINTICON
	dw	TTYSysChar		; WM_SYSCHAR
	dw	TTYSysKeyboard		; WM_SYSKEYDOWN
	dw	TTYSysKeyUp		; WM_SYSKEYUP
	dw	TTYSetFocus		; WM_SETFOCUS
	dw	TTYKillFocus		; WM_KILLFOCUS
	dw	TTYvscroll		; WM_VSCROLL
	dw	TTYhscroll		; WM_HSCROLL
	dw	TTYcommand		; WM_SYSCOMMAND
	dw	TTYinitMenu		; WM_INITMENU
	dw	TTYsize 		; WM_SIZE
	dw	TTYsize 		; WM_USER+1
	dw	TTYClose		; WM_CLOSE
	dw	TTYopen 		; WM_QUERYOPEN
	dw	TTYQuery		; WM_QUERYQUIT
	dw	TTYEnable		; WM_ENABLE
	dw	TTYDestroy		; WM_DESTROY
	dw	TTYCOMMAND		; WM_COMMAND
DDEBeg:
	dw	TTYDDE			; WM_DDE_INITIATE
	dw	TTYDDE			; WM_DDE_TERMINATE
	dw	TTYDDE			; WM_DDE_ADVISE
	dw	TTYDDE			; WM_DDE_UNADVISE
	dw	TTYDDE			; WM_DDE_ACK
	dw	TTYDDE			; WM_DDE_DATA
	dw	TTYDDE			; WM_DDE_REQUEST
	dw	TTYDDE			; WM_DDE_POKE
	dw	TTYDDE			; WM_DDE_EXECUTE
	dw	TTYdefault		; Any other

ProcLen equ    $-WndProc

errnz	<ProcLen-FuncLen>	; these had better be the same!



;**
;
; TTYWndProc - Windows proceedure for old apps
;
;   All this routine does is dispatch to the appropriate message handler
;	as defined by the above tables
;
; ENTRY:
;	DS = Data of app getting message
; EXIT:
;	From approp handler
; USES:
;	C standard
;
cProc	TTYWndProc,<FAR,PUBLIC>,<si,di>
	ParmW	hWindow
	ParmW	Message
	ParmW	wParam
	ParmD	lParam
cBegin
	cld
	mov	ax,cs
	mov	es,ax
	mov	ax,Message		; AX = Message number for all handlers
	mov	cs:[DefMsg],ax
	mov	di,codeOffset WndFunc
	mov	cx,FuncLen/2
	repnz	scasw
	lea	bx,[di+FuncLen-2]
	mov	cx,wParam		; CX = wParam for all handlers
	les	di,lParam		; ES:DI = lParam for all handlers
	mov	si,hWindow		; SI = Window handle for all handlers
	call	cs:[bx]
cEnd

;**
;
; TTYEnable - Handler for enable message
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	AL = TRUE
; USES:
;	AL
;
TTYEnable:
	mov	[fEnabled],cl
	mov	al,TRUE 	; just return TRUE
	ret

;**
;
; TTYCommand - Handle command messages
;
;    All this routine does is dispatch to the approp handler for the
;	system menu commands we added.
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	DX = AX = 0
; USES:
;	C standard
;
TTYCommand:
   ;
   ; We have a menu command
   ; If the UserLock is not set, execute the associated macro.
   ;
	test	fUserLock, UL_ON
	jz	tc3
tc2a:
	jmp	TTYdefault
tc3:
	test	pifbehavior,BADBITS
	jnz	tc2a
	call	LockMenuSeg
	or	DX,DX
	jz	tc2a
	push	ES
	push	AX
	mov	ES,DX
	mov	AX,CX
	push	CX
	call	GetItem
	pop	CX
	jc	tc4
	mov	SI,AX			;get in index reg
	test	ES:[SI].flags,MF_POPUP+MF_DISABLED+MF_GRAYED+MF_SEPARATOR
	jnz	tc4
	cCall	ExecuteMacro,ES:[SI].item	;KMF 041587
	call	UnlockMenuSeg
	pop	AX
	pop	ES
	ret

tc4:
	call	UnlockMenuSeg
	pop	AX
	pop	ES
	jmp	TTYDefault

;**
;
; RemoveAccelerators - Removes the accelerator strings from control menu
;		       items.
;
; ENTRY:
;
; EXIT:
;
; USES:
;	C Convention
   ;
   ; This is the table which defines what strings to load to replace the
   ; current control menu strings.
   ;

RATable label	word
	dw	SC_RESTORE
	dw	SC_MOVE
	dw	SC_SIZE
	dw	SC_MINIMIZE
	dw	SC_MAXIMIZE
	dw	0FFFFH
	dw	SC_CLOSE
	dw	0

RemoveAccelerators:
	push	si
	xor	si,si
	dec	si
RA10:
	inc	si
	shl	si,1
	mov	ax,RATable[si]
	shr	si,1
	cmp	ax,0FFFFH			;ignore
	je	RA20
	or	ax,ax
	jz	RADone
	push	AX				;save the ID
	mov	cx,MaxMenuWidth
	regptr	dsdi,ds,di
	cCall	LoadString,<ds, ax, dsdi, cx>
	pop	AX				;get the ID back
	mov	dx,MF_BYPOSITION+MF_CHANGE
	regptr	dsdi,ds,di
	cCall	ChangeMenu,<hMenu, si, dsdi, ax, dx>
RA20:
	jmp	RA10
RADone:
	pop	si
	ret



;**
;
; TTYinitMenu - Handler for WM_INITMENU
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	System menu dealt with
;	DX = AX = 0
; USES:
;	C standard
;
TTYinitMenu:
	mov	bx,MF_ENABLED		; Disable Quit if App still running
	mov	ax,SC_CLOSE		; Close item in system menu
	cmp	byte ptr [OldApState],SF_TERMINATED; App terminated?
	jne	mit1			; no, CLOSE disabled
	call	MyEnableItem		; Enable close
	jmp	short mit1a

mit1:	call	MyDisableItem
mit1a:	test	[pifBehavior],BadBits	; Bad App?
	jz	mitP			; No, more menu stuff
	jmp	Okay			; All done

    ;
    ; Good app menu management
    ;
mitP:	mov	ax,cmPaste
	cmp	[fIcon],SIZEICONIC	; App ICONIC?
	jnz	mitni			; No
    ;
    ; App is ICONIC, disable Paste, Select, Scroll
    ;
	call	MyDisableItem
	mov	ax,cmSelect
	call	MyDisableItem
	mov	ax,cmScroll
	jmp	short mit3

    ;
    ; App is not ICONIC
    ; Disable Paste if app not running, else enabled
    ;
mitni:	cmp	byte ptr [OldApState],SF_RUNNING; App running?
	jne	mit1c			; No, disable paste
	mov	ax, CF_TEXT		;if text enable paste
	cCall	IsClipboardFormatAvailable, <ax>
	or	ax, ax
	jnz	mit1b
	mov	ax, CF_OEMTEXT		;if oem text enable paste
	cCall	IsClipboardFormatAvailable, <ax>
	or	ax, ax
	jz	mit1c			;otherwise disable paste
mit1b:
	mov	bx, MF_ENABLED
	mov	ax, cmPaste
	call	MyEnableItem		; Enable paste
	jmp	short mit2

mit1c:	mov	ax, cmPaste
	call	MyDisableItem

    ;
    ; App is not ICONIC
    ; Disable Scroll if app has no client area or in mark mode, else enabled
    ;
mit2:	mov	bx,MF_ENABLED
	mov	ax,cmScroll
	mov	cx,[WinWidth]		; Width
	or	cx,[WinHeight]		; ored with height zero (no client)?
	jz	mit2b			; Yes disable scroll
	test	[fSelect], FS_ACTIVE
	jnz	mit2b
	call	MyEnableItem
	jmp	short mitS
mit2b:	call	MyDisableItem

    ;
    ; App is not ICONIC
    ; Disable Mark if in scroll mode, else enabled
    ;
mitS:	mov	bx,MF_ENABLED
	mov	ax,cmSelect
	cmp	[fScroll],0		; In scroll mode?
	jnz	mit3			; Yes, diable select (mark)
	call	MyEnableItem
	jmp	Okay			; All done
mit3:	call	MyDisableItem
mit4:	jmp	Okay			; All done
;**
;
; MyChangeMenu - Append an item to the system menu
;
; ENTRY:
;	DS = Data of app to modify
;	AL = WINOLDAP.RC string ID of string to append
;	SI = hMenu
; EXIT:
;	Indicated item appended to system menu (enabled)
; USES:
;	ALL but DS,ES,SI,SS,BP
;
MyChangeMenu:
	or	[DI].wChange,MF_APPEND+MF_STRING
MyChangeMenu1:
	mov	CX,MaxMenuWidth
	push	DI
	add	DI,wItem
	regptr	dsdi,ds,di
	push	AX
	push	BX
	cCall	LoadString,<ds,ax,dsdi,cx>
	pop	BX
	pop	AX
	pop	DI
	mov	[DI].wIDNewItem,AX
	mov	[DI].wMacro,BX
	regptr	dsdi,ds,di
	cCall	UpdateMenu,dsdi
	ret

;***

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


;**
;
; TTYDefault - Handle message via DefWindowProc
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	From DefWindowProc
; USES:
;	C standard
;
TTYdefault:
	regptr	esdi,es,di
	cCall	DefWindowProc,<si,ax,cx,esdi>
	ret

;**
;
; TTYClose - Handle close from SYSTEM menu
;
;    App can be closed if and only if app is terminated
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	App closed if app terminated and no control conversation
;	    DX = AX = 0
;	else error posted via TTYQuery
; USES:
;	C standard
;
TTYClose:
	cmp	byte ptr [OldApState],SF_RUNNING; App running?
	jne	tcl1				; NO, blast window
	jmp	TTYQuery			; YES, prints error for us
tcl1:
	mov	byte ptr [OldApState], SF_CLOSED
tcl2:	jmp	Okay

;**
;
; TTYDestroy - Handle WM_DESTROY
;
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	App closed if app terminated
;	    DX = AX = 0
;
; USES:
;	C standard
;
TTYDestroy:
	xor	ax, ax
	cCall	PostQuitMessage,<ax>	; Send ourselves the quit
	jmp	Okay

;**
;
; TTYSetFocus - Handler for message about getting the focus
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	fFocus set and cursor restored if app is not terminated
;	AX = DX = 0
; USES:
;	C standard
;
TTYSetFocus:
	mov	[fFocus],1		; We have focus
	cmp	byte ptr [OldApState],SF_TERMINATED; App terminated?
	jge	tsf1			;  Yes, nothing to do
	mov	ax,[VidCursLook]
	call	SetCursor		; Turn cursor on
	call	DDESwitchIn		;  No, notify the shell
tsf1:	jmp	Okay

;**
;
; TTYKillFocus - Handler for loosing the focus
;
; ENTRY:
;	DS = Data of app
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;	fFocus = 0
;	Cursor not displayed
;	DX = AX = 0
; USES:
;	C standard
;
TTYKillFocus:
	mov	[fFocus],0		; Don't have focus
	cmp	byte ptr [OldApState],SF_TERMINATED; App terminated?
	jge	tkf1			;  Yes, nothing to do
	cmp	byte ptr ShowWindowKludge,0 ; see BeginExecution in TTYTASK.ASM
					    ; for details on this flag
	jnz	tkf1
	cCall	DestroyCaret		; No cursor displayed
    ;
    ; Execute the switch out macro
    ;
	mov	ax, SwitchOutID
	cCall	ExecuteMacro, <ax>
tkf1:	jmp	Okay

;**
;
; ShowTerminate - If not fDestroy, enter into inactive mode.
;
;    This routine is the one that checks the fDestroy PIF bit to
;	see if app is to just go away or hang around. When in inactive
;	mode, the this routine will spin in a Int16 loop to drive
;	the menu/pasting mechanism. Since the application is really dead,
;	any keystrokes comming through will cause a beep.
;
;    This code works for both bad and good applications.
;
; ENTRY:
;	Must be TopLevel
;	A BufSeg must be valid for marking.
;	DS = Data of app RUNNING IN A WINDOW
; EXIT:
;
; USES:
;	ALL but DS,SS,BP
;
cProc	ShowTerminate, <NEAR>, <es,si,di>
cBegin
    ; Set OldApState
    ;
	mov	[OldApState], SF_TERMINATED
    ;
    ; Execute the Exit macro. The Exit macro may contain
    ; keystrokes, but these will be ignored.
    ;
    ; We must call DisableGrabber for bad apps since ExecuteMacro assumes this
    ;
	test	[pifBehavior],BADBITS
	jz	st10
	call	DisableGrabber
st10:
	mov	fExitMacro, 1		; Cleared by FinishExitMacro
	mov	ax, ExitID
	cCall	ExecuteMacro, <ax>
	or	AX,AX			; was there a macro to execute
	jz	stExitSpin
	mov	AX,ExitID
	call	FinishExitMacro 	; call Finish macro routine since
					;ExecuteMacro did not.
    ;
    ; Enter a loop which does INT 16s to drive the macro engine.
    ; Exit the loop when the macro finishes.
    ;
stExitSpin:
	test	fExitMacro, 1
	jz	swTestDestroy
	mov	ah, 11			; Doing int16 powers menu mode.
	call	GoFunc
	or	al, al
	jz	stExitSpin
	mov	ah, 6
	call	GoFunc
	jmp	stExitSpin
    ;
    ; Test whether we should enter menu mode
    ;
swTestDestroy:
	test	[pifMsFlags],fDestroy	; Yes, can we destroy window?
	jnz	stExit
    ;
    ; We should try to enter inactive mode
    ;
	call	PrepMenusForClose	; Inactivate the menus
	call	SetInactiveText 	; Put (Inactive) in the title
    ;
    ; Do different things for bad and good
    ;
	test	[pifBehavior], BADBITS	; Are we bad?
	jnz	stBad			;
    ;
    ; GoodAp:
    ;
	call	FlushComBufferNoYield	; Update the screen
	call	DestroyCaret		; Remove caret
	jmp	stMenuSpin
    ;
    ; BadAp:
    ;
stBad:
	call	CanWeDoMenus		; Can we enter menu mode?
	jc	stExit			;   Couldn't, just exit.
	call	SetMenuMode		; Enter menu mode
	jc	stExit			;
	errn$	stMenuSpin
    ;
    ; Drive WinOldAp by doing int16s
    ; If a character is pressed beep
    ;
stMenuSpin:
	cmp	[OldApState], SF_CLOSED ; Loop until closed
	je	stExit
	mov	ah, 11			; Doing int16 powers menu mode.
	call	GoFunc
	or	al, al
	jz	stMenuSpin
	call	Beep			; Tell the user "We are dead"
stNoBeep:
	mov	ah, 6
	call	GoFunc
	jmp	stMenuSpin
stExit:
cEnd

;**
;
; FinishExitMacro - Called when a macro is finished.
;
; Entry
;	AX - MacroID
;	DS - Instance Data segment
;
; Exit
;
; Uses
;	None
;
FinishExitMacro:
	cmp	ax, ExitID
	jne	feExit
	mov	fExitMacro, 0
feExit:
	ret

;****
;
; TTYDDE
;
;	Calls the appropriate DDE routine using the DDEMessageHanlders
;	structure. hTTYWnd is used for the DDE messages to save the
;	space needed for the seperate DDE window.
;
; ENTRY:
;	DS = Data of app
;	BX = offset in WndProc jump table
;	CX = wParam from message
;	ES:DI = lParam from message
;	SI = Window Handle
;	AX = Message number
; EXIT:
;
; USES:
;	C standard
;
TTYDDE:
	push	cx
	push	es
	push	di
	sub	bx, codeOffset DDEBeg
	call	DDEMessageHandlers[bx]
	jmp	Okay


;****
;
; FindNextArg
;
;ENTRY:
;	DS:SI => string
;
;EXIT:
;	ZF = 0 if no next arg
;
;	ZF = 1 and DS:SI points to next arg.
;
;MODIFIES:
;	AX,SI
FindNextArg:
	lodsb
	cmp	AL,0DH		;end of command line?
	je	NoArg
	call	Delim		; is this a DOS delimiter?
	jnz	FindNextArg
  ; found it
	dec	si		;point to this char
	xor	AL,AL		; set zero flag
	jmp	FNARet
NoArg:
	dec	si
	or	AL,AL		; If end then AL is non zero
FNARet:
	ret

;*******
;
;DeLim - Check for DOS (command.com) delimiters
;
;ENTRY:
;	AL = char to check
;
;EXIT:
;	ZF = 1 if char is a delim
;	ZF = 0 if char is not a delim
;
;MODIFIES:
;	none
Delim:
	cmp	AL,' '
	jz	DeLimRet
	cmp	AL,'='
	jz	DeLimRet
	cmp	AL,','
	jz	DeLimRet
	cmp	AL,';'
	jz	DeLimRet
	cmp	AL,09H			;tab
	jz	DeLimRet
	cmp	AL,0AH			; Line Feed
DeLimRet:
	ret



;*******
;
;FixWinText - changes the prepend strings for the title bar if good apps
;
;ENTRY:
;
;EXIT:
;
;MODIFIES:
;	none
FixWinText:
	push	si
	push	es
	push	ax
	mov	ax,ds
	mov	es,ax
	mov	si,dataOffset szMark
	call	FixText
	mov	si,dataOffset szScroll
	call	FixText
	mov	si,dataOffset szInactive
	call	FixText
	pop	ax
	pop	es
	pop	si
	ret


sEnd	CODE
	end

