;--------------------------------------------------------------------
;
;   Screen Grabber for HERC adaptor - HERCUTIL.ASM
;
;   These routines perform paints and all other Display specific
;	aspects of WINOLDAP (VMDOSAPP)
;
;    (C) Copyright MICROSOFT Corp. 1986-1990
;
;    ************* MICROSOFT CONFIDENTIAL ******************
;

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
	NORASTOPS	= 1
	NOMETAFILE	= 1
	NOWM		= 1
	NOMDI		= 1
	NOWINMESSAGES	= 1
	NOSYSMETRICS	= 1
	NOCOLOR 	= 1
	NOCOMM		= 1

	include windows.inc
	include vmda.inc
	include grabpnt.inc
	.list
	include grabmac.inc
	include vmdaherc.inc
	include herc.inc
	include statusfl.inc

IF1
    IFDEF DEBUG
	%out DEBUG VERSION!!!!!
    ENDIF
ENDIF

;-----------------------------------------------
;
; External Windows Procedures
;
externFP	BitBlt
externFP	InvertRect
externFP	CreateCaret
externFP	SetCaretPos
externFP	SetDIBits
ifdef	DBCS
if1
%out DBCS code enabled
endif
externFP	IsDBCSLeadByte
endif

;-----------------------------------------------
;
; External GRABBER Procedures
;
externNP	ModeGrfx

sBegin	code
	assumes cs,code

        public  RenderRectText
        public  RenderRectGrfx
	public	ScreenAdjust
	public	ComputeSelTextRect
	public	InvSel,InvSel2
	public	AdjustSelGrxMax
	public	AdjustSelTextMax
	public	CursorPos
	public	CheckCtrlState
	public	GetVidSel
	public	ClearVidSel
	public	MakeNewCursor
	public	LineBlt
	public	GetTxtColor
ifdef	DBCS
	public	AdjustDBCSRenderRect
	public	AdjustDBCSBound
	public	IsDBCSMiddlePos
	public	IsDBCSMiddleText
endif

;**
;
; ComputeSelTextRect - Compute bounds of text selection rectangle
;
; ENTRY:
;	DS:SI -> Extended paint structure
;	DS:BX -> TXTPAINTSUB sub structure of DS:SI
; EXIT:
;	[si.SelStruc.GrabArea.Irect] contains text rectangle info
;	    First Word is Row,Col of Upper Left
;	    Second Word is Row,Col of Lower Right
;	    Third Word is Hight,Width in chars of rect
;	DX = The Third word (can be tested for zero to detect NUL selection)
; USES:
;	AX,CX,DX,FLAGS
;
ComputeSelTextRect proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	mov	ax,[si.SelStruc.GrabArea.StartPointX]
	mov	cx,[si.SelStruc.GrabArea.EndPointX]
	cmp	ax,cx				; This way?
	jbe	short OkIX2			; Yes
	xchg	ax,cx				; Nope, other way
OkIX2:
	mov	dx,[bx.TPFntWid]
	div	dl				; AL is Left Col
	xchg	ax,cx				; CL is Left Col
	div	dl				; AL is Right Col
	mov	ah,cl				; AH is Left Col
	push	ax

	mov	ax,[si.SelStruc.GrabArea.StartPointY]
	mov	cx,[si.SelStruc.GrabArea.EndPointY]
	cmp	ax,cx				; This way?
	jbe	short OkIY2			; Yes
	xchg	ax,cx				; Nope, other way
OkIY2:
	mov	dx,[bx.TPFntHgt]
	div	dl				; AL is Top Row
	xchg	ax,cx				; CL is Top Row
	div	dl				; AL is Bottom Row
	mov	ah,cl				; AH is Top Row, AL is Bottom
						; Row
	pop	dx				; DH is Left Col, DL is Right
						; Col
	xchg	al,dh				; AH is Top Row, AL is Left Col
						; DH is Bottom Row, DL is Right
						; Col
	mov	[si.SelStruc.GrabArea.Irect.rcLeft],ax
	mov	[si.SelStruc.GrabArea.Irect.rcTop],dx
	sub	dh,ah				; DH is height in Lines
	sub	dl,al				; DL is Width in Cols
	mov	[si.SelStruc.GrabArea.Irect.rcRight],dx
	ret

ComputeSelTextRect endp

;**
;
; InvSel InvSel2 - Invert selection
;
;   InvSel computes the invert rectangle
;   InvSel2 assumes InvSel called previously to compute rectangle
;
; ENTRY:
;	SS:BP is graphics paint frame pointer
;	DS:SI -> Extended paint structure
; EXIT:
;	Selection region inverted in ScreenDC if selection is on
; USES:
;	ALL but DS,SI,DI
;
InvSel proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn ; In progress?
	jz	NoSelP				; No, done

	mov	ax,[si.SelStruc.GrabArea.StartPointX]
	mov	cx,[si.SelStruc.GrabArea.EndPointX]
	cmp	ax,cx				; This way?
	jbe	short OkIX			; Yup
	xchg	ax,cx				; Nope, other way
OkIX:
	mov	[si.SelStruc.GrabArea.Irect.rcLeft],ax
	mov	[si.SelStruc.GrabArea.Irect.rcRight],cx

	mov	ax,[si.SelStruc.GrabArea.StartPointY]
	mov	cx,[si.SelStruc.GrabArea.EndPointY]
	cmp	ax,cx				; This way?
	jbe	short OkIY			; Yup
	xchg	ax,cx				; Nope, other way
OkIY:
	mov	bx,DDRCTop
	cmp	bx,cx
	ja	short NoSelSet			; Bottom of sel is above paint
	add	bx,DDRCHig			; BX is bottom of paint
	cmp	bx,ax
	jb	short NoSelSet			; Top of sel is below paint
	sub	ax,DDRCTop
	jae	short SetST
	xor	ax,ax
SetST:
	mov	[si.SelStruc.GrabArea.Irect.rcTop],ax
	mov	ax,DDRCHig
	sub	bx,cx
	jae	short SetSB
	xor	bx,bx
SetSB:
	sub	ax,bx
	mov	[si.SelStruc.GrabArea.Irect.rcBottom],ax
	mov	ax,[si.SelStruc.GrabArea.Irect.rcLeft]
	mov	cx,[si.SelStruc.GrabArea.Irect.rcRight]
	cmp	DDRCLeft,cx
	ja	short NoSelSet			; Sel right is to left of paint
	mov	bx,DDRCRight
	cmp	bx,ax
	jb	short NoSelSet			; Sel left is to right of paint
	sub	ax,DDRCLeft
	jae	short SetSL
	xor	ax,ax
SetSL:
	mov	[si.SelStruc.GrabArea.Irect.rcLeft],ax
	mov	ax,DDRCWid
	sub	bx,cx
	jae	short SetSR
	xor	bx,bx
SetSR:
	sub	ax,bx
	mov	[si.SelStruc.GrabArea.Irect.rcRight],ax

InvSel2:
	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn
	jz	short NoSelP
	cmp	[si.SelStruc.GrabArea.Irect.rcTop],-1	; InvSel comp ok?
	jz	short NoSelP			; No
    ;
    ; Invert this rect in the screen DC
    ;
	push	ScreenDC			; hSrcDC
	push	ds				; lpRect
	lea	ax,[si.SelStruc.GrabArea.Irect]
	push	ax
	cCall	InvertRect
NoSelP:
	ret

NoSelSet:
	mov	[si.SelStruc.GrabArea.Irect.rcTop],-1	; Flag InvSel2
	ret
InvSel	endp

;**
;
; RenderRectText - Render the indicated rectangle
;
; ENTRY:
;	DS:BX -> Extended paint structure
;       DS:SI -> Rectangle in Row/Col coords, is Text mode
;		       First word is Row,Col of upper left
;		       Second word is Row,Col of lower right
;		       Third Word is Hight,Width in chars
;	ES:DI -> Memory to place rendering in 
; EXIT:
;	If Error
;               Carry set
;	else
;	        Carry Clear
; USES:
;	ALL but ES,DS

RenderRectText proc near

    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

    ; 
    ; BX is used to address the local variables in this procedure
    ; Set up local variables
    ;
        push    bx              ; save Old BX
        push    bx              ; space for LinWidTR
	push	[si.rcLeft]
	push	[si.rcTop]
	push	[si.rcRight]
ifdef	DBCS
	push	ds
	mov	bx, sp
	add	bx, 12
else
        mov     bx, sp
        add     bx, 10          ; BX is SP on entry to this procedure
endif
OldBX           equ word ptr    ss:[bx-2]
LinWidTR        equ word ptr    ss:[bx-4]
TxTopRw         equ byte ptr    ss:[bx-5]
TxLftCol        equ byte ptr    ss:[bx-6]
TxBotRw         equ byte ptr    ss:[bx-7]
TxRgtCol        equ byte ptr    ss:[bx-8]
TxHgt	        equ byte ptr    ss:[bx-9]
TxWid	        equ byte ptr    ss:[bx-10]
ifdef	DBCS
OldDS		equ word ptr	ss:[bx-12]	
endif	; DBCS

    ;
    ; Compute parameters of rectangle
    ;
	mov	cx,80*2 		; cx is bytes per line
	mov	LinWidTR,cx
	mov	al,TxTopRw
	mul	cl			; ax is byte index of first line
    ;
    ; Note that we ignore errors! This means we will grab "random trash"
    ;
	push	es              ; preserve es:di ptr to render buffer
	push	di
        les     edi,VidAddr
	movzx	eax,ax
	add	edi,eax		; Point to first char of first line
	mov	esi,edi
	push	es
	pop	ds
    ;
    ; Now ds:esi points to first char of first line in video memory
    ;
	pop	di
	pop	es              ; restore ptr to Render buffer
	mov	cl,TxWid
	mov	ch,TxHgt
	xor	edx,edx
	mov	dl,TxLftCol
	shl	dx,1			; edx is byte index of left char
nextlint:
	push	cx			; Save width and height
	push	esi			; Save start of line
	push	di			; Save start of line in target buffer
ifdef	DBCS
	xor	ch,ch			; CX is width
	call	AdjustDBCSRenderRect
else	; DBCS
	add	esi,edx 		; Go to left char of selection
	xor	ch,ch			; CX is width
endif
	cld
Dochr:
        lods    word ptr ds:[esi]
	or	al,al
	jz	short FixNul
NulFixed:
	stosb				; store char
	loop	short Dochr
    ;
    ; Now remove any trailing spaces from this line
    ;
	pop	ax			; This limits how much we will back up
Bkup:
	cmp	di,ax			; Check limit
	jbe	short SetLin		; Hit limit, done
	cmp	byte ptr es:[di-1]," "	; Trailing space?
	jnz	short SetLin		; No, done
	dec	di			; Remove trailing space
	jmp	short Bkup		; Look for more

    ;
    ; NUL is a valid OEM key (looks like a space), but if we stick a NUL in the
    ;	text, it terminates it. So we change NULs to SPACES.
    ;
FixNul:
	mov	al," "
	jmp	short NulFixed

SetLin:
	mov	ax,0A0DH
	stosw				; CR LF at end of line
	pop	esi			; recover start of line
	movzx	ecx,LinWidTR		; Next line
	add	esi,ecx
	pop	cx			; recover height width
	dec	ch			; One line done
	jnz	short nextlint		; More lines

	xor	ax,ax			; null terminate entire string
	stosb
	mov	si,OldBX
	push	ss
	pop	ds
	push	es
	call	ClearVidSel
	pop	es
ifdef	DBCS
	add	sp, 12
else
        add     sp, 10                  ; clean the stack - remove local vars
endif
	clc
        ret
RenderRectText  endp

ifdef	DBCS
;**
;
; AdjustDBCSRenderRect - Adjust render rect for DBCS character
;
; ENTRY:
;       DS:ESI -> Point to first char of the line
;	EDX    = Offset of left char
;	CX     = Width
; EXIT:
;	DS:ESI -> Point to left char of selection
;	CX     = Adjusted width
; USES:
;	ALL FLAGS
;
AdjustDBCSRenderRect	proc	near

    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	di
	push	dx

	mov	di,OldBX
	push	ds
	mov	ax,OldDS
	mov	ds,ax
	cmp	[di.EPGrabDTA.CntrlSt.VDA_HERC_Mode],HERC_Mode_Text
	pop	ds
	jnz	ADRX

	call	IsDBCSMiddleText    ;left bound sit on a DBCS 2nd byte ?
	jc	short ADR0
	sub	edx,2		    ;yes, set left bound to DBCS 1st byte
	inc	cx		    ;and increase the line width
ADR0:
	push	dx
	add	dx,cx
	add	dx,cx		    ;to the right bound
	call	IsDBCSMiddleText    ;right bound sit on a DBCS 1st byte ?
	jc	short ADR1
	inc	cx		    ;yes, we have to out one more for 2nd byte
ADR1:
	pop	dx
ADRX:
	add	esi,edx
	pop	dx
	pop	di
	ret

AdjustDBCSRenderRect	endp

endif	;DBCS

;**
;RenderRectGrfx
;
; ENTRY:
;	DS:BX -> Extended paint structure
;       DS:SI -> Rectangle in screen coords
;		  NOTE: X coords known to be byte aligned
;			due to ScreenAdjust
; EXIT:
;       If Error
;	        Carry set
;	else
;	        Carry Clear
;	        DS:SI -> Extended Paint Structure
;	        AX = BITMAP handle
; USES:
;	ALL but ES,DS
;

RenderRectGrfx proc near

	mov	ax,[si.rcLeft]
	mov	[bx.PGVDRect.rcLeft],ax
	mov	ax,[si.rcTop]
	mov	[bx.PGVDRect.rcTop],ax
	mov	ax,[si.rcRight]
	mov	[bx.PGVDRect.rcRight],ax
	mov	ax,[si.rcBottom]
	mov	[bx.PGVDRect.rcBottom],ax
	mov	si,bx
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_HERC_Mode]
	mov	cx,1			; Build BITMAP
	call	ModeGrfx                ; a procedure which builds DIBs
	or	ax,ax			; Worked? (clears carry too)
	jz	short RRGD_Error	; No
	ret

RRGD_Error:
	stc
	ret

RenderRectGrfx endp

;**
;
; AdjustSelGrxMax - Adjust Graphics Selection end point for MAX X,Y
;
; INPUT:
;	DX,AX is Y,X of endpoint
; EXIT:
;	DX,AX adjusted
; USES:
;	DX,AX,FLAGS
;
AdjustSelGrxMax proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	cmp	ax,GrxBitWid
	jbe	short TXMinG
	mov	ax,GrxBitWid
TXMinG:
	cmp	dx,GrxBitHeight
	jbe	short TYMinG
	mov	dx,GrxBitHeight
TYMinG:
	ret

AdjustSelGrxMax endp

;**
;
; AdjustSelTextMax - Adjust Text Selection end point for MAX X,Y
;
; INPUT:
;	DS:BX -> Extended paint structure
;	DX,AX is Y,X of endpoint
;	CX is Font width
;	SI is Font height
; EXIT:
;	DX,AX adjusted
; USES:
;	DX,AX,CX,FLAGS
;
AdjustSelTextMax proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	dx
	push	ax
	mov	ax,80
	mul	cx			; AX is max width
	mov	dx,ax
	pop	ax
	cmp	ax,dx
	jbe	short OKX
	xchg	ax,dx
OKX:
	mov	cx,ax
	xor	ax,ax
	mov	al,25			; 25 rows of text
	mul	si			; AX is max height
	pop	dx
	cmp	dx,ax
	jbe	short OKY
	xchg	ax,dx
OKY:
	mov	ax,cx
	ret

AdjustSelTextMax endp

;**
;
; ScreenAdjust - Adjust end point if appropriate
;
; ENTRY:
;	DX,AX = (Y,X) screen coord of new end point
;	DS:BX -> Extended Paint structure
; EXIT:
;	DX,AX adjusted for screen size maxima
;	      adjusted for character alignment if text mode screen
;	      Adjusted for Byte alignment if graphics mode screen
; USES:
;	AX,DX,CX,SI,FLAGS
;
; For Text: 
;       Get Font Width and Height
;       /* Round EndPointX to nearest char boundary */
;       If (EndPointX > StartPointX) /* forward sel */
;                EndPointX = ((EndPointX+FontWid-1)/FontWid) * FontWid;
;       else    
;                EndPointX = EndPointX/FontWid;
;       Similarly for EndPointY using Font Height.
;
;       Adjust end point for maxima
;
; Graphics:
;       /* Round EndPointX to Dword boundary */
;       If (EndPointX > StartPointX) /* forward sel */
;                EndPointX = ((EndPointX+32-1)/32) * 32;  
;       else    
;                EndPointX = EndPointX/32; 
;       /* Round EndPointY to dWord boundary */
;       If (EndPointY > StartPointY) /* downward selection */
;               EndPointY =((EndPointY+3)/4) * 4;
;       else
;               EndPointY = EndPointY/4;
;       Adjust end point for maxima
;****************************************************************************
ScreenAdjust proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_HERC_Mode],HERC_MODE_Grfx
	je	short SA_Grfx
    ;
    ; Text Adjust to char boundaries
    ;
SA_Text:
	mov	cx,[bx.AltFnt1.FontWid]
	mov	si,[bx.AltFnt1.FontHgt]
	cmp	ax,[bx.SelStruc.GrabArea.StartPointX]
	jbe	short RoundDownX
	add	ax,cx
	dec	ax
RoundDownX:
	push	dx
	xor	dx,dx
	div	cx
	mul	cx
	pop	dx
DoY:
	xchg	ax,dx
	cmp	ax,[bx.SelStruc.GrabArea.StartPointY]
	jbe	short RoundDownY
	add	ax,si
	dec	ax
RoundDownY:
	push	dx
	xor	dx,dx
	div	si
	mul	si
	pop	dx
	xchg	ax,dx
	call	AdjustSelTextMax
	ret

    ;
    ; Graphics adjust
    ;
SA_Grfx:
	cmp	ax,[bx.SelStruc.GrabArea.StartPointX]
	jbe	short RoundDownGrxX
	add	ax,31
RoundDownGrxX:
	and	ax,1111111111100000B
DoYGrx:
	cmp	dx,[bx.SelStruc.GrabArea.StartPointY]
	jbe	short RoundDownGrxY
	add	dx,3
RoundDownGrxY:
	and	dx,1111111111111100B
	call	AdjustSelGrxMax
	ret

ScreenAdjust endp

;**
;
; CheckCtrlState - Check if the controller state is valid
;
; ENTRY:
;	SS:BP -> Std stack frame
;	DS:SI -> EXTPAINTSTRUC structure
; EXIT:
;	DS:BX -> EXTPAINTSTRUC structure
;	DS:SI -> EXTPAINTSTRUC structure
; USES:
;	ALL but BP,AX,DX
;
CheckCtrlState	proc	near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

	mov	bx,si
	bts	[si.EPStatusFlags],fVValidBit ; Valid state(?)
	jc	short CCSR		; Yes, done
    ;
    ; Suck out the current controller state
    ;
        push    eax

	test	GrbFlags,GrbFlags_DoingGrab
	jz	short NotGrab3
	test	GrbFlags,GrbFlags_DoingWindowSelGrab
	jnz	short NotGrab3
	mov	ax,GRB_Get_GrbState
	jmp	short GetGrbCtrl
NotGrab3:
	mov	ax,GRB_Get_State
GetGrbCtrl:
	mov	ebx,[si.ThisVMHand]
IFDEF   DEBUG
	mov	cx,(SIZE VDA_HERC_State)
ENDIF
	push	ds
	pop	es
	lea	di,[si.EPGrabDTA.CntrlSt]
	movzx	edi,di
        call    [si.GGrbArea.VddApiProcAdr]
IFDEF   DEBUG
        or      cx,cx
        jnz     short CCSContinue1
	mov	bx,si
	mov	esi,codeOffset StateErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	mov	si,bx
	jmp	short CCSContinue1

StateErr  db	  "Grabber zero return on Get State",0

CCSContinue1:
ENDIF
	mov	bx,si
        pop     eax
	cmp	[si.EPGrabDTA.CntrlSt.VDA_HERC_Mode],HERC_MODE_Grfx
        jbe     short ModeAOK
BadMode:
	bts	[si.EPStatusFlags],fGrbProbBit	; Tell caller we can't deal
						; with this
ModeAOK:
	mov	bx,si
CCSR:
	ret

CheckCtrlState	endp

;**
;
; MakeNewCursor - Create, position, and Display a new cursor
;
; ENTRY:
;	DS:SI -> Extended paint structure
;	DX,AX = (Y,X) screen coord of lower left of cursor
;	      = (-1,-1) if no cursor
; EXIT:
;	Carret created and displayed in window
; USES:
;	ALL
;
MakeNewCursor proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	inc	dx
	jz	short COR	; No cursor
	dec	dx
	sub	ax,[si.ColOrg]	; Org onto display
	push	ax		; X for call to SetCaretPos
	sub	dx,[si.RowOrg]	; Org onto display
	push	dx		; Y for call to SetCaretPos
    ;
    ; Create the caret
    ;
	push	[si.WindHand]	; hWnd
	push	0		; hBitmap (flashing block)
	mov	ax,[si.AltFnt1.FontWid]
	push	ax		; nWidth
	mov	al,[si.EPGrabDTA.CntrlSt.VDA_HERC_Mode]
	mov	[si.EPGrabDTA.CurCursMode],al

	mov	ax,[si.EPGrabDTA.CntrlSt.VDA_HERC_CurEnd]
	mov	[si.EPGrabDTA.CurCursEnd],ax

	mov	bx,[si.EPGrabDTA.CntrlSt.VDA_HERC_CurBeg]
	mov	[si.EPGrabDTA.CurCursBeg],bx

	sub	ax,bx		;Set Cursor height.
	jb	short NoCursor	;Negative height means no Cursor.
	inc	ax
    ;
    ; The HERC physical font is 14 high, but our windows font is 13 high
    ; so we have to fudge a little. 1-7 ok, 8-14, dec by one
    ; Now no fudging, be true to current font size...
    ;
	cmp	ax,14
	jbe	short HgtInrng
	mov	ax,14			; first limit to 14
HgtInrng:
	mul	[si.AltFnt1.FontHgt]	; multiply by current font height
	mov	cx,14
	div	cx			; size it down
	shr	cx,1
	cmp	dx,cx			; round off
	jbe	SHORT HgtDone
	inc	ax
HgtDone:
	push	ax			; nHeight
	cCall	CreateCaret
    ;
    ; Put the caret at its correct position
    ;
	cCall	SetCaretPos		; Args pushed above
	ret
    ;
    ; Difference between cursor end and cursor start scan lines is negative!
    ;
NoCursor:
	xor	ax,ax			; No cursor
	jmp	short HgtDone

COR:
	mov	[si.EPGrabDTA.CurCursEnd],ax
	mov	[si.EPGrabDTA.CurCursBeg],ax
	mov	[si.EPGrabDTA.CurCursMode],al
	ret

MakeNewCursor endp

;**
;
; GetVidSel - Get selector to video memory, 
;
; ENTRY:
;	SS:BP -> Std stack frame
;	DS:SI -> EXTPAINTSTRUC structure
; EXIT:
;	Carry Clear
;	    ES:EDI -> start of first video page
;	Carry Set
;	    ERROR!!! App cant run in a Window
;
;	NOTE WARNING!!!!! SS=DS ASSUMED!!!!!!
; USES:
;	ES,DI,FLAGS
;
GetVidSel proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	esi
	push	ecx
	push	ebx
	push	eax

	lea	di,MemState
	movzx	edi,di
	cmp	VidSel,0
	jnz	short MapDone
	mov	ax,[si.RNG1DSSEL]
	mov	VidSel,ax

	push	ss
	pop	es

	test	GrbFlags,GrbFlags_DoingGrab
	jz	short NotGrab1
	test	GrbFlags,GrbFlags_DoingWindowSelGrab
	jnz	short NotGrab1
	mov	ax,GRB_Get_GrbMem
	jmp	short GetMemCall
NotGrab1:
	mov	ax,GRB_Get_Mem
GetMemCall:
	mov	ebx,[si.ThisVMHand]
IFDEF   DEBUG
        mov     cx, (SIZE VDA_Mem_State)
ENDIF
        call    [si.GGrbArea.VddApiProcAdr]
IFDEF   DEBUG
        or      cx,cx
        jnz     short GVSContinue1
	push	dx
	push	bx
	push	esi
	mov	bx,si
	mov	esi,codeOffset GMErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	pop	esi
	pop	bx
	pop	dx
	jmp	short GVSContinue1

GMErr  db      "Grabber zero return on Get Mem",0
GVSContinue1:
ENDIF        

MapDone:
	xor	ecx,ecx 	        ; Plane 0
	GetStartDispAddr eax,edi,ecx,ss
	mov	PgOffst,eax
	and	ax,0FFFH
	jz	short VarsSet
    ;
    ; Not on a 4k boundary, MUST be a Text mode
    ;
	cmp	[si.EPGrabDTA.CntrlSt.VDA_HERC_Mode],HERC_MODE_Grfx
	jb	short VarsSet
    ;
    ; Have a graphics mode which doesn't fall on a 4k page boundary ERROR!
    ;
IFDEF DEBUG
	mov	bx,si
	mov	esi,codeOffset AlignErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short GVSContinue3

AlignErr  db	  "Grabber Graphics mode not on 4k boundary",0

GVSContinue3:
	mov	si,bx
ENDIF
	bts	[si.EPStatusFlags],fGrbProbBit ; Tell caller we can't deal with this
	stc				; Flag error
	jmp	short BadMap		; Bomb out

VarsSet:
	les	edi,VidAddr
	clc
BadMap:
	pop	eax
	pop	ebx
	pop	ecx
	pop	esi
	ret
        
GetVidSel endp

;**
;
; ClearVidSel - Release the Vid selector
;
; ENTRY:
;	SS:BP -> Std stack frame
;	DS:SI -> EXTPAINTSTRUC structure
; EXIT:
;	Selector released
;
; NOTE: This routine is a NOP if GrbFlags_DoingGrab is set
;	and GrbFlags_DoingWindowSelGrab is clear. We release
;	Grab memory in a different place (GrabComplete)
;
; USES:
;	ALL but DS,SI,DI,BP (C standard)
;
ClearVidSel proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

	xor	cx,cx
	xchg	cx,VidSel
	jcxz	short CVMR
	test	GrbFlags,GrbFlags_DoingGrab
	jz	short NotGrab2
	test	GrbFlags,GrbFlags_DoingWindowSelGrab
	jz	short CVMR
NotGrab2:
	mov	ax,GRB_Free_Mem
	mov	ebx,[si.ThisVMHand]
        call    [si.GGrbArea.VddApiProcAdr]
CVMR:
	ret

ClearVidSel endp

;**
;
; CursorPos - Return position of cursor on display
;
; ENTRY:
;	DS:SI -> EXTPAINTSTRUC structure
; EXIT:
;	DX,AX = (Y,X) screen coord of upper left of cursor
;	      = (-1,-1) if no cursor
; USES:
;	AX,CX,DX,FLAGS
;
CursorPos  proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	cmp	[si.EPGrabDTA.CntrlSt.VDA_HERC_Mode],HERC_MODE_Grfx
	je	short CPR		; Graphics, no cursor
	mov	ax,[si.EPGrabDTA.CntrlSt.VDA_HERC_CurY]
	or	ah,ah
	jnz	short CPR		; Cursor is off screen
	cmp	al,25			; Past 25th line?
	jae	short CPR		; Cursor is off screen
	mov	cx,[si.AltFnt1.FontHgt]
	mul	cx			; ax = cur pos
	mov	dx,ax

    ; Scale the cursor position (if necessary) to account for the higher

	mov	ax,[si.EPGrabDTA.CntrlSt.VDA_HERC_CurBeg]
	cmp	ax,[si.EPGrabDTA.CntrlSt.VDA_HERC_CurEnd]
	ja	short CPR		; Cursor height is < 0, no cursor
    ;
    ; Scale the cursor position to account for user selectable fonts
    ; cursor_pos = cursor_pos * fonthgt / 14
    ;
	push	dx
	mul	cx
	mov	cx,14
	div	cx
	shr	cx,1			; for rounding off
	cmp	dx,cx
	jbe	SHORT CurNoAdj
	inc	ax
CurNoAdj:
	pop	dx
CurBegOK:
	add	ax,dx
	push	ax
	mov	ax,[si.EPGrabDTA.CntrlSt.VDA_HERC_CurX]
col80Cur:
	cmp	ax,79
	ja	short CPRP		; Cursor is off screen
	mov	cx,[si.AltFnt1.FontWid]
CP1:
	mul	cx			; AX is X pos of cursor
	pop	dx			; Y pos to dx
	ret

CPRP:
	pop	ax			; Clean stack
CPR:
	mov	dx,-1
	mov	ax,dx
	ret

CursorPos endp


;**
;
; LineBlt - Blit the one line bitmap into the display bitmap
;
; ENTRY:
;	SS:BP -> Graphics paint frame
;	DS:SI -> ExtPaintStruc
;	AX = Line # in display bitmap of line
; EXIT:
; USES:
;	ALL but DI,SI,BP,DS
;
LineBlt proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
    ;
    ; DIB is upside Line L is actually (DDRCHig-L-1)
    ;
        neg     ax
        add     ax,DDRCHig
        dec     ax
	push	ax			; Save line #
    ; 
    ; call SetDIBits(hDC, hBM, nStartScan, nNumScans, lpBits, lpDIBHdr, wUsage)
    ; 
        push    ScreenDC                ; hDC
	push	ScreenBitmap		; hBitmap
        push    ax                      ; nStartScan
        push    1                       ; nNumScans
	push	ss			; lpBits
	lea	ax,LineBits
	test	ax,0000000000000011B	; Dword aligned?
	jz	short DwAlign21		; Yes
	inc	ax			; Go to dword align
	inc	ax
DwAlign21:
	push	ax
        push    ss                      ; DIBHdr
        lea     ax, DIBHdr
        push    ax                      
        push    0                       ; wUsage
	cCall	SetDIBits
IFDEF DEBUG
        or      ax,ax
        jnz     short SSBContinue1
        push    bx
        push    esi
        push    edx
	mov	bx,si
	mov	esi,codeOffset SetDIBErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
        pop     edx
        pop     esi
        pop     bx
	jmp	short SSBContinue1

SetDIBErr db "SetDIBits failed",0

SSBContinue1:
ENDIF
        pop     ax                      ; Line #
	ret

LineBlt endp

ifdef	DBCS

;**
;
; AdjustDBCSBound - Adjust coordinate for DBCS charcter
;
; ENTRY:
;	DS:BX -> Extended paint structure
;	DX,AX = (Y,X) screen coord of upper left
;	DX,CX = (Y,X) screen coord of upper right
; EXIT:
;	DX,AX = (Y,X) Adjusted screen coord of upper left
;	DX,CX = (Y,X) Adjusted screen coord of upper right
; USES:
;	FLAGS	
;
AdjustDBCSBound proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	si
	push	edi
	push	es
	push	bx

	mov	si,bx
	call	GetVidSel

	push	cx
	mov	cx,[bx.AltFnt1.FontWid]
	mov	si,[bx.AltFnt1.FontHgt]
;	 call	 GetTextWidHgt
	call	IsDBCSMiddlePos 	;left bound on DBCS 2nd byte?
	jc	ADB0
	sub	ax,cx			;yes, adjust x coor to DBCS 1st byte
ADB0:
	xchg	bp, sp			;xchg ax, [esp]
	xchg	ax, [bp]		;save adjusted left bound and get
	xchg	bp, sp			;unadjusted right bound
	call	IsDBCSMiddlePos 	;right bound on DBCS 1st byte?
	jc	ADB1
	add	ax,cx			;yes, adjust right bound to DBCS 2nd byte
ADB1:
	mov	cx,ax
	pop	ax

	pop	bx
	pusha
	mov	si,bx
	call	ClearVidSel
	popa

	pop	es
	pop	edi
	pop	si
	ret

AdjustDBCSBound	endp

;**
;
; IsDBCSMiddlePos - Test if the coord is middle of DBCS character
;
; ENTRY:
;	DS:BX  -> EXTPAINTSTRUCT
;	ES:EDI -> Start of first video page
;	DX,AX = (Y,X) screen coord of upper left
;	SI,CX = (Hight, Width) of character
; EXIT:
;	Carry ON = Not middle of DBCS character
;             OFF =  Middle of DBCS character
; USES:
;	FLAGS	
;
IsDBCSMiddlePos proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	pushad

	push	dx
	xor	dx,dx
	div	cx
	mov	cx,ax
	pop	ax
	xor	dx,dx
	div	si		; CX,AX = (X, Y)
	mov	dx,80*2
	mul	dx
	movzx	esi,ax		; ES:ESI = Start line pos.
	add	esi,edi
	shl	cx,1
	add	ax,cx
	movzx	eax,ax
	add	edi,eax		; ES:EDI = Check pos.
IDP0:
	cmp	esi,edi
	je	short IDP2
	ja	short IDP3
	mov	ax,es:[esi]
	xor	ah,ah
	cCall	IsDBCSLeadByte,<ax>
	or	ax,ax
	jz	short IDP1
	add	si,2
IDP1:
	add	si,2
	jmp	short IDP0
IDP2:
	stc
	jmp	short IDPX
IDP3:
	clc
IDPX:
	popad
	ret

IsDBCSMiddlePos endp

;**
;
; IsDBCSMiddleText - Test if the character is middle of DBCS char
;
; ENTRY:
;       DS:ESI -> Point to first char of the line
;	EDX    = Offset of the char
; EXIT:
;	Carry ON = Not middle of DBCS character
;             OFF = Middle of DBCS character
; USES:
;	ALL FLAGS
;
IsDBCSMiddleText	proc	near

    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	pushad
IDM0:
	or	dx,dx
	jz	short IDM2
	js	short IDM3
	lods	word ptr ds:[esi]
	xor	ah,ah
	push	dx
	cCall	IsDBCSLeadByte,<ax>
	pop	dx
	or	ax,ax
	jz	short IDM1
	add	esi,2
	sub	dx,2
IDM1:
	sub	dx,2
	jmp	short IDM0
IDM2:
	stc
	jmp	short IDMX
IDM3:
	clc
IDMX:
	popad
	ret

IsDBCSMiddleText	endp

endif	; DBCS

;
; This table defines how to map a text attribute value into a monochrome
;   forground and background color. Table has 128 entries as the high bit
;   of a text attribute (blink bit) is ignored.
;
TxtColMapTable label byte
	db	0			; 0000000	00
	db	FgTxtCol		; 0000001	01
	db	FgTxtCol		; 0000010	02
	db	FgTxtCol		; 0000011	03
	db	FgTxtCol		; 0000100	04
	db	FgTxtCol		; 0000101	05
	db	FgTxtCol		; 0000110	06
	db	FgTxtCol		; 0000111	07
	db	0			; 0001000	08
	db	FgTxtCol		; 0001001	09
	db	FgTxtCol		; 0001010	0A
	db	FgTxtCol		; 0001011	0B
	db	FgTxtCol		; 0001100	0C
	db	FgTxtCol		; 0001101	0D
	db	FgTxtCol		; 0001110	0E
	db	FgTxtCol		; 0001111	0F

	db	FgTxtCol		; 0010000	10
	db	FgTxtCol		; 0010001	11
	db	FgTxtCol		; 0010010	12
	db	FgTxtCol		; 0010011	13
	db	FgTxtCol		; 0010100	14
	db	FgTxtCol		; 0010101	15
	db	FgTxtCol		; 0010110	16
	db	FgTxtCol		; 0010111	17
	db	FgTxtCol		; 0011000	18
	db	FgTxtCol		; 0011001	19
	db	FgTxtCol		; 0011010	1A
	db	FgTxtCol		; 0011011	1B
	db	FgTxtCol		; 0011100	1C
	db	FgTxtCol		; 0011101	1D
	db	FgTxtCol		; 0011110	1E
	db	FgTxtCol		; 0011111	1F

	db	FgTxtCol		; 0100000	20
	db	FgTxtCol		; 0100001	21
	db	FgTxtCol		; 0100010	22
	db	FgTxtCol		; 0100011	23
	db	FgTxtCol		; 0100100	24
	db	FgTxtCol		; 0100101	25
	db	FgTxtCol		; 0100110	26
	db	FgTxtCol		; 0100111	27
	db	FgTxtCol		; 0101000	28
	db	FgTxtCol		; 0101001	29
	db	FgTxtCol		; 0101010	2A
	db	FgTxtCol		; 0101011	2B
	db	FgTxtCol		; 0101100	2C
	db	FgTxtCol		; 0101101	2D
	db	FgTxtCol		; 0101110	2E
	db	FgTxtCol		; 0101111	2F

	db	FgTxtCol		; 0110000	30
	db	FgTxtCol		; 0110001	31
	db	FgTxtCol		; 0110010	32
	db	FgTxtCol		; 0110011	33
	db	FgTxtCol		; 0110100	34
	db	FgTxtCol		; 0110101	35
	db	FgTxtCol		; 0110110	36
	db	FgTxtCol		; 0110111	37
	db	FgTxtCol		; 0111000	38
	db	FgTxtCol		; 0111001	39
	db	FgTxtCol		; 0111010	3A
	db	FgTxtCol		; 0111011	3B
	db	FgTxtCol		; 0111100	3C
	db	FgTxtCol		; 0111101	3D
	db	FgTxtCol		; 0111110	3E
	db	FgTxtCol		; 0111111	3F

	db	FgTxtCol		; 1000000	40
	db	FgTxtCol		; 1000001	41
	db	FgTxtCol		; 1000010	42
	db	FgTxtCol		; 1000011	43
	db	FgTxtCol		; 1000100	44
	db	FgTxtCol		; 1000101	45
	db	FgTxtCol		; 1000110	46
	db	FgTxtCol		; 1000111	47
	db	FgTxtCol		; 1001000	48
	db	FgTxtCol		; 1001001	49
	db	FgTxtCol		; 1001010	4A
	db	FgTxtCol		; 1001011	4B
	db	FgTxtCol		; 1001100	4C
	db	FgTxtCol		; 1001101	4D
	db	FgTxtCol		; 1001110	4E
	db	FgTxtCol		; 1001111	4F

	db	FgTxtCol		; 1010000	50
	db	FgTxtCol		; 1010001	51
	db	FgTxtCol		; 1010010	52
	db	FgTxtCol		; 1010011	53
	db	FgTxtCol		; 1010100	54
	db	FgTxtCol		; 1010101	55
	db	FgTxtCol		; 1010110	56
	db	FgTxtCol		; 1010111	57
	db	FgTxtCol		; 1011000	58
	db	FgTxtCol		; 1011001	59
	db	FgTxtCol		; 1011010	5A
	db	FgTxtCol		; 1011011	5B
	db	FgTxtCol		; 1011100	5C
	db	FgTxtCol		; 1011101	5D
	db	FgTxtCol		; 1011110	5E
	db	FgTxtCol		; 1011111	5F

	db	FgTxtCol		; 1100000	60
	db	FgTxtCol		; 1100001	61
	db	FgTxtCol		; 1100010	62
	db	FgTxtCol		; 1100011	63
	db	FgTxtCol		; 1100100	64
	db	FgTxtCol		; 1100101	65
	db	FgTxtCol		; 1100110	66
	db	FgTxtCol		; 1100111	67
	db	FgTxtCol		; 1101000	68
	db	FgTxtCol		; 1101001	69
	db	FgTxtCol		; 1101010	6A
	db	FgTxtCol		; 1101011	6B
	db	FgTxtCol		; 1101100	6C
	db	FgTxtCol		; 1101101	6D
	db	FgTxtCol		; 1101110	6E
	db	FgTxtCol		; 1101111	6F

	db	BkTxtCol		; 1110000	70
	db	FgTxtCol		; 1110001	71
	db	FgTxtCol		; 1110010	72
	db	FgTxtCol		; 1110011	73
	db	FgTxtCol		; 1110100	74
	db	FgTxtCol		; 1110101	75
	db	FgTxtCol		; 1110110	76
	db	FgTxtCol		; 1110111	77
	db	BkTxtCol		; 1111000	78
	db	FgTxtCol		; 1111001	79
	db	FgTxtCol		; 1111010	7A
	db	FgTxtCol		; 1111011	7B
	db	FgTxtCol		; 1111100	7C
	db	FgTxtCol		; 1111101	7D
	db	FgTxtCol		; 1111110	7E
	db	FgTxtCol		; 1111111	7F


;;;;Txt48KColMapTable label byte
;;;;	    db	    FgTxtCol		    ; 0000	 00
;;;;	    db	    FgTxtCol		    ; 0001	 01
;;;;	    db	    FgTxtCol		    ; 0010	 02
;;;;	    db	    FgTxtCol		    ; 0011	 03
;;;;	    db	    BkTxtCol		    ; 0100	 04
;;;;	    db	    BkTxtCol		    ; 0101	 05
;;;;	    db	    BkTxtCol		    ; 0110	 06
;;;;	    db	    BkTxtCol		    ; 0111	 07
;;;;	    db	    FgTxtCol		    ; 1000	 08
;;;;	    db	    FgTxtCol		    ; 1001	 09
;;;;	    db	    FgTxtCol		    ; 1010	 0A
;;;;	    db	    FgTxtCol		    ; 1011	 0B
;;;;	    db	    BkTxtCol		    ; 1100	 0C
;;;;	    db	    BkTxtCol		    ; 1101	 0D
;;;;	    db	    BkTxtCol		    ; 1110	 0E
;;;;	    db	    BkTxtCol		    ; 1111	 0F


;**
;
; GetTxtColor - Convert text attribute to Monochrome attribute
;
; ENTRY:
;	AL = Attribute of character
;	DS:SI -> Extended paint structure
; EXIT:
;	AL = Mono Color map (FgTxtCol and BkTxtCol value)
;     NOTE that this routine handles the mapping of 48K RAMFONT mode
;	characters to normal looking characters
; USES:
;	AL
;
GetTxtColor proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	bx
;;;;	    cmp     [si.EPGrabDTA.CntrlSt.VDA_HERC_Mode],HERC_MODE_48KText
;;;;	    je	    short Map48K
	and	al,01111111B	; Mask off blink bit
	mov	bx,offset TxtColMapTable
Domap:
	xlat	cs:[bx]
	pop	bx
	ret

;;;;Map48K:
;;;;	    shr     al,4		    ; Get Attributes in low 4 bits
;;;;	    mov     bx,offset Txt48KColMapTable
;;;;	    jmp     short Domap

GetTxtColor endp

sEnd	code
	end
