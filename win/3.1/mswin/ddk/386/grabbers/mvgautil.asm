;--------------------------------------------------------------------
;
;   Screen Grabber for Mono VGA
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

	include    windows.inc
	include    vmda.inc
	include    grabpnt.inc
	.list
	include    grabmac.inc
	include    vmdaega.inc
	include    mvga.inc
	include    statusfl.inc

IF1
    IFDEF DEBUG
	%out DEBUG VERSION!!!!!
    ENDIF
ENDIF

;-----------------------------------------------
;
; External Windows Procedures
;
externFP	CreateBitmap
externFP	BitBlt
externFP	InvertRect
externFP	CreateCaret
externFP	SetCaretPos
externFP	SetDIBits
externFP	SetDIBitsToDevice

;-----------------------------------------------
;
; External GRABBER Procedures
;
externNP	ModeGrfx
externNP	LookupPalette

sBegin	code
	assumes cs,code


	public	ScreenAdjust
	public	ComputeSelTextRect
	public	RenderRectGrxDisplay
	public	RenderRectText
	public	InvSel,InvSel2
        public  GetTextWidHgt
	public	AdjustSelGrxMax
	public	AdjustSelTextMax
	public	CursorPos
	public	CheckCtrlState
	public	GetVidSel
	public	ClearVidSel
	public	MakeNewCursor
	public	SetScreenBMBits

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
	mov	ah,cl				; AH is Top Row, AL is Bottom Row
	pop	dx				; DH is Left Col, DL is Right Col
	xchg	al,dh				; AH is Top Row, AL is Left Col
						; DH is Bottom Row, DL is Right Col
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
	ja	NoSelSet			; Bottom of sel is above paint
	add	bx,DDRCHig			; BX is bottom of paint
	cmp	bx,ax
	jb	NoSelSet			; Top of sel is below paint
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
	ja	NoSelSet			; Sel right is to left of paint
	mov	bx,DDRCRight
	cmp	bx,ax
	jb	NoSelSet			; Sel left is to right of paint
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
	jz	NoSelP
	cmp	[si.SelStruc.GrabArea.Irect.rcTop],-1	; InvSel comp ok?
	jz	short NoSelP			; No
	bt	GrbFlags,GrbFlgs_GrfxDispBltBit	; Special stuff?
	jnc	short DoInvScreen		; No
	bts	GrbFlags,GrbFlgs_GrfxInvSelReCompBit
	jc	short DoInvSelDisp		; Already done computation
    ;
    ; [si.SelStruc.GrabArea.Irect] is selction rect in Full Screen DC which
    ;	we are not using. We need to modify the rect a little bit to undo
    ;	the rounding then convert it in to a rectangle on the screen
    ;
	lea	bx,[si.SelStruc.GrabArea.Irect]
    ;
    ; Resolve Right and Left
    ;
	mov	ax,[si.PGVDRect.rcLeft]
	sub	ax,DDRCLeft
	sub	[bx.rcLeft],ax
	sub	[bx.rcRight],ax
	mov	ax,[si.Pstruct.PSrcPaint.rcLeft]
	add	[bx.rcRight],ax
	add	[bx.rcLeft],ax
	cmp	[bx.rcLeft],ax
	jae	short InvS10
	mov	[bx.rcLeft],ax
InvS10:
	mov	ax,[si.Pstruct.PSrcPaint.rcRight]
	cmp	[bx.rcRight],ax
	jbe	short InvS20
	mov	[bx.rcRight],ax
InvS20:
	mov	ax,[bx.rcRight]
	cmp	ax,[bx.rcLeft]
	jbe	short NoSelSet
    ;
    ; Resolve Bottom and Top
    ;
	mov	ax,[si.PGVDRect.rcTop]
	sub	ax,DDRCTop
	sub	[bx.rcTop],ax
	sub	[bx.rcBottom],ax
	mov	ax,[si.Pstruct.PSrcPaint.rcTop]
	add	[bx.rcBottom],ax
	add	[bx.rcTop],ax
	cmp	[bx.rcTop],ax
	jae	short InvS30
	mov	[bx.rcTop],ax
InvS30:
	mov	ax,[si.Pstruct.PSrcPaint.rcBottom]
	cmp	[bx.rcBottom],ax
	jbe	short InvS40
	mov	[bx.rcBottom],ax
InvS40:
	mov	ax,[bx.rcBottom]
	cmp	ax,[bx.rcTop]
	jbe	short NoSelSet
DoInvSelDisp:
	push	[si.Pstruct.psHdc]		  ; hSrcDC
	jmp	short DoInvSel

DoInvScreen:
    ;
    ; Invert this rect in the screen DC
    ;
	push	ScreenDC			; hSrcDC

DoInvSel:
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

        mov     bx, sp
        add     bx, 10          ; BX is SP on entry to this procedure

OldBX           equ word ptr    ss:[bx-2]
LinWidTR        equ word ptr    ss:[bx-4]
TxTopRw         equ byte ptr    ss:[bx-5]
TxLftCol        equ byte ptr    ss:[bx-6]
TxBotRw         equ byte ptr    ss:[bx-7]
TxRgtCol        equ byte ptr    ss:[bx-8]
TxHgt	        equ byte ptr    ss:[bx-9]
TxWid	        equ byte ptr    ss:[bx-10]

    ;
    ; Compute parameters of rectangle
    ;
	mov	cx,80*2 		; cx is bytes per line
        mov     si, OldBX               ; si = ptr to Ext PAINTSTRUC
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],1
	ja	short GotLinWid
	shr	cx,1			; 40 col lines are half 80 col lines
GotLinWid:
	mov	LinWidTR,cx
	mov	al,TxTopRw
	mul	cl			; ax is byte index of first line
    ;
    ; Note that we ignore errors! This means we will grab "random trash"
    ;
	push	es              ; preserve es:di ptr to render buffer
	push	di
	call	GetVidSel	; Set up
    ;
    ; Now es:edi points to video memory
    ;
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
	add	esi,edx 		; Go to left char of selection
	xor	ch,ch			; CX is width
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
        add     sp, 10                  ; clean the stack - remove local vars
	clc
        ret
RenderRectText  endp

;**
;RenderRectGrxDisplay 
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

RenderRectGrxDisplay proc near

	mov	ax,[si.rcLeft]
	mov	[bx.PGVDRect.rcLeft],ax
	mov	ax,[si.rcTop]
	mov	[bx.PGVDRect.rcTop],ax
	mov	ax,[si.rcRight]
	mov	[bx.PGVDRect.rcRight],ax
	mov	ax,[si.rcBottom]
	mov	[bx.PGVDRect.rcBottom],ax
	mov	si,bx
	mov	ah,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
	mov	cx,1			; Build BITMAP
	call	ModeGrfx                ; a procedure which builds DIBs
	or	ax,ax			; Worked? (clears carry too)
	jz	short RRGD_Error	; No
	ret

RRGD_Error:
	stc
	ret

RenderRectGrxDisplay endp

;**
;
; AdjustSelGrxMax - Adjust Graphics Selection end point for MAX X,Y
;
; INPUT:
;	DX,AX is Y,X of endpoint
;	DS:BX -> Extended Paint structure
; EXIT:
;	DX,AX adjusted
;       Ensures AX <= 640
;               DX <= 400 for modes 4,5,6,D,E
;                  <= 350 for modes F,10
;                  <= 480 for modes 11,12 - VGA/8514 only
;
; USES:
;	DX,AX,FLAGS
;
AdjustSelGrxMax proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	cmp	ax,GrxBitWid640
	jbe	short CheckHeight
	mov	ax,GrxBitWid640
CheckHeight:
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],11H
	je	short Height480
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],12H
	je	short Height480
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0FH
	je	short Height350
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],10H
	je	short Height350
Height400:
	cmp	dx,GrxBitHeight400
	jbe	short AGM_Done
	mov	dx,GrxBitHeight400
AGM_Done:
        ret

        
Height350:
	cmp	dx,GrxBitHeight350
	jbe	short AGM_Done
	mov	dx,GrxBitHeight350
	jmp	short AGM_Done

Height480:
	cmp	dx,GrxBitHeight480
	jbe	short AGM_Done
	mov	dx,GrxBitHeight480
	jmp	short AGM_Done

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
;       Ensures AX <= (40 * FontWid)  for Mode 0,1
;                  <= (80 * FontWid)  for Mode 2,3
;               DX <= (Rows * FontHeight)
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
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],1	  ; 80 col?
	ja	short DoXB		; Yes
	shr	ax,1			; no, 40 col
DoXB:
	mul	cx			; AX is max width
	mov	dx,ax
	pop	ax
	cmp	ax,dx
	jbe	short OKX
	xchg	ax,dx
OKX:
	mov	cx,ax
	xor	ax,ax
	mov	al,[bx.EPGrabDTA.CntrlSt.VDA_EGA_Rows]
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
; GetTextWidHgt
;
; ENTRY: DS:BX -> EXTPAINTSTRUC
;
; EXIT: Font width in CX, 
;       Font Height in SI
;
; USES: CX, SI, FLAGS
;
GetTextWidHgt proc near

	mov	cx,[bx.AltFnt1.FontWid] 	; 80 column 25 line
	mov	si,[bx.AltFnt1.FontHgt]
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jbe	short CheckCols
	mov	cx,[bx.AltFnt3.FontWid] 	; 80 column 43 line
	mov	si,[bx.AltFnt3.FontHgt]
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],1
	ja	short Done
	mov	cx,[bx.AltFnt4.FontWid] 	; 40 column 43 line
	mov	si,[bx.AltFnt4.FontHgt]
	jmp	short Done

CheckCols:
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],1
	ja	short Done
	mov	cx,[bx.AltFnt2.FontWid] 	; 40 column 25 line
	mov	si,[bx.AltFnt2.FontHgt]
Done:
        ret
GetTextWidHgt endp

;**
;
; ScreenAdjust - Adjust selection end point if appropriate
;
; ENTRY:
;	DX,AX = (Y,X) screen coord of new end point
;	DS:BX -> Extended Paint structure
; EXIT:
;	DX,AX adjusted for screen size maxima
;	      adjusted for character alignment if text mode screen
;	      Adjusted for DWORD alignment if graphics mode screen
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
;                EndPointX = ((EndPointX+32-1)/32) * 32;  Use 64 for mode D,13
;       else    
;                EndPointX = EndPointX/32;    Use 64 for mode D,13
;       /* Round EndPointY to Word boundary */
;          Skip this for modes F,10,11,12
;       If (EndPointY > StartPointY) /* downward selection */
;               EndPointY =((EndPointY+1)/2) * 2;
;       else
;               EndPointY = EndPointY/2;
;       Adjust end point for maxima

ScreenAdjust proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7
	je	short SA_Text
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
	jae	short SA_Grfx
    ;
    ; Text, Adjust to char boundaries
    ;
SA_Text:
    ;
    ; get font width, height in CX and SI
    ;
        call    GetTextWidHgt
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
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0DH
	je	short QuadAlignX
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],13H
	je	short QuadAlignX
	cmp	ax,[bx.SelStruc.GrabArea.StartPointX]
	jbe	short RoundDownGrxX
	add	ax,31
RoundDownGrxX:
	and	ax,1111111111100000B
DoYGrx:
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],10H
	je	short NoGrxYAdjust
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0FH
	je	short NoGrxYAdjust
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],11H
	je	short NoGrxYAdjust
	cmp	[bx.EPGrabDTA.CntrlSt.VDA_EGA_Mode],12H
	je	short NoGrxYAdjust
	cmp	dx,[bx.SelStruc.GrabArea.StartPointY]
	jbe	short RoundDownGrxY
	inc	dx
RoundDownGrxY:
	and	dx,1111111111111110B
NoGrxYAdjust:
	call	AdjustSelGrxMax
	ret

;
; MODE D (and mode 13) screens are 320 wide. To get DWORD selection alignment
;   we must align the window points on QWORD boundaries so that when we divide
;   by 2 we are still aligned.
;
QuadAlignX:
	cmp	ax,[bx.SelStruc.GrabArea.StartPointX]
	jbe	short RoundDownGrxXD13
	add	ax,63
RoundDownGrxXD13:
	and	ax,1111111111000000B
	jmp	short DoYGrx

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
	mov	bx,si				; Set return for do nothing case
	bts	[si.EPStatusFlags],fVValidBit	; Valid state(?)
	jc	short CCSR			; Yes, done
    ;
    ; Suck out the current controller state
    ;
        push    eax

	test	GrbFlags,GrbFlgs_DoingGrab
	jz	short NotGrab3
	test	GrbFlags,GrbFlgs_DoingWindowSelGrab
	jnz	short NotGrab3
	mov	ax,GRB_Get_GrbState
	jmp	short GetGrbCtrl
NotGrab3:
	mov	ax,GRB_Get_State
GetGrbCtrl:
	mov	ebx,[si.ThisVMHand]
IFDEF   DEBUG
	mov	cx,(SIZE VDA_EGA_State)
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
;	Caret created and displayed in window
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
	mov	ax,[si.AltFnt1.FontWid] 	; 80 column 25 line
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jbe	short CheckEGA25401
	mov	ax,[si.AltFnt3.FontWid] 	; 80 column 43 line
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],1
	ja	short GotTWid
	mov	ax,[si.AltFnt4.FontWid] 	; 40 column 43 line
	jmp	short GotTWid

CheckEGA25401:
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],1
	ja	short GotTWid
	mov	ax,[si.AltFnt2.FontWid] 	; 40 column 25 line

GotTWid:
	push	ax		; nWidth
	mov	al,[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode]
	mov	[si.EPGrabDTA.CurCursMode],al
	mov	ax,[si.EPGrabDTA.CntrlSt.VDA_EGA_CurEnd]
	mov	[si.EPGrabDTA.CurCursEnd],ax
	mov	dx,[si.EPGrabDTA.CntrlSt.VDA_EGA_CurBeg]
	mov	[si.EPGrabDTA.CurCursBeg],dx
	sub	ax,dx

	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jae	short HgtOK
    ;
    ; CONVERT. The EGA character fonts are 14 tall. The EGA High Res
    ;	fonts are 12 high. The conversion factor is 12/14 = 6/7.
    ;	This applies only to modes where def EGA font is used
    ;
	mov	cx,ax
	shl	ax,1		; AX = (AX * 2)
	add	ax,cx		; AX = (AX * 3)
	shl	ax,1		; AX = (AX * 6)
	mov	cx,7
	xor	dx,dx
	div	cx		; AX = (AX * 6)/7
	or	ax,ax
	jnz	short HgtOK
	inc	ax		; Make it at least one
HgtOK:
	push	ax		; nHeight
	cCall	CreateCaret
    ;
    ; Put the caret at its correct position
    ;
	cCall	SetCaretPos
	ret

COR:
	mov	[si.EPGrabDTA.CurCursEnd],ax
	mov	[si.EPGrabDTA.CurCursBeg],ax
	mov	[si.EPGrabDTA.CurCursMode],al
	ret

MakeNewCursor endp

;**
;
; GetVidSel - Get selector to video memory, 
;             For graphics planar modes the offsets for the color planes too
;
; ENTRY:
;	SS:BP -> Std stack frame
;	DS:SI -> EXTPAINTSTRUC structure
; EXIT:
;	Carry Clear
;	    ES:EDI -> start of first video page
;	    If Graphics planar mode(D,E,F,10)
;		  IPlaneOffset  = offset in the Vid Mem for the I plane
;		  RPlaneOffset  = offset in the Vid Mem for the R plane
;		  GPlaneOffset  = offset in the Vid Mem for the G plane
;		  BPlaneOffset  = offset in the Vid Mem for the B plane
;		  
;	Carry Set
;	    ERROR!!! App cant run in a Window
;
;	NOTE WARNING!!!!! SS=DS ASSUMED!!!!!!
;       NOTE: The I,R,G,B planes are not necessarily Planes 3,2,1 and 0.
;             LookUpPalette determines which planes correspond to I,R,G and B
;             planes and sets IPlaneNum,RPlaneNum,GPlaneNum,BPlaneNum. 
;             This routines sets IPlaneOffset etc.
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

	test	GrbFlags,GrbFlgs_DoingGrab
	jz	short NotGrab1
	test	GrbFlags,GrbFlgs_DoingWindowSelGrab
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
    ;
    ; Graphics Planar mode, set more variables
    ;
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0Dh
        jae     short GVS_SetColorPlanes      

GVS_TestPageAlign:
	and	ax,0FFFH
	jz	short VarsSet
    ;
    ; Not on a 4k boundary, MUST be a Text mode
    ;
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7
	je	short VarsSet
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4
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

GVS_SetColorPlanes:
        push    edi
	call	LookupPalette		; Look up Palette
        pop     edi
	jc	GVSNonStdPgrm		; Yipe!!!!
        
        xor     ecx,ecx
	mov	cl,IPlaneNum          ; Intensity Plane #
	GetStartDispAddr eax,edi,ecx,ss
        mov     IPlaneOffset,eax
	mov	cl,RPlaneNum          ; Red Plane #
	GetStartDispAddr eax,edi,ecx,ss
        mov     RPlaneOffset,eax
	mov	cl,GPlaneNum          ; Green Plane #
	GetStartDispAddr eax,edi,ecx,ss
        mov     GPlaneOffset,eax
	mov	cl,BPlaneNum          ; Blue Plane #
	GetStartDispAddr eax,edi,ecx,ss
        mov     BPlaneOffset,eax
        mov     eax,PgOffst
        jmp     GVS_TestPageAlign

GVSNonStdPgrm:
IFDEF	DEBUG
	mov	bx,si
	mov	esi,codeOffset PalErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short GVSContinue4

PalErr	db	"Grabber unrecognized palette program",0

GVSContinue4:
	mov	si,bx
ENDIF
	bts	[si.EPStatusFlags],fGrbProbBit ; Tell caller we can't deal with this
	jmp	GVS_TestPageAlign

        
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
; NOTE: This routine is a NOP if GrbFlgs_DoingGrab is set
;	and GrbFlgs_DoingWindowSelGrab is clear. We release
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
	test	GrbFlags,GrbFlgs_DoingGrab
	jz	short NotGrab2
	test	GrbFlags,GrbFlgs_DoingWindowSelGrab
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
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],7 
	je	SHORT CPText			
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],4	  
	jae	CPR			; graphics no cursor
CPText:
	mov	ax,[si.EPGrabDTA.CntrlSt.VDA_EGA_CurY]
	or	ah,ah
	jnz	short CPR		; Cursor is off screen
	cmp	al,[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows]
	jae	short CPR		; Cursor is off screen

	mov	cx,[si.AltFnt1.FontHgt] 	; 80 column 25 line
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jbe	short CheckEGA25402
	mov	cx,[si.AltFnt3.FontHgt] 	; 80 column 43 line
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],1
	ja	short GotFH1
	mov	cx,[si.AltFnt4.FontHgt] 	; 40 column 43 line
	jmp	short GotFH1

CheckEGA25402:
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],1
	ja	short GotFH1
	mov	cx,[si.AltFnt2.FontHgt] 	; 40 column 25 line

GotFH1:
	mul	cx
	mov	dx,[si.EPGrabDTA.CntrlSt.VDA_EGA_CurBeg]
	add	ax,dx
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jae	short CRXFFF
	sub	ax,2			; Adjust
CRXFFF:
	sub	dx,[si.EPGrabDTA.CntrlSt.VDA_EGA_CurEnd]
	jae	short CPR		; Cursor height is <= 0, no cursor
	push	ax
	mov	ax,[si.EPGrabDTA.CntrlSt.VDA_EGA_CurX]
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],2
	jb	short col40Cur
col80Cur:
	cmp	ax,79
	ja	short CPRP		; Cursor is off screen
	mov	cx,[si.AltFnt1.FontWid] 	; 80 column 25 line
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jbe	short CP1
	mov	cx,[si.AltFnt3.FontWid] 	; 80 column 43 line
	jmp	short CP1

col40Cur:
	cmp	ax,39
	ja	short CPRP		; Cursor is off screen
	mov	cx,[si.AltFnt2.FontWid] 	; 40 column 25 line
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jbe	short CP1
	mov	cx,[si.AltFnt4.FontWid] 	; 40 column 43 line
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
; SetScreenBMBits - Set bits in the Screen bitmap
;
; ENTRY:
;	SS:BP -> Graphics paint frame
;	DS:SI -> ExtPaintStruc
;	AX = Line # in display bitmap of line
;	LineBits on stack has the DIB
;       DIBHdr on stack is the DIB Header
;
; Alg:
;	If (ScreenBM exists)
;              Do SetDIBits into ScreenBM at AX and AX.1 
;              except in mode F,10H,11H and 12H
;       else  /* DispBltBit set */
;              Does a SetDIBitsToDevice to Display at AX and AX.1 
;              except in Modes F,10h,11h,12h
;
; EXIT:
;
; USES:
;	ALL but DI,SI,BP,DS
;
SetScreenBMBits proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

	bt	GrbFlags,GrbFlgs_GrfxDispBltBit	; Display or ScreenDC?
	jc	DispLineBlt	                ; Display

    ;
    ; DIB is upside Line L is actually (DDRCHig-L-1)
    ;
        neg     ax
        add     ax,DDRCHig
        dec     ax

        push    ax                      ; Line #
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

	cmp	Gmode,10H
	je	short OnlyOne
	cmp	Gmode,0FH
	je	short OnlyOne
	cmp	Gmode,11H
	je	short OnlyOne
	cmp	Gmode,12H
	je	short OnlyOne

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
        pop     ax                      ; Get Line # back
        push    ax

        push    ScreenDC                ; hDC
	push	ScreenBitmap		; hBitmap
        dec     ax                      ; DIB is upside down,hence dec for next line
        push    ax                      ; nStartScan
        push    1                       ; nNumScans
	push	ss			; lpBits
	lea	ax,LineBits
	test	ax,0000000000000011B	; Dword aligned?
	jz	short DwAlign22		; Yes
	inc	ax			; Go to dword align
	inc	ax
DwAlign22:
	push	ax
        push    ss                      ; DIBHdr
        lea     ax, DIBHdr
        push    ax                      
        push    0                       ; wUsage
OnlyOne:
	cCall	SetDIBits
IFDEF DEBUG
        or      ax,ax
        jnz     short SSBContinue2
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
	jmp	short SSBContinue2

SSBContinue2:
ENDIF
        pop     ax                      ; Line #
	ret

;
; AX is line number in ScreenDC
;
DispLineBlt:
        push    di

	mov	cx,[si.PGVDRect.rcTop]
	sub	cx,DDRCTop		; Rounding error in lines from top
	mov	dx,ax
	cmp	Gmode,0FH
	je	short OnlyOneA
	cmp	Gmode,10H
	je	short OnlyOneA
	cmp	Gmode,11H
	je	short OnlyOneA
	cmp	Gmode,12H
	je	short OnlyOneA
	inc	dx			; Line number of second line
OnlyOneA:
	mov	bx,[si.Pstruct.PSrcPaint.rcRight]
	sub	bx,[si.Pstruct.PSrcPaint.rcLeft]  ; Width parameter
	push	dx
	mov	dx,GrxBitWid640
	sub	dx,[si.PGVDRect.rcLeft]
	cmp	bx,dx
	jbe	short widok
	xchg	dx,bx			; Don't overrun bitmap
widok:
	pop	dx
	cmp	dx,cx
	jb	short DispBltDone
	sub	dx,cx			; Second Line number relative to top paint line
	sub	ax,cx			; First Line number relative to top paint line
	jae	short LBOK1
	mov	ax,dx			; First line is off paint rect
					; second line isn't, change first to
					; second and say no second
LBOK1:
	add	ax,[si.Pstruct.PSrcPaint.rcTop]
	add	dx,[si.Pstruct.PSrcPaint.rcTop]
	cmp	ax,[si.Pstruct.PSrcPaint.rcBottom]
	jae	short DispBltDone
	push	[si.Pstruct.psHdc]	  ; hDC
	push	[si.Pstruct.PSrcPaint.rcLeft] ;DestX
	push	ax			; DestY
	push	bx			; nWidth
	push	1			; nHeight
	mov	cx,[si.PGVDRect.rcLeft]
	sub	cx,DDRCLeft
	push	cx			; XSrc
	push	0			; YSrc
	push	0                       ; nStartScan
	push	1                       ; nNumScans
	push	ss			; lpBits
	lea	di,LineBits
	test	di,0000000000000011B	; Dword aligned?
	jz	short DwAlign23		; Yes
	inc	di			; Go to dword align
	inc	di
DwAlign23:
	push	di
        push    ss                      ; lpBitmapInfo
        lea     di, DIBHdr
        push    di                      
        push    0                       ; wUsage
	cmp	ax,dx			; Only one line?
	je	short OnlyOneB		; Yes.
	cmp	dx,[si.Pstruct.PSrcPaint.rcBottom] ; Second line off paint rect?
	jae	short OnlyOneB		; Yes, only do one
	push	[si.Pstruct.psHdc]	  ; hDestDC
	push	[si.Pstruct.PSrcPaint.rcLeft] ; X
	push	dx			; Y
	push	bx			; nWidth
	push	1			; nHeight
	push	cx			; XSrc
	push	0			; YSrc
	push	0                       ; nStartScan
	push	1                       ; nNumScans
	push	ss			; lpBits
	lea	di,LineBits
	test	di,0000000000000011B	; Dword aligned?
	jz	short DwAlign24		; Yes
	inc	di			; Go to dword align
	inc	di
DwAlign24:
	push	di
        push    ss                      ; lpBitmapInfo
        lea     di, DIBHdr
        push    di                      
        push    0                       ; wUsage

	cCall	SetDIBitsToDevice
OnlyOneB:
	cCall	SetDIBitsToDevice
DispBltDone:
        pop     di
	ret

SetScreenBMBits endp

sEnd	code
	end



