;--------------------------------------------------------------------
;
;   Screen Grabber for Video Seven VRAM/FASTWRITE VGA adaptors
;
;   These routines perform paints and all other Display specific
;	aspects of WINOLDAP (VMDOSAPP)
;
;    (C) Copyright MICROSOFT Corp. 1986-1990
;
;    ************* MICROSOFT CONFIDENTIAL ******************
;
;--------------------------------------------------------------------

.386p
?386 = 1

memS	equ	1

	.xlist
	include cmacros.inc

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

	include    windows.inc
	include    vmda.inc
	include    int2fapi.inc
	include    grabpnt.inc
	include    grabmac.inc
	include    vmdaega.inc
	include    8514.inc
	include    statusfl.inc
	.list

IF1
    IFDEF DEBUG
	%out DEBUG VERSION!!!!!
    ENDIF
ENDIF

;--------------------------------------------------------------------
;
; External Windows Procedures
;
externFP	DestroyCaret
externFP	SetCaretPos
externFP	SelectObject
externFP	PatBlt
externFP	GetUpdateRect
externFP	GetDC
externFP	ReleaseDC
externFP	InvertRect
externFP	ScrollWindow
externFP	UpdateWindow

externFP	GlobalFree
externFP	GlobalAlloc
externFP	GlobalLock
externFP	GlobalUnlock
externFP	CreateBitMap
externFP	GetPrivateProfileString

;--------------------------------------------------------------------
;
; External GRABBER Procedures
;
externNP	ScreenAdjust
externNP        GetTextWidHgt
externNP	AdjustSelGrxMax
externNP	AdjustSelTextMax
externNP	ComputeSelTextRect
externNP	RenderRectGrxDisplay
externNP	RenderRectGrxMono
externNP	RenderRectText
externNP	CursorPos
externNP	CheckCtrlState
externNP	MakeNewCursor
externNP	ModeText
externNP	ModeGrfx
externNP	GetVidSel
externNP	ClearVidSel


sBegin	code
	assumes cs,code

	public	LookupPalette
	public	StandardPal
	public	GetWinRGB
	public	TextScroll
	public	PrepPnt
        public  PaintClientRect
        public  SavePaintRect
        public  GetPageBitmap

;
; This is the VDD type string that we expect as part of the version check
;
VDD_Type db	"VIDEOVGA"

;
; Table for conversion of Palette contents to a dword color value(xBGR)
; Used in building the PelColorTable for DIBs
;
 	public RGBTable                ; use for modes 4,5,6,11
 	public RGBTableDE              ; modes D,E
 	public RGBTable10              ; modes 10,1,2,3,12
;
; NOTE: IN all the RGBTables(RGBTable,RGBTable10,RGBTableDE) we should be 
; using C4h instead of 80h. C4h is used by the display driver. Currently
; GDI is using it's own table which has a 80h instead of C4h. Hence we shall
; stick to 80h whenever we are building DIBs.
;
IF1
%OUT Change RGBTables later to use C4H instead of 80H
ENDIF
RGBTable        Label   word
	dd	00000000H			; 0000	Black
	dd	00800000H			; 0001	Blue
	dd	00008000H			; 0010	Green
	dd	00808000H			; 0011	Cyan
	dd	00000080H			; 0100	Red
	dd	00800080H			; 0101	Magenta
	dd	00008080H			; 0110	Brown
	dd	00C0C0C0H			; 0111	White (low intensity)
	dd	00808080H			; 1000	Gray
	dd	00FF0000H			; 1001	Light Blue
	dd	0000FF00H			; 1010	Light Green
	dd	00FFFF00H			; 1011	Light Cyan
	dd	000000FFH			; 1100	Light Red
	dd	00FF00FFH			; 1101	Light Magenta
	dd	0000FFFFH			; 1110	Yellow
	dd	00FFFFFFH			; 1111	White (high intensity)

RGBTable10      Label   word
dd 00000000h,00800000h,00008000h,00808000h,00000080h,00800080h,00008080h,00808080h
dd 00808080h,00FF0000h,00008000h,00FF8000h,00000080h,00FF0080h,00008080h,00FF8080h
dd 00000000h,00800000h,0000FF00h,0080FF00h,00000080h,00800080h,0000FF80h,0080FF80h
dd 00808080h,00FF0000h,0000FF00h,00FFFF00h,00000080h,00FF0080h,0000FF80h,00FF8080h

dd 00000000h,00800000h,00008000h,00808000h,000000FFh,008000FFh,000080FFh,008080FFh
dd 00808080h,00FF0000h,00008000h,00FF8000h,000000FFh,00FF00FFh,000080FFh,00FF80FFh
dd 00000000h,00800000h,0000FF00h,0080FF00h,000000FFh,008000FFh,0000FFFFh,0080FFFFh
dd 00808080h,00FF0000h,0000FF00h,00FFFF00h,000000FFh,00FF00FFh,0000FFFFh,00FFFFFFh


;RGBTable10      Label   word           ; DAC Values
;dd 00000000h,002a0000h,00002a00h,002a2a00h,0000002ah,002a002ah,00002a2ah,002a2a2ah
;dd 00150000h,003f0000h,00152a00h,003f2a00h,0015002ah,003f002ah,00152a2ah,003f2a2ah
;dd 00001500h,002a1500h,00003f00h,002a3f00h,0000152ah,002a152ah,00003f2ah,002a3f2ah
;dd 00151500h,003f1500h,00153f00h,003f3f00h,0015152ah,003f152ah,00153f2ah,003f2a2ah
;
;dd 00000015h,002a0015h,00002a15h,002a2a15h,0000003fh,002a003fh,00002a3fh,002a2a3fh
;dd 00150015h,003f0015h,00152a15h,003f2a15h,0015003fh,003f003fh,00152a3fh,003f2a3fh
;dd 00001515h,002a1515h,00003f15h,002a3f15h,0000153fh,002a153fh,00003f3fh,002a3f3fh
;dd 00151515h,003f1515h,00153f15h,003f3f15h,0015153fh,003f153fh,00153f3fh,003f2a3fh

RGBTableDE      Label   word            
dd 00000000h,00800000h,00008000h,00808000h,00000080h,00800080h,00008080h,00C0C0C0h
dd 00000000h,00800000h,00008000h,00808000h,00000080h,00800080h,00008080h,00C0C0C0h
dd 00808080h,00FF0000h,0000FF00h,00FFFF00h,000000FFh,00FF00FFh,0000FFFFh,00FFFFFFh
dd 00808080h,00FF0000h,0000FF00h,00FFFF00h,000000FFh,00FF00FFh,0000FFFFh,00FFFFFFh

dd 00000000h,00800000h,00008000h,00808000h,00000080h,00800080h,00008080h,00C0C0C0h
dd 00000000h,00800000h,00008000h,00808000h,00000080h,00800080h,00008080h,00C0C0C0h
dd 00808080h,00FF0000h,0000FF00h,00FFFF00h,000000FFh,00FF00FFh,0000FFFFh,00FFFFFFh
dd 00808080h,00FF0000h,0000FF00h,00FFFF00h,000000FFh,00FF00FFh,0000FFFFh,00FFFFFFh

;RGBTableDE      Label   word           ; DAC Values
;dd 00000000h,002a0000h,00002a00h,002a2a00h,0000002ah,002a002ah,0000152ah,002a2a2ah
;dd 00000000h,002a0000h,00002a00h,002a2a00h,0000002ah,002a002ah,0000152ah,002a2a2ah
;dd 00151515h,003f1515h,00153f15h,003f3f15h,0015153fh,003f153fh,00153f3fh,003f3f3fh
;dd 00151515h,003f1515h,00153f15h,003f3f15h,0015153fh,003f153fh,00153f3fh,003f3f3fh
;
;dd 00000000h,002a0000h,00002a00h,002a2a00h,0000002ah,002a002ah,0000152ah,002a2a2ah
;dd 00000000h,002a0000h,00002a00h,002a2a00h,0000002ah,002a002ah,0000152ah,002a2a2ah
;dd 00151515h,003f1515h,00153f15h,003f3f15h,0015153fh,003f153fh,00153f3fh,003f3f3fh
;dd 00151515h,003f1515h,00153f15h,003f3f15h,0015153fh,003f153fh,00153f3fh,003f3f3fh

;
; This table defines the pallete programs we understand and know how to
;	interpret.
;
;   First is an array of pallete settings = Size of the pallete array - 1
;	(pallete 0 is the background which we ignore)
;   Next is one byte of special flags. These flags are ORed into the low byte
;	of GrbFlgs.
;   Next is four bytes. The first byte tells us which plane is BLUE
;			 The second byte tells us which plane is GREEN
;			 The third byte tells us which plane is RED
;			 The fourth byte tells us which plane is INTENSITY
;
StandardPal	Label	word
	; Mode 10 standard program
	db	001H,002H,003H,004H,005H,014H,007H,038H,039H,03AH,03BH,03CH,03DH,03EH,03FH
	db	0
	db	0,1,2,3
	; Lotus 123 standard program
	db	001H,002H,003H,004H,005H,006H,007H,008H,009H,00AH,00BH,00CH,00DH,00EH,01FH
	db	0
	db	0,1,2,3
	; Microsoft Word standard program
	db	001H,002H,003H,004H,005H,006H,007H,026H,00BH,018H,035H,014H,019H,024H,03FH
	db	0
	db	0,1,2,3
	; Mode E standard program
	db	001H,002H,003H,004H,005H,006H,007H,010H,011H,012H,013H,014H,015H,016H,000H
	db	0
	db	0,1,2,3
	; Mode D standard program
	db	001H,002H,003H,004H,005H,006H,007H,010H,011H,012H,013H,014H,015H,016H,017H
	db	0
	db	0,1,2,3
        ; Mode 11 standard program
	db	03FH,03FH,03FH,03FH,03FH,03FH,03FH,03FH,03FH,03FH,03FH,03FH,03FH,03FH,03FH
	db	0
	db	0,1,2,3
	; Mode 0F standard program
	db	08H,0,0,018H,018H,0,0,0,08H,0,0,0,018H,0,0
	db	0
	db	0,1,2,3
        ; The special "Inverse palette" used by GEM, Ventura publisher
	db	000H,000H,000H,000H,000H,000H,000H,000H,000H,000H,000H,000H,000H,000H,000H
	db	GrbFlgs_GrfxInvPal
	db	0,1,2,3
    ;
    ; NOTE!!!! THIS PALLETE MUST BE LAST!!!!! it is the default pallete that is
    ;	used for apps who's pallete we don't really understand.
    ;
	db	0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH,0FFH
	db	0
	db	0,1,2,3
StandardPalCnt	equ	($-StandardPal)/((SIZE VDA_EGA_Pal)+5)

IF2
    IF (SIZE VDA_EGA_Pal) NE 16
	%out ****** ERROR !!!!!!! Change in size of EGA Palette array
	Syntax error
    ENDIF
ENDIF


;*****************************************************************************
;
; LookupPalette - See if we recognize the EGA pallete
;
; ENTRY:
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	SS:BP -> Std GRAPHICS stack frame
;
; DESCRIPTION:
;	This grabber treats the planes of the EGA as COLOR planes. This is
;	NOT really correct, but it would be very slow to actually do what
;	the EGA does and treat each pixel 4 bit value as a palette index
;	which determines the color of that pixel. What this routine does
;	is determine which planes correspond to the RED GREEN BLUE and
;	"INTENSITY" COLORS.
;
; EXIT:
;	Carry Set
;	    Palette program is not recognized, application cannot be displayed
;			in a Window
;	Carry Clear
;	    BPlaneNum on Stack frame set to Plane Number of BLUE plane
;	    GPlaneNum on Stack frame set to Plane Number of GREEN plane
;	    RPlaneNum on Stack frame set to Plane Number of RED plane
;	    IPlaneNum on Stack frame set to Plane Number of INTENSITY plane
;	    Extra GrbFlags set from palette table
; USES:
;	ES,DI,CX,FLAGS
;
;*****************************************************************************
LookupPalette proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	si
	cld
	lea	si,[si.EPGrabDTA.CntrlSt.VDA_EGA_Pal.1]
	push	cs
	pop	es
	mov	di,codeOffset StandardPal
	mov	cx,StandardPalCnt
LkLp:
	push	si
	push	cx
	mov	cx,(SIZE VDA_EGA_Pal) - 1
	repe	cmpsb
	je	short FoundPal
	add	di,cx
	add	di,5
	pop	cx
	pop	si
	loop	LkLp
    ;
    ; We didn't find a palette. We will use the "default" pallete which is the
    ;	last one in the list.
    ;
IFDEF DEBUG
	pop	cx
	push	cx
	push	bx
	push	si
	push	di
	mov	bx,cx
	movzx	edi,si
	mov	esi,codeOffset BadPal
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short LPContinue

BadPal	db	"Grabber - unrecognized palette -> ds:edi.. Type G to continue",0

LPContinue:
	pop	di
	pop	si
	pop	bx
ENDIF
        add     di,(SIZE VDA_EGA_Pal)-1     ; Point to default palette flags
	jmp	short LkUDn

FoundPal:
	add	sp,4			; Clean stack
LkUDn:
	movzx	cx,byte ptr es:[di]
.erre GrbFlgs_GrfxInvPalBit	   LE	 7
	or	GrbFlags,cx		 ; Set bits from palette lookup
	mov	cx,word ptr es:[di.1]
	mov	BPlaneNum,cl
	mov	GPlaneNum,ch
	mov	cx,word ptr es:[di.3]
	mov	RplaneNum,cl
	mov	IPlaneNum,ch
	clc
	pop	si
	ret

LookupPalette endp

;*****************************************************************************
;
; GrabEvent - Private VDD -> Grabber messages
;
;   Provides a private channel of event communication between the VDD
;   and the grabber. EGA/VGA/8514 grabbers do not use this.
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
;  Called after the grab is complete. Time to call the VDD
;  and have him free the grab memory.
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
    ;	controller state and re-get the Paint state.
    ;
	btr	[si.EPStatusFlags],fVValidBit
	mov	ax,GRB_Free_Grab
	mov	ebx,[si.ThisVMHand]
        call    [si.GGrbArea.VddApiProcAdr]
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

	call	GetVidSel       ; Get the video mem and FREEZE the app
        jc      PS_SetAX        ; paint screen black if error

	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ; Check to see if the screen is off
    ;
	test	[si.EPGrabDTA.CntrlSt.VDA_EGA_Flags],fVDA_V_ScOff
	jnz	short NulScreen
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
	cmp	ah,7
	je	short PSText
	cmp	ah,4
	jae	short PSGraphics
PSText:
    ;
    ; The paint should be done from the copy mem and not from the main mem.
    ; GetVidSel is called twice once for locking the APP(the selector for main 
    ; memory which is returned is not used), and again to get the copy memory.
    ;
IF1
%OUT BEGIN HACK! Get copy mem for WM_PAINT in text modes
ENDIF
	mov	VidSel,0                        ; discard main mem sel
	or 	GrbFlags,GrbFlgs_DoingGrab      ; set flags to get copy mem
        and     GrbFlags, NOT GrbFlgs_DoingWindowSelGrab
	call	GetVidSel                       ; error unlikely
;	btr	[si.EPStatusFlags],fVValidBit   ; get the copy mem state
;	call	CheckCtrlState
	and 	GrbFlags,NOT GrbFlgs_DoingGrab  ; turn off grab Flag
%OUT END HACK! Get copy mem for WM_PAINT in text modes
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
	jz	short PS_NoCursor		; No cursor
	dec	dx
	sub	ax,[si.ColOrg]		; Org onto display
	sub	dx,[si.RowOrg]		; Org onto display
	cCall	SetCaretPos,<ax,dx>     ; position cursor
PS_NoCursor:
	popf				; Recover carry setting
	jmp	short PS_SetAX
PSGraphics:
	xor	cx,cx
	call	ModeGrfx		
	jmp	short PS_SetAX          ; no cursor in graphics mode!
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

;************************************************************************************
;
; GetWinRGB - Get XBGR dword value for the given Palette index.
;
; ENTRY: DS:SI -> EXTPAINTSTRUC, 
;        AL = 4 bit Palette index
; EXIT:  xBGR value returned in dx:ax
;
; Alg:
; 1.  Index into the Palette 
; 2.  Interpret the Palette contents based on the Mode:
;       Mode 4,5,6,11 - xxx1x111, i.e. <b4,b2,b1,b0> is a four bit index into RGBTable
;       Mode 1,2,3,10,12 - b5-b0 is a six bit index into RGBTable10
;       Mode D,E - b5-b0 is a six bit index into RGBTableDE
; 3. The dword value in the RGBTable gives the required RGBvalue.
;
;*************************************************************************************

GetWinRGB proc near
    assume ds:nothing,es:nothing,ss:nothing

        cmp     Gmode, 3
        jbe     short WinRGB10
	cmp	Gmode, 0Fh
        je      short WinRGB10
        cmp     Gmode, 10h
        je      short WinRGB10
        cmp     Gmode, 12h
        je      short WinRGB10

        cmp     Gmode, 0Dh
        je      short WinRGBDE
        cmp     Gmode, 0Eh
        je      short WinRGBDE

        jmp     short WinRGB45611

WinRGB10:
	push	bx
	and	ax,0000000000001111B		; Mask to bits of interest
	lea	bx,[si.EPGrabDTA.CntrlSt.VDA_EGA_Pal]
	add	bx,ax
	mov	bl,byte ptr [bx]		; BL = Palette register
	xor	bh,bh
	shl	bx,2				; BX now has the index into table
	mov	ax,[bx.RGBTable10]
	mov	dx,[bx.RGBTable10.2]
	pop	bx
	ret

WinRGBDE:
	push	bx
	and	ax,0000000000001111B		; Mask to bits of interest
	lea	bx,[si.EPGrabDTA.CntrlSt.VDA_EGA_Pal]
	add	bx,ax
	mov	bl,byte ptr [bx]		; BL = Palette register
	xor	bh,bh
	shl	bx,2				; BX now has the index into table
	mov	ax,[bx.RGBTableDE]
	mov	dx,[bx.RGBTableDE.2]
	pop	bx
	ret
WinRGB45611:
	push	bx
	and	ax,0000000000001111B		; Mask to bits of interest
	lea	bx,[si.EPGrabDTA.CntrlSt.VDA_EGA_Pal]
	add	bx,ax
	mov	bl,byte ptr [bx]		; AL = Palette register
	mov	bh,bl
	and	bl,00000111B			; Mask to RGB values
	shl	bl,1
	and	bh,00010000B			; Mask to intensity bit
	or	bl,bh
	xor	bh,bh
	shl	bx,1				; BX is now IRGB * 4
	mov	ax,[bx.RGBTable]
	mov	dx,[bx.RGBTable.2]
	pop	bx
	ret
GetWinRGB endp

;*****************************************************************************
;
; ScreenFree - Free any stuff associated with this app
;
;   This routine frees any resources associated with this app
;	that are not allocated on an as need basis local to
;	each call on the stack, or staticly stored in the grabber
;	area of the extended paint structure.
;
;   FOR THE EGA/VGA/8514 grabbers this routine is a NOP.
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
;		Here "screen CoOrd" means a window client area position
;		corrected by the scroll bar positions.
; EXIT:
;	[lpPntStruc.SelStruc.SelctSRect] Display rect in extended paint
;	    selection structure set
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
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7
	je	short AIE_Text
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
        jae     short GotWidHgt
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
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7
	je	short KS_Text
	mov	cx,64		; Assume graphics mode 0D or 13
				; WARNING!!!! This value is carefully chosen.
				; See proc ScreenAdjust, graphics mode code
	mov	si,[bx.DefFont.FontHgt]  ; Assume graphics, will move height of OEM font
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0DH
	je	short KS_GotWidHgt
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],13H
	je	short KS_GotWidHgt
	mov	cx,32		; Assume other graphics mode
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
	jae	short KS_GotWidHgt
    ;
    ; Text mode, move by one char or line
    ;
KS_Text:
        call    GetTextWidHgt
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
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7
	je	short KS_Text1
        cmp     [bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
	jae	short KSGraphics
KS_Text1:
        call    AdjustSelTextMax        ; Adjust max for AX,DX - Text mode
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
;		Here "screen CoOrd" means a window client area position
;		corrected by the scroll bar positions.
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
;	SizeType == 0 for Normal size (text or MONO bitmap)
;		 != 0 for Display format size of BITMAP (error if text)
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
	ParmW	SizeType
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
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7
        je      short TextSel
        cmp     [bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
	jb	short TextSel
GraphicsSel:
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0FH
	je	short NoDiv
        cmp     [bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],10H
	je	short NoDiv
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],13H
	je	short NoSel 		; No grabs in mode 13
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],11H
	je	short NoDiv
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],12H
	je	short NoDiv
	shr	dx,1			; Convert to display
NoDiv:
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0DH
	je	short Div2
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
	je	short Div2
        jmp     short NoDiv2
Div2:
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
	cmp	SizeType,0
	jz	short CheckSize 	; For MONO (normal), only one plane
	mov	cx,4			; Display format is 4 planes
	mul	cx			; DX:AX is size for display format
CheckSize:
	mov	bx,ax
	or	bx,dx
	jz	short NoSel
	jmp	short GSSDone

TextSel:
	cmp	SizeType,0		; Valid for text?
	jnz	short NoSel		; No, error

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
;	C standard      Ensure this
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

	or	GrbFlags,GrbFlgs_DoingGrab
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
	or	GrbFlags,GrbFlgs_DoingWindowSelGrab
RenderFromGrabState:
	call	CheckCtrlState
    ;
    ; Now ds:bx,ds:si -> EXTPAINTSTRUC structure
    ;
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]	; Get mode
	cmp	cl,7
	je	RSText
        cmp     cl,4
	jb	RSText

	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn
	jz	RS_NoSelection
    ;
    ; Check Mode Byte
    ; Valid modes for 8514/VGA are 4,5,6,D,E,F,10,11,12
    ;
	cmp	cl,12H
	ja	RS_Error

	cmp	cl,7
	jb	short RSGraphics

	cmp	cl,0DH
	jb	RS_Error
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
	cmp	bx,cx			; NUL (zero width)?
	jz	RS_NoSelection		; Yes

	mov	bx,[si.SelStruc.GrabArea.StartPointY]
	mov	cx,[si.SelStruc.GrabArea.EndPointY]
	cmp	bx,cx			; This way?
	jbe	short SetTopBottom	; Yes
	xchg	bx,cx			; no, other way
SetTopBottom:
	mov	[si.SelStruc.GrabArea.Irect.rcTop],bx
	mov	[si.SelStruc.GrabArea.Irect.rcBottom],cx
	cmp	bx,cx			; NUL (zero height)?
	jz	RS_NoSelection
    ;
    ; Get a Display Context 
    ;
	cCall	GetDC,<[si.WindHand]>
	or	ax,ax           ; Valid hDC?
	jz	RS_Error        ; No
	mov	[si.Pstruct.psHdc], ax
DisplayFormat:
	mov	bx,si
	lea	si,[si.SelStruc.GrabArea.Irect]
        call    RenderRectGrxDisplay
	jc	short TryGrxMonoFormat 	; Barfed, try mono format

        push    ax                      ; preserve Bitmap Handle
	cCall	ReleaseDC, <[si.WindHand], [si.Pstruct.psHdc]>
        pop     ax

        mov     dx, CF_BITMAP           ; AX has the handle
	jmp	RS_Done

TryGrxMonoFormat:
	cCall	ReleaseDC, <[si.WindHand], [si.Pstruct.psHdc]>
    ;
    ; Allocate buffer for mono format.
    ;
	cCall	GetSelectionSiz,<lpPntStruc,0,wParam,lParam>   ; mono format size
	mov	cx,ax
	or	cx,dx
	jz	short RS_Error
    ;
    ; Try to allocate the memory
    ;
	regptr	ddSize,dx,ax
	cCall	GlobalAlloc,<GMEM_MOVEABLE+GMEM_ZEROINIT+GMEM_DDESHARE,ddSize>
	or	ax,ax
	jz	short RS_Error
	mov	hMemory,ax
	cCall	GlobalLock,<ax>
	mov	es,dx
	mov	di,ax		; es:di points at save area (mem just alloced)
        or      dx, ax
	jz	short RS_Error

	mov	bx,si
	lea	si,[si.SelStruc.GrabArea.Irect]
        call    RenderRectGrxMono
	jc	short RS_Error
	cCall	GlobalUnLock,<hMemory>
        cCall   GlobalFree,<hMemory>        
        cmp     ax, hMemory
        je      short RS_Error
        mov     dx, CF_BITMAP
	jmp	RS_Done
   ;
   ; Error in Rendering.
   ;
RS_Error:
IFDEF DEBUG
	mov	bx,si
	mov	esi,codeOffset RendErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short REContinue

RendErr db	"Grabber Got rendering error",0

REContinue:
ENDIF
        mov     dx, -1
	jmp	short RS_Done

   ;
   ; No Selection exists to render
   ;
RS_NoSelection:         
        mov     dx, 0
	jmp	short RS_Done
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
	or	bl,bl		; NUL selec?
	jz	RS_NoSelection 	; Yes
	mov	bx,si
    ;
    ; get size of the render buffer 
    ;
	cCall	GetSelectionSiz,<lpPntStruc,0,wParam,lParam>   ; mono format size
	mov	cx,ax
	or	cx,dx
	jz	RS_Error
    ;
    ; Try to allocate the memory
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

        mov     bx, si
	lea	si,[si.SelStruc.GrabArea.Irect]
        call    RenderRectText
        pushf                           ; save return value
	cCall	GlobalUnLock,<hMemory>
        popf
	jc	RS_Error
        mov     ax, hMemory             ; return Memory Handle in AX
	mov     dx, CF_OEMTEXT	        ; set return value
RS_Done:
	pop	edi
	pop	esi
cEnd

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
	mov	dl,[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
	cmp	dl,7
	je	short FontYes
        cmp     dl,4
	jb	short FontYes
    ;
    ; Graphics screen
    ;
	xor	ax,ax
	mov	[bx.FntHgt],ax
	mov	[bx.FntWid],ax
	mov	cx,GrxBitWid640
	mov	bx,GrxBitHeight400
	cmp	dl,0FH
	jb	short LowScrS
	cmp	dl,13H
	je	short LowScrS
	mov	bx,GrxBitHeight350
	cmp	dl,0FH
	je	short LowScrS
        cmp     dl,10H
	je	short LowScrS
	mov	bx,GrxBitHeight480
LowScrS:
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
	xor	ax,ax
	mov	al,[bx.EPGrabDTA.CntrlSt.VDA_EGA_Rows]
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
	xor	dx,dx
	mov	dl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows]
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
;	us (actually only need four extra).

FontList	Label	byte
F1	db	"EGA80WOA.FON",0	; Name	    <= 25 line mode 80 column
.erre ($-F1) EQ 13
	dw	12			; Height
	dw	8			; Width

F2	db	"EGA40WOA.FON",0	; Name	    <= 25 line mode 40 column
.erre ($-F2) EQ 13
	dw	12			; Height
	dw	16			; Width

F3	db	"CGA80WOA.FON",0	; Name	    >  25 line mode 80 column
.erre ($-F3) EQ 13
	dw	8			; Height
	dw	8			; Width

F4	db	"CGA40WOA.FON",0	; Name	    >  25 line mode 40 column
.erre ($-F4) EQ 13
	dw	8			; Height
	dw	16			; Width

	db	13 DUP (0)		; Unused
	dw	0
	dw	0

	db	13 DUP (0)		; Unused
	dw	0
	dw	0
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
	mov	cx,4
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
        mov     cx,VDD_MOD_MAX                  ; Size of DispMod structure
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

	test	[si.EPGrabDTA.CntrlSt.VDA_EGA_Flags],fVDA_V_HCurTrk
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
;          if (page bitmap)
;              convert to Rects and paint each
;          else if (Rect List and Grx mode)
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
	mov	ax,0
        jc      ClearModf               ; jmp on error
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
	jz	short DoUpdates 	; Controller state is unchanged

    ;
    ; Controller state has changed, paint whole client area. 
    ; Paint rect is set to whole client area, so we're ready to paint
    ;
        call    PaintClientRect
	jmp	JustCursor		; Check on cursor

    ; 
    ; Controller state is unchanged but video RAM might have changed
    ; 
DoUpdates:
	test	[si.EPGrabDTA.DispMod.VDD_Mod_Flag],fVDD_M_VRAM
	jz	JustCursor		; Just Cursor change, carry clear if jmp
	mov	ax,[si.EPGrabDTA.DispMod.VDD_Mod_Flag]
	and	ax,fVDD_M_Type
	cmp	ax,fVDD_M_Type_Rect     ; Rectangle updates?
	jz	DoRects                 ; Yes

    ;
    ; Given a page bitmap of the modified pages for the video memory,
    ; convert it to rectangles on screen and paint it.
    ;
DoPageBitmap:
IFDEF DEBUG
	cmp	ax,fVDD_M_Type_Page
	jz	short NoStallBM
	push	si
	push	bx
	push	dx
	mov	bx,si
	mov	esi,codeOffset PageErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short USContinue1

PageErr db	"Grabber expecting fVDD_M_Type_Page got AX",0

USContinue1:
	pop	dx
	pop	bx
	pop	si
NoStallBM:
ENDIF
        call    SavePaintRect           ; Save the client rect on the stack

	mov	cx,[si.EPGrabDTA.DispMod.VDD_Mod_Count] 
IFDEF DEBUG
	or	cx,cx
	jnz	short NoStallRCTA
	push	si
	push	bx
	push	dx
	mov	bx,si
	mov	esi,codeOffset CountErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short USContinue2

CountErr db	 "Grabber mod count = 0 Page type",0

USContinue2:
	pop	dx
	pop	bx
	pop	si
NoStallRCTA:
ENDIF
	or	cx,cx
	jz	JustCursor	; Hmmm.... Bit lied, carry clear if jmp

        mov     bx,si           
    ;
    ; get page bitmap in AX, LinesTable ptr in SI
    ;
        call    GetPageBitmap
    ; 
    ; Scan the PageBitmap for 1s
    ; 
NextPageBit:
        or      ax, ax          ; All pages done?
        jz      short PageBitsDone    ; Yes, Carry clear if jmp
        shr     ax, 1
        jnc     short NextTableEntry
    ;
    ; Setup update rect on the stack in "screen CoOrds" - client area CoOrds
    ; corrected for scroll bar. 
    ;
        lea     di,UpdateRect
        push    ax                      ; save PageBitmap
        mov     ax,[bx.ColOrg]
	mov	[di.rcLeft],ax
	add	ax,[bx.Pstruct.PSrcPaint.rcRight]
	mov	[di.rcRight],ax
        mov     ax,cs:[si.StartLine]
	mov	[di.rcTop],ax
        mov     ax,cs:[si.EndLine]
	mov	[di.rcBottom],ax

        add     si,4                    ; Next entry in the LinesTable

        push    si
        push    bx
        lds     si,lpPntStruc           ; restore DS
    ; 
    ; set up EXTPAINTSTRUC for painting the UpdateRect
    ; 
        call    PrepPnt
        pop     bx
        pop     si
        pop     ax                      ; restore PageBitmap
        jc      NextPageBit             ; Rect is off screen
    ; 
    ; Rect is not off screen, paint it
    ; 
        push    ax
        push    si
        push    bx
        lds     si,lpPntStruc
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
        xor     cx,cx
        call    ModeGrfx               
        pop     bx
        pop     si
        pop     ax
        jc      short PageBitsDone
        jmp     short NextPageBit              
NextTableEntry:
        add     si,4
        jmp     NextPageBit
PageBitsDone:
        mov     si,bx                   ; restore si -> EXTPAINTSTRUC
        jmp     JustCursor
    ;
    ; Re-paint indicated rects of screen
    ;
DoRects:
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
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7
	je	short TextUpdate
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
	jb	short TextUpdate
GrxUpdate:
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],11H
	je	short NoGAdj
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],12H
	je	short NoGAdj

	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0FH
	je	short NoGAdj
        cmp     [si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],10H
	je	short NoGAdj
	shl	[di.rcTop],1		  ; Convert to display
	shl	[di.rcBottom],1
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],13H
	je	short DoGAdj
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0DH
	je	short DoGAdj
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
	je	short DoGAdj
        jmp     short NoGAdj
DoGAdj:
	shl	[di.rcLeft],1		  ; Convert to display
	shl	[di.rcRight],1
NoGAdj:
	call	PrepPnt
	jc	NextRect
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
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
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
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
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
	cmp	cl,[si.EPGrabDTA.CurCursMode]	; Change in cursor mode?
	jnz	short MkNCur		; Yes
	mov	cx,[si.EPGrabDTA.CntrlSt.VDA_EGA_CurEnd]
	cmp	cx,[si.EPGrabDTA.CurCursEnd]	; Change in cursor shape?
	jnz	short MkNCur		; Yes
	mov	cx,[si.EPGrabDTA.CntrlSt.VDA_EGA_CurBeg]
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
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
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

	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
	cmp	ah,7
	je	short PCText
        cmp     ah,4
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

;****************************************************************************
;
; GetPageBitmap -  return PageBitmap in AX, LinesTable ptr in SI
;
; ENTRY: DS:SI -> EXTPAINTSTRUC
;
; EXIT:  AX = PageBitmap, DS:SI -> LinesTable
;
; USES: AX,DL,DS,SI
;
; Loads the Page bitmap(16 bits) from the modification structure and masks it.
; Atmost 2 4K pages in Mode D, 4 in Mode E, 7,F in Mode10, 10 in Mode12.
;
; Also loads SI with ptr to appropriate LinesTable.
;
; Note: called from UpdateRect only
;
;****************************************************************************

GetPageBitmap   proc near

	mov     ax,word ptr [si.EPGrabDTA.DispMod.VDD_Mod_List] ; Load Page Bitmap
	mov	dl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
        cmp     dl,0Dh
        je      short GPModeD
        cmp     dl,0Eh
        je      short GPModeE
	cmp	dl,0Fh
        je      short GPModeF10
        cmp     dl,10h
	je	short GPModeF10
        and     ax,1111111111000000B    ; Mode 12 - 10 pages atmost
        mov     si,codeOffset LinesTable12
        ret
GPModeF10:
        and     ax,0000000001111111B   ; Mode 10 - 7 pages atmost
        mov     si,codeOffset LinesTable10
        ret
GPModeD:
        and     ax,0000000000000011B   ; Mode D - 2 pages atmost
        mov     si,codeOffset LinesTableD
        ret
GPModeE:
        and     ax,0000000000001111B   ; Mode E - 4 pages atmost
        mov     si,codeOffset LinesTableE
        ret
    ; 
    ; Each 4K of video memory corresponds to a set of lines on the screen
    ; These tables map video page number to the line number range on the screen 
    ; 
LinesTableD LineTabStruc <0,103*2>
            LineTabStruc <103*2,400>
LinesTableE LineTabStruc <0,52*2>
            LineTabStruc <52*2,104*2>
            LineTabStruc <104*2,156*2>
            LineTabStruc <156*2,400>
LinesTable10 LineTabStruc <0,52>
            LineTabStruc <52,104>
            LineTabStruc <104,156>
            LineTabStruc <156,208>
            LineTabStruc <208,260>
            LineTabStruc <260,312>
            LineTabStruc <312,350>
LinesTable12 LineTabStruc <0,52>
             LineTabStruc <52,104>
             LineTabStruc <104,156>
             LineTabStruc <156,208>
             LineTabStruc <208,259>
             LineTabStruc <260,312>
             LineTabStruc <312,364>
             LineTabStruc <364,416>
             LineTabStruc <416,468>
             LineTabStruc <468,480>

GetPageBitmap endp

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
;                Left = L + ColOrg
;                Right = R + ColOrg
;                Top = T + RowOrg
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
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7	; Text mode?
	je	short PPText1		
        cmp     [si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4   ; Text mode?
	jae	short NotText1		; No, skip
PPText1:
	cwd
	idiv	di
	mov	[si.PTVDRect.rcLeft],ax   ; first column of repaint
	mul	di
	sub	ax,bx
	mov	[si.TDXpos],ax		; bit position on screen
NotText1:
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
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7
	je	short PPText2
        cmp     [si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4   ; Text mode?
	jae	short NotText2		; No, skip
PPText2:
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
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows]
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
NotText2:
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

        lds     si,lpPntStruc
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
	or	ebx,ebx 	       ; Q: Black ?
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
