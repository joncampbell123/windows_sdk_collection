page ,132
;-------------------------------------------------------------------------
;
; MODULE TTYFUNC.ASM - TTY functions and the InterChange proc to call them
;
;	$Revision:   1.25  $
;	$Author:   rcp  $
;	$Date:   13 Nov 1987 10:23:56  $
;
;	The TTY functions are handlers for various INTerrupt services and
;	DOS system call services.
;

	.xlist
	include cmacros.inc
	include windows.inc
	include tty.inc
	include mac.inc 		;~~tqn 061187
	.list

;========  HP modification  ==================================================
;
;  The modifications are only related to the support of Mark/Copy/Paste
;  for BAD applications.
;
;  - Check for Menu flag in routines Processmessage, Keystatus, Func6
;
;  History:	08/20/86	Integrate with version 1.03
;		03/05/87	~~TN~~: Integrate with the documented version
;				of Winoldap
;		05/11/87	~~tqn: Change Interchange to set CY error status
;				       for the DOS emulated functions
;
;==============================================================================

page

;-----------------------------------------------
;
; External Windows Procedures
;
externFP	<ShowCaret,CreateCaret,SetCaretPos>
externFP	<ScrollWindow,UpdateWindow,InvalidateRect>
externFP	SetScrollPos
externFP	<PeekMessage,GetMessage,TranslateMessage,DispatchMessage>
externFP	MessageBeep

;-----------------------------------------------
;
; External WINOLDAP Procedures
;
externNP	WinExec
externNP	InsChrOutCommand
externNP	InsScrollCommand
externNP	ScrollUpdate
externNP	ScrollDownUpdate
externNP	ClearUpdate
externNP	<FlushComBuffer,FlushComBufferNoYield>
externNP	GetDatax
externNP	PeekDatax
externNP	WaitIcon
externNP	ClearSelection
externNP	TrackRowCol
externNP	UpdateDisplay
externNP	GetKey
externNP	CheckPartition
externNP	Func27
externNP	_exit

;!! HP	 MCP
externFP	PasteInt16
externNP	DoMenu
externNP	DoBeep
externW 	fMenu
externW 	fMenuActive
externW 	fPasteActive
externB 	fBeep
;!! HP	 MCP END


ExternNP	<Alloc_Partition, Free_Memory, Modify_Partition> ;~~tqn 060187

externNP	ResumeExecMacro 	;KMF 041587
externB 	fEMPaste		;KMF 042287

page
;-----------------------------------------------
;
; Per task data stored in app DS
;
sBegin	data
	public	RowCol,Row

;
; App type indicator
;
externB pifbehavior


;
; Indicates our state (Iconic or not)
;
externW 	fIcon
;
; Font parameters
;
externW 	CharHeight
externW 	CharWidth
;
; Screen vars (VisiRow/VisiCol is location (in pix) on 80X25 screen of
;		display 0,0)
;
externW 	VisiRow
externW 	VisiCol
externW 	LastRow
;
; Window handle of our app window
;
externW 	hTTYWnd
;
; Flag that tells if we have the focus
;
externB 	fFocus
;
; Flag that tells if we are in a partition load
;
externB 	fChecking
;
; Flag that tells if the OEM layer is enabled
;
externB 	fWindowsEnabled
;
; UserLock flags which disable user input
;
externB 	fUserLock

;
; Flag that controls yields on status calls
;
StatusFlag	db	1
;
; State of the state machine for ANSI driver
;
State		dw	codeOffset State1
;
; Row and Col of cursor
;
RowCol		label	word
Col		db	0
Row		db	0
;
; Save cursor location for ANSI restore cursor command
;
SavRowCol	dw	?
;
; Vars for DOS call 10 StartCol is screen pos of buffer start InsMode is
;	state flag
;
StartCol	db	0
InsMode 	db	0
;
; Parm structure for ANSI state machine command parsing
;
pParms		dw	?
Parms		db	4 dup (?)
ParmEnd 	label	byte
;
; char Attribute set by Set Graphics Rendition ANSI command
;
Attr		db	-1			; just use default
;
; Storage for second byte of two byte INT 16H characters
;
AltAH		db	0
;
; Pointer into InBuf for Func5 reads
;
ConPos		dw	0
;
; Flag used to control ^C detection
;
EntryBP 	dw	0
;
; Flag to indicate ^C detected
;
fCtrlC		dw	0
;
; Internal buffer for DOS call 10 and other char buffer usage
;
globalB 	InBuf,?,256
;
; Counter for use of InBuf as DOS call 10 Template
;
LastCount	db	0
;
; Pointer into video buffer of cursor location
;
		Public	VidAddr 	;!! V1.03
VidAddr 	label	dword
globalW 	VidPointer,0		; address of current cursor pos
globalW 	VidSegment,0		; segment of screen buffer
;
; Flag for cursor correctness
;
globalB 	VidDirty,0		; non zero if cursor position wrong
;
; Storage for the cursor "type"
;    VidCursLook stores the Windows normalized version
;    OrgVidCursLook stores the non-normalized INT 10H version
;
globalW 	VidCursLook,0
globalW 	OrgVidCursLook,0B0CH

;
; This is the keyboard key buffer. It is a simple head tail pointer list.
;     Keys go in at KbBufferHead.
;     Keys come out at KbBufferTail.
;     Buffer is empty if KbbufferHead == KbBufferTail
;
globalW 	KbBuffer,?,16
KbBufferEnd	Label	word
globalW 	KbBufferHead,KbBuffer
globalW 	KbBufferTail,KbBuffer
;
; Message structure for calls to Get/PeekMessage etc.
;
globalB 	Msg,?,%(SIZE MSGSTRUCT)


sEnd		data

sBegin	code
	assumes cs,code
	assumes ds,data

;-----------------------------------------------
;
; Global task data stored in WINOLDAP code segment
;
	;
	; Hooks in for this app
	;
	externB fHook
	;
	; Windows INT 3F handler address
	;
	externD Int3FVector
	;
	; Pointer to DOS error mode byte
	;
	externD pErrMode
	;
	; Flag set if AT II compatable bios is present
	;
	externB fATIIBIOS

	public	SetRowCol,MoveCursor,SetCursor,MulCol
	public	NewCur				;!! V1.03
	public	Interchange,ProcessMessage,MyYield
	public	AddKey,Beep,ProcessMessage2
	public	Func16, gotch, outansi, outtab, bufout

	public	CheckExtKey, RemoveKey, PeekKey
	public	StatusCheck, Func6, Func11, Func31, Func32, f26
	public	Func12, Func18, Func19


externD 	CritErrPtr		; used in good apps

GlobalD 	Caller_Stack,0		;!!HP: Caller's SS:SP ~~TN 082086 ~~

;
; NOTE:
;	Functions 31 and 32 were created to handle Int 16H extended
;	keycode calls.	They are the extended keycode equivalents of
;	Functions 6 and 11.  These needed to be created since there is no way
;	of passing parameters to the function through GoFunc.
;
; Dispatch table for the TTY functions
;
FuncJump	dw	Func1
		dw	Func2
		dw	Func3
		dw	Func4
		dw	Func5
		dw	Func6
		dw	Func7
		dw	Func8
		dw	Func9
		dw	Func10
		dw	Func11
		dw	Func12
		dw	Func13
		dw	Func14
		dw	Func15
		dw	Func16
		dw	Func17
		dw	Func18
		dw	Func19
		dw	Func20
		dw	Func21
		dw	Func22
		dw	Func23
		dw	Func24
		dw	Func25
		dw	Func26
		dw	Func27
		dw	Func28		;~~tqn 060187
		dw	Func29		;~~tqn 060187
		dw	Func30		;~~tqn 060187
		dw	Func31		;~~RCP 081187
		dw	Func32		;~~RCP 081187

;**
;
; Interchange - Func dispatcher to the TTY functions
;
;  This is the routine used to invoke the TTY functions contained
;  in this module.
;
;  NOTE: This routine conforms to standard C convention so it preserves
;	DS,SI,DI,BP Thus none of the TTY Funcs can use these registers
;	to RETURN information.
;	The CX register is used to return ^C detection information
;	so no TTY func can use CX to RETURN information.
;	The DS,SI and BP registers are modified during the dispatch
;	so they can not be used to INPUT information.
;
;	VALID INPUT REGISTERS:
;		AX,BX,CX,DX,DI,ES
;	VALID RETURN REGISTERS:
;		AX,BX,DX,ES
;
; ENTRY:
;	AH
;	    Low 7 bits = TTY Function number
;	    High bit set
;		^S,^C checking enabled on this call
;	    High bit re-set
;		^S,^C checking NOT enabled on this call
;	SS -> WINOLDAP application DS/SS
;	OTHERS As for particular TTY func indicated in AH
;	Int3FVector contains address of Windows INT 3FH handler
; EXIT:
;	CX != 0 if ^C detected during function, but not processed
;	SI,DI,DS,BP preserved
;	OTHERS As for particular TTY func
;	Functions 5,9 always returns CY=0	;~~tqn 061187
;	For Functions 28,29,30:
;	 CY=	0:	Success
;		1:	Fail: Error code returned in AX
;
; USES:
;	AX,BX,CX,DX,ES,BP,FLAGS
;
Interchange proc far
    ;
    ; If we are loading a partition, we cannot perform function
    ;
	cmp	ss:[fChecking],0	; loading partition?
	jz	interS			; No, do func
	xor	cx,cx			; Return no ^C
	ret

    ;
    ; Set up standard stack frame
    ;
InterS: inc	bp
	push	bp
	mov	bp,sp
	push	ds
	push	di
	push	si
;!! HP
;
	mov	Word Ptr CS:[Caller_Stack], SI
	mov	Word Ptr CS:[Caller_Stack +2], DI
;*****
    ;
    ; Make sure the correct INT 3F handler is in
    ;
	xor	si,si
	mov	ds,si
	mov	si,word ptr cs:[Int3FVector]
	xchg	word ptr ds:[3FH*4],si		; set/get INT 3FH
	push	si				; save current 3FH offset
	mov	si,word ptr cs:[Int3FVector+2]
	xchg	word ptr ds:[3FH*4+2],si	; set/get INT 3FH
	push	si				; save current 3FH segment
    ;
    ; DS = Data
    ;
	push	ss
	pop	ds
	test	pifbehavior,BADBITS		; bad app?
	jnz	interB				; yes, don't much with INt 24H
	call	CheckCritErr
interB:
    ;
    ; Point at correct function handler
    ;
	push	[EntryBP]	; Save this
	cld			; Direction correct
	push	ax		; Gonna muck
	mov	al,ah		; Function to AL
	shl	ax,1		; strip off ^C, ^S flag in AH and mult func # by 2
	cbw			; Make function index a word
	mov	si,ax		; Point with SI
    ;
    ; Handle ICONIC special cases
    ;
	cmp	[fIcon],SIZEICONIC	; App Iconic?
	jnz	inter0		; no
	cmp	al,23*2 	; the yield function?
	jz	inter2		; don't yield if iconic
	cmp	al,6*2		; don't wait if Func6
	je	inter0
	cmp	al,11*2 	; don't wait fi Func11
	je	inter0
	call	WaitIcon	; Wait until not iconic
inter0: pop	ax		; Recover input
    ;
    ; Set EntryBP to 0 or non-zero to control ^C checking based on AH input
    ;
;
	mov	[EntryBP],0	; assume ^C checking off for this call
	mov	[fCtrlC],0	; no ^C detected
	test	ah,10000000b	; ^C, ^S checking on?
	jz	inter1		; no, assumption correct
	mov	[EntryBP],bp	; turn on ^C checking
	call	StatusCheck	; Check for ^C ^S
	jc	inter2		; Hit ^C, don't do func
    ;
    ; Dispatch
    ;
inter1: call	cs:[si+FuncJump-2]	; Call the func
inter2: mov	cx,[fCtrlC]	; Set CX return
;
;~~tqn 061187
;
;!!BE CAREFUL !! CY returned by functions must be preserved for the caller.
;
	pushf
	pop	SI		; SI= FLAGS
	call	CheckPartition	; make sure we still have partition
;
; Messy code: the reason we cannot orderly POP things from stack is because
; inter2 can be also jumped into from MyAbort. In that case, ther are a whole
; bunch of garbage in the stack so we have to resort to the saved BP to clean
; it up.
;
	sub	bp,6*2		; six things pushed after BP
	mov	sp,bp

	pop	ss:[EntryBP]
	push	SI		; Restore returned flags
	POPFF
;~~
    ;
    ; Recover entry INT 3F handler
    ;
	mov	SI, 0
	mov	ds,si
	pop	word ptr ds:[3FH*4+2]		; restore INT 3FH
	pop	word ptr ds:[3FH*4  ]
	mov	Word Ptr CS:[Caller_Stack +2], 0 ;!! HP
	pop	si
	pop	di
	pop	ds
	pop	bp
	dec	bp		; Dec opcode does not modify CY
	ret
Interchange endp

;**
;
; StatusCheck - ^C, ^S management routine for checking enabled calls
;
;  This routine checks for ^C and ^S. For ^C it just returns an indication
;	that a ^C was typed after eating the ^C and printing ^C CR LF to
;	stdout. For ^S this routine sits in a spin loop waiting for
;	another key to be typed (pause). The ^S and the response key
;	are both eaten.
;
; ENTRY:
;	DS = Data
; EXIT:
;	Carry Clear
;	    ^C not detected
;	Carry Set
;	    ^C detected
; USES:
;	FLAGS
;
; NOTES:
;	KeyStatus Function 11H is used as this will get all keys from PeekKey
;	whether it is a AT I or AT II keyboard.
;
StatusCheck:
	push	ax		; We only muck FLAGS
	push	bx
	push	cx
	push	dx
	push	es
	push	di
	push	si
	mov	ah,11H
	call	KeyStatus	; is there a key in the buffer?
	jz	sc2		; No, ^C not detected
	cmp	dl,'S'-'@'      ; is it ^S?
	jz	sc1		; Yes, go spin
	cmp	dl,'C'-'@'      ; is it ^C?
	jnz	sc2		; no, ^C not detected
    ;
    ; ^C detected
    ;
	call	Func7i		; eat the ^C
	call	GotCtrlC	; print ^C
	stc			; Flag ^C detected
	jmp	short sc3

    ;
    ; ^S detected
    ;
sc1:	call	Func7i		; eat the CTRL-S
	call	Func7i		; wait for a key
sc2:	clc			; ^C not detected
sc3:	pop	si
	pop	di
	pop	es
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret

DupHighByte	Label	Byte
	db	0AH
	db	0DH
	db	2FH
	db	00H		; list terminator
DupHighLen	equ	$-DupHighByte

DupHighReplacement	Label	Word
	dw	0000H
	dw	1C0DH
	dw	352FH

TrashCodes	Label	Byte
	db	001H
	db	00eH
	db	01aH
	db	01bH
	db	01cH
	db	027H
	db	028H
	db	029H
	db	02bH
	db	033H
	db	034H
	db	035H
	db	037H
	db	04aH
	db	04eH
	db	072H
	db	080H
	db	085H
	db	086H
	db	087H
	db	088H
	db	089H
	db	08aH
	db	08bH
	db	08cH
	db	08dH
	db	08eH
	db	08fH
	db	090H
	db	091H
	db	092H
	db	093H
	db	095H
	db	096H
	db	097H
	db	098H
	db	099H
	db	09bH
	db	09dH
	db	09fH
	db	0a0H
	db	0a1H
	db	0a2H
	db	0a3H
	db	0a4H
	db	0a5H
	db	0a6H
	db	000H			; List Terminator
TrashCodesLen	equ	$ - TrashCodes

;**
;
; CheckExtKey - Checks to see if keycode passed is an extended key
;
; ENTRY:
;	DL = Key
;	DH = Scan Code
; EXIT:
;	DX = 0 if Key is not accessable in AT I keyboard
;	   else
;	      DL = Key (Translation may take place)
;	      DH = Scan Code (Translation may take place)
;
; USES:
;	DX
;
CheckExtKey:

	push	ES
	push	AX
	push	CX
	push	DI

	test	CS:fATIIBIOS,0FFH	; Is there an ATII BIOS
	jnz	CEK10
	jmp	CEKRet
  ;
  ; Check for Duplicate keys first
  ;
  ; The duplicate keys with codes 47H - 53H, 73H-77H and 84H should be
  ; mapped to the appropriate AT 1 key code which is simply done
  ; by clearing DL.  These keys are the CCP keys and the ctrl-CCP keys.
CEK10:
	cmp	DL,0E0H 		;duplicate key flag
	jne	CEK15
	cmp	DH,84H
	je	CEK12
	cmp	DH,47H
	jb	CEKTrash
	cmp	DH,53H
	jbe	CEK12
	cmp	DH,73H
	jb	CEK15
	cmp	DH,77H
	ja	CEK15
CEK12:
	xor	DL,DL			; Map to AT I key code
	jmp	CEKRet

CEK15:
	cmp	DH,0E0H
	jne	CEK20
	mov	AL,DL
	lea	DI,DupHighByte
	push	CS
	pop	ES
	mov	CX,DupHighLen
	cld
	repne	scasb
	jcxz	CEK20
	lea	DI,DupHighReplacement
	mov	AX,DupHighLen
	inc	CX
	sub	AX,CX
	shl	AX,1			; adjust for words
	add	DI,AX
	mov	DX,ES:[DI]
	jmp	CEKRet

  ;
  ; Check for keys to trash
  ;
CEK20:
	or	DL,DL			;is it trashable
	jnz	CEKRet
	mov	AL,DH
	lea	DI,TrashCodes
	push	CS
	pop	ES
	mov	CX,TrashCodesLen
	cld
	repne	scasb
	jcxz	CEKRet
CEKTrash:
	xor	DX,DX			; Trash the key!!!!!

CEKRet:
	pop	DI
	pop	CX
	pop	AX
	pop	ES
	ret


;**
;
; RemoveKey - Check key status and get key if present
;
; ENTRY:
;	DS = Data
;	AH = function number 00H or 10H
; EXIT:
;	Zero Set
;		No key available
;	Zero Clear KEY REMOVED FROM BUFFER!!!!
;		DL = KEY
;		DH = Scan Code
; USES:
;	BX,DX,FLAGS
;
; NOTES:
;	RemoveKey gets keys already filtered for function 0 from
;	PeekKey.
;
RemoveKey:
	push	AX
	inc	AH			; function number for PeekKey
	call	PeekKey 		; Is there a KEY?
	jz	rmk1			; No
	call	NextKbBuffer		; Advance tail pointer
	mov	[KbBufferTail],bx	; Remove key
	or	bx,bx			; return NZ if key found
rmk1:
	pop	AX
	ret

;**
;
; PeekKey - Check key status and get key if present but don't remove it
;
; ENTRY:
;	AH = Function emulation code
;	DS = Data
; EXIT:
;	BX = [KbBufferTail] (pointer to key if present)
;	Zero Set
;		No key available
;	Zero Clear   KEY NOT REMOVED FROM BUFFER!!!!
;		DL = KEY
;		DH = Scan Code
; USES:
;	BX,DX,FLAGS
;
; NOTES:
;	PeekKey does not call the Previous Int 16H handler directly.
;	It pulls keys from the buffer that GetKey fills.  This buffer
;	may contain Extended Keycodes from an AT II keyboard if one
;	is attatched to the system.  Function 1 does not return extended
;	keycodes and therefore must go through the CheckExtKey filter.
;	If the filter returns zero for the key, PeekKey will update the
;	tail pointer to the buffer to throw away the key.  Then will
;	attempt to get the next key.  This is the same as is done in the
;	BIOS Int 16H handler.
;
PeekKey:
	mov	bx,[KbBufferTail]
	cmp	bx,[KbBufferHead]	; Head == Tail?
	jz	pkk1			; Yes, no key
	mov	dx,[bx] 		; Get key
	cmp	AH,1
	jne	pkk2
	call	CheckExtKey
	or	DX,DX
	jnz	pkk2
	call	NextKbBuffer
	mov	[KbBufferTail],bx
	jmp	PeekKey
pkk2:
	or	bx,bx			; set non-zero
pkk1:	ret


;**
;
; NextKbBuffer - advance key buffer pointer
;
; ENTRY:
;	BX = Key buffer pointer
; EXIT:
;	BX = key buffer pointer advanced by one key
; USES:
;	BX,FLAGS
;
NextKbBuffer:
	add	bx,2				; Next key
	cmp	bx,dataOffset KbBufferEnd	; Wrap around?
	jb	nkbx				; no
	mov	bx,dataOffset KbBuffer		; Wrap around to start
nkbx:	ret

;!!HP	MCP		this is required to enable relative jumps below
;			due to code required for MCP
MyYieldjmp:
	jmp	MyYield
;!!HP	Done

;**
;
; ProcessMessage2 - Process Windows messages
;
; This routine is primarily used to get KEY messages. Its
; behavior is determined by which hooks are in.
;
;	KEYBOARD HOOKS OUT, VIDEO HOOKS EITHER IN OR OUT
;		Do normal Windows message processing
;	KEYBOARD HOOKS IN, VIDEO HOOKS IN
;		Do normal Windows message processing
;	KEYBOARD HOOKS IN, VIDEO HOOKS OUT
;		Try to get key if available
;
; THIS ROUTINE DIFFERS FROM ProcessMessage ONLY IN THAT IT DOESN'T BLOCK
; ON KEYBOARD READ IN THE KEYBOARD HOOKS IN, VIDEO HOOKS OUT CASE.
;
;    NOTE: This routine MAY NOT RETURN as it executes an EXIT if WM_QUIT
;		is detected.
;
; ENTRY:
;	DS = Data
; EXIT:
;	Zero Set
;	    No keys found
;	Zero re-set
;	    A key was found and placed in the KEYBOARD buffer
; USES:
;	ALL but DS,BP
;
ProcessMessage2:
	test	cs:[fHook],KeyHook	; Keyboard hooks in?
	jz	MyYieldJmp		; No, do standard Windows message proc
	test	cs:[fHook],VidHook	; Video hooks in?
	jnz	MyYieldJmp		; Yes, Keyhook in, Videohook in, do standard Windows message proc
    ;
    ; Keyboard hooks are in, but Video hooks are not
    ;	get keys manually
    ;
pmx3:	call	GetKey
	jz	nkbx		; No key, return

Add1Key:
	mov	di,1		; One key into buffer
	errn$	AddKey		; falls into AddKey
;**
;
; AddKey - Put the key in AX into the keyboard buffer di times
;
; ENTRY:
;	DS = Data
;	AH = Scan code
;	AL = KEY
; EXIT:
;	Zero Set
;	    At least one key could not be placed in buffer as it is full
;	Zero re-set
;	    All Key(s) placed in keyboard buffer
; USES:
;	BX,SI,FLAGS
;
AddKey:
	mov	bx,[KbBufferHead]	; Next key goes here
	mov	si,bx			; In SI
	call	NextKbBuffer		; Advance buffer pointer
	cmp	bx,[KbBufferTail]	; Buffer full?
	jz	adk1			; Yes
	mov	[si],ax 		; Key into buffer
	mov	[KbBufferHead],bx	; Advance
	dec	di			; Done?
	jnz	AddKey			; no, next key
	or	si,si			; zf != 0 means we got all keys in
adk1:	ret

;**
;
; ProcessMessage - Process Windows messages
;
; This routine is primarily used to get KEY messages. Its
; behavior is determined by which hooks are in.
;
;	KEYBOARD HOOKS OUT, VIDEO HOOKS EITHER IN OR OUT
;		Do normal Windows message processing
;	KEYBOARD HOOKS IN, VIDEO HOOKS IN
;		Do normal Windows message processing
;	KEYBOARD HOOKS IN, VIDEO HOOKS OUT
;		Block (go into spin loop) waiting for the user
;			to type a key
;
;    NOTE: This routine MAY NOT RETURN as it executes an EXIT if WM_QUIT
;		is detected.
;
; ENTRY:
;	DS = DATA
; EXIT:
;	None
; USES:
;	ALL but DS,BP
;
ProcessMessage:
	test	cs:[fHook],KeyHook	; Key board hooks in?
	jz	pmx2			; No
	test	cs:[fHook],VidHook	; Video hooks in?
	jnz	pmx2			; Yes
    ;
    ; Keyboard hooks are in, but Video hooks are not
    ;
pmx1:
      ;
      ; Check for a Beep
      ;
	cmp	cs:[fBeep], 0
	je	pmNoBeep
	call	Beep
	mov	cs:[fBeep], 0
pmNoBeep:
      ;
      ; MCP	check if menu is requested
      ;
	cmp	word ptr CS:[fMenuActive],1
	jz	pmx1a		;jump if menu up , also means not pasting
	cmp	word ptr CS:[fMenu],1
	jnz	pmx1b		;jump if menu not requested - might be pasting
	push	DS
	call	DoMenu
	pop	DS
pmx1b:
	cmp	word ptr CS:[fPasteActive],1
	jnz	pmx1a
	mov	AH,0
	pushf
	call	PasteInt16	;if pasting get key from int 16 handler
	jmp	Add1Key

pmx1a:	call	GetKey			; wait for a key
	jz	pmx1			; No key, keep trying
	jmp	Add1Key 		; Got a key, put it in buffer

pmx2:	call	FilterSetup		; set up possible errmode filter
	regptr	pMsg,ds,si
	cCall	GetMessage,<pMsg,ax,cx,dx>
	or	ax,ax			; Quit message?
	jz	pm1			; Yes
	jmp	short pm2		; No
pmxRet: ret

;**
;
; MyYeild - Yeild if Windows wants to by looking for messages in app queue
;
;    NOTE: This routine MAY NOT RETURN as it executes an EXIT if WM_QUIT
;		is detected.
;
; ENTRY:
;	DS = DATA
; EXIT:
;	Zero Set
;	    No messages processed
;	Zero re-set
;	    A message was processed
; USES:
;	ALL but DS,BP
;
MyYield:
	call	FilterSetup		; set up possible errmode filter
	mov	bl,TRUE 		; BX = bRemoveMsg = Yes
	regptr	pMsg,ds,si
	cCall	PeekMessage,<pMsg,ax,cx,dx,bx>
	or	ax,ax				; any messages waiting?
	jz	pm3				; no, done
	cmp	[si+msMessage],WM_QUIT		; Quit message?
	jnz	pm2				; No, do normal stuff
    ;
    ; Here if WM_QUIT message is detected
    ;
pm1:	jmp	_exit				; EXIT (func 4B)

pm2:	cCall	TranslateMessage,<pMsg>
	cCall	DispatchMessage,<pMsg>
	or	si,si				; Reset Zero Flag
pm3:	ret

;**
;
; FilterSetup - Set up parms for call to GetMessage, PeekMessage
;
; This routine sets the regs up with the various parms for a call to GetMessage
;	or PeekMessage.
; If DOS is not in ERRORMODE, ALL messages.
; If DOS is in ERRORMODE, just key messages.
;
; ENTRY:
;	DS = DATA
; EXIT:
;	CX = MessageMin
;	DX = MessageMax
;	DS:SI -> Message buffer
;	AX = 0 (hWnd parm for message routines, 0 means message for any WINOLDAP Window)
; USES:
;	AX,CX,DX,SI,ES,FLAGS
;
FilterSetup:
	xor	cx,cx			; filter min
	xor	dx,dx			; filter max = filter min = 0, all messages
	les	si,cs:[pErrMode]	; get pointer to DOS errmode byte
	cmp	byte ptr es:[si],0	; are we inside an int 24?
	jz	fs1			; no..
	mov	cx,WM_KEYFIRST-1	; only take keyboard messages
	mov	dx,WM_KEYLAST
fs1:	mov	si,dataOffset Msg	; general setup
	xor	ax,ax			; we want it for all windows
	ret

;------------------------------------------------------
;
; The following set of functions are mostly DOS 1-12 system calls
;
; These DOS emulation routines are called by the INT 21H intercept code
; in the KERNEL (see INT21.ASM of KERNEL) via the Interchange proceedure.
;

;**
;
; Func1 - Read keyboard and echo (DOS call AH=1)
;
; ENTRY:
;	DS = Data
; EXIT:
;	Char from stdin in AL
; USES:
;	ALL but seg regs and BP
; SEE ALSO:
;	PC-DOS Programmers Manual
;
Func1:	call	Func7		; Read char
	push	ax		; Save char
	call	OutANSI 	; Echo char to stdout
	pop	ax		; Char in AL
	ret

;**
;
; Func2 OutANSI - Write TTY with ANSI escape sequences (DOS call AH = 2)
;
;	This routine handles DOS call 2 and part of DOS call 6.
;
;	This call duplicates the functionality of the DOS ANSI.SYS
;	device driver. This is implemented as a state machine.
;
;	OutANSI is the version of this call used internally.
;
;	NOTE: If the app using this call has the Screen bit set
;		(no video hooks) then we let DOS system call 6
;		handle this request.
;
; ENTRY:
;	DS = Data
;	DL = char to output for Func2
;	AL = char to output for OutANSI
; EXIT:
;	None
; USES:
;	ALL but seg regs and BP
; SEE ALSO:
;	PC-DOS Programmers Manual
;	DOS ANSI.SYS
;

;
; This is a table for dispatching the various commands
;
CmdTabl db	'A'
	dw	CUU		;cursor up
	db	'B'
	dw	CUD		;cursor down
	db	'C'
	dw	CUF		;cursor forward
	db	'D'
	dw	CUB		;cursor back
	db	'H'
	dw	CUP		;cursor position
	db	'J'
	dw	ED		;erase display
	db	'K'
	dw	EL		;erase line
	db	'f'
	dw	CUP		;cursor position
	db	'm'
	dw	SGR		;select graphics rendition
	db	'n'
	dw	DSR		;device status report
	db	's'
	dw	PSCP		;save cursor postion
	db	'u'
	dw	PRCP		;restore cursor position
	db	00

;
; This is a table of modes for the select graphics rendition command
;	Each entry is three bytes long.
;	    The first byte is the Graphics Rendition identifier
;	    The second byte is an AND mask for the specific rendition (turns
;		    selected attrib bits OFF)
;	    The Third byte is an OR mask for the specific rendition (turns
;		    selected attrib bits ON)
;	    The AND is performed first followed by the OR
; SEE ALSO:
;	IBM PC Tech Ref manual section on character attributes
;
GRMODE	db	00,00000000B,00000111B		; "normal" white on black
	db	01,11111111B,00001000B		; BOLD on
	db	04,11111000B,00000001B		; Underscore on (mono only)
	db	05,11111111B,10000000B		; Blink on
	db	07,11111000B,01110000B		; Reverse Video on
	db	08,10001000B,00000000B		; Cancelled on
	db	30,11111000B,00000000B		; Black foreground
	db	31,11111000B,00000100B		; Red foreground
	db	32,11111000B,00000010B		; Green foreground
	db	33,11111000B,00000110B		; Yellow foreground
	db	34,11111000B,00000001B		; Blue foreground
	db	35,11111000B,00000101B		; Magenta foreground
	db	36,11111000B,00000011B		; Cyan foreground
	db	37,11111000B,00000111B		; White foreground
	db	40,10001111B,00000000B		; Black background
	db	41,10001111B,01000000B		; Red background
	db	42,10001111B,00100000B		; Green background
	db	43,10001111B,01100000B		; Yellow background
	db	44,10001111B,00010000B		; Blue background
	db	45,10001111B,01010000B		; Magenta background
	db	46,10001111B,00110000B		; Cyan background
	db	47,10001111B,01110000B		; White background
	db	-1				; End of table

Func2:
	mov	al,dl			; Char to AL
OutANSI:
	test	cs:[fHook],VidHook	; Output hooks in?
	jnz	OutANSI2		; Yes, we do it
    ;
    ; If Video (output) hooks are not in, we let DOS handle this.
    ;
	mov	dl,al			; just print...
	mov	ah,6			; ...the character
	int	21h
	ret				; and return

OutANSI2:
	mov	si,dataOffset State
	jmp	[si]			; Dispatch to correct state

;
; State1 - The "normal" state
;
State1: cmp	al,cEsc 		; ESC sequence?
	jz	s2			; Yes
s1:	mov	word ptr [si],codeOffset State1 ; Back to state 1
	cmp	al,9			; TAB?
	jnz	stb			; No, normal char
	jmp	TAB			; Do TAB

stb:	mov	ah,[Attr]		; Get attrib
	jmp	OutChr			; Output the char

s2:	mov	word ptr [si],codeOffset State2 ; Change to state 2
	ret

;
; State2 - The initial "doing escape sequence" state
;
State2: cmp	al,'['                  ; Char after ESC must be '['
	jnz	s1			; Isn't, reset to state 1
	mov	word ptr [si],codeOffset State3 ; Go to state 3
	mov	si,dataOffset Parms	; Init parms structure for state 3
	mov	[pParms],si
	mov	word ptr [si],0
	ret

;
; State3 - Process the ESC sequence
;
State3: mov	bx,[pParms]		; Point to parms with BX
	cmp	al,';'                  ; End of parm?
	jnz	s4			; No, process parm
	inc	bx			; Next parm
	cmp	bx,dataOffset ParmEnd	; Paem table overflow?
	jb	s3			; no, ok
	mov	bx,dataOffset ParmEnd-1 ; Overflow, set last element
s3:	mov	[pParms],bx		; Next parm addr
	mov	byte ptr [bx],0 	; Parm is 0
	ret

;
; Process a parm
;
s4:	cmp	al,"0"
	jb	s6
	cmp	al,"9"
	ja	s6
    ;
    ; Numeric parm
    ;
	sub	al,"0"
	xchg	al,byte ptr [bx]
	mov	ah,10
	mul	ah
	add	byte ptr [bx],al
s5:	ret

    ;
    ; = and ? are ignored here
    ;
s6:	cmp	al,"="
	jz	s5
	cmp	al,"?"
	jz	s5
    ;
    ; Must have gotten to the command byte
    ;
	mov	di,codeOffset CmdTabl-3 	; Point to command table
s7:	add	di,3				; Next command
	cmp	byte ptr cs:[di],0		; End of table?
	jz	s5				; Yes, ignore
	cmp	cs:[di],al			; Found command?
	jnz	s7				; No, keep looking
    ;
    ; Found command byte
    ;
	mov	bx,dataOffset Parms		; Start of parms
	mov	dl,[bx] 			; First parm
	xor	dh,dh				; make a word
	mov	cx,dx
	or	cx,cx				; First parm 0?
	jnz	s8				; No, ok
	inc	cx				; Make it 1
s8:	mov	word ptr [si],codeOffset State1 ; Back to state 1, ESC seq complete
    ;
    ; DX is unmodified first parm
    ; CX is modified first parm (if DX == 0, CX = 1, else CX = DX)
    ;
	jmp	cs:[di+1]			; Perform indicated command

;
; MovCur - Move cursor to new position
;	AH = Limit to move
;	AL = change amount
;	BX = Pointer to Col or Row variable
;	CX = Count of times to perform action (parm 0)
;
MovCur: cmp	[bx],ah 		; Reached limit?
	jz	SetCur			; Yes, done
	add	[bx],al 		; Change var
	loop	MovCur			; Do CX times
SetCur: mov	bx,[RowCol]		; Get new cursor pos
	errn$	NewCur			; SetCur falls into NewCur
;**
;
; NewCur - Compute video buffer addr for cursor position.
;
; ENTRY:
;	DS = Data
;	BH = ROW (0 indexed)
;	BL = COL (0 indexed)
; EXIT:
;	BX is vid buffer addr for this position
;	VidPointer = BX
;	VidDirty = 1 (cursor wrong)
; USES:
;	AX,BX,CX,FLAGS
;
NewCur: call	MulCol			; Convert to buffer addr
	mov	[VidPointer],bx 	; New cursor pos
	mov	[VidDirty],1		; indicate physical location wrong
	ret

;**
;
; MulCol - Compute video buffer addr for cursor position.
;
; This routine computes the following formula
;	((ROW * 80) + COL) * 2
;
; ENTRY:
;	BH = ROW (0 indexed)
;	BL = COL (0 indexed)
; EXIT:
;	BX is vid buffer addr for this position
; USES:
;	AX,BX,CX,FLAGS
;
MulCol: mov	al,bh
	cbw
	mov	cl,4
	shl	ax,cl			; multiply by 16
	mov	cx,ax
	shl	ax,1
	shl	ax,1			; multiply by 64
	add	ax,cx			; ax = Row*80
	xor	bh,bh
	add	bx,ax
	shl	bx,1			; * two bytes per char (char and attrib)
	ret

;
; Cursor position command
;
CUP:	cmp	cl,MaxLine	; Parm 0 (line) in range?
	ja	cup3		; No, ignore
	mov	ch,[bx+1]	; Get Parm 1 (col)
	or	ch,ch		; 0?
	jz	cup1		; Yes, leave it
	dec	ch		; Make col 0 based
cup1:	mov	al,Col80
	cmp	ch,al		; Col in range?
	jbe	cup2		; Yes
	mov	ch,al		; No, limit to max
cup2:	xchg	cl,ch		; CH = row, CL = col
	dec	ch		; Make row 0 based
	mov	dx,cx		; In DX
	jmp	SetRowCol	; Do it

cup3:	ret

;
; TAB code
;
TAB:	xor	cx,cx
	mov	cl,[Col]	; Get current col
	or	cl,11111000b	; Compute tab stop
	neg	cl		; CX is now count of cursor forewards to do
	errn$	CUF		; TAB falls into CUF
;**
;
; CUF - Cursor forward command
;
; ENTRY:
;	DS = Data
;	CX = Count of times to move cursor foreward
; EXIT:
;	Cursor moved
; USES:
;	AX,BX,CX,FLAGS
;
CUF:	mov	ax,Col80*256 + 1	; Set limits for MovCur
cuf1:	mov	bx,dataOffset col	; Change col
	jmp	MovCur

;
; Cursor backward command
;
CUB:	mov	ax,0*256 + 0ffh 	; Set limits for MovCur
	jmp	cuf1

;
; Cursor down command
;
CUD:	mov	ax,Line25*256 + 1	; Set limits for MovCur
cud1:	mov	bx,dataOffset row	; Changing row
	jmp	MovCur

;
; Cursor up command
;
CUU:	mov	ax,0*256 + 0ffh 	; Set limits for MovCur
	jmp	cud1

;
; Save Cursor position command
;
PSCP:	mov	ax,[RowCol]		; Get cursor
	mov	[SavRowCol],ax		; Save it
	ret

;
; Restore Cursor position command
;
PRCP:	mov	dx,[SavRowCol]		; Get saved cursor pos
	jmp	SetRowCol		; restore

;
; Select Graphics Rendition command
;
SGR:	mov	cx,[pParms]
	sub	cx,bx			; Number of parms - 1
	inc	cx			; Number of parms
sgr1:	mov	al,[bx] 		; Get parm
	inc	bx			; Next parm
	mov	si,codeOffset GrMode-3	; Point to rendition table
sgr2:	add	si,3			; Next rendition
	cmp	byte ptr cs:[si],-1	; End of table?
	jz	sgr3			; Yes, ignore this request
	cmp	cs:[si],al		; Correct rendition?
	jnz	sgr2			; no, keep looking
	mov	ax,cs:[si+1]		; Get setting
	and	[Attr],al		; Change rendition
	or	[Attr],ah
sgr3:	loop	sgr1
	ret

;
; Erase display command
;
ED:	dec	dl		; erase from beginning of screen?
	jnz	ed1		; No

	mov	dx,[RowCol]
	mov	dl,col80
	dec	dh
	jle	el0		; Erase is on first line = erase to cursor
	xor	cx,cx		; Start at 0,0
	call	edf		; Erase lines up to current line
	jmp	short el0	; Erase on current line

ed1:	dec	dl		; erase all of screen and home cursor?
	jnz	ed2		; no
	xor	cx,cx		; Whole screen (start at 0,0)
	call	ede		; Erase screen
	xor	dx,dx		; Home to 0,0
	jmp	SetRowCol

ed2:	mov	cx,[RowCol]	; erase to end of screen
	xor	cl,cl		; Col 0
	inc	ch		; Row 1 based
	cmp	ch,MaxLine	; In range?
	jae	el2		; No, erase is all on line
	call	ede		; Erase all lines
	jmp	short el2	; Erase chars on current line

;
; Erase line command
;
EL:	dec	dl		; erase from beginning of line to cursor?
	jnz	el1		; no
    ;
    ; Erase from start of current line to cursor
    ;
el0:	mov	dx,[RowCol]	; Get cursor
	mov	ch,dh		; Current line in CH
	xor	cl,cl		; Start at col 0
	jmp	short el4

el1:	dec	dl		; erase all of line?
	jnz	el2		; no
	mov	cx,[RowCol]	; Get current line
	xor	cl,cl		; Start at col 0
	jmp	short el3	; Erase from 0 to end

    ;
    ; erase from cursor to end of current line
    ;
el2:	mov	cx,[RowCol]	; Get cursor
el3:	mov	dh,ch		; ROW in CH
	mov	dl,Col80	; COL is end of line
el4:	call	GetAttribute
	xor	ax,ax		; Entire window blanked
	jmp	Func18		; Scroll up does erase since blank
				;   line is scrolled in at bottom

ede:	mov	dx,Line25*256+Col80	; Whole screen
edf:	call	GetAttribute		; attrib to BH
	jmp	ClearScreen		; clear area


;
; Get the blank line attribute in BH for scroll ops
;
GetAttribute:
	mov	bh,[Attr]
	cmp	bh,-1			; Attr set?
	jnz	ga1			; Yes, use it
    ;
    ; Get attribute of char in lower right corner of screen
    ;
	mov	es,[VidSegment]
	mov	bh,byte ptr es:[(Line25*MaxCol*2)+1]
	and	bh,01111111b		; Turn off blink bit
ga1:	ret

;
; Device status report command (not supported)
;
DSR:	ret

;**
;
; Func3 Func4 - Not used, reserved
;
Func3:
Func4:	ret

;**
;
; Func5 - Read Keyboard like a file (DOS call AH = 3FH (Xenix Read) to CON)
;
; This routine handles DOS XENIX style READ requests to the CON input device
;	via DOS call 3FH.
;
; ENTRY:
;	DS = Data
;	ES:DX -> Transfer address
; EXIT:
;	AX = Count of bytes transfered
;	CY = Cleared
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	PC-DOS Programmers Manual
;
Func5:	mov	di,dx			; ES:DI -> buffer
	push	dx			; Save start of buffer to compute count
	mov	si,[ConPos]		; My internal buffer
	or	si,si			; Buffer left from last call?
	jnz	rkf2			; Yes, continue from there
    ;
    ; Suck up some more input
    ;
rkf1:	push	es			; Save xfer addr
	push	di

	push	ds
	pop	es			; ES -> internal buffer
	mov	cx,128			; Max size I chose to cope with
	xchg	ch,[LastCount]		; get former length of buffer
					;  (length of template for this call)
	mov	dx,dataOffset InBuf	; Point at buffer
	call	Func10			; Fill buffer with line
	pop	di			; Recover xfer buffer
	pop	es
	mov	si,dataOffset InBuf	; DS:SI -> new line
	or	al,al			; Empty?
	jz	rkf2			; Yes, lastcount left as 0
	dec	al			; Don't include CR
	mov	[LastCount],al		; Length of next template is length of
					;   line just read
rkf2:	cmp	byte ptr [si],1ah	; ^Z?
	jnz	rkf3			; No
	mov	byte ptr es:[di],1ah	; ^Z to output buffer (no inc of DI!!!!)
	jmp	short rkf5		; all done, finish up

rkf3:	lodsb				; Transfer byte
	stosb
	cmp	al,13			; CR?
	jnz	rkf4			; No
	mov	byte ptr [si],10	; insert LF after CR to simulate file
rkf4:	cmp	al,10			; LF?
	loopnz	rkf3			; Continue while chars and NOT LF
	jnz	rkf7			; Stopped due to CX = 0, still have buffer
    ;
    ; Data transfered, finish up
    ;
rkf5:	mov	al,10			; Echo LF to stdout
	call	OutCh
rkf6:	xor	si,si			; nothing in internal buffer
rkf7:	mov	[ConPos],si		; Set internal buffer pointer for next call
	pop	dx			; Recover start of xfer buffer
	mov	ax,di
	sub	ax,dx			; Dif is count of chars transfered
	clc				;~~tqn 061187 return success
	ret

;**
;
;  Func6 - Read Keyboard
;  Func31 - Read Keyboard Extended
;
;	This routine handles INT 16H read char requests.
;	This routine has the side effect of causing screen update for
;		apps that run in a Window.
;	Extended reads are int 16H functions 10H and 11H
;
; ENTRY:
;	DS = Data
; EXIT:
;	AH = Scan code
;	AL = Key
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM PC Tech Ref manual
;
Func31:
	call	UpdateScreen
	mov	AH,11H
	jmp	f23b
Func6:	call	UpdateScreen		; update screen if video hooks in

f23a:
	mov	AH,1
f23b:
	push	AX			; save the code in case of looping
	call	KeyStatus		; is there a key in the buffer?
	pop	AX
	jz	f23c
	dec	AH			; set correct function
	jmp	EatKey			; yes, go get it
    ;
    ; If no key, go into input block state waiting for it
    ;
;**
;
; EatKey - Remove key/scan code from key buffer
;
; There are TWO places where keys can come from:
;	PASTE BUFFER - Use GetDataX to get keys
;	KEYBOARD - Use RemoveKey to get keys
;
; ENTRY:
;	DS = Data
;	AH = Function code
; EXIT:
;	AH = Scan code
;	AL = Key
; USES:
;	ALL but DS,BP
;
;!!HP	MCP
;	Keystatus returns 0 if paste has just started
;
f23c:
	cmp	word ptr CS:[fPasteActive],1
	jnz	f23b
	mov	AH,0			;if pasting then call int 16H
	pushf
	call	PasteInt16		;Paste ISR
	mov	DX,AX			;be compatible
	jmp short f26
;!!HP Done


f23:	call	ProcessMessage		; Process Windows messages
EatKey:
	push	AX			; save in case no paste keys
	call	GetDataX		; Check for PASTE keys
	pop	AX
	jnz	f25			; Have a PASTE key, return it
	call	RemoveKey		; Try to suck out KEYBOARD key
	jz	f23			; No key, continue until key
f25:	mov	ax,dx			; key/scan code to AX
f26:	ret

;**
;
; UpdateScreen - Update display if video hooks in (app running in Window)
;
;	This routine is used to force the display to be current.
;
; ENTRY:
;	DS = Data of app MUST BE RUNNING IN A WINDOW
; EXIT:
;	Display updated if app.
; USES:
;	ALL but DS,BP
;
UpdateScreen:
	test	cs:[fHook],VidHook	; video hooks in?
	jz	f26			; No, done
	call	FlushComBufferNoYield	; make display and logical buffer =
	mov	ax,[RowCol]
	jmp	TrackRowCol		; Update Window (scroll bars etc.)

;**
;
;  Func7 Func7i - Read Keyboard Byte
;
;  This routine handles DOS call 8 and part of DOS call 6.
;
;  Func7i is used internally, it makes an AX = 0 call to Func7 (no ^C check)
;
; ENTRY:
;	DS = Data
;	AX
;	  High Bit Set
;		Perform ^C check
;	  High Bit ReSet
;		DO NOT Perform ^C check
; EXIT:
;	fCtrlC set if ^C detected
;	AL = char
;	  NOTE WARNING: In the case where this routine returns with
;			fCtrlC non-zero, AL is not set to anything useful.
; USES:
;	ALL but DS,BP
;
Func7i: xor	ax,ax
Func7:	push	ax		; save function code
	xor	ax,ax
	xchg	al,[AltAH]	; Reset dual byte code, and get current
	or	al,al		; Second byte of dual byte?
	jnz	rky1		; Yes
rky0:	call	Func6		; Get key
	or	al,al		; Dual byte key?
	jnz	rky1		; No
	mov	[AltAH],ah	; Second byte to storage loc for next call
rky1:	pop	bx		; restore function code
	or	bh,bh		; should we check for ^C?
	jns	rkyx		; No
	or	ax,ax		; Code 0,0 is also ^C (Ctrl-Break key)
	jz	gcc
	cmp	al,"C"-"@"      ; ^C?
	jnz	rkyx		; No
	errn$	GotCtrlc	; falls into GotCtrlC
;**
;
; GotCtrlC - Print ^C CRLF to output
;
; ENTRY:
;	DS = Data
;	AL = ^C (03)
; EXIT:
;	^C CRLF printed to output
;	fCtrlC set
; USES:
;	AX,FLAGS
;
GotCtrlC:
	call	BufOut
	call	CRLF
gcc:	inc	byte ptr [fCtrlC]	; flag ^C as detected
rkyx:	ret

;**
;
;  Func8 OutChr MyAbort - Teletype output, no ANSI
;
;      Func8 handles INT 10H call 14 (Write TTY), calls OutChr with no attribute
;      OutChr is an internal version used to put char/attrib pairs up on the display
;      MyAbort is used when ^C is detected and is to mean program abort
;
;    WARNING: MyAbort can ONLY be used on a code path through InterChange!!!!
;
; ENTRY:
;	DS = Data
;	AL = Char
;	AH
;	   if using OutChr AH is attribute, a -1 indicates attribute at that
;		position is not to be modified.
;	EntryBP == 0 if no ^C check is allowed
; EXIT:
;	Char output
;	StatusFlag cleared to 0 for printable chars
; USES:
;	ALL but DS,BP
;
Func8:	mov	ah,-1		; dont change the attribute
OutChr: mov	si,dataOffset RowCol
	cmp	al,13		; is it a CR?
	ja	f14d		; Above CR, its printable
	jb	f14a		; Below CR, it may be a special character
	cmp	[EntryBP],0	; status checking enabled?
	jz	f14aa		; no, just update screen
	call	StatusCheck	; get keyboard status
	jnc	f14aa		; ^C not typed
    ;
    ; Here when ^C is detected and is to mean program abort
    ;
MyAbort:
	mov	bp,[EntryBP]
	jmp	inter2		; abort, Bail out of InterChange

    ;
    ; Process CR
    ;
f14aa:	mov	ax,[si] 	; get current cursor position
	mov	bx,ax		; save it
	mov	al,0		; set new column to zero
	mov	bh,al		; subtract Curr Col (in BL) from cursor to do CR
	jmp	f14f		; go do it...

    ;
    ; Char below CR
    ;
f14a:	cmp	al,10		; is it a LF?
	jnz	f14b		; no, check for back space
    ;
    ; LF
    ;
	mov	ax,[si] 	; get current cursor position
	inc	ah		; go to next row
	cmp	ah,MaxLine	; past last row?
	jb	f14q		; No
	jmp	ScreenScroll	; yes, go scroll the screen

f14q:	mov	bx,MaxCol	; add this much to current cursor position
	neg	bx		; Code at f14f does a subtract
	call	f14f		; go do it
	cmp	ah,byte ptr [LastRow]	; New last row?
	jb	f14lf			; No
	inc	byte ptr [LastRow]	; Yes
	jmp	TrackScroll		; Update scroll bars etc.

f14lf:	jmp	FlushComBuffer	; and show everybody

f14b:	cmp	al,8		; is it BACK SPACE?
	jnz	f14c		; no, check for bell
    ;
    ; Back Space
    ;
	mov	ax,[si] 	; get current cursor position
	or	al,al		; already at column zero?
	jz	f10c		; yes, don't do anything
	dec	al		; go to previous column
	mov	bx,2		; subract 2 to get to current cursor position
	jmp	short f14g

f14c:	cmp	al,7		; is it a BELL?
	jnz	f14d		; no, go print it
    ;
    ; Bell
    ;
Beep:
	cmp	fWindowsEnabled, 0	; Can we use Windows?
	jne	bWin
	call	DoBeep			;    No
bWin:
	mov	ax,MB_OK+MB_ICONEXCLAMATION; Yes
	cCall	MessageBeep,<ax>	; beep the speaker
f10c:	ret

    ;
    ; Printable Char
    ;
f14d:	mov	[StatusFlag],0		; don't yield on next status call
	call	ClearSelection		; clear any mouse selection
	les	di,[VidAddr]		; point es:di at screen position
	cmp	ah,-1			; do we know the attribute?
	jz	f14e			; No, just do char
	and	ah,01111111b		; clear select bit
	stosw				; print char and attribute
	jmp	short f14i		; continue

f14e:	stosb				; print byte in AL
	inc	di			; increment past attribute byte
f14i:	mov	[VidPointer],di 	; save new screen position
	mov	dx,1
	call	InsChrOutCommand	; tell'em what we did
	mov	ax,[RowCol]		; get col, row
	inc	al			; inc col
	cmp	al,MaxCol		; is it at the right edge?
	jb	f14h			; no, save current cursor position
	mov	al,0			; set column to zero...
	inc	ah			; ...and increment the row
	cmp	ah,MaxLine		; off the bottom?
	jb	f14h			; no, save the current cursor position

	call	ScreenScroll		; scroll the screen
	mov	ax,Line25*256+0 	; row 24, column 0
	mov	bx,MaxCol		; back current pos up this many chars
f14f:	shl	bx,1			; convert bytes to words
f14g:	sub	[VidPointer],bx 	; correct current screen position
f14h:	mov	[RowCol],ax		; save current row, col
	mov	[VidDirty],1		; indicate hardware cursor is wrong
	ret				; go home.

;**
;
; ScreenScroll - Scroll entire screen up one line
;
; ENTRY:
;	DS = Data
; EXIT:
;	Selections cleared, screen scrolled
; USES:
;	ALL but DS,BP
;
ScreenScroll:
	mov	al,1			; # lines to scroll
	mov	ch,0			; Top row
	mov	dh,Line25		; Bottom row
	mov	es,[VidSegment]
	mov	bh,byte ptr es:[(Line25*MaxCol*2)+1]	; Fill attribute
	call	InsScrollCommand	; Stick the scroll command in the update queue
	mov	ax,Line25*MaxCol	; ax = word count to scroll
	mov	si,MaxCol*2		; si = count of bytes going out
	xor	di,di			; start of scroll area is start of buffer
	errn$	ScreenScroll1		; ScreenScroll falls into SreenScroll1
;**
;
; ScreenScroll1 - Scroll screen buffer lines
;
; Scroll the indicated region of the logical video buffer
;
; ENTRY:
;	DS = Data
;	AX = Count of words to be scrolled in buffer (even number of lines)
;	SI = Count of BYTES to scroll out of buffer  (even number of lines)
;	DI = Start addr in buffer of scroll	     (on line boundary)
;	BH = Fill attrib for blank line scrolled in
;	WARNING: Area being scrolled MUST occupy contig area of video buffer
; EXIT:
;	Selections cleared, video buffer scrolled
; USES:
;	AX,BX,CX,SI,DI,ES,FLAGS
;
ScreenScroll1:
	call	ClearSelection		; Clear any selections
	mov	cx,ax			; get count of screen words to move
	push	ds
	mov	ds,[VidSegment] 	; Point into buffer with DS and ES
	push	ds
	pop	es
	mov	ah,bh			; ah = fill attribute
	and	ah,01111111b		; clear select bit
	mov	bx,si
	shr	bx,1			; bx = # chars (words) to scroll in as blank
	add	si,di			; Source is addr plus words leaving
	rep	movsw			; Scroll
	mov	cx,bx			; Count of blank chars to scroll in
	mov	al," "                  ; Char in AL attrib in AH
	rep	stosw			; Blank region
	pop	ds			; Recover input DS
	ret

;**
;
; TrackScroll - Track vert one line scroll ups with vert scroll bar
;		  and update Window display
;
;	This routine is used to track one line up scrolls on the displayed
;	Window for apps that run in a Window
;
; ENTRY:
;	DS = Data
; EXIT:
;	Vert scroll bar and client area updated for one line up scroll
; USES:
;	ALL but DS,BP
;
TrackScroll:
	cmp	[fIcon],SIZEICONIC	; Iconic?
	jz	tsc1			; Yes, don't bother
	call	FlushComBuffer		; Update buffer
	xor	bx,bx
	mov	ax,[CharHeight]
	add	[VisiRow],ax		; VisiRow one line further along
	neg	ax			; Make neg (scroll up) for YAmnt to ScrollWindow
	mov	si,[hTTYWnd]		; This Window
	cCall	ScrollWindow,<si,bx,ax,bx,bx,bx,bx>	; Scroll Window
	cCall	UpdateWindow,<si>	; RePaint
	mov	bx,SB_VERT		; Update vert scroll bar
	mov	ax,TRUE 		; Repaint
	cCall	SetScrollPos,<si,bx,VisiRow,ax>
tsc1:	ret

;**
;
;  Func9 - Write TTY string with ANSI
;
; This routine handles DOS Call 9 and DOS XENIX style WRITE requests to the
;	CON input device via DOS call 40H.
;
; Imbedded ^Zs in the string are not allowed (write is in cooked mode)
;
; ENTRY:
;	DS = Data
;	ES:DX -> String to write
;	CX = Count of chars in string
;	App partition may or may not be loaded
; EXIT:
;	String written
;	AX is count of chars NOT written (may be <= input CX)
;	CY Cleared
; USES:
;	ALL but DS,BP
;
Func9:	jcxz	wts2			; Handle 0 case by ignoring it
	mov	si,dx			; ES:SI -> string
wts1:	call	CheckPartition		; Make sure partition is loaded
	lods	byte ptr es:[si]	; Get next char
	cmp	al,"Z"-"@"              ; ^Z?
	jz	wts2			; Yes, done
	push	es			; Save regs we care about
	push	si
	push	cx
	call	OutANSI2		; Output Char
	pop	cx
	pop	si
	pop	es
	loop	wts1			; Go as long as requested
wts2:	mov	ax,cx			; AX is return count
	clc
	ret

;**
;
; Func10 - Read keyboard buffer
;
;     This routine handles DOS Call 10, it is also used internaly in Func5.
;	A close examination of this code by an experianced DOS programer
;	will reveal that this code is lifted DIRECTLY out of DOS
;	Std_CON_String_Input code.
;
; ENTRY:
;	DS = Data
;	ES:DX -> Buffer (for structure, see DOS programmers Manual)
;	CL = size of buffer at ES:DX		The CX register contains the first
;	CH = size of Template buffer at ES:DX	  two bytes of the DOS Func10 buffer
;	App partition may or may not be loaded
; EXIT:
;	Buffer contains next input line
;	AX = Count of chars put in buffer including trailing CR
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	PC-DOS Programmers Manual
;

;
; Codes for the edit functions. This is a table of the "command codes" for
; DOS edit functions. It is the codes returned as the second byte of a DOS
; two byte Escape sequence.
; SEE ALSO:
;	MS-DOS OEM Adaptation Quide
;
EscTab	db	64			; Ctrl-Z - F6
	db	77			; Copy one char - -->
	db	59			; Copy one char - F1
	db	83			; Skip one char - DEL
	db	60			; Copy to char - F2
	db	62			; Skip to char - F4
	db	61			; Copy line - F3
	db	61			; Kill line (no change to template ) - Not used
	db	63			; Reedit line (new template) - F5
	db	75			; Backspace - <--
	db	82			; Enter insert mode - INS (toggle)
	db	82			; Exit insert mode - INS (toggle)
	db	65			; Escape character - F7
	db	65			; End of table
EscEnd	label	byte
EscTabLen equ	EscEnd-EscTab

;
; This is the table of command ACtions taken when the command codes in
; the above table are detected. It cooresponds in REVERSE order. In other
; words the FIRST command in this table cooresponds to the LAST command
; byte in the above table
; SEE ALSO:
;	MS-DOS OEM Adaptation Quide
;
EscFunc dw	GetCh		    ; Ignore the escape sequence
	dw	TwoEsc
	dw	ExitIns
	dw	EnterIns
	dw	BackSP
	dw	ReEdit
	dw	KilNew
	dw	CopyLin
	dw	SkipStr
	dw	CopyStr
	dw	SkipOne
	dw	CopyOne
	dw	CopyOne
	dw	CtrlZ

Func10: push	bp			; Code below trashes BP
	call	CheckPartition		; Make sure partition is loaded
	call	GetInBuf		; Perform function
	pop	bp
	ret

GetInBuf:
	mov	di,dx		; di,si points to user buffer
	mov	si,dx
	mov	[ConPos],0	; Make sur ConPos is 0 (see Func5 code)
	mov	ax,cx		; AL is buffer length, AH is template length
	xor	ch,ch
	or	al,al		; 0 length buffer?
	jz	wts2		; Yes, done
	xor	bx,bx
	mov	bl,ah		;init template counter, BL is temp length, BH position (0)

	cmp	al,bl
	jbe	noedit		;if length of buffer inconsistent with contents
	cmp	byte ptr es:[bx+si],13
	jz	editon		;if cr correctly placed edit is ok
    ;
    ; noedit if number of chars in template is >= size of buffer, or CR is not
    ;	correctly placed in template. Throw away the template.
    ;
noedit: xor	bx,bx		;reset buffer
editon: mov	dx,ax
	dec	dx		;dl is # of bytes we can put in the buffer
    ;
    ; Read in a line.
    ;
newlin:
    ;
    ; We now want to init StartCol to the correct screen position of
    ;	the first character of the buffer. If the Video hooks are in
    ;	we do this by setting StartCol = Col. In the case where the
    ;	video hooks are NOT in, Col is not relevant as we are not tracking
    ;	output, so we must init StartCol from the ROM idea of the current
    ;	column by issuing an approp INT 10h.
    ;
	test	cs:[fHook],VidHook
	jnz	NormCol 		; Vid hooks in

	push	bx
	push	cx
	push	dx
	push	ax
	mov	ah,15
	int	10h			; Get current active page to BH
	mov	ah,3
	int	10h			; Get cursor pos in dl
	pop	ax
	mov	al,dl			; Col to AL
	pop	dx
	pop	cx
	pop	bx
	jmp	short SetStrtCol

NormCol:
	mov	al,[Col]
SetStrtCol:
	mov	[StartCol],al	;remember position in raw buffer
	push	si		      ; Save start of user buffer
	mov	di,dataOffset InBuf   ; build the new line here
	mov	[insmode],0	      ; insert mode off
	mov	bh,0		      ; no chars from template yet
	mov	dh,0		      ; no chars to new line yet
	call	InCh		      ; get first char
	cmp	al,10		      ; linefeed?
	jnz	gotch		      ; If yes, don't jump so we filter out LF so < works
    ;
    ; This is the main loop of reading in a character and processing it.
    ;	BH is the index of the next byte in the template
    ;	BL is the length of the template
    ;	DH is the number of bytes in the buffer
    ;	DL is the length of the buffer
    ;	DS:DI points into buffer at next char location
    ;	ES:SI points into user buffer at next template char location
    ;
getch:	call	InCh
gotch:	cmp	al,"F"-"@"              ; ^F?
	jz	getch			; yes, ignore ^F
	cmp	al,StartEdit		; Start of edit command?
	jz	doesc			; Yes, process
	cmp	al,127			; DEL?
	jz	backspj 		; yes, DEL key = backspace
	cmp	al,8			; BackSpace?
	jz	backspj 		; Yes
	cmp	al,13			; CR?
	jz	endlin			; Yes, finished
	cmp	al,10			; LF?
	jz	phycrlf 		; Yes, new line and keep reading
	cmp	al,CancelEdit		; CANCEL? (Esc for IBM)
	jz	kilnew			; yes, kill line and start over
    ;
    ; Just have a normal char to stick in buffer
    ;
savch:
	cmp	dh,dl		; Room for char?
	jae	bufful		; no room
	mov	[di],al 	; Char into buffer
	inc	di		; Next char
	inc	dh		; got a char
;~~RCP
	cmp	al,9		; is it a tab
	jne	savch0
	call	OutTab
	jmp	savch1
savch0:
;~~RCP
	call	bufout		; print control chars nicely
savch1:
	cmp	byte ptr [insmode],0
	jnz	getch		;in insert mode, get more chars but don't advance in template
	cmp	bh,bl		; Past end of template?
	jae	getch		; yes, we are out of chars in template
	inc	si		; skip to next char in template
	inc	bh
	jmp	short getch

backspj:
	jmp    short backsp



bufful:
	mov	al,7		; bell
	call	OutCh		; Hoot the hooter
	jmp	short getch	; Keep looking for CR

    ;
    ; Process edit command
    ;
DoEsc:	push	di		; save DI (cannot change it!)
	push	es
	call	InCh		; Get command byte
	mov	cx,cs		; Point at command table
	mov	es,cx
	mov	cx,EscTabLen		; length of table for scan
	mov	di,codeOffset ESCTAB	; offset of second byte table
	repnz	scasb			; Look it up in the table
	shl	cx,1			; convert byte offset to word
	mov	bp,cx			; move to BP to index function table
	pop	es
	pop	di			; restore DI
	jmp	cs:[bp+EscFunc] 	; Go to the right routine

    ;
    ; CR detected, end of function
    ;
endlin:
	mov	[di],al 	;put the cr in the buffer
	call	OutCh		;echo it
	pop	di		;get start of user buffer, ES:DI -> user buffer
	inc	dh		;dh is length including cr
    ;
    ; copynew is used when the reedit command is detected
    ;
copynew:
	mov	si,dataOffset InBuf	; DS:SI -> new line
	mov	cl,dh
	push	cx
	rep	movsb		;copy final line to user buffer
	pop	ax		;return with ax containing final length
	ret			;all done

;**
;
; CRLF - output a crlf
;
; ENTRY:
;	DS = Data
; EXIT:
;	None
; USES:
;	AX,FLAGS
;
CRLF:	mov	al,13
	call	OutCh
	mov	al,10
	jmp	OutCh

;
; output a crlf which is not terminate buffer
;
phycrlf:
	call	CRLF
	jmp	getch

;
; zap the line without zapping the template
;
kilnew: mov	al,'\'
	call	OutCh		;print the cancel indicator
	pop	si		;remember start of edit buffer
    ;
    ; Putnew is used by Reedit command
    ;
putnew: call	CRLF		;go to next line on screen
	push	ax
	push	bx
	push	cx
	xor	cx,cx
	mov	cl,[StartCol]

;~~ Fixes (07/22/86)

	test	CS:[fhook], VidHook	; Is Video hook in ?
	jz	OutSpaces		; No, can't use CUF
	call	CUF		;tab over

OverDone:
	pop	cx
	pop	bx
	pop	ax
	jmp	newlin		;start over again

OutSpaces:
	jcxz	OverDone		; do not start loop with CX=0
	mov	AL, " "

OutSpacesLp:
	call	OutCh
	loop	OutSpacesLp
	jmp	OverDone
;~~
;
; back up one char
;
backsp:
	or	dh,dh
	jz	oldbak		; no chars in line, do nothing to line
	call	backup		; do one char backup
	mov	al,[di] 	; get the deleted char
	cmp	al,' '
	jae	oldbak		; was a normal char
	cmp	al,9
	jz	baktab		; was a tab, fix up users display
	call	backmes 	; was a control char, zap the '^'
oldbak:
	cmp	byte ptr [insmode],0
	jnz	getch1		; in insert mode, get more chars
	or	bh,bh
	jz	getch1		;not advanced in template, stay where we are
	dec	bh		;go back in template
	dec	si
getch1:
	jmp	getch

;
; Delete a tab, one char worth of the tab has already been deleted
;
baktab: push	di
	dec	di			; back up one char
	mov	cl,dh			; number of chars currently in line
	mov	al,' '
	push	bx
	mov	bl,7			; max
	jcxz	figtab			; at start, do nothing

fndpos: cmp	al,[di] 		; look back
	jna	chkcnt
	cmp	byte ptr [di],9
	jz	havtab			; found a tab
	dec	bl			; back one char if non tab control char
chkcnt: dec	di
	loop	fndpos

figtab: sub	bl,[StartCol]
havtab: sub	bl,dh
	add	cl,bl
	and	cl,7			; cx has correct number to erase
	cld				; back to normal
	pop	bx
	pop	di
	jz	oldbak			; nothing to erase
tabbak:
	call	backmes
	loop	tabbak			; erase correct number of chars
	jmp	short oldbak

;
; Back up over one char
;
backup:
	dec	dh		;back up in line
	dec	di
;
; Delete one char on screen
;
backmes:
	mov	al,8		;backspace
	call	OutCh
	mov	al,' '          ;erase
	call	OutCh
	mov	al,8		;backspace
	jmp	OutCh		;done

;
; user really wants an esc character in his line
;
twoesc: mov	al,StartEdit
	jmp	savch

;
; copy the rest of the template
;
copylin:
	mov	cl,bl		;total size of template
	sub	cl,bh		;minus position in template, is number to move
	jmp	short copyeach

;
; Copy template up to indicated char
;
copystr:
	call	FindOld 	;find the char
	jmp	short copyeach	;copy up to it

;
; copy one char from template to line
;
copyone:
	mov	cl,1

;
; copy cx chars from template to line
;
copyeach:
	mov	byte ptr [insmode],0	; all copies turn off insert mode
	cmp	dh,dl
	jz	getch2			; at end of line, can't do anything
	cmp	bh,bl
	jz	getch2			; at end of template, can't do anything
	lods	byte ptr es:[si]	; Get byte from template
	mov	[di],al 		; Save in buffer
	inc	di			; Next char in line
	call	bufout			; Print char
	inc	bh			; ahead in template
	inc	dh			; ahead in line
	loop	copyeach
getch2:
	jmp	getch

;
; skip one char in template
;
skipone:
	cmp	bh,bl
	jz	getch2			; at end of template
	inc	bh			; ahead in template
	inc	si
	jmp	getch

;
; skip to indicated char in template
;
skipstr:
	call	FindOld 		;find out how far to go
	add	si,cx			;go there
	add	bh,cl
	jmp	getch

;
; get the next user char, and look ahead in template for a match
; cx indicates how many chars to skip to get there on output
; NOTE: WARNING: if the operation cannot be done, the return
;	address is popped off and a jump to getch is taken.
;	make sure nothing extra on stack when this routine
;	is called!!! (no pushes before calling it).
;
findold:
	call	InCh			; Get the char to look for
	cmp	al,StartEdit		; did he type a function key?
	jnz	findsetup		; no, set up for scan
	call	InCh			; eat next char
	jmp	notfnd			; go try again

findsetup:
	mov	cl,bl
	sub	cl,bh		; cx is number of chars to end of template
	jz	notfnd		; at end of template
	dec	cx		; cannot point past end, limit search
	jz	notfnd		; if only one char in template, forget it
	push	di
	mov	di,si		; template to es:di
	inc	di
	repne	scasb		; look
	pop	di
	jnz	notfnd		; didn't find the char
	not	cl		; turn how far to go into how far we went
	add	cl,bl		; add size of template
	sub	cl,bh		; subtract current pos, result distance to skip
	ret

notfnd:
	pop	bp		;chuck return address
	jmp	getch

;
; Discard the current template, and make line typed till now the template
;
reedit:
	mov	al,'@'          ; output re-edit character
	call	OutCh
	pop	di		; Recover addr of start of template buffer
	push	di
	call	copynew 	; copy current line into template
	pop	si
	mov	bl,dh		; size of line is new size template
	jmp	putnew		; start over again

;
; Toggle insert mode
;
exitins:
enterins:
	not	byte ptr [insmode]
	jmp	getch

;
; put a real live ^z in the buffer (embedded)
;
ctrlz:
	mov	al,'Z'-'@'
	jmp	savch


;~~RCP
OutTab:
	test	cs:[Fhook],VidHook	;are hooks in
	jnz	BufOut
	push	DX
	mov	AH,3
	int	10H			;get cursor pos
	xor	CX,CX
	mov	CL,DL
	or	CL,11111000b		;get tab stop
	neg	CL
	mov	AL,' '                  ;get a space
OutTab1:
	call	BufOut
	Loop	OutTab1
	pop	DX
	ret
;~~RCP

;**
;
; Bufout - Print char, print control chars as ^char
;
;    This routine is used to print chars with the added feature
;	of printing control chars in a user friendly way as ^char
;
; ENTRY:
;	DS = Data
;	AL = Char
; EXIT:
;	Char printed
; USES:
;	AX,FLAGS
;
bufout: cmp	al,9		; Tab is not converted to ^char
	jz	OutCh
	cmp	al,' '          ; Chars above and = to space are normal
	jae	OutCh
	push	ax		; Save char
	mov	al,'^'          ; Print ^
	call	OutCh
	pop	ax
	add	al,'@'          ; Make char printable and print it
	errn$	OutCh		; BufOut falls into OutCh
;**
;
; OutCh - Print char
;
;    This is the main routine used to print chars
;
; ENTRY:
;	DS = Data
;	AL = Char
; EXIT:
;	Char printed
; USES:
;	FLAGS
;
OutCh:	push	ax
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	es
	mov	bp,[EntryBP]	; reset BP because Func10 trashes it
	call	OutANSI
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
; InCh - Read the next user typed char
;
; ENTRY:
;	DS = Data
;	App partition may or may not be loaded
; EXIT:
;	AL is char
; USES:
;	AX,FLAGS
;
InCh:	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	es
	mov	bp,[EntryBP]	; reset BP because Func10 trashes it
	mov	ah,87H		; Perform ^C check
	call	Func7		; Get char
	cmp	byte ptr [fCtrlC],0	; ^C?
	jz	inch1		; No
	jmp	MyAbort 	; ^C = abort

inch1:	call	CheckPartition	; Get partition in
	pop	es
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	ret

;**
;
; Func11 - Check Keyboard Status
; Func32 - Check Keyboard Extended
;
; This routine handles DOS Call 11, part of DOS call 6, DOS call 44H,
;	and part of INT 16H.  Func32 entry works for Int 16H func 11H.
;
; This routine has an IMPLIED Yield which is executed whenever it is
;	called with StatusFlag non-zero. StatusFlag is cleared by
;	the Func8/OutChr code.
;
; This routine forces the display to be updated if the app has the video
;	hooks in.
;
; ENTRY:
;	DS = Data
; EXIT:
;	StatusFlag = 1
;	AL = -1 if key is available KEY NOT REMOVED FROM BUFFER
;	    DL is key
;	AL = 0 if no key available
; USES:
;	ALL but DS,BP
;
Func32:
	mov	ah,11H
	jmp	fnc11a
Func11:
	mov	ah,1
fnc11a:
	call	KeyStatus	; Look for key set AX,DX return
	push	ax		; Save return
	push	dx
	jnz	fnc11		; Is a key, update screen and return it
	cmp	[StatusFlag],0	; only yield if StatusFlag non-zero
	jz	fnc11		; No Yeild
	call	DoYield 	; only perform if video hooks in
	jmp	short fnc11b

fnc11:	call	UpdateScreen	; Update screen if video hooks in
fnc11b: mov	[StatusFlag],1	; StatusFlag set
	pop	dx		; Set return
	pop	ax
	ret

;**
;
; KeyStatus - entry point for Func12, Func11, Func6, ^C check to suck up a key
;
;   This is the main low level driver for finding keys. We have several places
;     to look:
;	AltAH - Second byte of two byte key
;	PASTE data exchange buffer
;	KEYBOARD buffer
;     If there are no keys in any of these places we need to look in the
;	WINOLDAP Windows application queue for any keyboard messages
;	and process them into the KEYBOARD buffer
;
; ENTRY:
;	AH = Function Emulation number
;	DS = Data
; EXIT:
;	Zero re-set, key is available KEY NOT REMOVED FROM BUFFER
;	    AL = -1
;	    DL is key
;	Zero set, key is not available
;	    AL = 0
;	This routine may YEILD if it has to go to the application queue
;	  to look for keys.
; USES:
;	ALL but DS,BP
;
; NOTES:
;	KeyStatus has been modified to accept a function number (01H or 11H)
;	in AH.	This number is saved and used only in the call to PeekKey.
;	All other processing is as was before.
;
cProc KeyStatus,<NEAR, PUBLIC>
localW	Function
cBegin
   ;
   ; Check for beep
   ;
	mov	Function,AX
	cmp	cs:[fBeep], 0
	je	ksNoBeep
	call	DoBeep
	mov	cs:[fBeep], 0
ksNoBeep:
   ;
   ; Check for Menu processing
   ;

	cmp	word ptr CS:[fMenuActive],1
	jz	KeyStat1		;jump if already in menu
	cmp	word ptr CS:[fMenu],1
	jnz	KeyStat1		;jump if menu not flagged
	push	DS
	call	DoMenu			;Process the Menu
	pop	DS

KeyStat1:
	cmp	word ptr CS:[fPasteActive],1
	jnz	KeyStat2
	mov	AH,1
	pushf
	call	PasteInt16	;ZF = 1 if no chars
	mov	DX,AX		;characters are returned in AL
	jmp	f11c		;done if pasting

KeyStat2:
	mov	dl,[AltAH]	; Second byte of dual key?
	or	dl,dl
	jnz	f11b

Spin:	call	PeekDatax	; PASTE buffer info?
	jnz	f11b		; yes, return it
	mov	AX,Function
	call	PeekKey 	; did he type anything?
	jnz	f11b		; Yes, return it

	cmp	[fEMPaste], 0	 ;KMF 042287
	jz	NoResume	 ;KMF 042287
	cmp	[fEMPaste], 1	 ;KMF 051087
	je	YesResume	 ;KMF 051087
	dec	[fEMPaste]	 ;KMF 051087
	mov	al, 0		 ;KMF 051087
	jmp	f11c		 ;KMF 051087
YesResume:			 ;KMF 051087
	call	ResumeExecMacro  ;KMF 042287
NoResume:			 ;KMF 042287


    ;
    ; Here if no key, try a process message to get a key
    ;	WE MAY YEILD HERE!!!!
    ;
	call	ProcessMessage2
	jnz	Spin		; Key appeared, suck it up at PeekKey
	mov	al,0		; No key
	jmp	short f11c	; Return no key

f11b:	mov	al,-1
f11c:	or	al,al

cEnd

;**
;
; Func12 - Flush buffer
;
; This routine handles the flush aspect of DOS Call 12
;
; ENTRY:
;	DS = Data
; EXIT:
;	Input buffer flushed
; USES:
;	ALL but DS,BP
;
; NOTES:
;	Function 00H and 01H are used for Keystatus and EatKey since these
;	will trash any extended keycodes automatically.
;
Func12:
	mov	AH,01H
	call	PeekKey 		; any keys in buffer?
	jz	f12a			; no, we're done with flush part
	xor	AH,AH			;correct function
	call	RemoveKey		; eat the key
	jmp	Func12			; and continue til no keys

f12a:
	call	ProcessMessage2 	; see if BIOS has a key
	jnz	Func12			; if so suck it up


f12:	ret

;------------------------------------------------------
;
; The following set of functions are mostly INT 10H handlers
;
; These routines are called by the interrupt service code
;
; NOTE: Since INT 10H is the video hook, we only get to these funcs
;	for apps that are running in a Window
;

;**
;
; Func13 - Set Cursor Size
;
;  This routine handles INT 10H call 1 (Set Cursor Type)
;
; ENTRY:
;	DS = Data
;	CH bits 0-4 - Start line for cursor
;	CL bits 0-4 - End line for cursor
;	VisiRow, VisiCol, row, col are cursor and display position
; EXIT:
;	Cursor set and displayed in app Window if cursor exists and app has focus
;	VidDirty = 0 (cursor correct)
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func13: mov	ax,cx
	mov	[OrgVidCursLook],ax	; Save un-normalized cursor type
	call	Normalize		; Normalize end line
	mov	cl,al			; Save in CL
	mov	al,ch
	call	Normalize		; Normalize start line
	mov	ah,al
	mov	al,cl			; AX is normalized cursor type
	mov	[VidCursLook],ax	; Save normalized cursor
	errn$	SetCursor		; Func13 falls into SetCursor
;**
;
; SetCursor - Set up and display the old app cursor
;
; ENTRY:
;	DS = Data
;	OrgVidCursLook = Correct cursor type
;	VisiRow, VisiCol, row, col are cursor, display position
;	AX = Normalized Cursor type
; EXIT:
;	Cursor displayed in app Window if cursor exists and app has focus
;	VidDirty = 0 (cursor correct)
; USES:
;	ALL but DS,BP
;
SetCursor:
	xor	bx,bx
	cmp	[fFocus],bl		; Does app have the Focus?
	jz	mcr1			; No, don't bother
	mov	cx,[OrgVidCursLook]	; Get non-normalized cursor type
	cmp	cl,ch			; Is End less than start?
	jb	mcr1			; yes, no cursor to display
    ;
    ; Create the cursor
    ;
	sub	al,ah			; Size of cursor is END-START
	inc	al			; Inclusive
	cbw				; Make it a word
	cCall	CreateCaret,<hTTYWnd,bx,CharWidth,ax>
	errn$	MoveCursor		; SetCursor falls into MoveCursor
;**
;
; MoveCursor - Display the old app cursor at the correct location
;
; ENTRY:
;	DS = Data
;	OrgVidCursLook = Correct cursor type
;	VisiRow, VisiCol, row, col are cursor display position
; EXIT:
;	Cursor displayed in app Window if cursor exists and app has focus
;	VidDirty = 0 (cursor correct)
; USES:
;	ALL but DS,SS,BP
;
MoveCursor:
	mov	bx,[OrgVidCursLook]	; Get cursor type
	cmp	bl,bh			; Is End less than start?
	jb	mcr1			; yes, no cursor

	mov	ax,[CharHeight]
	cmp	[fFocus],ah		; Does app have focus?
	jz	mcr1			; no, don't display

	mul	[row]			; VERTICAL Location on Virtual screen
	sub	ax,[VisiRow]		; Sub off part not displayed on top
	xor	bx,bx
	mov	bl,byte ptr [VidCursLook+1]	; Add on cursor height to get
	add	bx,ax				;  line of bottom of cursor
	mov	ax,[CharWidth]
	mul	[col]			; HORIZ Location on Virtual screen
	sub	ax,[VisiCol]		; Sub off part not displayed on left
	cCall	SetCaretPos,<ax,bx>	; Place cursor
	cCall	ShowCaret,<hTTYwnd>	; Display cursor
	mov	[VidDirty],0		; Cursor is correct
mcr1:	ret

;**
;
; Normalize - Input normalizer for Func13 parameters
;
;  Basically this routine makes a "best approximation" of what
;	this parameter should be for the "Virtual PC" screen
;	buffer that WINOLDAP maintains for each app. Character
;	height in this buffer is 14 scan lines (mono mode).
;	We make the best approx to correct location based on
;	Windows character height.
;
; ENTRY:
;	AL is Func13 parameter to normalize
; EXIT:
;	AL is normalized parameter
; USES:
;	AX,BL,FLAGS
;
Normalize:
	and	al,00001111b		; Mask to relevant bits to get line
	mul	byte ptr [CharHeight]	; Convert to Windows scan lines in AX
	mov	bl,14			; Out buffer is 14 scan lines/line
	div	bl			; AL best approx for our buffer
	ret

;**
;
; Func14 - Set Cursor position
;
; This routine handles INT 10H call 2 (Set Cursor Position)
;
; This routine does a Yield if the cursor is moved to a different line
;
; ENTRY:
;	DS = Data
;	DH = ROW
;	DL = COL
; EXIT:
;	Cursor updated (but MIGHT not get displayed yet)
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func14: cmp	dh,MaxLine		; Off screen?
	jb	SetRowCol		; No
	sub	dh,MaxLine		; Modulo screen height
SetRowCol:
	mov	bx,dx
	xchg	dx,[RowCol]		; Set new pos and get old
	call	NewCur			; Set new cursor pos
	cmp	dh,[Row]		; same row?
	jz	mcr1			; Yes, no yield
	errn$	DoYield 		; Func14 falls into DoYield
;**
;
; Func23 DoYield - Yeild
;
;  This routine handles implied yeilds on DOS I/O calls (3FH and 40H)
;   and other Yield situations.
;
;  It also has the side effect of updating all display info.
;
;  MAY BE CALLED FOR APP NOT RUNNING IN WINDOW in which case it is a no-op
;
; ENTRY:
;	DS = Data
; EXIT:
;	None
; USES:
;	ALL but DS,BP
;
Func23:
DoYield:
	test	cs:[fHook],VidHook	; only perform if video hooks in
	jz	doy1
	jmp	FlushComBuffer		; Update all, yield too

;**
;
; Func15 - Get Cursor Information
;
;   This routine handles INT 10H call 3 (Get Cursor Position)
;
; ENTRY:
;	DS = Data
; EXIT:
;	AX = Cursor type
;	DX = Cursor position
; USES:
;	AX,DX
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func15: mov	ax,[OrgVidCursLook]
	mov	dx,[RowCol]
doy1:	ret

;**
;
; Func16 - Exec Call
;
;   This routine is the handler for DOS call 4BH (Exec). It is simply
;	the InterchangeProc pipe to WinExec.
;
; ENTRY:
;	DS = Data
;	AL is subfuction
;	ES:DX -> Name to exec
;	CX:BX -> Exec parameter block
; EXIT:
;	See WinExec
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	MS-DOS programmers manual
;
Func16:
	regptr	pPathname,es,dx
	regptr	pParmBlk,cx,bx
	cCall	WinExec,<pPathname,pParmBlk,ax>
	ret

;**
;
;  Func17 - Not Used, Reserved
;
Func17: ret

;**
;
; Func18 - Scroll Area Up
;
;   This routine handles INT 10H call 6 (Scroll active page up)
;
; ENTRY:
;	DS = Data
;	AL = Number of lines (lines blanked at bottom) = 0 means whole area
;	CH,CL = Row,Col of upper left corner of area
;	DH,DL = Row,Col of lower right corner of area
;	BH is fill attrib for blank area scrolled in
; EXIT:
;	Scroll performed (screen MIGHT not be updated till later)
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func18: and	bh,01111111b		; Turn off blink on fill attrib
	or	cl,cl			; Left edge column 0?
	jnz	f6b			; No
	cmp	dl,Col80		; Right edge >= col 80?
	jb	f6b			; No
    ;
    ; Left edge is col 0, right is col80 (full screen width)
    ;
	mov	dl,Col80		; If > 80 make it 80
	cmp	dh,Line25		; lower edge >= last line?
	jbe	fdb8			; No
	mov	dh,Line25		; Make it last line
fdb8:	or	al,al			; Whole screen?
	jnz	ScrollAreaUp		; No, is really a scroll
	errn$	ClearScreen		; Func18 falls into ClearScreen
;**
;
; ClearScreen - Clear indicated FULL WIDTH area of screen
;
; ENTRY:
;	DS = Data
;	CH,CL = Row,Col of upper left corner of area CL MUST BE 0!!!!!
;	DH,DL = Row, Column of lower right corner    DL MUST BE Col80!!!!!
;	BH = Fill attribute
; EXIT:
;	Video buffer updated
;	Window display updated
; USES:
;	ALL but DS,BP
;
ClearScreen:
	call	ClearUpdate		; Clear Window display
	call	ScrollSetUp		; Index vid buffer
	mov	es,[VidSegment] 	; ES:DI -> buffer to start of area
	mov	bl," "                  ; Fill with blanks
	mov	ax,bx
	rep	stosw			; Clear logical vid buffer
	ret

;
; Scroll is FULL WIDTH scroll up
;
ScrollAreaUp:
	call	InsScrollCommand	; Stick command in the update queue
	mov	bl,MaxCol
	mul	bl			; AX is size of blank area in chars (words)
	push	ax
	call	ScrollSetUp		; cx = count of words in area, di = first byte
	pop	si
	sub	cx,si			; CX is size of area scrolled
	shl	si,1			; SI is size of area to be blanked in BYTES
	mov	ax,cx			; Size of scroll to AX for ScreenScroll1
	jmp	ScreenScroll1

;
; Scroll area is < full screen width
;
f6b:	or	al,al		; Clear screen?
	jnz	ScrollAreaUp2	; No
	jmp	ClearScreen2

ScrollAreaUp2:
	push	ax
	push	bx
	push	cx
	push	dx
	push	bp
	call	UpdateDisplay	; make sure screen is valid
	call	ss20		; Set up scroll parms

	push	di		; Fill word
	mul	ch		; MaxCol X top row
	xor	ch,ch
	add	ax,cx		; plus left col is char index of first char in area
	shl	ax,1		; two bytes per char
	mov	di,ax		; di = destination of scroll
	mov	dx,ax
	mov	ax,MaxCol
	mul	bl		; MaxCol X scroll lines is size of scroll in chars
	shl	ax,1		; Two btes per char
	add	dx,ax		; dx = source of scroll
	mov	cx,si		; cx = word count per line
	pop	ax		; ax = attribute and fill char

	push	bx		; bx = number of lines to scroll
	mov	si,dx		; si = source
	mov	dx,MaxCol	; Width of each line (index to next line)
	call	MoveLine	; Do scroll
	pop	bp		; Number of lines to blank in BP
	call	StoreLine	; Do blank
	cld
	pop	bp
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	jmp	ScrollUpdate	; Update display to agree with new buffer

;**
;
; ScrollSetup - Set up video buffer pointer and count to access given area
;		for FULL LINE SCROLL
;
; ENTRY:
;	CH,CL = Row,Col of upper left corner of area  CL MUST BE 0!!!!!
;	DH,DL = Row, Column of lower right corner     DL MUST BE Col80!!!!!
; EXIT:
;	CX is count of words given area occupies in video buffer
;	DI is index into video buffer of start of area
; USES:
;	AX,CX,DX,DI,FLAGS
;
ScrollSetup:
	cmp	dh,MaxLine	; Make sure bottom is in range
	jb	scsu1
	mov	dh,Line25
scsu1:	mov	ax,MaxCol
	mul	ch		; Start is AX chars into buffer
	shl	ax,1		; one word in buffer per char
	mov	di,ax		; That is the buffer index
	mov	ax,MaxCol
	inc	dh		; Bounds are inclusive
	sub	dh,ch		; DH is number of lines in area
	mul	dh		; X MaxCol is number of words in scroll area
	mov	cx,ax		; to CX
	ret

;
; Clear area is < full screen width
;
ClearScreen2:
	push	bp			; ScrollSetup2 bops BP
	call	ClearUpdate		; Do the display
	call	ScrollSetUp2		; Set up area
	mov	es,[VidSegment] 	; ES:DI-> first char
	mov	bl," "
	mov	ax,bx			; AH = Attrib, AL = " "
	and	ah,01111111b		; Make sure blink is off
	mov	bx,cx			; Save contig block count in BX
cs20:	mov	cx,bx			; Next block is same size
	rep	stosw			; Save it
	add	di,dx			; Add index skip to get to next block
	dec	bp			; One more block completed
	jnz	cs20			; More to go
	pop	bp
	ret

;
; ENTRY:
;	CH,CL = Row,Col of upper left corner of area
;	DH,DL = Row, Column of lower right corner
; EXIT:
;	BP is count of contig blocks to clear
;	CX is size in words of each contig block
;	DX is index skip between blocks
;	DI is vid buffer index to first block
;
ScrollSetup2:
	mov	ax,MaxCol
	mov	si,ax
	push	cx
	mul	ch		; AX is char index to first char of top line
	xor	ch,ch		; CX is col of left edge
	add	ax,cx		; AX is char index of first char to change
	shl	ax,1		; two bytes per char in buffer
	mov	di,ax		; points to upper left corner
	pop	cx
	mov	al,dl
	sub	al,cl		; AL is width of area in chars NON-INCLUSIVE
	inc	al		; Make width inclusive
	cbw			; make it a word
	sub	si,ax		; SI is # cols NOT in area
	shl	si,1		; SI is now number of words to skip between blocks
	mov	dl,dh
	sub	dl,ch		; DL is NON-INCLUSIVE number of blocks (rows)
	inc	dl		; Make DL inclusive
	xor	dh,dh		; Make it a word
	mov	bp,dx		; row count
	mov	dx,si		; add this to di to get to next row
	mov	cx,ax		; words to clear per row
	ret

;**
;
; Func19 - Scroll Screen down
;
;   This routine handles INT 10H call 7 (Scroll active page down)
;
; ENTRY:
;	DS = Data
;	AL = Number of lines (lines blanked at top) = 0 means whole screen
;	CH,CL = Row,Col of upper left corner of area
;	DH,DL = Row,Col of lower right corner of area
;	BH is fill attrib for blank area scrolled in
; EXIT:
;	Scroll performed (screen MIGHT not be updated till later)
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func19: and	bh,01111111b		; Turn off blink on fill attrib
	or	cl,cl			; Left edge column 0?
	jnz	f7b			; No
	cmp	dl,MaxCol		; Right edge >= col 80?
	jb	f7b			; No
    ;
    ; Left edge is col 0, right is col80 (full screen width)
    ;
	mov	dl,MaxCol		; If > 80 make it 80
	cmp	dh,Line25		; lower edge >= last line?
	jbe	fdb9			; No
	mov	dh,Line25		; Make it last line
fdb9:	or	al,al			; Whole screen?
	jnz	ScrollAreaDown2 	; No, is really a scroll
	jmp	ClearScreen		; Clear screen
;
; Scroll area is < full screen width
;
f7b:	or	al,al			; Clear screen?
	jz	ClearScreen2		; yes
ScrollAreaDown2:
	push	ax
	push	bx
	push	cx
	push	dx
	push	bp
	call	UpdateDisplay	; make sure screen is valid
	call	ss20		; Set up scroll parms

	push	di		; save fill word
	mov	cx,si		; cx = word count per line
	mul	dh		; MaxCol X bottom row
	xor	dh,dh
	add	ax,dx		; plus right col is char index of last char in area
	shl	ax,1		; two bytes per char
	mov	si,ax		;
	mov	dx,ax		; dx = source of scroll
	mov	ax,MaxCol
	mul	bl		; Lines of scroll X MaxCol gets us to dest
	shl	ax,1		; Two bytes per char
	sub	si,ax		; si = osurce of scroll
	std			; For scroll DOWN we go backwards
	pop	ax		; ax = fill word

	push	bx		; bx = number of lines to scroll
	mov	di,dx		; di = destination scroll
	mov	dx,MaxCol	; index to next line
	neg	dx		; And we go BACKWARDS
	call	MoveLine
	pop	bp		; Number of blank lines to BP

				;Dest for blank is current dest
	call	StoreLine
	cld			; Fix direction flag

	pop	bp
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	jmp	ScrollDownUpdate ; Update display to new buffer

;**
;
; MoveLine - Perform SCROLL part of a scroll operation
;
; ENTRY:
;	DS = Data
;	Direction flag set correctly (STD for scroll DOWN, CLD for scroll UP)
;	BP is number of lines to move (scroll)
;	CX is count of words to move on each line (width of scroll area)
;	DI is vid buffer index Destination of scroll (end for scroll DOWN, start for scroll UP)
;	SI is vid buffer index Source of scroll (end for scroll DOWN, start for scroll UP)
;	DX is # words between start of each line
;	    this is ADDED to DI and SI to get to next line
; EXIT:
;	DX is # BYTES between start of each line (input DX times 2)
;	DI is buff indx of next dest line (start of blank line area scroll UP)
;	SI is buff indx of next src line (start of blank line area scroll DOWN)
; USES:
;
MoveLine:
	push	ds
	mov	bx,[VidSegment] 	; Address video buffer
	mov	ds,bx
	mov	es,bx
	shl	dx,1			; Make word count byte count
	mov	bx,cx			; Width in bx
rs22:	mov	cx,bx			; Next block size
	push	si			; Save start addrs
	push	di
	rep	movsw			; Do move
	pop	di
	pop	si
	add	si,dx			; Next line
	add	di,dx
	dec	bp			; Line completed
	jnz	rs22			; more lines
	pop	ds
	ret

;**
;
; StoreLine - Perform "move in blank line" part of SCROLL operation
;
; ENTRY:
;	Direction flag set correctly (STD for scroll DOWN, CLD for scroll UP)
;	BP is number of lines to blank
;	AX is fill word (attrib/char)
;	BX is count of words to blank on each line (width of scroll area)
;	ES:DI -> Start of blank area (end for scroll DOWN, start for scroll UP)
;	DX is # BYTES between start of each line
;	    this is ADDED to DI to get to next line
; EXIT:
;	Area blanked
; USES:
;	DI,CX,BP
;
StoreLine:
ss23:	mov	cx,bx		; Size to blank
	push	di		; Save addr
	rep	stosw		; Blank this line
	pop	di
	add	di,dx		; Next line
	dec	bp		; Line completed
	jnz	ss23		; Next line
	ret

;**
;
; ss20 - Set up parameters for video area (NOT FULL WIDTH) scroll
;
; ENTRY:
;	AL = Number of lines (lines blanked at top or bottom) != 0!!!!!!!!
;	CH,CL = Row,Col of upper left corner of area
;	DH,DL = Row, Column of lower right corner
;	BH = Fill attribute
; EXIT:
;	AL = MaxCol
;	BL = Number of lines (lines blanked at top or bottom) != 0!!!!!!!!
;	BP is count of rows in scroll area
;	DI is fill word
;	SI is width of scroll area in chars (words)
; USES:
;	AX,BX,DI,SI,FLAGS
;
ss20:	xor	ah,ah
	mov	bl," "
	xchg	bx,ax		; AX = fill word, BL = count of lines to scroll
	push	ax
	mov	al,dl
	sub	al,cl		; NON-INCLUSIVE width in chars (words)
	cbw
	inc	ax		; INCLUSIVE width of scroll area in chars (words)
	mov	si,ax
	mov	al,dh
	sub	al,ch		; NON-INCLUSIVE height in lines of whole area
	cbw
	inc	ax		; Make inclusive
	mov	bp,ax		; Total rows in BP
	sub	bp,bx		; - scroll lines is number of lines to scroll
	mov	al,MaxCol
	pop	di		; di = fill word
	ret

;**
;
; Func20 - Read Character and attribute at cursor
;
;   This routine handles INT 10H call 8 (Get Char/Attrib at cursor)
;
; ENTRY:
;	DS = Data
; EXIT:
;	AL = char
;	AH = attrib
; USES:
;	AX,DI,ES
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func20: les	di,[VidAddr]	; Addr of char at cursor
	mov	ax,es:[di]	; Get char and attrib
	ret

;**
;
; Func21 - Write Character and attribute string at cursor
;
;   This routine handles INT 10H call 9 (Write Char/Attrib at cursor)
;
; ENTRY:
;	DS = Data
;	AL = char
;	BL = attrib
;	CX = Count of chars to write
; EXIT:
;	Char write queued
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func21: cmp	cx,MaxCol*MaxLine-1	; Whole screen?
	jb	wca1			; No
	cmp	al,' '                  ; Writing blanks to whole screen?
	jnz	wca1			; No
	cmp	word ptr [RowCol],0	; Starting at upper left?
	jnz	wca1			; No
    ;
    ; Clearing ENTIRE screen
    ;
	xor	ax,ax
	xor	cx,cx
	mov	dx,Line25*256+Col80
	mov	bh,bl
	jmp	ClearScreen		; just clear it directly

wca1:	call	SetMax			; Set up
	mov	ah,bl			; Attrib
	and	ah,01111111b		; clear select bit
	rep	stosw			; Store chars and attribs
	jmp	InsChrOutCommand	; Insert in screen update queue

;**
;
; SetMax - Limit screen update commands and set count input parm to InsChrOutCommand
;
; ENTRY:
;	CX = count of chars being changed
; EXIT:
;	ES:DI -> video buffer at current char position
;	CX,DX = Count of chars being inserted (may be less than input count)
; USES:
;	CX,DX,DI,ES,FLAGS
;
SetMax: les	di,[VidAddr]		; get where we are going to put chars
	mov	dx,di			; save it
	shr	dx,1			; Convert to char count
	add	dx,cx			; get last position
	sub	dx,(MaxCol*MaxLine)	; still on screen?
	jbe	smx1			; yes..
	sub	cx,dx			; no adjust length
smx1:	mov	dx,cx			; save the count in dx
	ret

;**
;
; Func22 - Write Character Only
;
;   This routine handles INT 10H call 10 (Write Char string at cursor)
;
; ENTRY:
;	DS = Data
;	CX = Count of chars to write
;	AL = char to write
; EXIT:
;	Screen update queued
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func22: call	SetMax		; Set up
f10b:	stosb			; Store char
	inc	di		; Skip attribute
	loop	f10b
	jmp	InsChrOutCommand	; Insert in screen update queue

;**
;
; Func24 - Return address of video buffer in es:ax
;
;  This routine handles TopView INT 10H call FEH (Get video buffer address)
;
; ENTRY:
;	DS = Data
; EXIT:
;	ES:AX -> Logical video buffer for this app
; USES:
;	AX,ES,FLAGS
; SEE ALSO:
;	IBM TopView Programmers toolkit
;
Func24: mov	es,[VidSegment] 	; Get seg
	xor	ax,ax			; Offset is 0
	ret

;**
;
; Func25 - Update area of screen
;
;   This routine handles TopView INT 10H call FFH (Update video buffer)
;
;   NOTE: This function is not as fast or nice as TopView is.
;	  This is very apparent for real stupid apps that update
;	  large areas of the screen when they really don't need to.
;	  THERE IS NOT ANYTHING THAT CAN BE DONE ABOUT THIS. GDI
;	  re-paints are expensive operations, much more expensive than
;	  TopView character mode stuff.
;
; ENTRY:
;	DS = Data
;	ES:BX -> First modified char of logical video buffer from call FE
;	CX = Number of bytes modified
; EXIT:
;	Display updated
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM TopView Programmers toolkit
;
Func25: push	[RowCol]		; save current row and column
	jcxz	clrd			; Zero bytes is no-op
	and	bl,11111110b		; round to even cell position
	mov	ax,bx			; get position on screen
	shr	ax,1			; convert to character cells
	mov	dl,MaxCol		; get cells per line
	div	dl			; al = row, ah = column
	mov	[Row],al
	mov	[Col],ah
	mov	dx,cx			; count goes in dx
	mov	di,bx			; position goes in di
	mov	es,[VidSegment]

clrb:	inc	bx			; point at attribute
	and	byte ptr es:[bx],01111111b	; remove any blink bits
	inc	bx			; move to next cell
	loop	clrb			; and repeat

	cmp	dx,MaxCol*MaxLine	; Whole screen?
	jb	clrc			; No, only part of screen
	or	di,di			; Start at beginning?
	jnz	clrc			; No, only part of screen
	cCall	InvalidateRect,<hTTYWnd,di,di,di> ; Whole screen invalid
	jmp	short clrd

clrc:	call	InsChrOutCommand	; update this area of the screen
	mov	[VidDirty],1		; this might be different
clrd:	pop	[RowCol]		; restore row and column
	ret

;**
;
; Func26 - Out String with no ANSI
;
;   This routine handles INT 10H call 19 (Write string EGA)
;
; ENTRY:
;	DS = Data
;	ES:BX = points to string
;	CX    = length of string
;	DL    = cursor position, col is low nybble, row is upper nybble
;	DH    = attribute (if AL = 0 or 1)
;
;	AL    = 0 means string does not contain attribute info, so use DH
;	      = 1 same as above, but also move cursor
;	      = 2 means string contains attribute info, organized as words
;	      = 3 same as above, but also move cursor
; EXIT:
;	String written
; USES:
;	ALL but DS,BP
; SEE ALSO:
;	IBM PC Tech Ref manual section on INT 10H
;
Func26:
	jcxz	osn5			; cx has count, handle zero case
	mov	si,bx			; es:si points to string
	mov	ah,dh			; ah has attribute
    ;
    ; Convert packed cursor position back to unpacked
    ;
	mov	dh,dl
	and	dl,00001111b
	shr	dh,1
	shr	dh,1
	shr	dh,1
	shr	dh,1			; dx = cursor position DH row, DL col
	push	[RowCol]		; Save current pos
	push	ax			; save function code
	call	SetRowCol		; set cursor position
	pop	ax			; recover function code
	or	al,al
	jnz	osn1
    ;
    ; AL = 0
    ;
	call	OutStringByte		; output bytes but not attribs
	jmp	short osn2a		; and recover old cursor position

osn1:	dec	al
	jnz	osn2
    ;
    ; AL = 1
    ;
	call	OutStringByte
	pop	dx			; throw away old Row,Col
	ret

osn2:	dec	al
	jnz	osn3
    ;
    ; AL = 2
    ;
	call	OutStringWord
osn2a:	pop	[RowCol]		; Restore original cursor
	mov	[VidDirty],1		; Cursor invalid
	ret

osn3:	dec	al
	jnz	osn4
    ;
    ; AL = 3
    ;
	call	OutStringWord
osn4:	pop	dx			; throw away old Row,Col
osn5:	ret

;
; Output Char and attrib pair from ES:SI
;	App partition may or may not be loaded
;
OutStringWord:
	call	CheckPartition
	lods	word ptr es:[si]	; Get char and attrib
	push	es
	push	si
	push	cx
	call	OutChr
	pop	cx
	pop	si
	pop	es
	loop	OutStringWord		; Do CX times
	ret

;
; Output Char only from ES:SI
;	App partition may or may not be loaded
;
OutStringByte:
	call	CheckPartition
	lods	byte ptr es:[si]	; Get char
	push	es
	push	si
	push	cx
	mov	ah,-1			; Output char only
	call	OutChr
	pop	cx
	pop	si
	pop	es
	loop	OutStringByte
	ret

;**
;
; Func27 - Halt error
;
;  This routine handles the case where a system halting error has been detected
;	Currently INT 27H or DOS CALL 31H without mem bit in PIF file is
;	is only case.
;
;  THIS CODE IS IN TTYTASK.ASM
;

;
;~~tqn 060187
;
.xlist
Subttl	"Old App memory management routines"
page
.list
;======= Func28 ==============================================================
;
; AllocateMemory:
;  This routine emulates the DOS Allocate Memory function (48H) and is accessed
;  when an old app issue an int 21h, AH= 48h call.
;
;
; Entry:
;  BX=	Number of paragrahs of memory requested
;
;
; Exit:
;  CF=	0: AX:0 -> allocated memory block
;	1: AX	   Error codes (See DOS tech ref)
;	   BX=	   Size of largest block of memory available (in paras)
;
; Regs:
;
;
;=============================================================================

Func28		Proc	NEAR

	call	Alloc_Partition
	ret

Func28		EndP



;======= Func29 ==============================================================
;
; Free Allocated Memory:
;  This routine emulates the DOS Free Allocated Memory function (48H) and is
;  accessed when an old app issued an int 21h, AH= 49h call.
;
; Entry:
;  ES=	Segment of block to be freed.
;
; Exit:
;  CF=	0: Success
;	1: AX	   Error codes (See DOS tech ref)
;
; Regs:
;
;
;=============================================================================

Func29		Proc	NEAR

	call	Free_Memory
	ret

Func29		EndP


;======= Func30 ==============================================================
;
; SetBlock:
;  This routine emulates the DOS SetBlock function (4AH) and is accessed
;  when an old app issue an int 21h, AH= 4Ah call.
;
;
; Entry:
;  ES=	Segment of the block
;  BX=	New number of paragrahs of memory requested
;
;
; Exit:
;  CF=	0: Success
;	1: AX	   Error codes (See DOS tech ref)
;	   BX=	   Maximum poolsize possible on a "grow" request
;
; Regs:
;
;
;=============================================================================

Func30		Proc	NEAR

	call	Modify_Partition
	ret

Func30		EndP
;
;~~

;======= CheckCritErr ========================================================
;
;     This routine checks the current Int24H vector to see if it is different
;      from the kernel int24H vector.  If so it stuffs in the kernel vector
;      address.  ONLY FOR GOOD APPS!!!!!!
;
; Entry:
;	DS: winoldap data segment
;
; Exit:
;
; Regs:
;
;
;=============================================================================
CheckCritErr	PROC	NEAR
	push	ds
	push	es
	push	si
	push	di
	push	ax
	push	cs
	pop	ds
	mov	si,codeOffset CritErrPtr
	xor	ax,ax
	mov	es,ax
	mov	di,24H*4
	cli				; no interrupt's here or we die
	cld				; forward please!!
	movsw
	movsw
	sti
	pop	ax
	pop	di
	pop	si
	pop	es
	pop	ds
	ret

CheckCritErr	ENDP


sEnd	code
	end
