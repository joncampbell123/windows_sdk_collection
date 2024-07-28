;--------------------------------------------------------------------
;
;   Screen Grabber for CGA adaptor - CGA.ASM
;
;   These routines perform paints and all other Display specific
;	aspects of WINOLDAP (VMDOSAPP)
;
;    (C) Copyright MICROSOFT Corp. 1986-1990
;    (C) Copyright Compaq Computer Corp. 1987
;
;    ************* MICROSOFT CONFIDENTIAL ******************
;-----------------------------------------------------------------------------

.386p
?386 = 1

memS	equ	1

	.xlist
	include wcmacros.inc

	NOTEXT		= 1
	NOGDICAPMASKS	= 1
	NOMB		= 1
	NOVK		= 1
	NOWH		= 1
	NOMST		= 1
	NOMETAFILE	= 1
	NOWM		= 1
	NOMDI		= 1
	NOWINMESSAGES	= 1
	NOSYSMETRICS	= 1
	NOCOLOR 	= 1
	NOCOMM		= 1

	include windows.inc
	include vmda.inc
	include int2fapi.inc
	include grabpnt.inc
	include grabmac.inc
	include vmdacga.inc
	include cga.inc
	include statusfl.inc
	.list

IF1
    IFDEF DEBUG
	%out DEBUG VERSION!!!!!
    ENDIF
ENDIF

;-----------------------------------------------
;
; External Windows Procedures
;
externFP	DestroyCaret
externFP	SetCaretPos
externFP	SelectObject
externFP	PatBlt
externFP	GetDC
externFP	ReleaseDC
externFP	GlobalAlloc
externFP	GlobalLock
externFP	GlobalUnLock
externFP        GetUpdateRect
externFP        InvertRect
externFP        ScrollWindow
externFP        UpdateWindow
externFP	GetPrivateProfileString

;-----------------------------------------------
;
; External GRABBER Procedures
;
externNP	ScreenAdjust
externNP	ComputeSelTextRect
externNP	AdjustSelGrxMax
externNP	AdjustSelTextMax
externNP	CursorPos
externNP	CheckCtrlState
externNP	MakeNewCursor
externNP	ModeText
externNP	ModeGrfx
externNP	GetVidSel
externNP	ClearVidSel
externNP	GetTextWidHgt
externNP	RenderRectText
externNP	RenderRectGrfx
ifdef	DBCS
if1
%out DBCS code enabled
endif
externNP	AdjustDBCSBound
endif


sBegin	code
	assumes cs,code

	public	PrepPnt
        public  PaintClientRect
        public  SavePaintRect
;
; This is the VDD type string that we expect as part of the version check
;
IFDEF CPQ_PLASMA
        VDD_Type db	"VIDEOIDC"    ; Compaq, Integrated Display Controller: 640 x 400
ELSE
        VDD_Type db	"VIDEOCGA"    ; CGA: 640 x 200 resolution
ENDIF

;*****************************************************************************
;
; GrabEvent - Private VDD -> Grabber messages
;
;   Provides a private channel of event communication between the VDD
;   and the grabber. CGA grabbers do not use this.
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	wParam = parameter from VDD message
;	lParam = parameter from VDD message EVENT ID
; EXIT:
;	None
; USES:
;	C Standard
;
;*****************************************************************************
cProc  GrabEvent,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	wParam
	ParmD	lParam

	localV	FrameVars,StdGrbFrameSz
cBegin
;	    push    esi
;	    push    edi
;
;	    InitBasicFrame
;
;	    pop     edi
;	    pop     esi
cEnd

;*****************************************************************************
;
; GrabComplete - Signal that we are finished with the grab
;
; This is called after the grab is complete. Time to call the VDD
; and have him free the grab memory.
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	wParam = parameter from VDD message (= -1 if VMDOSAPP origin)
;	lParam = parameter from VDD message EVENT ID
; EXIT:
;	None
; USES:
;	C Standard
;
;*****************************************************************************
cProc  GrabComplete,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	wParam
	ParmD	lParam

	localV	FrameVars,StdGrbFrameSz
cBegin
	push	esi
	push	edi

	InitBasicFrame

	cmp	wParam,-1			; Origin not VDD?
	jz	short GCDone			; Yes, NOP
	lds	si,lpPntStruc
    ;
    ; Reset the controller state bit so that we chuck the grab
    ; controller state and re-get the Paint state.
    ;
	btr	[si.EPStatusFlags],fVValidBit
	mov	ax,GRB_Free_Grab                ; Function number
	mov	ebx,[si.ThisVMHand]             
        call    [si.GGrbArea.VddApiProcAdr]     ; Call VDD
GCDone:
	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; PaintScreen - Paint the indicated region of the screen
;
;     This routine Paints the old app screen into a window
;     Origin of this is a WINDOWS paint as opposed to a display
;     update (handled in UpdateScreen)
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	AX != 0
;	    Screen Painted
;	AX == 0
;	    Screen not painted, probably low Windows memory problem
; USES:
;	C Standard
; NOTE:
;	This routine may set the fGrbProb bit in EPStatusFlags in the
;	Extended paint structure to indicate that this app is in a mode
;	which cannot be rendered into a Window. This routine must return
;	AX != 0 in this case.
;
;*****************************************************************************
cProc  PaintScreen,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi
	InitBasicFrame

	lds	si,lpPntStruc
       
	call	GetVidSel        ; FREEZE the app
        jc      PS_SetAX         ; paint screen black if error

	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ; Check to see if the screen is off
    ;
	test	[si.EPGrabDTA.CntrlSt.VDA_CGA_Flags],fVDA_V_ScOff
	jnz	short NulScreen
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode]
	cmp	ah,4
	jae	short PSGraphics
PSText:
    ;
    ; The paint should be done from the copy mem and not from the main mem.
    ; GetVidSel is called twice once for locking the APP(the selector for main 
    ; memory which is returned is not used), and again to get the copy memory.
    ;
IF1
%OUT HACK! Get copy mem for WM_PAINT
ENDIF
	mov	VidSel,0                        ; discard main mem sel
	or 	GrbFlags,GrbFlags_DoingGrab      ; set flags to get copy mem
        and     GrbFlags, NOT GrbFlags_DoingWindowSelGrab
	call	GetVidSel                        ; error unlikely
;	btr	[si.EPStatusFlags],fVValidBit    ; get the copy state - safety
;	call	CheckCtrlState
	and 	GrbFlags,NOT GrbFlags_DoingGrab  ; turn off grab Flag
	call	ModeText		; 0 1 2 3 are text
	pushf				; Save returned CF
    ;
    ; If we have the focus, we'll do the cursor
    ;
	bt	[si.EPStatusFlags],fFocusBit
	jnc	short PS_NoCursor		; Don't have focus, no cursor
	bt	[si.EPStatusFlags],fSelectBit
	jc	short PS_NoCursor		; In Select mode, don't do cursor
	call	CursorPos
	inc	dx
	jz	short PS_NoCursor	; No cursor
	dec	dx
	sub	ax,[si.ColOrg]		; Org X onto display
	sub	dx,[si.RowOrg]		; Org Y onto display
	cCall	SetCaretPos,<ax,dx>     ; Position Cursor
PS_NoCursor:
	popf				; Recover carry setting
	jmp	short PS_SetAX
PSGraphics:
        xor     cx,cx           ; Not grabbing
	call	ModeGrfx		
	jmp	short PS_SetAX          ;no cursor in graphics mode!
    ;
    ; Paint with the Black Brush
    ;
NulScreen:
	cCall	SelectObject,<[si.Pstruct.psHdc],[si.BlkBrshH]>
	lea	di,[si.Pstruct.PSrcPaint]
	mov	ax,[di.rcRight]
	sub	ax,[di.rcLeft]
	mov	dx,[di.rcBottom]
	sub	dx,[di.rcTop]
	cCall	PatBlt,<[si.Pstruct.psHdc],[di.rcLeft],[di.rcTop],ax,dx,PATCOPY_H,PATCOPY_L>
	clc
PS_SetAX:
	mov	ax,0			; DO NOT XOR!!!!!!!!
	jc	short PS_Done
	inc	ax
PS_Done:
	push	ax
	call	ClearVidSel
	pop	ax

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; ScreenFree - Free any stuff associated with this app
;
;   Frees any resources associated with this app
;   that are not allocated on an as need basis local to
;   each call on the stack, or staticly stored in the grabber
;   area of the extended paint structure.
;
;   FOR THE CGA grabbers this routine is a NOP.
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	Any allocated stuff associated with app is freed
; USES:
;	C standard
;
;*****************************************************************************
cProc  ScreenFree,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
;	    push    esi
;	    push    edi
;
;	    InitBasicFrame
;
;	    pop     edi
;	    pop     esi
cEnd

;*****************************************************************************
;
; BeginSelection - Start up a selection at the indicated point
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	YCoOrd,XCoOrd = (Y,X) screen CoOrd of start point
;		(Here "screen CoOrd" means a window client area position
;		corrected by the scroll bar positions.)
; EXIT:
;	[lpPntStruc.SelStruc.SelctSRect] Display rect in extended paint
;	selection structure set
; USES:
;	C standard
;
;*****************************************************************************
cProc  BeginSelection,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	YCoOrd
	ParmW	XCoOrd

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ;  
	mov	dx,YCoOrd
	mov	ax,XCoOrd
	mov	[bx.SelStruc.GrabArea.StartPointX],ax
	mov	[bx.SelStruc.GrabArea.StartPointY],dx
	mov	[bx.SelStruc.GrabArea.EndPointX],ax
	mov	[bx.SelStruc.GrabArea.EndPointY],dx
    ;
    ; Adjust for alignment
    ;   Adjust DX, AX for character alignment if text mode
    ;	              for DWORD alignment if graphics mode
    ;	Adjust DX, AX for screen size maxima too
    ;
	call	ScreenAdjust
	mov	[bx.SelStruc.GrabArea.StartPointX],ax
	mov	[bx.SelStruc.GrabArea.StartPointY],dx
	mov	[bx.SelStruc.GrabArea.EndPointX],ax
	mov	[bx.SelStruc.GrabArea.EndPointY],dx
	or	[bx.SelStruc.GrabArea.SelecFlags],SelectOn
	mov	[bx.SelStruc.SelctSRect.rcLeft],ax
	mov	[bx.SelStruc.SelctSRect.rcRight],ax
	mov	[bx.SelStruc.SelctSRect.rcTop],dx
	mov	[bx.SelStruc.SelctSRect.rcBottom],dx

	pop	edi
	pop	esi
cEnd


;*****************************************************************************
;
; EndSelection - Stop selection
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	Selection stopped
; USES:
;	C standard
;
;*****************************************************************************
cProc  EndSelection,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	bx,lpPntStruc
	and	[bx.SelStruc.GrabArea.SelecFlags],NOT SelectOn

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; AdjustInitEndPt - Adjust initial selection end point
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	YCoOrd,XCoOrd = (Y,X) point to be adjusted NOTE: NOT IN ANY RELATIVE COORD!!!!
; EXIT:
;	DX,AX = (Y,X) end point adjust down and to right for initial selection
; USES:
;	C Standard
; NOTE:
;	No alignment of this end point can be done as the CoOrd is not in any
;	form that the grabber can understand. Adjustment is purely to "add the
;	right value" to the CoOrd. This is primarily done so the initial move
;	of a keyboard selection endpoint results in a complete character being
;	selected. This routine simply assists in providing visual feedback
;	to the user that he now HAS a selction.
;
;*****************************************************************************
cProc  AdjustInitEndPt,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	YCoOrd
	ParmW	XCoOrd

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ;  
	mov	ax,XCoOrd
	mov	dx,YCoOrd
	mov	cx,[bx.DefFont.FontWid] ; Assume graphics, will move width of OEM font
	mov	si,[bx.DefFont.FontHgt] ; Assume graphics, will move height of OEM font
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4
	jae	short GotWidHgt
AIE_Text:
    ;
    ; Text, move by one char or line
    ;
        call    GetTextWidHgt   ; returns font width, height in CX and SI
GotWidHgt:
	add	ax,cx		; Right
	add	dx,si		; Down

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; KeySelection - Keyboard Selection
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	StartType == 0 if SHIFT key UP
;		  != 0 if SHIFT key DOWN
;	MFunc == 0 To right
;	      == 1 To Left
;	      == 2 Down
;	      == 3 Up
; EXIT:
;	DX,AX = (Y,X) screen CoOrd of new selection end point
;		Here "screen CoOrd" means a window client area position
;		corrected by the scroll bar positions. We don't care about
;		the scroll bars here, VMDOSAPP takes care of the correction.
;	NOTE: Selection structure in extended paint structure should not
;		be updated. A call to MakeSelctRect or BeginSelection
;		will follow.
; USES:
;	C standard
;
; Algorithm:
;     If SHIFT key UP(StartType == 0) return new start point;
;        if (LEFT Key)
;                return (X-DELTAx, Y);
;        else if (RIGHT Key)
;                return (X+DELTAx, Y);
;        else if (DOWN Key)
;                return (X, Y+DELTAy);
;        else if (UP Key)
;                return (X, Y-DELTAy);
;
;        where <X,Y> is the current start point
;        DELTAx DELTAy are the font width and height in text mode,
;                      are some appropriate value in graphics mode
;
;    If SHIFT key DOWN(StartType != 0) return new End Point;
;        Similar to above except <X,Y> is current end point.
;
;*****************************************************************************
cProc  KeySelection,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	StartType
	ParmW	Mfunc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ;
    ; Assume SHIFT key UP
    ;
	mov	ax,[bx.SelStruc.GrabArea.StartPointX]
	mov	dx,[bx.SelStruc.GrabArea.StartPointY]
	cmp	StartType,0
	jz	short GotPT		; Assumption correct
    ;
    ; SHIFT key is DOWN, start from previous end point
    ;
	mov	ax,[bx.SelStruc.GrabArea.EndPointX]
	mov	dx,[bx.SelStruc.GrabArea.EndPointY]
    ;
    ; Get Width in CX, Height in SI, depending on the mode
    ;
GotPT:
	mov	cx,32		; Assume graphics mode
	mov	si,[bx.DefFont.FontHgt]  ; will move height of OEM font
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4
	jae	short KS_GotWidHgt
    ;
    ; Text mode, move by one char or line
    ;
        call    GetTextWidHgt   ; returns font width, height in CX and SI
KS_GotWidHgt:
	cmp	Mfunc,0
	jnz	short TLeft
	add	ax,cx		; Right
	jmp	short CheckXY
TLeft:
	dec	Mfunc
	jnz	short TDown
	sub	ax,cx		; Left
	jmp	short CheckXY
TDown:
	dec	Mfunc
	jnz	short IsUp
	add	dx,si		; Down
	jmp	short CheckXY
IsUp:
	sub	dx,si		; Up
CheckXY:
	or	ax,ax
	jns	short KS_xOK    ; AX >= 0?
	xor	ax,ax           ; No, Make AX = 0
KS_xOK:
	or	dx,dx           ; DX >= 0?
	jns	short KS_yOK    ; No, Make DX = 0
	xor	dx,dx
KS_yOK:
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4
	jae	short KSGraphics
	call	AdjustSelTextMax        ; Adjust max for AX,DX - Text mode
	jmp	short KSDone
KSGraphics:
	call	AdjustSelGrxMax         ; Adjust max for AX,DX - Graphics mode
KSDone:
	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; SetSelRect - Using selection (StartPt,EndPt) set Selection Rect in PAINTSTRUC
;
; ENTRY: DS:BX -> EXTPAINTSTRUC
;        
; EXIT:  DS:[BX.SelStruc.SelctRect set
;
; USES:  AX, CX
;
;*****************************************************************************

SetSelRect proc near

	mov	ax,[bx.SelStruc.GrabArea.EndPointX]
	mov	cx,[bx.SelStruc.GrabArea.StartPointX]
	cmp	cx,ax			; This way?
	jle	short SSR1		; yes
	xchg	ax,cx			; no, other way
SSR1:
	mov	[bx.SelStruc.SelctSRect.rcLeft],cx
	mov	[bx.SelStruc.SelctSRect.rcRight],ax

	mov	ax,[bx.SelStruc.GrabArea.EndPointY]
	mov	cx,[bx.SelStruc.GrabArea.StartPointY]
	cmp	cx,ax			; This way?
	jle	short SSR2		; yes
	xchg	ax,cx			; no, other way
SSR2:
	mov	[bx.SelStruc.SelctSRect.rcTop],cx
	mov	[bx.SelStruc.SelctSRect.rcBottom],ax

        ret
SetSelRect endp

;*****************************************************************************
;
; MakeSelctRect - Set a new selection
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	YCoOrd,XCoOrd = (Y,X) screen CoOrd of new end point
;		(Here "screen CoOrd" means a window client area position
;		corrected by the scroll bar positions)
; EXIT:
;	[lpPntStruc.SelStruc.SelctSRect] Display rect in extended paint
;		selection structure set
;	AX == 0
;	    if no change was made to selection parameters
;		NOTE: [lpPntStruc.SelStruc.SelctSRect] MUST STILL BE SET in
;		this case!!!!
; USES:
;	C Standard
;
;*****************************************************************************
cProc  MakeSelctRect,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	YCoOrd
	ParmW	XCoOrd

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ;
	mov	dx,YCoOrd
	mov	ax,XCoOrd
	call	ScreenAdjust
	cmp	ax,[bx.SelStruc.GrabArea.EndPointX]
	jnz	short SaveFlags
	cmp	dx,[bx.SelStruc.GrabArea.EndPointY]
SaveFlags:
	pushf				; Save whether end point is different
	mov	[bx.SelStruc.GrabArea.EndPointX],ax
	mov	[bx.SelStruc.GrabArea.EndPointY],dx

        call    SetSelRect

	popf				; recover end point compare
	mov	ax,0			; DO NOT XOR!!!!!!
	jz	short MSRDone
	inc	ax
MSRDone:
	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; ConsSelecRec - Make display rectangle consistent with selection
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	[lpPntStruc.SelStruc.SelctSRect] Display rect in extended paint
;	    selection structure set
;
;	What we are doing here is simply making sure that SelctSRect is
;	a TRUE correct representation of the selection. Basically this
;	is where any of the selection alignment additions or subtractions
;	to the rectangle CoOrds get resolved.
;
; USES:
;	C standard
;
;*****************************************************************************
cProc  ConsSelecRec,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame
	lds	bx,lpPntStruc
        call    SetSelRect

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; InvertSelection - Invert selection so that all paints will paint selection
;                   inverted from its current state.
;
;  Note this may mostly be a NOP. In which case the only important thing 
;  is to set the return value
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	DX,AX = (Y,X) screen CoOrd of "active" selection endpoint
; USES:
;	ALL
;
;*****************************************************************************
cProc  InvertSelection,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	bx,lpPntStruc
	mov	ax,[bx.SelStruc.GrabArea.EndPointX]
	mov	dx,[bx.SelStruc.GrabArea.EndPointY]

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; GetSelectionSiz - Return size to render selection
;
; NOTE THIS IS NO LONGER AN EXPORT. It is only called internally to the
;	grabber.
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	wParam = parameter from VDD message (= -1 if VMDOSAPP origin)
;	lParam = parameter from VDD message (= 0  if VMDOSAPP origin) Event ID
; EXIT:
;	AX == DX == 0
;	    error
;	Else
;	    DX:AX is size in bytes to render selection
;		NOTE: For graphics renders, the rendering MUST be an EVEN
;			number of bytes wide! This is because Windows
;			Bitmaps are accessed with WORD string instructions.
; USES:
;	C standard
;
;*****************************************************************************
cProc  GetSelectionSiz,<NEAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	wParam
	ParmD	lParam

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
    ;
    ; Compute width and height
    ; NOTE: This code works for text and graphics because we know
    ;	byte alignment is maintained for graphics screens, and character
    ;	alignment is maintained for text screens (see ScreenAdjust)
    ;
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ;
	mov	ax,[bx.SelStruc.GrabArea.EndPointX]
	sub	ax,[bx.SelStruc.GrabArea.StartPointX]
	jns	short GotWid
	neg	ax
GotWid:
	mov	dx,[bx.SelStruc.GrabArea.EndPointY]
	sub	dx,[bx.SelStruc.GrabArea.StartPointY]
	jns	short GotHig
	neg	dx
GotHig:
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4
	jb	short TextSel
GraphicsSel:
IFDEF CPQ_PLASMA
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],VMode640x400    ; 640 x 400
	je	short NoDiv
	shr	dx,1			; Convert to display
NoDiv:
ENDIF
%OUT This piece of code correct? - Test it
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4
	jne	short NoDiv2
	shr	ax,1			; Convert to display
NoDiv2:
    ;
    ; Compute size of one plane
    ;
	add	ax,15			; Round up width to multiple of 16
	and	ax,1111111111110000B
	mov	cx,dx
	mul	cx			; DX:AX is number of pixels
	add	ax,7
	adc	dx,0
	mov	cx,8			; 8 pix per byte
	div	cx			; AX is # bytes for one plane
	xor	dx,dx
CheckSize:
	mov	bx,ax
	or	bx,dx
	jz	short NoSel
	jmp	short GSSDone

TextSel:
        call    GetTextWidHgt
    ;
    ;	NOTE ABOUT THIS. You will note that this code simply computes
    ;	    the width in chars of the selection rect, adds two for a CR LF,
    ;	    then multiplies this times the height in lines of the selection
    ;	    rect. The actual Rendering done by RenderRect MAY NOT USE ALL OF
    ;	    THIS SPACE. The reason is that the render code "optimizes out
    ;	    trailing spaces" on each line. To compute the TRUE selection size
    ;	    we would have to do the same thing here. WE DON'T because it is
    ;	    simply not worth the trouble. The MAX size of a text grab is:
    ;			(80*43)+(2*43)+1
    ;	    In worst case (grab of totally blank screen) this will get
    ;	    optimized by the render code down to:
    ;			(2*43)+1
    ;	    wasting (80*43)=3,440 bytes of the allocated GRAB buffer.
    ;
    ;	    This amount of space is just not worth the effort required to
    ;	    not include it in the allocation.
    ;
	push	dx
	xor	dx,dx
	add	ax,cx
	dec	ax
	div	cx			; AX is width in chars
	pop	dx
	or	ax,ax
	jz	short NoSel
	inc	ax			; CR
	inc	ax			; LF
ifdef	DBCS
;if in text mode, added two more bytes for DBCS characters:
; sssssDDDDsssssssDDDDDDDDsssssDDDD	might be tranformed to
;	^		^
; sssssDDDDsssssssDDDDDDDDsssssDDDD
;      ^		 ^		added one byte for each side
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],3	; for only text mode
	ja	@F
	inc	ax			; for DBCS char
	inc	ax
@@:
endif	;DBCS

	xchg	dx,ax
	push	dx
	xor	dx,dx
	add	ax,si
	dec	ax
	div	si			; AX is height in lines
	pop	dx
	or	ax,ax
	jz	short NoSel
	mov	cx,dx
	mul	cx			; DX:AX is size in bytes
	add	ax,1			; NUL terminated
	adc	dx,0
	jmp	short CheckSize

NoSel:
	xor	ax,ax
	mov	dx,ax
GSSDone:
	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; RenderSelection - Render selection into clipboard format
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	wParam = parameter from VDD message (= -1 if VMDOSAPP origin)
;	lParam = parameter from VDD message (= 0  if VMDOSAPP origin) Event ID
; EXIT:
;       if (DX < 0)
;               Error
;       else if (DX = 0)
;               No Selection
;       else if (DX > 0)
;               DX = format, (CF_OEMTEXT or CF_BITMAP)
;               AX = Handle, <Memory Handle or Bitmap Handle)
;
; USES:
;	C standard      
;
;*****************************************************************************
cProc  RenderSelection,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	wParam
	ParmD	lParam

	localV	FrameVars,StdGrbFrameSz
        localW  hMemory         ; Memory Handle for the render buffer
                                ; used for mono bitmaps or text renders

cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	or	GrbFlags,GrbFlags_DoingGrab
	lds	si,lpPntStruc
	cmp	wParam, -1
	jz	short RenderFromPaintState
    ;
    ; Reset the controller state bit so that we chuck the PAINT state
    ;	and get the GRAB state.
    ;
	btr	[si.EPStatusFlags],fVValidBit
	jmp	short RenderFromGrabState

RenderFromPaintState:
	or	GrbFlags,GrbFlags_DoingWindowSelGrab
RenderFromGrabState:
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ;
	test	[si.EPGrabDTA.CntrlSt.VDA_CGA_Flags],fVDA_V_ScOff
	jnz	RS_Error
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode]	; Get mode
	cmp	cl,4
	jb	RSText

	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn
	jz	RS_NoSelection

IFDEF CPQ_PLASMA
  	cmp	cl,VMode640x400		; check for modes 4, 5, 6 and 640 x 400
ELSE
  	cmp	cl,6    		; check for modes 4, 5, 6 
ENDIF        
        ja      short RS_Error
    ;
    ; Graphics (bitmap) render
    ;
RSGraphics:
	mov	bx,[si.SelStruc.GrabArea.StartPointX]
	mov	cx,[si.SelStruc.GrabArea.EndPointX]
	cmp	bx,cx			; This way?
	jbe	short SetLeftRight	; Yes
	xchg	bx,cx			; no, other way
SetLeftRight:
	mov	[si.SelStruc.GrabArea.Irect.rcLeft],bx
	mov	[si.SelStruc.GrabArea.Irect.rcRight],cx
	cmp	bx,cx			; zero width?
	jz	RS_NoSelection	        ; Yes

	mov	bx,[si.SelStruc.GrabArea.StartPointY]
	mov	cx,[si.SelStruc.GrabArea.EndPointY]
	cmp	bx,cx			; This way?
	jbe	short SetTopBottom	; Yes
	xchg	bx,cx			; no, other way
SetTopBottom:
	mov	[si.SelStruc.GrabArea.Irect.rcTop],bx
	mov	[si.SelStruc.GrabArea.Irect.rcBottom],cx
	cmp	bx,cx			; zero height?
	jz	short RS_NoSelection
    ;
    ; Get a Display Context
    ;
	cCall	GetDC,<[si.WindHand]>
	or	ax,ax		; Got a Valid hDC?
	jz	short RS_Error  ; No
	mov	[si.Pstruct.psHdc], ax

        call    GetVidSel
	mov	bx,si
	lea	si,[si.SelStruc.GrabArea.Irect]
        call    RenderRectGrfx
        pushf
        push    ax
	lds	si,lpPntStruc
        call    ClearVidSel
        pop     ax
        popf
	jc	short RS_Error

        push    ax                      ; preserve Bitmap Handle
	cCall	ReleaseDC, <[si.WindHand], [si.Pstruct.psHdc]>
        pop     ax

        mov     dx, CF_BITMAP              ; AX has the handle
	jmp	RS_Done
    ;
    ; Error in Rendering.
    ;
RS_Error:
IFDEF   DEBUG
	mov	bx,si
	mov	esi,codeOffset RendErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short REContinue

RendErr db	"Grabber Got rendering error",0

REContinue:
ENDIF
        mov     dx, -1
	jmp	RS_Done

   ;
   ; No Selection exists to render
   ;
RS_NoSelection:         
        mov     dx, 0
	jmp	RS_Done
    ;
    ; Text Render
    ;
RSText:
	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn
	jz	RS_NoSelection		; Done
	lea	bx,[si.PTVDRect]
	push	ax
	push	dx
	call	ComputeSelTextRect
	mov	bx,dx			; Save dimen of selection
	pop	dx
	pop	ax
	or	bh,bh			; NUL selec?
	jz	RS_NoSelection 	; Yes
	or	bl,bl			; NUL selec?
	jz	RS_NoSelection 	; Yes
	mov	bx,si
    ;
    ; get size of the render buffer 
    ;
	cCall	GetSelectionSiz,<lpPntStruc,wParam,lParam> 
	mov	cx,ax
	or	cx,dx
	jz	RS_Error
    ;
    ; Allocate memory for the render buffer
    ;
	regptr	ddSize,dx,ax
	cCall	GlobalAlloc,<GMEM_MOVEABLE+GMEM_ZEROINIT+GMEM_DDESHARE,ddSize>
	or	ax,ax
	jz	RS_Error
	mov	hMemory,ax
	cCall	GlobalLock,<ax>
	mov	es,dx
	mov	di,ax		; es:di points at save area (mem just alloced)
        or      dx, ax
	jz	RS_Error

        push    es
        push    edi
        call    GetVidSel
        pop     edi
        pop     es

        mov     bx, si
	lea	si,[si.SelStruc.GrabArea.Irect]
        call    RenderRectText
        pushf                           ; save return value 
	lds	si,lpPntStruc
        call    ClearVidSel
	cCall	GlobalUnLock,<hMemory>
        popf
	jc	RS_Error                ; RenderRectText failed?
        mov     ax, hMemory             ; return Memory Handle in AX
	mov     dx, CF_OEMTEXT	        ; set return value
RS_Done:
	pop	edi
	pop	esi
cEnd

ifdef	DBCS
;*****************************************************************************
;
; AdjustDBCSRect - Adjust rectangle for DBCS text. 
;
; ENTRY:
;	lpSlctRect - Points to a RECT data that specifies selected region.
;	lpDBCSRect - Points to a RECT data that returns adjusted region.
;		     
; EXIT:
;	AX = TRUE Continue
;	   = FALSE End, no more region need to be inverted
; USES:
;	C standard
;
;*****************************************************************************
cProc  AdjustDBCSRect,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmD	lpSlctRect
	ParmD	lpDBCSRect

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUCT structure
    ;
;Only support mode #3
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],3	   ; Text Mode?
	jbe	short ADR0	; if (Mode == 3) {

	lds	si,lpSlctRect	;    lpDBCSRect = lpSlctRect;
ADR3:
	les	di,lpDBCSRect	;    return (0);
	mov	cx,4		; }
	rep	movsw
	xor	ax,ax
	jmp	short ADRX
ADR0:
	lds	si,lpSlctRect
	mov	ax,[si.rcBottom]
	cmp	ax,[si.rcTop]
	jz	short ADR3
	mov	ax,[si.rcRight]
	cmp	ax,[si.rcLeft]
	jz	short ADR3

	mov	cx,[bx.AltFnt1.FontWid]
	mov	si,[bx.AltFnt1.FontHgt]
;	 call	 GetTextWidHgt	 ; GetTextWidHgt();
	mov	dx,si		; if (lpDBCSRect.rcBottom == 0)
	lds	si,lpSlctRect	;    lpDBCSRect.rcTop = lpSlctRect.rcTop;
	lds	di,lpDBCSRect	; else
	cmp	[di.rcBottom],0	;    lpDBCSRect.rcTop += Hgt;
	jnz	short ADR1
	mov	ax,[si.rcTop]
	mov	[di.rcTop],ax
	jmp	short ADR2
ADR1:
	add	[di.rcTop],dx
ADR2:
	mov	ax,[di.rcTop]	; lpDBCSRect.rcBottom = lpDBCSRect.rcTop+Hgt;
	add	ax,dx
	mov	[di.rcBottom],ax
	mov	ax,[si.rcLeft]	; lpDBCSRect.rcLeft = lpSlctRect.rcLeft;
	mov	[di.rcLeft],ax	
	mov	cx,[si.rcRight]	; lpDBCSRect.rcRight = lpSlctRect.rcRight;
	mov	[di.rcRight],cx	; AdjustDBCSBound();
	mov	dx,[di.rcTop]
	call	AdjustDBCSBound ;He will adjust left and right bound
	mov	[di.rcLeft],ax
	mov	[di.rcRight],cx
	mov	ax,[si.rcBottom] ; if (lpDBCSRect.rcBottom >=
	cmp	[di.rcBottom],ax ;		lpSlctRect.rcBottom)
	mov	ax,0		 ;    return(0);
	jge	ADRX		 ; else
	not	ax		 ;    return(-1);
ADRX:
	pop	edi
	pop	esi
cEnd

endif	; DBCS

;*****************************************************************************
;
; SetPaintFnt - Set the paint font
;
; This routine Sets the font for painting in the Extended paint structure
;	so that WINOLDAP can compute the paint rectangle for us on PaintScreen
;	calls. This is called right before a call to PaintScreen. It is also
;	called right before a call to UpdateScreen.
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	lpWidFullScr	word pointer for width return
;	lpHeightFullScr word pointer for height return
; EXIT:
;	FntHgt and FntWid values in EXTPAINTSTRUC set
;	NOTE: Values set to 0 if graphics screen!!!!!
;	[lpWidFullScr] = Width of full screen in pix (text or Graphics)
;	[lpHeightFullScr] = Height of full screen in pix (text or Graphics)
;	DX is height of full screen in scan lines if Graphics
;				   in text lines if Text
;	AX is width of full screen in Pix if Graphics
;				   in chars if Text
; USES:
;	C Standard
;
;*****************************************************************************
cProc  SetPaintFnt,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmD	lpWidFullScr
	ParmD	lpHeightFullScr

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ;
	mov	dl,[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode]
	cmp	dl,4
	jb	short FontYes
	xor	ax,ax
	mov	[bx.FntHgt],ax
	mov	[bx.FntWid],ax
	mov	cx,GrxBitWid
	mov	bx,GrxBitHeight
	mov	dx,bx
	mov	ax,cx
	jmp	short SPDone

FontYes:
    ;
    ; Text, set font parms
    ;
        call    GetTextWidHgt
	mov	[bx.FntWid],cx
	mov	[bx.FntHgt],si
	mov	ax,25
	push	dx
	mul	si
	pop	dx
	mov	si,bx
	mov	bx,ax			; BX is height in PIX of full screen

	mov	ax,80			; Assume 80 cols
	cmp	dl,1
	ja	short TxtFW
	shr	ax,1			; 40 cols
TxtFW:
	push	ax			; Save total columns
	mul	cx
	mov	cx,ax			; CX is width in PIX of full screen
	pop	ax
	mov	dx,25
SPDone:
	les	di,lpWidFullScr
	mov	word ptr es:[di],cx
	les	di,lpHeightFullScr
	mov	word ptr es:[di],bx

	pop	edi
	pop	esi
cEnd

ProfileSect	db	"386ENH",0
ProfileFile	db	"SYSTEM.INI",0

;*
;
; The following table defines the 6 extra fonts we want VMDOSAPP to load for
; us (actually only need two extra, but the EXTPAINTSTRUC has room for six).

IFDEF CPQ_PLASMA

FontList	Label	byte
F1	db	"EGA80WOA.FON",0	; Name		80 column font
.erre ($-F1) EQ 13
	dw	12			; Height
	dw	8			; Width

F2	db	"EGA40WOA.FON",0	; Name		40 column font
.erre ($-F2) EQ 13
	dw	12			; Height
	dw	16			; Width

	db	13 DUP (0)		; Unused
	dw	0
	dw	0

	db	13 DUP (0)		; Unused
	dw	0
	dw	0

	db	13 DUP (0)		; Unused
	dw	0
	dw	0

	db	13 DUP (0)		; Unused
	dw	0
	dw	0

ELSE

FontList	Label	byte
F1	db	"CGA80WOA.FON",0	; Name		80 column font
.erre ($-F1) EQ 13
	dw	8			; Height
	dw	8			; Width

F2	db	"CGA40WOA.FON",0	; Name		40 column font
.erre ($-F2) EQ 13
	dw	8			; Height
	dw	16			; Width

	db	13 DUP (0)		; Unused
	dw	0
	dw	0

	db	13 DUP (0)		; Unused
	dw	0
	dw	0

	db	13 DUP (0)		; Unused
	dw	0
	dw	0

	db	13 DUP (0)		; Unused
	dw	0
	dw	0
ENDIF

FListSize equ	 $-FontList

;*****************************************************************************
;
; GetFontList - Return pointer to font list
;
; This routine returns a pointer to the list of extra fonts we want loaded
;
; ENTRY:
;	lpFontBuf -> Buffer for font info
; EXIT:
;	Font Buffer filled in
; USES:
;	C standard
;
;*****************************************************************************
cProc  GetFontList,<FAR,PUBLIC>

	ParmD	lpFontBuf

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	push	cs
	pop	ds
	mov	si,codeOffset FontList
	les	di,lpFontBuf
	mov	cx,FListSize
	cld
	rep	movsb

	lds	di,lpFontBuf
	mov	cx,2
	regptr	pFnt,ds,di
GetProLoop:
	mov	ax,codeOffset ProfileSect
	regptr	pSect,cs,ax
	mov	bx,codeOffset ProfileFile
	regptr	pFnm,cs,bx
	push	cx
	cCall	GetPrivateProfileString,<pSect,pFnt,pFnt,pFnt,13,pFnm>
	pop	cx
	add	di,17
	loop	GetProLoop

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; GrbUnlockApp - Unlock the application (allow it to run)
;
;     This routine undoes the implied LOCK OUT of app changes done by
;	GetDisplayUpd
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	None
; USES:
;	C Standard
;
;*****************************************************************************
cProc  GrbUnlockApp,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc

	mov	ax,GRB_Unlock_APP
	mov	ebx,[si.ThisVMHand]
        call    [si.GGrbArea.VddApiProcAdr]

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; GetDisplayUpd - Call the VDD to get a display update
;
;     This routine gets the update (if any) and stores it in the Paint struct
;
;     NOTE: This call "locks" the app (prevents furthur changes from occuring).
;	    The app starts again via:
;
;	    o Call to UpdateScreen
;
;	    o Call to PaintScreen
;
;	    o Call to GrbUnlockApp
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	wParam = parameter from VDD message (= -1 if VMDOSAPP origin)
;	lParam = parameter from VDD message (= 0  if VMDOSAPP origin) Event ID
; EXIT:
;	AX = Display update flags (see grabpnt.inc for fDisp_ flags
; USES:
;	C Standard
;
;*****************************************************************************
cProc  GetDisplayUpd,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	wParam
	ParmD	lParam

	localV	FrameVars,StdGrbFrameSz
cBegin
	push	esi
	push	edi

	InitBasicFrame
    ;
    ; NOTE THAT THIS ROUTINE MUST NOT CALL ClearVidSel. This will open a
    ;	race window in the call from UpdateScreen.
    ;
	lds	si,lpPntStruc
	push	ds
	pop	es

	mov	ax,GRB_Get_Mod
	mov	ebx,[si.ThisVMHand]
IFDEF   DEBUG   
        mov     cx,VDD_MOD_MAX                  ; Size of EPDispMod structure
ENDIF
	lea	di,[si.EPGrabDTA.DispMod]
	movzx	edi,di
        call    [si.GGrbArea.VddApiProcAdr]
IFDEF   DEBUG
        or      cx,cx
	jnz	short GDUContinue2
	push	si
	push	bx
	push	dx
	mov	bx,si
	mov	esi,codeOffset GetModErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short GDUContinue1

GetModErr db	  "Grabber Got error trying to get mods",0

GDUContinue1:
	pop	dx
	pop	bx
	pop	si
GDUContinue2:
ENDIF
	mov	ax,[di.VDD_Mod_Flag]
	test	ax,fVDD_M_Err
	jnz	short NoCtrlChng
	test	ax,fVDD_M_Ctlr+fVDD_M_Curs
	jz	short NoCtrlChng
	btr	[si.EPStatusFlags],fVValidBit
	call	CheckCtrlState
NoCtrlChng:
	and	ax,fDisp_Ctlr+fDisp_VRAM+fDisp_Curs

.erre	fVDD_M_Ctlr	    EQ	 fDisp_Ctlr
.erre	fVDD_M_VRAM	    EQ	 fDisp_VRAM
.erre	fVDD_M_Curs	    EQ	 fDisp_Curs

	test	[si.EPGrabDTA.CntrlSt.VDA_CGA_Flags],fVDA_V_HCurTrk
	jz	short NoHTrk
	or	ax,fDisp_HCurTrack
NoHTrk:
	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; UpdateScreen - Update changed portions of screen indicated
;
;   NOTE: We know SetPaintFnt has been called
;
; ENTRY:
;	lpPntStruc Extended paint structure
;	wParam = parameter from VDD message
;	lParam = parameter from VDD message EVENT ID
; EXIT:
;	AX == 1
;	    Screen Painted
;	AX == 0
;	    Screen not painted, probably low Windows memory problem
; USES:
;	C Standard
; NOTE:
;	This routine may set the fGrbProb bit in EPStatusFlags in the
;	Extended paint structure to indicate that this app is in a mode
;	which cannot be rendered into a Window. This routine must return with
;	AX == 1 in this case.
;
; Alg:
;       call    GetVidSel - freeze the app
;       call    GetDisplayUpd   - get latest controller state
;       
;       if (Change in controller state)
;          paint whole client area and do cursor
;       else if (no video Ram change)
;          do cursor
;       else    {    /* video ram change */
;          if (Rect List and Grx mode)
;              for each rect {
;                   convert rect to display co-ords
;                   call PrepPnt, ModeGrfx
;               }
;          else if (Rect List and Text mode)
;               for (each rect)  {
;                  if (Scroll && no outstanding updates for rects)
;                      Call Textscroll
;                      continue;
;                  else if (Scroll and outstanding updates exist)
;                      Convert the client area to "screen" coords
;                      paint whole client area - call PrepPnt,modeText
;                      exit
;                  else if (No scroll)
;                      Convert Rect in Cols & Rows to pixel CoOrds
;                      call PrepPnt,ModeText     
;                      continue;
;               }
;          do cursor;
;       } 
;*****************************************************************************

cProc  UpdateScreen,<FAR,PUBLIC>

	ParmD	lpPntStruc
	ParmW	wParam
	ParmD	lParam

	localV	FrameVars,StdGrbFrameSz
	LocalW	RecBottom
	LocalW	RecRight
	LocalW	RecTop
	LocalW	RecLeft
	LocalV	UpdateRect,(4*2)
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	GetVidSel		; Get the memory and FREEZE the app
    ;
    ; Get the display updates again with the app FROZEN. This also does
    ; CheckCtrlState for us.
    ;
	cCall	GetDisplayUpd,<lpPntStruc,wParam,lParam>
	lds	si,lpPntStruc
	test	[si.EPGrabDTA.DispMod.VDD_Mod_Flag],fVDD_M_Err
        mov     ax,0                    ; Assume error, (Do Not XOR!!!)
	jnz	ClearModf
	test	[si.EPGrabDTA.DispMod.VDD_Mod_Flag],fVDD_M_Ctlr
	jz	short DoRects    	; Controller state is unchanged

    ;
    ; Controller state has changed, paint whole client area. 
    ; Paint rect is set to whole client area, so we're ready to paint
    ;
        call    PaintClientRect
	jmp	JustCursor		; Check on cursor

    ; 
    ; Controller state is unchanged but video RAM might have changed
    ; 
DoRects:
    ;
    ; Re-paint indicated rects of screen
    ;
	test	[si.EPGrabDTA.DispMod.VDD_Mod_Flag],fVDD_M_VRAM
	jz	JustCursor		; Just Cursor change, carry clear if jmp
IFDEF DEBUG
	mov	ax,[si.EPGrabDTA.DispMod.VDD_Mod_Flag]
	and	ax,fVDD_M_Type
	cmp	ax,fVDD_M_Type_Rect     ; Rectangle updates?
	jz	short NoStallBM
	push	si
	push	bx
	push	dx
	mov	bx,si
	mov	esi,codeOffset PageErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short USContinue1

PageErr db	"Grabber expecting fVDD_M_Type_Rect got AX",0

USContinue1:
	pop	dx
	pop	bx
	pop	si
NoStallBM:
ENDIF
        call    SavePaintRect           ; save the client rect on the stack

	lea	di,[si.EPGrabDTA.DispMod.VDD_Mod_List]
	mov	cx,[si.EPGrabDTA.DispMod.VDD_Mod_Count]
IFDEF DEBUG
	or	cx,cx
	jnz	short NoStallRCT
	push	si
	push	dx
	push	bx
	mov	bx,si
	mov	esi,codeOffset CountErr2
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short USContinue3

CountErr2 db	  "Grabber mod count = 0 Rect type",0

USContinue3:
	pop	bx
	pop	dx
	pop	si
NoStallRCT:
ENDIF
	or	cx,cx
	jz	JustCursor	  ; Hmmm.... Bit lied, carry clear if jmp
UpdtRect:
	push	cx
	push	di
	cmp	[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4
	jb	short TextUpdate
GrxUpdate:
IFDEF CPQ_PLASMA
	cmp	[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4
	je	short DoGAdjMode4
	cmp	[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode],6
	je	short DoGAdjMode6
        jmp     short NoGAdj
DoGAdjMode4:
	shl	[di.rcLeft],1		  ; Convert to display
	shl	[di.rcRight],1
DoGAdjMode6:
	shl	[di.rcTop],1		  ; Convert to display
	shl	[di.rcBottom],1
NoGAdj:
ELSE
	cmp	[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4
	je	short DoGAdj
        jmp     short NoGAdj
DoGAdj:
	shl	[di.rcLeft],1		  ; Convert to display
	shl	[di.rcRight],1
NoGAdj:
ENDIF
	call	PrepPnt
	jc	NextRect
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode]
        xor     cx,cx
	call	ModeGrfx
        jc      UpdPntErr
	jmp	NextRect

TextUpdate:
	test	[di.rcLeft],1000000000000000B	  ; Scroll Rect?
	jz	short NormRect			; No, normal
ScrollUpdate:
    ;
    ; We must check here for Updates. If we have any we cannot do the scroll
    ;	because for Windows we must paint these updates before doing the Scroll.
    ;	The problem with this is that the video memory is now in its
    ;	"post scroll" state, so these pre scroll paints we might do would
    ;	paint the wrong stuff and the screen will get all messed up. What we
    ;	will do is paint the whole screen (like for a controller change).
    ;	Since we do the whole screen, we can ignore the rest of the updates.
    ;
	lea	ax,UpdateRect
	push	[si.WindHand]		; hWnd
	push	ss			; lpRect
	push	ax
	push	0			; bErase
	cCall	GetUpdateRect
	or	ax,ax			; Any updates outstanding?
	jnz	short NoScrollx 	; Yes
	call	TextScroll
	jmp	short NextRect

NoScrollx:
	pop	di
	pop	cx
	push	1			; Make this the last rect
	push	di
    ;
    ; Reset this rect to whole client area
    ;
	mov	ax,RecBottom
	add	ax,[si.RowOrg]
	mov	[di.rcBottom],ax
	mov	ax,RecRight
	add	ax,[si.ColOrg]
	mov	[di.rcRight],ax
	mov	ax,RecTop
	add	ax,[si.RowOrg]
	mov	[di.rcTop],ax
	mov	ax,RecLeft
	add	ax,[si.ColOrg]
	mov	[di.rcLeft],ax
	jmp	short DoTextRect

NormRect:
    ;
    ; Convert Rect in Cols & Rows to pixel CoOrds
    ;
	mov	cx,[si.FntWid]
	mov	ax,[di.rcLeft]
	mul	cx
	mov	[di.rcLeft],ax
	mov	ax,[di.rcRight]
	mul	cx
	mov	[di.rcRight],ax
	mov	cx,[si.FntHgt]
	mov	ax,[di.rcTop]
	mul	cx
	mov	[di.rcTop],ax
	mov	ax,[di.rcBottom]
	mul	cx
	mov	[di.rcBottom],ax
DoTextRect:
	call	PrepPnt
	jc	short NextRect
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode]
	call	ModeText
	jc	short UpdPntErr
NextRect:
	pop	di
	add	di,SIZE RECT
	pop	cx
	dec	cx
	jnz	UpdtRect

	clc				; All ok
JustCursor:
	pushf				; Save carry setting
	bt	[si.EPStatusFlags],fFocusBit
	jnc	short SetReturnAX		; Don't have focus, no cursor
	bt	[si.EPStatusFlags],fSelectBit
	jc	short SetReturnAX		; In Select mode, don't do cursor
	call	CursorPos		; Get new cursor
	inc	dx
	jz	short NoCursor		; Cursor gone
	dec	dx
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode]
	cmp	cl,[si.EPGrabDTA.CurCursMode]	; Change in cursor mode?
	jnz	short MkNCur		; Yes
	mov	cx,[si.EPGrabDTA.CntrlSt.VDA_CGA_CurEnd]
	cmp	cx,[si.EPGrabDTA.CurCursEnd]	; Change in cursor shape?
	jnz	short MkNCur		; Yes
	mov	cx,[si.EPGrabDTA.CntrlSt.VDA_CGA_CurBeg]
	cmp	cx,[si.EPGrabDTA.CurCursBeg]	; Change in cursor shape?
	jz	short SetCurPos 	; No, just change in position
MkNCur:
	push	ax
	push	dx
	cCall	DestroyCaret
	pop	dx
	pop	ax
	call	MakeNewCursor		; Make new cursor
	jmp	short SetReturnAX

UpdPntErr:
	add	sp,4			; Discard saved DI and CX
	stc
	jmp	short JustCursor

NoCursor:
	mov	[si.EPGrabDTA.CurCursEnd],ax
	mov	[si.EPGrabDTA.CurCursBeg],ax
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode]
	mov	[si.EPGrabDTA.CurCursMode],cl
	cCall	DestroyCaret
	jmp	short SetReturnAX

SetCurPos:
	sub	ax,[si.ColOrg]	; Org onto display
	push	ax		; X for call to SetCaretPos
	sub	dx,[si.RowOrg]	; Org onto display
	push	dx		; Y for call to SetCaretPos
    ;
    ; Position Cursor
    ;
	cCall	SetCaretPos
SetReturnAX:
	popf
	mov	ax,0		; DO NOT XOR!!!!
	jc	short ClearModf
	inc	ax
ClearModf:
	push	ax		; Save return
    ;
    ; Clear off the MODs, app still "frozen"
    ;
	mov	ax,GRB_Clear_Mod
	mov	ebx,[si.ThisVMHand]
        call    [si.GGrbArea.VddApiProcAdr]
    ;
    ; Unfreeze the app
    ;
	call	ClearVidSel
	pop	ax		; Recover return

	pop	edi
	pop	esi
cEnd

;****************************************************************************
; 
; PaintClientRect - Calls the ModeGrfx/ModeText depending on mode
;  
; ENTRY: DS:SI -> EXTPAINTSTRUC
;
; EXIT: returns CF set by paint code
;
; USES:	ALL but DS,SI,BP
;
;****************************************************************************
PaintClientRect proc near

	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode]
	cmp	ah,4
	jb	short PCText
        xor     cx,cx
	call	ModeGrfx
	jmp	short PCDone
PCText:
	call	ModeText
PCDone:
        ret
PaintClientRect endp

;****************************************************************************
;
; SavePaintRect - save the PSrcPaint rect in EXTPAINTSTRUC on stack
;
; ENTRY: DS:SI -> EXTPAINTSTRUC
;
; EXIT: Paint rect copied to stack
;
; USES: BX,ES,DI
;
; Note: Called only from UpdateScreen where RectLeft is defined
;
;****************************************************************************

SavePaintRect proc near 
    ; 
    ; Save the client rect on the stack
    ; 
	mov	bx,si
	push	ss
	pop	es
	lea	di,RecLeft
	lea	si,[bx.Pstruct.PSrcPaint]
	cld
	movsw				
	movsw
	movsw
	movsw
	mov	si,bx
        ret
SavePaintRect endp

;*****************************************************************************
;
; TextScroll - Do a re-paint of the indicated scroll region
;
; ENTRY:
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	DS:DI -> ScrollRect structure
;	SS:BP -> Stack frame set at DoRects
; EXIT:
;	Screen Painted
; USES:
;	ALL but DS,SI,DI,BP
;
; Alg:
;     - Org the scroll rect onto the window client area.
;       AX = ScrTop*FontHgt - Top in pixels
;       BX = ScrBot*FontHgt - Bot in pixels
;       BX =- RowOrg
;       AX =- RowOrg
;       If (BX <= 0 || AX >= RecBottom || AX >= BX)
;               Offscreen
;       Ensure AX >= RecTop, BX <= RecBottom
;
;     - Build client area rect on screen
;               Left, Right = RecLeft, RecRight
;               Top, Bottom = AX, BX
;
;     - If Sel exists, uninvert before scroll
;       First Org the Sel onto window display
;
;       AX = StartPointX, CX = EndPointX
;       Ensure AX <= CX
;       if (AX < ColOrg)
;               AX = RecLeft;
;       else
;               AX =- ColOrg;
;       Ensure RecLeft <= AX <= RecRight;
;
;       Similarly for CX;
;       if ( AX >= CX)  Offscreen
;
;       Similarly for StartPointY, EndPointY
;               
;       Using these new co-ordinates set Rect on stack
;       Invert the Sel rect;
;     - ScrollWindow;
;       Invert the Sel Rect;
;       clean the stack and call UpdateWindow;
;
;*****************************************************************************
                
TextScroll proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
IFDEF DEBUG
    public  stallBS
	test	[di.ScrFlgs],Scr_M_FullWid
	jnz	Short NostallBS
stallBS:
	push	si
	mov	bx,si
	mov	esi,codeOffset WidErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	pop	si
	jmp	short TSContinue

WidErr	db	"Grabber problem with text scroll update",0

NostallBS:
	test	[di.ScrFlgs],Scr_M_Up
	jz	stallBS
TSContinue:
ENDIF

    ;
    ; First org the scroll rectangle onto the Window Client area
    ;
	mov	cx,[si.FntHgt]
	xor	bx,bx
	mov	ax,word ptr [di.ScrBot] ; AL = bottom, AH = Top
	mov	bl,ah
	xor	ah,ah			; BX = Top, AX = Bottom
	mul	cx
	xchg	bx,ax
	mul	cx			; BX = Bottom in PIX, AX = Top in PIX
    ;
    ; Org the Top and Bottom boundaries onto the display and
    ;	adjust them to client area
    ;
	sub	bx,[si.RowOrg]
	jbe	NulScr			; Scroll is off screen to Top, ignore
	sub	ax,[si.RowOrg]
	jnc	short NoAdT
	mov	ax,RecTop		; Limit to Top of client area
NoAdT:
	cmp	ax,RecBottom		; Bottom edge of client area
	jae	NulScr			; Scroll is off screen to bottom, ignore
	cmp	bx,RecBottom
	jbe	short NoAdB
	mov	bx,RecBottom		; Limit to Bottom of client area
NoAdB:
	cmp	ax,bx
	jae	NulScr			; Top is >= bottom
    ;
    ; Build Client area scroll rectangle
    ;
	sub	sp,(SIZE Rect) * 2
	mov	dx,bx
	mov	bx,sp
	mov	[bx.rcTop],ax
	mov	[bx.rcBottom],dx
	mov	ax,RecRight		; Is full width so left and right are
	mov	[bx.rcRight],ax 	  ;   of clent area
	mov	ax,RecLeft
	mov	[bx.rcLeft],ax

	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn ; Selection?
	jz	NoSelX			; No, done
    ;
    ; We must uninvert the selection rect before the scroll
    ;
    ; But first we must ORG the Selection rect onto the Window display
    ;
	mov	ax,[si.SelStruc.GrabArea.StartPointX]
	mov	cx,[si.SelStruc.GrabArea.EndPointX]
	cmp	ax,cx			; This way?
	jbe	short OkIX8		; Yup
	xchg	ax,cx			; Nope, other way
OkIX8:
	sub	ax,[si.ColOrg]
	jnc	short OkLX8
	mov	ax,RecLeft
OkLX8:
	cmp	ax,RecRight
	jbe	short OkLX82
	mov	ax,RecRight
OkLX82:
	cmp	ax,RecLeft
	jae	short OkLX83
	mov	ax,RecLeft
OkLX83:
	sub	cx,[si.ColOrg]
	jnc	short OkRX8
	mov	cx,RecLeft
OkRX8:
	cmp	cx,RecRight
	jbe	short OkRX82
	mov	cx,RecRight
OkRX82:
	cmp	cx,RecLeft
	jae	short OkRX83
	mov	cx,RecLeft
OkRX83:
	cmp	ax,cx
	jae	short NoSelX		; Selection rect is off screen
	mov	[bx.(SIZE rect).rcLeft],ax
	mov	[bx.(SIZE rect).rcRight],cx

	mov	ax,[si.SelStruc.GrabArea.StartPointY]
	mov	cx,[si.SelStruc.GrabArea.EndPointY]
	cmp	ax,cx			; This way?
	jbe	short OkIY8		; Yup
	xchg	ax,cx			; Nope, other way
OkIY8:
	sub	ax,[si.RowOrg]
	jnc	short OkTY8
	mov	ax,RecTop
OkTY8:
	cmp	ax,RecBottom
	jbe	short OkTY82
	mov	ax,RecBottom
OkTY82:
	cmp	ax,RecTop
	jae	short OkTY83
	mov	ax,RecTop
OkTY83:
	sub	cx,[si.RowOrg]
	jnc	short OkBY8
	mov	cx,RecTop
OkBY8:
	cmp	cx,RecBottom
	jbe	short OkBY82
	mov	cx,RecBottom
OkBY82:
	cmp	cx,RecTop
	jae	short OkBY83
	mov	cx,RecTop
OkBY83:
	cmp	ax,cx
	jae	short NoSelX		; Selection rect is off screen
	mov	[bx.(SIZE rect).rcTop],ax
	mov	[bx.(SIZE rect).rcBottom],cx
	push	bx
	add	bx,SIZE rect		; Poiint to selection rect
    ;
    ; Uninvert the selection rect
    ;
	push	[si.Pstruct.psHdc]	  ; hDC
	push	ss
	push	bx			; lpRect
	cCall	InvertRect
NoSelXP:
	pop	bx
NoSelX:
    ;
    ; Scroll the Window
    ;
	xor	ax,ax
	mov	cx,[si.FntHgt]
	mov	al,[di.ScrCnt]
	mul	cx			; AX is height of scroll in Pix
	neg	ax			; Scroll up is negative
	push	bx

	push	[si.WindHand]		; hWnd
	push	0			; XAmount
	push	ax			; YAmount
	push	ss
	push	bx			; lpRect
	push	ss
	push	bx			; lpClipRect
	cCall	ScrollWindow

	pop	bx
    ;
    ; Turn the selection back on by Inverting it if needed
    ;
	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn ; Selection?
	jz	short NoSelX2		; No, done
	add	bx,SIZE rect		; Point to selection rect
	push	[si.Pstruct.psHdc]	  ; hDC
	push	ss
	push	bx			; lpRect
	cCall	InvertRect
NoSelX2:
	add	sp,(SIZE Rect) * 2
    ;***********************************************************************
    ;
    ; A word of warning about the following call!!!!!!!!!!
    ;  This call causes us to re-enter the grabber to do a SetPaintFont and
    ;  a PaintScreen to paint the lines at the bottom of the scroll.
    ;  We must be re-entrant!
    ;
    ;  The paint code had better not muck up [si.EPGrabDTA.DispMod]!
    ;  We must take care of any areas of the Extended Paint struct that get
    ;	   mucked by the paint call!!!!!!!
    ;
    ;***********************************************************************

    ;***********************************************************************
    ; Note: 
    ;      The UpdateWindow call results in a call to paintscreen. Before 
    ; winoldap calls paintscreen, it queries the VDD for any updates. If we
    ; do not clear the modfs now, the VDD will report again the existing modfs.
    ; This will result in winoldap painting the entire screen, making our scrolling
    ; effort redundant. With the modfs cleared, winoldap will not see any pending 
    ; updates and hence will paint only the empty portions of the scrolled area.
    ;
    ; Also note that during this call we do not want the rest of the 
    ; VDD_Mod_List trampled upon. Hence GetDisplayUpd should not clear the modf
    ; area before calling VDD.
    ;
    ;***********************************************************************
    ;
    ; Clear off the MODs, app still "frozen"
    ;
	mov	ax,GRB_Clear_Mod
	mov	ebx,[si.ThisVMHand]
        call    [si.GGrbArea.VddApiProcAdr]

	push	[si.Pstruct.psHdc]	  ; Gets zapped by paint code

	push	[si.WindHand]		; hWnd
	cCall	UpdateWindow

	pop	[si.Pstruct.psHdc]
NulScr:
	ret

TextScroll endp

;*****************************************************************************
;
; PrepPnt - Prepare paint rects
;
; ENTRY:
;	DS:SI -> Extended Paint structure
;	ES:DI -> Input screen CoOrd rect
;	SS:BP -> Stack frame set at DoRects
; EXIT:
;	Carry Clear
;	    Extended paint structure variables set up to paint this rect
;	Carry Set
;	    Rectangle is off the screen
; USES:
;	ALL but DS,SI,BP
;
;
; Alg:
;       Let given rect be L,R,T,B- Left etc
;        L =- ColOrg, R =- ColOrg
;        If (R <= 0 || L > RecRight)
;                It is Offscreen
;        Ensure L >=0 R <= RecRight
;        
;        Similarly for T and B using RowOrg
;
;      - Build params in ext paint structure
;	 PSrcPaint.rcRight = R; .rcLeft = L; .rcTop = T; .rcBottom = B;
;        (Client area CoOrds)
;      - In PGVDRect,
;		 rcLeft = L + ColOrg
;		 rcRight = R + ColOrg
;		 rcTop = T + RowOrg
;		 rcBottom = B + RowOrg
;        "Screen" CoOrds (in pixels)
;        
;      - if (Text mode)
;            PTVDRect - "Screen" CoOrds in Cols & Rows
;		 PTVDRect.rcLeft = PGVDRect.rcLeft/FontWid - first col of repaint
;		 TDXPos = PTVDRect.rcLeft*FontWid - ColOrg;
;                                - bit position on the screen
;		 PTVDRect.rcTop = PGVDRect.rcTop/FontHgt - first row of repaint
;		 TDYPos = PTVDRect.rcTop*FontHgt - RowOrg;
;                                - bit position on the screen
;         
;            # of rows being painted
;	     = AX = (PSrcPaint.rcBottom - TDYPos + FontHgt - 1)/FontHgt
;            Max # of rows that can be painted
;            = CX = (# of rows - first row of repaint)
;                Ensure AX <= CX
;		 PTVDRect.rcBottom = AX + first row of repaint
;            # of cols being painted
;	     AX = (PSrcPaint.rcRight - TDXPos + FontWid - 1)/FontWid
;            Max # of cols that can be painted
;            CX = (# of cols - first col of repaint)
;                Ensure AX <= CX
;		 PTVDRect.rcRight = AX + first col of repaint
;
;*****************************************************************************

PrepPnt proc	near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
    ;
    ; Org the Right and Left boundaries onto the display and
    ;	adjust them to client area
    ;
	mov	ax,[si.ColOrg]
	sub	[di.rcRight],ax
	jbe	Cret			; Rect is off screen to left, ignore
	sub	[di.rcLeft],ax
	jnc	short NoAdjL
	mov	[di.rcLeft],0
NoAdjL:
	mov	ax,RecRight		; Right edge of client area
	cmp	ax,[di.rcLeft]
	jbe	Cret			; Rect is off screen to right, ignore
	cmp	ax,[di.rcRight]
	jae	short NoAdjR
	mov	[di.rcRight],ax
NoAdjR:
    ;
    ; Org the Top and Bottom boundaries onto the display and
    ;	adjust them to client area
    ;
	mov	ax,[si.RowOrg]
	sub	[di.rcBottom],ax
	jbe	Cret			; Rect is off screen to Top, ignore
	sub	[di.rcTop],ax
	jnc	short NoAdjT
	mov	[di.rcTop],0
NoAdjT:
	mov	ax,RecBottom		; Bottom edge of client area
	cmp	ax,[di.rcTop]
	jbe	Cret			; Rect is off screen to bottom, ignore
	cmp	ax,[di.rcBottom]
	jae	short NoAdjB
	mov	[di.rcBottom],ax
NoAdjB:
    ;
    ; Build Other parms in Extended paint structure
    ;
    ; Set paint rect
    ;
	mov	eax,dword ptr [di.rcRight]
	mov	dword ptr [si.Pstruct.PSrcPaint.rcRight],eax
	mov	eax,dword ptr [di.rcLeft]		   ; AX is Left
	mov	dword ptr [si.Pstruct.PSrcPaint.rcLeft],eax
    ;
    ; Figure left col of repaint
    ;
	mov	bx,[si.ColOrg]
	mov	di,[si.FntWid]
	add	ax,bx
	mov	[si.PGVDRect.rcLeft],ax
	mov	dx,[si.Pstruct.PSrcPaint.rcRight]
	add	dx,bx
	mov	[si.PGVDRect.rcRight],dx
	cmp	[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4	; Text mode?
	jae	short NtTxt1		; No, skip
	cwd
	idiv	di
	mov	[si.PTVDRect.rcLeft],ax   ; first column of repaint
	mul	di
	sub	ax,bx
	mov	[si.TDXpos],ax		; bit position on screen
NtTxt1:
    ;
    ; Figure Top Row of repaint
    ;
	mov	ax,[si.Pstruct.PSrcPaint.rcTop]
	mov	bx,[si.RowOrg]
	mov	cx,[si.FntHgt]
	add	ax,bx
	mov	[si.PGVDRect.rcTop],ax
	mov	dx,[si.Pstruct.PSrcPaint.rcBottom]
	add	dx,bx
	mov	[si.PGVDRect.rcBottom],dx
	cmp	[si.EPGrabDTA.CntrlSt.VDA_CGA_Mode],4	; Text mode?
	jae	short NtTxt2		; No, skip
	cwd
	idiv	cx
	mov	[si.PTVDRect.rcTop],ax	  ; First line of repaint
	mul	cx
	sub	ax,bx
	mov	[si.TDYpos],ax		; Bit position on screen
    ;
    ; Figure Bottom Row of repaint
    ;
	sub	ax,[si.Pstruct.PSrcPaint.rcBottom]
	neg	ax
	add	ax,cx
	dec	ax
	cwd
	idiv	cx
	xor	cx,cx
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_CGA_Rows]
	sub	cx,[si.PTVDRect.rcTop]
	cmp	ax,cx
	jle	short pnt1
	mov	ax,cx
pnt1:	add	ax,[si.PTVDRect.rcTop]
	mov	[si.PTVDRect.rcBottom],ax
    ;
    ; Figure Right Col of repaint
    ;
	mov	ax,[si.Pstruct.PSrcPaint.rcRight]
	sub	ax,[si.TDXpos]
	add	ax,di			; add in character width
	dec	ax
	cwd
	idiv	di
	mov	cx,80
	sub	cx,[si.PTVDRect.rcLeft]
	cmp	ax,cx
	jle	short pnt2
	mov	ax,cx
pnt2:	add	ax,[si.PTVDRect.rcLeft]
	mov	[si.PTVDRect.rcRight],ax
NtTxt2:
	clc
	ret

Cret:
	stc
	ret

PrepPnt endp


;*****************************************************************************
;
; CursorOn - Create cursor for app if it has one
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	Carret created
; USES:
;	C standard
;
;*****************************************************************************
cProc  CursorOn,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; now, ds:bx,ds:si -> EXTPAINTSTRUC
    ;
	call	CursorPos
	call	MakeNewCursor

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; CursorOff - Destroy cursor for app
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	Carret Destroyed
; USES:
;	C standard
;
;*****************************************************************************
cProc  CursorOff,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; now, ds:bx,ds:si -> EXTPAINTSTRUC
    ;
	mov	[si.EPGrabDTA.CurCursEnd],-1
	mov	[si.EPGrabDTA.CurCursBeg],-1
	mov	[si.EPGrabDTA.CurCursMode],-1
	cCall	DestroyCaret

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
;
; CursorPosit - Return position of cursor on display
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	DX,AX = (Y,X) screen CoOrd of upper left of cursor
;	      = (-1,-1) if no cursor
; USES:
;	C Standard
;
;*****************************************************************************
cProc  CursorPosit,<FAR,PUBLIC>,<esi,edi>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

	lds	si,lpPntStruc
	call	CheckCtrlState
    ;
    ; now, ds:bx,ds:si -> EXTPAINTSTRUC
    ;
	call	CursorPos

	pop	edi
	pop	esi
cEnd

;*****************************************************************************
; TranslateWinToFScreen
;
; ENTRY:
;	ds:si -> lpPntStruc
;	edi = (X,Y) in windowed coordinated
; EXIT:
;	edi = (X,Y) in full-screen coordinates
;*****************************************************************************
cProc TranslateWinToFScreen,<FAR,PUBLIC>,<dx>
	parmD	lpPntStruc
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

    ;
    ; No translation for modes other than 0,1
    ;
	lds	bx,lpPntStruc

	mov	[bx.EPGrabDTA.CntrlSt.VDA_CGA_Mode],1
	ja	SHORT DontDoubleX
	rol	edi,16			; for modes 0,1 halve X
	shl	di,1
	rol	edi,16

DontDoubleX:

cEnd

;*****************************************************************************
;
; CheckGRBVersion - Check out the VDD version
;
; ENTRY:
;	lpPntStruc Extended paint structure
; EXIT:
;	AX == 0
;	    OK
;	AX != 0   Bad version
;	    AX == 1
;		Version # error
;	    AX == 2
;		Display type mismatch
;	DX = Grabber Version number
; USES:
;	C Standard
;
;*****************************************************************************
cProc  CheckGRBVersion,<FAR,PUBLIC>

	ParmD	lpPntStruc

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame
    ;
    ; Get address of the procedure to call for VDD services
    ;
        mov     bx,SHELL_Call_Dev_VDD 	
	mov	ax,(W386_Int_Multiplex SHL 8) OR W386_Get_Device_API
	int	W386_API_Int
        mov     cx,es           
        or      cx,di
        jz      short cgv1              ; Invalid address, return version check failure
	lds	bx,lpPntStruc           
    ;
    ; Save the far proc address 
    ;
	mov	word ptr [bx.GGrbArea.VddApiProcAdr],di
	mov	word ptr [bx.GGrbArea.VddApiProcAdr.2],es

	mov	ax,GRB_Get_Version
	push	ds
	pop	es
	lea	di,[bx.VerStrng]		; ES:DI -> type string
	movzx	edi,di
        call    [bx.GGrbArea.VddApiProcAdr]
	cmp	ax,Grabber_VerNum
	je	short cgv2			; Version OK
cgv1:
	mov	ax,1
	jmp	short cgv3
cgv2:
	push	cs
	pop	ds
    assumes ds,code
	mov	si,codeOffset VDD_Type
	mov	cx,4
	cld
	repe	cmpsw
	mov	ax,2				; Assume bad
	jne	short cgv3			; Type Bad
	xor	ax,ax
cgv3:
	mov	dx,(GRB_VER_HIGH SHL 8) OR GRB_VER_LOW

	pop	edi
	pop	esi
cEnd
;*****************************************************************************
;
; GrbGetTextColor - given a Windows RGB Color return text color in display
;                specific format
;
; ENTRY: 
;	lpPntStruc Extended paint structure
;       RGBValue a windows RGB color
;
; EXIT: DX:AX = text color in display specific format (IRGB for EGA/VGA)
;
; USES: C standard
;
;*****************************************************************************
cProc   GrbGetTextColor,<FAR,PUBLIC>

        ParmD   lpPntStruc
        ParmD   RGBValue

	localV	FrameVars,StdGrbFrameSz
cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	InitBasicFrame

        xor     dx,dx
        xor     ax,ax

        mov     ebx,RGBValue
        or      ebx,ebx                ; Q: Black ?
        jz      SHORT NoInt            ;   Y: Don't set Int bit
        or      al,00001000B           ;   N: Set Int bit for all other colors
NoInt:
        mov     cx,bx
        shr     ebx,8
    ;
    ; CL = Red, CH = Green, BH = Blue
    ;
        or      cl,cl
        jz      short NoRed
        or      al,00000100B           ; Set red
NoRed:
        or      ch,ch
        jz      short NoGreen
        or      al,00000010B           ; Set green
NoGreen:
        or      bh,bh
        jz      short NoBlue
        or      al,00000001B           ; set blue
NoBlue:
        pop     edi
        pop     esi
cEnd


;*****************************************************************************
;
; WEP - DLL Exit proc (does nothing in grabber case)
;
; ENTRY:
;	fSysExit word param != 0 if System Exit as opposed to just LIB exit
; EXIT:
;	None
; USES:
;	FLAGS
;
;*****************************************************************************
cProc	Wep,<FAR,PUBLIC,NODATA>

cBegin	nogen
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	ret	2
cEnd	nogen

;*****************************************************************************
;
; InitGrabber - Library Initialization Routine
;
; ENTRY:
;	DI = Module handle of the library
;	CX = Size of local heap (should be 0)
;	DS = Seg addr of library data segment (isn't one)
; EXIT:
;	AX == 0
;	  Init Error
;	AX != 0
;	  OK
; USES:
;	AX, FLAGS
;
;*****************************************************************************
cProc	InitGrabber,<FAR,PUBLIC,NODATA>

cBegin
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	edi

	xor	ax,ax
	inc	ax		; Set non-zero return

	pop	edi
	pop	esi
cEnd

sEnd	code
	end	InitGrabber
