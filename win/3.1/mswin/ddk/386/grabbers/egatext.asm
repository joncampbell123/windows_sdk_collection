;--------------------------------------------------------------------
;
;   Screen Grabber for IBM EGA/VGA adaptor
;
;   Text mode painting routines.
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
	include    grabmac.inc
IFDEF VGA31
	include    vmdavga.inc
ELSE
	include    vmdaega.inc
ENDIF
	include    ega.inc
	include    statusfl.inc
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
externFP	SelectObject
externFP	GetNearestColor
externFP	CreateSolidBrush
externFP	DeleteObject
externFP	PatBlt
externFP	SetBkColor
externFP	TextOut
externFP	SetTextColor
ifdef	DBCS
if1
%out DBCS code enabled
endif
externFP	IsDBCSLeadByte
endif	; DBCS

;-----------------------------------------------
;
; External GRABBER Procedures
;
externNP	ComputeSelTextrect
ifdef	DBCS
externNP	IsDBCSMiddleText
endif	; DBCS

sBegin	code
	assumes cs,code

	public	ModeText
	public	AdjustSelText
	public	Outline
	public	CreateOverScanBrTxt
        public  InitTextFrameVars
        public  SetParamsText
        public  SelectTextFont
        public  PaintExtraAreaText
        public  SetNewBkFg
        public  DeAllocResText
        public  GetWinRGbText

	extrn	RGBTable:word

;****************************************************************************
;
; ModeText - Paint a text mode display
;
;    This routine Paints the old app text screen into a window
;
;    Our strategy is:
;	PatBlt the background color of the last char on the top line
;	    of the paint rectangle into the paint rectangle in the display DC.
;	For each text line in the paint rectangle build a text line
;	    from the display memory and use TextOut to output it into
;	    the paint rectangle
;	    Pieces of lines that are spaces in the background color PatBlted
;		above may be skipped over if they are long enough
;
; ENTRY:
;	SS:BP -> Std stack frame
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	AH = Mode byte from above
; EXIT:
;	Carry Clear
;	    Screen Painted
;	Carry Set
;	    Screen not painted, probably low Windows memory problem
; USES:
;	ALL but DS,SI,BP
; NOTE:
;	This routine may set the fGrbProb bit in EPStatusFlags in the
;	Extended paint structure to indicate that this app is in a mode
;	which cannot be rendered into a Window. This routine must return with
;	Carry clear in this case.
;
;****************************************************************************
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

ModeText proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

        call    InitTextFrameVars       ; Initialize frame variables
    ;
    ; If there is a selection, compute selection rectangle
    ;
	lea	bx,[si.PTVDRect]
	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn
	jz	short NoSel
	push	cx
	call	ComputeSelTextRect
	pop	cx
NoSel:
    ;
    ; Compute various parameters of the paint and See if the paint rect is
    ;	"on screen"
    ;
        call    SetParamsText
ifdef	DBCS
	jc	MTDone
else
        jc      short MTDone    ; Rect is Off screen or invalid rect 
endif
    ;
    ; BytesPerLine,BytesToRight set 
    ; ES:EDI -> first line of paint rect
    ;
        call    SelectTextFont          ; based on mode
ifdef	DBCS
	jc	MTError
else
        jc      short MTError
endif
        call    PaintExtraAreaText

	push	edi
	movzx	eax,BytesToRight
	add	edi,eax
	mov	es,VidSel
	mov	ax,word ptr es:[edi]	; Get attribute of first char in AH
	call	AdjustSelText		; Do adjustment if needed
	not	ah			; Make colors "different" to cause
					;  initial set
	mov	word ptr Currcols,ax
	jmp	short Firstline

    ;
    ; Set up for next line
    ;
NextLineText:
	push	edi
	movzx	eax,BytesToRight
	add	edi,eax
Firstline:
	lea	ax,ToutBuf
	mov	pToutBuf,ax
ifdef	DBCS
ifdef	VGA
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode], 7
	je	@F
endif
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],3
	jg	short NoMode30
@@:
	push	esi
	push	edx
	mov	esi,edi
	movzx	edx,BytesToRight
	sub	esi,edx
	push	ds
	push	es
	pop	ds
	call	IsDBCSMiddleText	;the left side is 2nd byte of a DBCS?
	pop	ds
	pop	edx
	pop	esi
	mov	cx,WidPaint
	mov	ax,[bx.TPXpos]		    ; X
	jc	short @F
	dec	edi			;left side is DBCS 2nd byte
	dec	edi			;extend the left side to DBCS 1st byte
	inc	cx			;so is the line width
	sub	ax,[bx.TPFntWid]	;and x starting position
@@:
	mov	TLineXPos,ax
	jmp	short NextChar
NoMode30:
endif	; DBCS

	mov	ax,[bx.TPXpos]		    ; X
	mov	TLineXPos,ax
	mov	cx,WidPaint
NextChar:
    ;
    ; Get the next char and adjust it if it's in the selection rectangle
    ;
	mov	ax,word ptr es:[edi]
	call	AdjustSelText
	inc	edi
	inc	edi
    ;
    ; Set new colors if needed
    ;
	cmp	ah,byte ptr Currcols.1	; Q:Colors changed?
	jz	short StoreChar 	;  No
        call    SetNewBkFg              ;  Yes
StoreChar:
	push	edi
	mov	di,pToutBuf		; Char goes here
	mov	byte ptr [di],al
	inc	pToutBuf		; Next char
	pop	edi
ifdef	DBCS
ifdef	VGA
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode], 7
	je	short MT_DBCSText
endif
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],3
	jg	short @F
MT_DBCSText:
	xor	ah,ah
	push	bx
	push	cx
	cCall	IsDBCSLeadByte,<ax>
	pop	cx
	pop	bx
	or	ax,ax
	jz	short @F
	mov	ax,word ptr es:[edi]

	call	AdjustSelText
	cmp	ah,byte ptr Currcols.1
	jz	short NoColChg

	push	dx
	push	cx
	push	ax
	mov	eax,edi
	sub	eax,PgOffst
	xor	dx,dx
	mov	cx,BytesPerLine
	div	cx			; AX is row, DX is col*2
	shr	dx,1			; DX is col
	cmp	dl,byte ptr [si.SelStruc.GrabArea.Irect.rcLeft]
	jne	short NoSelLft
	cmp	al,byte ptr [si.SelStruc.GrabArea.Irect.rcLeft.1]
	jb	short NoSelLft
	cmp	al,byte ptr [si.SelStruc.GrabArea.Irect.rcTop.1]
	jae	short NoSelLft

	pop	ax
	push	ax
	dec	pToutBuf
	call	SetNewBkFg
	push	ax
	mov	ax,word ptr es:[edi-2]
	push	edi
	mov	di,pToutBuf
	mov	byte ptr [di],al
	inc	pToutBuf
	pop	edi
	pop	ax
NoSelLft:
	pop	ax
	pop	cx
	pop	dx
NoColChg:
	inc	edi
	inc	edi
	push	edi
	mov	di,pToutBuf
	mov	byte ptr [di],al
	inc	pToutBuf
	pop	edi
	dec	cx
@@:
endif	; DBCS
	dec	cx			; Faster than loop to label which is too far
ifdef	DBCS
;cx might be negative if we have DBCS on the right side.
	jg	NextChar
else	; DBCS
	jnz	NextChar
endif
LineDoneText:
	call	OutLine 		; Flush
    ;
    ; Next line
    ;
	pop	edi
	movzx	eax,BytesPerLine
	add	edi,eax 		; Next line in vid buff
	mov	ax,[bx.TPFntHgt]
	add	[bx.TPYpos],ax		; Next line on display
	dec	HigPaint		; One line done
	jnz	NextLineText		; More lines
        
MTDone:
        call    DeAllocResText
        clc
	ret

MTError:
        call    DeAllocResText
        stc
        ret

ModeText endp

;*****************************************************************************
;
; InitTextFrameVars - Initialize text frame vars
;
; ENTRY:
;
; EXIT: Text Frame Vars initialized, 
;       CX = # of cols
;
;*****************************************************************************

InitTextFrameVars proc near

	xor	bx,bx 
	mov	BkTxtBrsh,bx
	mov	WidRgt,bx
	mov	HigBot,bx
	mov	OldBrshHand,bx
	mov	OldFontHand,bx

	mov	cx,40           ; assume mode 0/1
	cmp	ah,1            ; Assumption correct?
	jbe	short ITDone    ; Yes
	mov	cx,80           ; No
ITDone:
        ret

InitTextFrameVars endp

;*****************************************************************************
;
; SetParamsText - 
;
; ENTRY: CX = # of Cols on the screen
;
; EXIT: 
;       If Carry Set 
;           Rect is Off Screen or invalid rect
;       else
;           Set
;               - BytesPerLine 
;               - BytesToRight
;               - ES:EDI -> first line of paint rect
;       
;*****************************************************************************

SetParamsText proc near
    ;
    ; Compute various parameters of the paint and See if the paint rect is
    ;	"on screen"
    ;
	shl	cx,1			; Two bytes per char in vid buff
	mov	BytesPerLine,cx
	shr	cx,1			; Get back screen width
	mov	dx,cx			; Stash it in DX
	cmp	cx,[bx.TPRect.rcLeft]	  ; Paint rect "on screen"?
	jbe	short NulPntTxt		; No

	mov	cx,[bx.TPRect.rcRight]
	cmp	cx,dx			; Q: Part of paint rect "off screen"?
	jbe	short WidOkTx		;  N:
	xchg	cx,dx			;  Y: Limit to screen
WidOkTx:
	sub	cx,[bx.TPRect.rcLeft]
	mov	WidPaint,cx
	jbe	short InvalidRectText            ; Invalid Rect 
	mov	ax,cx
	mul	[bx.TPFntWid]		; AX is width of paint text in pixels
	add	ax,[bx.TPXpos]
	cmp	ax,[si.Pstruct.PSrcPaint.rcRight]
	jb	short ExtrR
NoExtrR:
	xor	cx,cx
	mov	cl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows]
	mov	dx,cx			; Stash it in DX
	cmp	cx,[bx.TPRect.rcTop]	  ; Paint rect "on screen"?
	jbe	short NulPntTxt		; NO

	mov	cx,[bx.TPRect.rcBottom]
	cmp	cx,dx			; Q: Part of paint rect "off screen"?
	jbe	short HigOkTx		;   N:
	xchg	cx,dx			;   Y: Limit to screen
HigOkTx:
	sub	cx,[bx.TPRect.rcTop]
	mov	HigPaint,cx
	jbe	short InvalidRectText
	mov	ax,cx
	mul	[bx.TPFntHgt]		; AX is height of text
	add	ax,[bx.TPYpos]
	cmp	ax,[si.Pstruct.PSrcPaint.rcBottom]
	jb	short ExtrB
NoExtrB:
	mov	ax,[bx.TPRect.rcLeft]
	shl	ax,1			; Two bytes per char in vid buff
	mov	BytesToRight,ax
    ;
    ; Index into vid buffer to first char of top line of paint
    ;
	mov	ax,[bx.TPRect.rcTop]
	mul	BytesPerLine		; AX is byte index of first byte of first line
        les     edi,VidAddr
	movzx	eax,ax
	add	edi,eax 		; Point to start of this line
        clc
        ret

NulPntTxt:
	call	CreateOverScanBrTxt
	lea	di,[si.Pstruct.PSrcPaint]
	mov	ax,[di.rcRight]
	sub	ax,[di.rcLeft]
	mov	dx,[di.rcBottom]
	sub	dx,[di.rcTop]
	cCall	PatBlt,<[si.Pstruct.psHdc],[di.rcLeft],[di.rcTop],ax,dx,PATCOPY_H,PATCOPY_L>
InvalidRectText:
        stc
        ret
GVSFailedText:
	bts	[si.EPStatusFlags],fGrbProbBit ; Tell caller we can't deal with this
        jmp short InvalidRectText

ExtrR:
	mov	XRgt,ax
	sub	ax,[si.Pstruct.PSrcPaint.rcRight]
	neg	ax
	mov	WidRgt,ax
	jmp	NoExtrR

ExtrB:
	mov	YBot,ax
	sub	ax,[si.Pstruct.PSrcPaint.rcBottom]
	neg	ax
	mov	HigBot,ax
	jmp	NoExtrB

SetParamsText endp

;****************************************************************************
;
; SelectTextFont - Select the appropriate text font into hDC
;                  Returns Font handle in AX
;
; EXIT: Carry set if error
;
;****************************************************************************
SelectTextFont proc near

	cmp	BytesPerLine,40*2
	jz	short Font40
Font80:
	mov	ax,[si.AltFnt1.FontHand]	; Assume 25 line
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jbe	short SelFont
	mov	ax,[si.AltFnt3.FontHand]	; > 25 line

SelFont:
	or	ax,ax
	jnz	short OkFnt
	mov	ax,[si.DefFont.FontHand]    ; AIGGGGHHHHH!!!! use def font
OKFnt:
	push	bx
	cCall	SelectObject,<[si.Pstruct.psHdc],ax>
	pop	bx
	or	ax,ax
	jz	short STFError        ; Low memory most likely

	mov	OldFontHand,ax
        clc
        ret
Font40:
	mov	ax,[si.AltFnt2.FontHand]	; Assume 25 line
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Rows],25
	jbe	short SelFont
	mov	ax,[si.AltFnt4.FontHand]	; > 25 line
	jmp	short SelFont

STFError:
        stc
        ret

SelectTextFont endp

;****************************************************************************
;
; PaintExtraAreaText - Paint the Extra areas with Over scan brush
;
;****************************************************************************
PaintExtraAreaText proc near

        push    bx
	call	CreateOverScanBrTxt
    ;
    ; PatBlt the black brush into "extra areas" (if any)
    ;
	cmp	WidRgt,0
	jz	short CheckHig
	mov	ax,[si.Pstruct.PSrcPaint.rcTop]
	mov	cx,[si.Pstruct.PSrcPaint.rcBottom]
	sub	cx,ax
	cCall	PatBlt,<[si.Pstruct.psHdc],XRgt,ax,WidRgt,cx,PATCOPY_H,PATCOPY_L>
CheckHig:
	cmp	HigBot,0
	jz	short PEADone
	mov	ax,[si.Pstruct.PSrcPaint.rcLeft]
	mov	cx,[si.Pstruct.PSrcPaint.rcRight]
	sub	cx,ax
	cCall	PatBlt,<[si.Pstruct.psHdc],ax,YBot,cx,HigBot,PATCOPY_H,PATCOPY_L>
PEADone:        
        pop    bx
        ret

PaintExtraAreaText endp

;****************************************************************************
;
; SetNewBkFg - Set New Bk and Fg colors if necessary
;
; EXIT: CurrCols updated
;
; Algorithm:
;     Flush chars in the buffer in prev colors
;     Turn Off Blink bit in CurrCols and given attribute
;     if (Bk Color has changed)  {  /* AH.Bk != CurrCols.Bk) */
;         if (char in Sel) /* Indicated Color != True Color */
;             Get Win RGB for Indicatedcolor.Bk
;         else
;             Get Win RGB for TrueColor.Bk
;         SetBkColor
;     }
;     if (Fg Color has changed)  {  /* AH.Fg != CurrCols.Fg) */
;         if (char in Sel) /* Indicated Color != True Color */
;             Get Win RGB for Indicatedcolor.Fg
;         else
;             Get Win RGB for TrueColor.Fg
;         SetTextColor
;     }
;
;****************************************************************************
SetNewBkFg proc near

	call	OutLine 		; Flush chars in prev colors

	push	es
	push	bx
	push	cx
	push	ax

    ;
    ; Set new background color
    ;
	mov	bx,word ptr Currcols
	mov	bl,ah			; BH is old bk, BL is new bk
	and	bx,01111000011110000B	; Mask to bits of interest
	cmp	bh,bl			; Changed?
	jz	short SameBk		; No
SetBk:
	mov	cl,ah			; CL is indicated color
	mov	al,TrueColor		; AL is TRUE color
	mov	ch,al			; CH is TRUE color
	cmp	cl,ch			; Color effected by selection?
	jz	short GetBk		; No - pick true color for Bk
        mov     al,cl                   ; Yes - pick indicated color for Bk
GetBk:
	shr	al,4			; Background to low 4 bits
	call	GetWinRGBText
	cCall	SetBkColor,<[si.Pstruct.psHdc],dx,ax>
SameBk:
    ;
    ; Set new foreground color
    ;
	pop	ax
	push	ax
	mov	bx,word ptr Currcols
	mov	bl,ah			; BH is old foreground, BL new
	and	bx,0000111100001111B	; Mask to bits of interest
	cmp	bh,bl			; Changed?
	jz	short SameFg		; no
SetFg:
	mov	cl,ah			; CL is indicated color
	mov	al,TrueColor		; AL is TRUE color
	mov	ch,al			; CH is TRUE color
	cmp	cl,ch			; Color effected by selection?
	jz	short GetFg		; No - pick true color for Fg
        mov     al,cl                   ; Yes - pick indicated color for Fg
GetFg:
	call	GetWinRGBText
	cCall	SetTextColor,<[si.Pstruct.psHdc],dx,ax>
SameFg:
	pop	ax
	pop	cx
	pop	bx
	pop	es
	mov	word ptr Currcols,ax	; Set current colors

        ret

SetNewBkFg endp

;****************************************************************************
;
; DeAllocResText - DeAlloc allocated resources
;
;****************************************************************************

DeAllocResText proc near

	mov	cx,OldBrshHand
	jcxz	BkTextBr
	cCall	SelectObject,<[si.Pstruct.psHdc],cx>
BkTextBr:
	mov	cx,BkTxtBrsh
	jcxz	RestoreFont
	cCall	DeleteObject,<cx>
RestoreFont:
	mov	cx,OldFontHand
	jcxz	DTDone
	cCall	SelectObject,<[si.Pstruct.psHdc],cx>
DTDone:
        ret
DeAllocResText endp

;**
;
; AdjustSelText - Adjust current char if it is in the selection rect
;
; ENTRY:
;	SS:BP -> Text paint frame
;	DS:SI -> Extended paint structure
;	AX is attr/char of interest
;	EDI indicates AX's location in the video buffer
;	   together with pgOffst
; EXIT:
;	AH adjusted if in selection rectangle
;	TrueColor on stack frame set to input AH
;	NOTE: High bit of AH (blink bit) is ALWAYS turned off by this routine
; USES:
;	AH,FLAGS
;
AdjustSelText proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	and	ah,01111111B			; Ignore blink bit
    ;
    ; Set intensity bit for Bk Color(except Black/grey) 
    ;
        test    ah,01110000B            ; Q: Black Bk color?
        jz      SHORT AST_00            ;   Y: Skip turning on intensity bit
        push    ax
        and     ah,01110000B
        cmp     ah,01110000B            ; Q: Low Int white Bk color?
        pop     ax                      
        je      SHORT AST_00            ;   Y: Skip turning on intensity bit

        or      ax,8000h                ;   N: use intense Bk colors always
AST_00:
	mov	TrueColor,ah
	test	[si.SelStruc.GrabArea.SelecFlags],SelectOn ; Selection active?
	jnz	short LSel				   ; Yes
	ret

LSel:
    ;
    ; Compute row,col of char
    ;
	push	dx
	push	cx
	push	ax
	mov	eax,edi
	sub	eax,PgOffst	; AX is byte index into Text buffer
	xor	dx,dx
	mov	cx,BytesPerLine
	div	cx		; AX is row, DX is col*2
	shr	dx,1		; DX is col
    ;
    ; Is char in selection rect
    ;
	cmp	al,byte ptr [si.SelStruc.GrabArea.Irect.rcLeft.1]
	jb	short NotInSelRect
	cmp	al,byte ptr [si.SelStruc.GrabArea.Irect.rcTop.1]
	jae	short NotInSelRect
	cmp	dl,byte ptr [si.SelStruc.GrabArea.Irect.rcLeft]
	jb	short NotInSelRect
	cmp	dl,byte ptr [si.SelStruc.GrabArea.Irect.rcTop]
	jae	short NotInSelRect
    ;
    ; Char is in selection, NOT its attribute to invert the colors
    ;
	pop	ax
	not	ah
	and	ah,011111111B			; Ignore blink bit
	push	ax
NotInSelRect:
	pop	ax
	pop	cx
	pop	dx
	ret

AdjustSelText endp

;**
;
; Outline - Output a line in current color setting
;
; ENTRY:
;	SS:BP is stack frame pointer for text frame
;	DS:SI -> Extended paint structure
;	DS:BX -> Text sub structure of extended paint structure
; EXIT:
;	Chars in ToutBuf output at current location indicated by text sub struct
;	TLineXPos advanced to next char after last output
;	ToutBuf emptied
; USES:
;	ALL but ES,DS,AX,BX,CX,BP
;
Outline proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	es
	push	ax
	push	cx
	push	bx
	lea	cx,ToutBuf
	xchg	cx,pToutBuf			; Reset and get current
	sub	cx,pToutBuf			; Size
	jcxz	short nochars			; Empty, done
	mov	ax,[bx.TPFntWid]
	mul	cx				; AX is with in pix
	add	ax,TLineXPos			; AX is new X coord
	xchg	ax,TLineXPos			; Set new, get old X coord
	push	[si.Pstruct.psHdc]		  ; hDC
	push	ax				; X
IFDEF DEBUG
public LineNum
LineNum:
ENDIF
	push	[bx.TPYpos]			; Y
	push	ss				; pBuff
	push	pToutBuf
	push	cx				; Size
	cCall	TextOut
nochars:
	pop	bx
	pop	cx
	pop	ax
	pop	es
	ret
Outline endp

;**
;
; CreateOverScanBrTxt - Create the overscan brush for text mode and select it
;
; ENTRY:
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	SS:BP -> Text mode paint frame
; EXIT:
;	Correct brush selected in display DC
; USES:
;	ALL but DS,SI,DI,BP
;
CreateOverScanBrTxt proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
    ;
    ; Try to create a brush in the overscan color
    ;
	mov	bl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Colr]	; Get overscan color
	mov	bh,bl
	and	bl,00000111B		; Mask to RGB values
	shl	bl,1
	and	bh,00010000B		; Mask to intensity bit
	or	bl,bh
	xor	bh,bh			; bx is IRGB color * 2
	shl	bx,1			; * 4 for table
	mov	ax,bx
	cCall	GetNearestColor,<[si.Pstruct.psHdc],[bx.RGBTable.2],[bx.RGBTable]>
	cCall	CreateSolidBrush,<dx,ax>
	or	ax,ax
	jz	short DefOvscBr
GotOvscBr:
	mov	BkTxtBrsh,ax
    ;
    ; Select the brush into the display DC
    ;
	cCall	SelectObject,<[si.Pstruct.psHdc],ax>
	mov	OldBrshHand,ax
	ret

DefOvscBr:
	mov	ax,[si.BlkBrshH]	; Black background brush selected
	jmp	short GotOvscBr


CreateOverScanBrTxt endp

;*****************************************************************************
;
; GetWinRGBText - Convert to Windows RGB format 
;
; ENTRY:
;	AL is IRGB in low 4 bits
;	DS:SI -> EXTPAINTSTRUC
; EXIT:
;	DX:AX is Windows RGB
; USES:
;	AX,DX,FLAGS
;
;*****************************************************************************
GetWinRGBText proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
	push	bx
	and	ax,01111B		; Mask to bits of interest
        mov     bx,ax
	shl	bx,2			; BX is now IRGB * 4
	mov	ax,[bx.RGBTable]
	mov	dx,[bx.RGBTable.2]
	pop	bx
	ret

GetWinRGBText endp


sEnd	code
	end
