;--------------------------------------------------------------------
;
;   Screen Grabber for HERC adaptor - HERCPNT.ASM
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
	include grabmac.inc
	include vmdaherc.inc
	include herc.inc
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
externFP	PatBlt
externFP	SelectObject
externFP	CreateCompatibleDC
externFP	CreateCompatibleBitmap
externFP	BitBlt
externFP	DeleteDC
externFP	DeleteObject
externFP	SetBkColor
externFP	SetTextColor
externFP	TextOut
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
externNP	ComputeSelTextRect
externNP	InvSel
externNP	InvSel2
externNP	LineBlt
externNP	ClearVidSel
externNP	GetTxtColor
ifdef DBCS
externNP	IsDBCSMiddleText
endif

sBegin	code
	assumes cs,code

	public	ModeText
	public	AdjustSelText
	public	Outline
	public	CreateOVBrTxt
        public  InitTextFrameVars
        public  SetParamsText
        public  SelectTextFont
        public  PaintExtraAreaText
        public  SetNewBkFg
        public  DeAllocResText

	public	ModeGrfx
	public	CreateOVBrTxt
	public	CreateOVBrGrx
        public  InitFrameVars
        public  CheckMode
        public  PatBltOverScanColor
        public  BuildDIBHdr
        public  SetParams
        public  BuildDisplayRect
        public  SkipBkBits
        public  SetupLineBits
        public  AllocRes
        public  DeAllocRes
	public	XferBits

;*****************************************************************************
; Initialize frame variables used by grab/paint code
;
;*****************************************************************************
InitFrameVars proc near

	xor	bx,bx
	mov	ScreenDC,bx
	mov	ScreenBitMap,bx
	mov	BkBrush,bx
	mov	OvscnBrsh,bx
	mov	Gmode,ah
        mov     SavEXTP,si
        ret

InitFrameVars endp

;*****************************************************************************
; CheckMode - Check for valid HERC mode
;
; ENTRY: AH = video mode
;        DS:SI -> EXTPAINTSTRUC
;
; EXIT:  Carry set if mode is invalid
;
;*****************************************************************************
CheckMode proc near

	cmp	ah,HERC_MODE_Grfx	;Graphics Mode?
	je	short CMDone
IFDEF DEBUG
	mov	bx,si
	mov	esi,codeOffset ModeErr
	mov	edx,SHELL_Debug_Out
	call	[bx.ShellCall]
	jmp	short CMContinue
ModeErr  db	 "Grabber bad video mode in AH (paint/grab)",0
CMContinue:
        mov     si,bx
ENDIF
GrbProblem:
        stc
        ret
CMDone:
        clc
        ret

CheckMode endp

;*****************************************************************************
;
; PatBltOverScanColor - PatBlt the overscan color into the paint rect
;
; Called when the paint rect is off screen
; Creates OverScan brush. Deletes it after the PatBlt
;
;*****************************************************************************
PatBltOverScanColor proc near

	call	CreateOVBrGrx
	jz	short DeleteOvBr	; Hmmm.... Barfed don't complain though
	push	ax  		        ; Save old brush
	lea	di,[si.Pstruct.PSrcPaint]
	mov	ax,[di.rcRight]
	sub	ax,[di.rcLeft]
	mov	dx,[di.rcBottom]
	sub	dx,[di.rcTop]
    ;
    ; PatBlt(hDC,X,Y,nWidth,nHeight,PATCOPY_H,PATCOPY_L)
    ;
	cCall	PatBlt,<[si.Pstruct.psHdc],[di.rcLeft],[di.rcTop],ax,dx,PATCOPY_H,PATCOPY_L>
	pop	ax			; Recover previous brush
	cCall	SelectObject, <[si.Pstruct.psHdc],ax>
    ;
    ; Delete Overscan brush if any
    ;
DeleteOvBr:
        mov     cx,OvscnBrsh
        jcxz    short NoOvBr
        cCall   DeleteObject, <cx>
NoOvBr:
        ret        
PatBltOverScanColor endp

;*****************************************************************************
;
; BuildDisplayRect - Build display paint rectangle. 
;                    (Paint Rect CoOrds rounded to dword aligned values)
;
; ENTRY: DS:SI -> EXTPAINTSTRUC
; 
; EXIT: Zero flag set - rect is offscreen OR invalid paint rect
;                                            (bot < top or right < left)
;
;*****************************************************************************
;
BuildDisplayRect proc near

	mov	bx,[si.PGVDRect.rcLeft]
	and	bx,1111111111100000B	; Round down to DWORD (32 bit) boundary
	cmp	bx,GrxBitWid		; Is paint rect "on screen"?
	jae	short RectOffScreen		; No
	mov	DDRCLeft,bx
	shr	bx,3			; Divide by 8 for Byte index of this loc
	mov	DDRCLeftBI,bx
	mov	bx,[si.PGVDRect.rcRight]
	add	bx,31
	and	bx,1111111111100000B	; Round up to DWORD (32 bit) boundary
	cmp	bx,GrxBitWid		; Limit to max size of physical screen
	jbe	short OkBWid
    ;
    ; Add sixteen to screen width as this stops the 2 byte wrapround 
    ; that appears when the graphics screen is scrolled fully right.
    ;
        mov	bx,GrxBitWid+16
OkBWid:
	mov	DDRCRight,bx
	sub	bx,DDRCLeft
	jbe	short InvalidCoOrd
	mov	DDRCWid,bx
	shr	bx,5			; Divide Width by 8 to get width in Bytes
	mov	DDPWid,bx		; NOTE: For mode 0D display this is width in WORDS!
    ;
    ;  What follows is to round the top and height to a multiple of 4.
    ;  We do this so that the first line is always an even scan line, and
    ;  the height represents equal numbers of even and odd scan lines. This
    ;  prevents us from having to do wierd boundary checks to make sure we
    ;  don't overflow the full screen bitmap.
    ;
	mov	cx,1111111111111100B	; Top, Height round is 4 on HERC modes
	mov	dx,3			; Top, Height round is 4 on HERC modes
	mov	bx,[si.PGVDRect.rcTop]
	mov	di,GrxBitHeight		; Max height
	and	bx,cx			; Round top down to mult
	cmp	bx,di			; Is paint rect "on screen"?
	jae	short RectOffScreen		; NO
	mov	DDRCTop,bx
	mov	ax,[si.PGVDRect.rcBottom]
	cmp	ax,di
	jbe	short OkBHig2 		; Limit to physical screen
	mov	ax,di
OkBHig2:
	sub	bx,ax
	jae	short InvalidCoOrd
	neg	bx
	add	bx,dx			; Round height up to mult
	and	bx,cx
	cmp	bx,di			; Limit to height of physical screen
	jbe	short OkBHig
	mov	bx,di
OkBHig:
	mov	DDRCHig,bx
    ;
    ; At this point, the following values have been set up:
    ;
    ;	DDRCLeft = si.PGVDRect.rcLeft rounded down to a 64 bit boundary.
    ;	DDRCLeftBI = DDRCLeft shifted right 3.
    ;	DDRCRight = si.PGVDRect.rcRight rounded up to a 64 bit boundary.
    ;	DDRCWid = DDRCRight - DDRCLeft.
    ;	DBPWid = BDsWid = DDRCWid shifted right 3.
    ;	DDPWid = DBPWid shifted right 2.
    ;	DDRCTop = si.PGVDRect.rcTop rounded down to multiple of four.
    ;	DDRCHig = (si.PGVDRect.rcBottom - DDRCTop) rounded up to multiple
    ;		  of four.
        mov     ax,1                    
        or      ax,ax                   ; clear zero flag 
        ret
RectOffScreen:
    ;
    ; PatBlt the overscan color into the paint rect and return
    ;
        call    PatBltOverScanColor
InvalidCoOrd:
        mov     ax,0
        or      ax,ax                   ; set zero flag
        ret

BuildDisplayRect endp

;*****************************************************************************
;
; AllocRes - Allocate resources for paint/grab
;
; ENTRY: DS:SI -> EXTPAINTSTRUC
;
; EXIT: carry flag set if error
;
;*****************************************************************************

AllocRes proc near
    ;
    ; Create a memory DC for full screen compatible with the display
    ;
	cCall	CreateCompatibleDC, <[si.Pstruct.psHdc]>

	or	ax,ax
	jz	short ACRError
	mov	ScreenDC,ax
    ;
    ; Set black background brush
    ;
	mov	ax,[si.BlkBrshH]
	mov	BkBrush,ax
    ;
    ; Select the background brush into the screen DC
    ;
	cCall	SelectObject,<ScreenDC,ax>
	or	ax,ax                   
        jz      short ACRError
%OUT Use CreateBitmap here
    ;
    ; Create the full screen bitmap to the dimensions of the rounded paint rect
    ;
	cCall	CreateCompatibleBitmap, <[si.Pstruct.psHdc],DDRCWid,DDRCHig>
	or	ax,ax
	jz	short ACRError	; Fail.....
	mov	bx,ax
        push    bx
        cCall   SelectObject, <ScreenDC,bx>
        pop     bx
        or      ax,ax
        jz      short ReturnBitmap
	mov	ScreenBitMap,bx

        clc
        ret

ReturnBitmap:
        mov     ScreenBitmap,0
        cCall   Deleteobject,<bx>
ACRError:
        stc
        ret

AllocRes endp

;*****************************************************************************
;
; DeAllocRes - Deallocate all allocated resources 
;
; ENTRY: DS:SI -> EXTPAINTSTRUC
;
; EXIT: Resources if allocated are deallocated
;
;*****************************************************************************

DeAllocRes proc near

	mov	cx,ScreenDC
	jcxz	short TestSBM
	cCall	DeleteDC,<cx>
TestSBM:
	mov	cx,ScreenBitMap
	jcxz	short TestBkBr
	bt	GrbFlags,GrbFlags_GrfxGrabBMBit	; Keep This?
	jc	short TestBkBr  		; Yes
	cCall	DeleteObject,<cx>
TestBkBr:
	mov	cx,BkBrush
	jcxz	short TestOvBr
	cCall	DeleteObject,<cx>
TestOvBr:
	mov	cx,OvscnBrsh
	jcxz	short DAADone
	cCall	DeleteObject,<cx>
DAADone:
	ret

DeAllocRes endp

;****************************************************************************
;
; SetParams - return ptr to first scan line of paint rect in es:edi
;                    and count in CX   
;
;
;
;****************************************************************************

SetParams proc near

	les	edi,VidAddr             ; es:edi -> Video page 0 at B0000/B8000

	xor	eax,eax 	     
        mov     ax,GrbFlags
        and	ax,GrbFlags_GrfxIntrlv
        mov     dx,2*4096
        mul     dx                      ; get page 0/2/4/6 offset in AX
	add	edi,eax 		; Point to start of evens or odds
    ;
    ; The two pages starting at 
    ; B8000 or B0000 contains lines 0, 4, 8 ..  
    ; BA000 or B2000 contains lines 1, 5, 9 ..
    ; BC000 or B4000 contains lines 2, 6, 10 ..
    ; BE000 or B6000 contains lines 3, 7, 11 ..
    ;
        xor     eax,eax
	mov	ax,DDRCTop		; Get index to first line
	shr	ax,2			; Every fourth line in memory
	mov	cx,GrxBitWid/8		; This many bytes per line
	mul	cx			; This many bytes in a scan line

	add	ax,DDRCLeftBI		
	add	edi,eax			
	mov	cx,DDRCHig		; This many scan lines to do
	shr	cx,2			; height/4
        ret

SetParams endp

;*****************************************************************************
;
; SkipBkBits - Skip background bits
;
; ENTRY: es:edi -> start of line in video memory
;        
; EXIT:  es:edx -> start of line
;        If (Zero flag set)
;               Only Bk bits in the line
;        else
;               es:edi -> first non-bk dword  in the line
;
;*****************************************************************************

SkipBkBits proc near
    ;
    ; Look for non-background Bits
    ;
	mov	edx,edi 		; Save start of this line
	movzx	ecx,DDPWid		; This many Dwords wide
	xor	eax,eax
	mov	es,VidSel
	cld
	repe	scas dword ptr es:[edi] ; Look for non background

        ret

SkipBkbits endp


;*****************************************************************************
;
; SetupLineBits - return ptr to LineBits in ES:EDI(dword aligned)
;                 (All modes)
;
;*****************************************************************************
SetupLineBits proc near
	push	ss
	pop	es
        xor     edi,edi
	lea	di,LineBits		; es:di -> bits
	test	di,0000000000000011B	; Line bits dword aligned?
	jz	short SLDone		; Yes
	inc	di			; Adjust to dword align
	inc	di
SLDone:
        ret
SetupLineBits endp

;*****************************************************************************
;
; XferBits - Xfer bits from Vid mem to LineBits
;
; ENTRY:
;	SS:BP -> Graphics paint frame
;	GrbFlags_RevColBit indicates if this is a normal, or inverted color
;	DS:ESI -> mono bits (one line) to render
;	ES:EDI -> LineBits
; EXIT:
;
; USES:
;	AX,CX,DI,SI,FLAGS
;
;*****************************************************************************
XferBits proc near

	push	ax			; Line # of this line

        call    SetupLineBits
        mov     ds,VidSel
        mov     esi,edx                 ; ds:esi -> start of line in video mem
;
; Graphics is quad-scan line interleaved at addresses
; B0000, B2000, B4000, and B6000 or
; B8000, BA000, BC000, and BE000 depending on which graphics page is current
;
; Each pixel is represented by a single
; bit.	The background color is black.	The foreground color is white
; All we have to do is MOVSD the bits into the one line bitmap
;
        cld
        movzx   ecx,DDPWid              ; DDPWid*32pixels = DDPWid*32 bits
        rep movs dword ptr es:[edi], dword ptr ds:[esi] ; ds:esi = video mem, es:edi = LineBits
;******
	db	67h			; Problem Workaround DO NOT REMOVE
	nop
;******

	pop	ax			; Line #
        ret

XferBits endp

;****************************************************************************
;
; ModeGrfx - Paint a graphics mode display
;
;    This routine Paints the old app graphics screen into a window
;
;    Modified from the EGA version, converted into a HERC version.
;
;    Our Strategy is as follows:
;	Construct a "screen DC" as large as the paint rectangle
;	Construct a "line DC" as wide as the paint rectangle and one scan high
;	Patblt the background color (white or black) into the screen DC
;	Look at the physical display bits in the paint rectangle for PELs which
;		are not background
;	    When a non-background PEL is found, build the line the PEL is on
;		in the Line Bitmap then BitBlt the line into the screen DC
;	BitBlt the screen DC onto the Display DC where the paint rectangle is
;
; ENTRY:
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	AH = Mode byte from controller state
;	CX != 0 if we are building a BITMAP for a display format grab
; EXIT:
;       If CX == 0 on input
;           if (Carry Clear)
;	        Screen Painted
;           else         
;	        Screen not painted, probably low Windows memory problem
;	If CX != 0 on input, 
;           if (Carry Clear)
;	        AX is bit map handle
;           else         
;	        AX = 0, probably low Windows memory problem
;
; USES:
;	ALL but DS,SI,BP
; NOTE:
;	This routine may set the fGrbProb bit in EPStatusFlags in the
;	Extended paint structure to indicate that this app is in a mode
;	which cannot be rendered into a Window. This routine must return with
;	Carry clear in this case.
;
; The background color is black.  The foreground colour is white.
;
;****************************************************************************

ModeGrfx proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

        call    InitFrameVars
	or	cx,cx                           ; Grabbing?
	jz	short NoGrab                    ;  N: 
	or	GrbFlags,GrbFlags_GrfxGrabBM	;  Y:Indicate grabbing BitMap
NoGrab:
        call    CheckMode
        jc      SetProbBit
    ;
    ; Build display paint rectangle - paint rect
    ; rounded to convenient dword aligned values
    ;
        call    BuildDisplayRect
        jz      InvalidRect     ;rect was off screen or invalid rect CoOrds

        call    AllocRes
	jc	MGError         ; alloc failed

        call    BuildDIBHdr

    ;
    ; PatBlt the background brush into the full screen DC
    ;	 to paint in the background color
    ;
	push	bx
	cCall	PatBlt,<ScreenDC,0,0,DDRCWid,DDRCHig,PATCOPY_H,PATCOPY_L>
	pop	bx

AllModeBits:
        call    SetParams
LookBits:
    ;
    ; Look for non-background Bits
    ;
	push	edi			
	push	cx			
        call    SkipBkBits
	jz	short LookMoreBits		; All background bits
DoGBits:
    ;
    ; Compute line number (in the bitmap) of the line with non-background bits,
    ; and backup the index to the start of this line.
    ;
	pop	ax			; Get line left count
	push	ax			; back on stack
	shl	ax,2

	mov	bx,GrbFlags
	and	bx,GrbFlags_GrfxIntrlv	; get interleave number
	sub	ax,bx

	sub	ax,DDRCHig
	neg	ax			; Line number
        push    ax
        call    XferBits
LineDone:
	pop	ax			; Line # 
	push	ss
	pop	ds
        mov     si,SavEXTP
	call	LineBlt
    ;
    ; Found some (more) non-background bits
    ;
LookMoreBits:
	pop	cx			; recover count
	pop	edi			; Recover start
	add	edi,GrxBitWid/8		; Next line
	dec	cx			; One line done
	jnz	LookBits		; More lines
	mov	ax,GrbFlags
	and	ax,GrbFlags_GrfxIntrlv	; get scan line interleave bits
	cmp	al,3			; fourth interleave just finished?
	jz	short DisplayMap	; Done
	inc	GrbFlags		; increment to next scan line interleave
	jmp	AllModeBits		; Do odd scans

;----------------------------------------------------------------------------

AdjWid:
	xchg	cx,bx			; Don't overrun screen bitmap
	sub	bx,cx			; BX is width of "extra" on right
	mov	RightWid,bx
	mov	dx,[si.Pstruct.PSrcPaint.rcRight]
	sub	dx,bx			; DX is Left side of it (X coord)
	mov	RightXpos,dx
	jmp	short widok

AdjHig:
	xchg	cx,bx			; Don't overrun screen bitmap
	sub	bx,cx			; BX is height of "extra" on bottom
	mov	BottomHig,bx
	mov	dx,[si.Pstruct.PSrcPaint.rcBottom]
	sub	dx,bx			; DX is Top side of it (Y coord)
	mov	BottomYpos,dx
	jmp	short HighOK

DisplayMap:
	bt	GrbFlags,GrbFlags_GrfxGrabBMBit  ; Skip This?
	jc	CleanUp		             ; Yes
    ;
    ; Display the paint part of the Screen Bitmap
    ;
    ; Because of rounding and scroll bar position the paint rect may overflow
    ;  the physical display causing an "extra" area on the top or bottom or
    ;  both. We deal with these by blitting them in the overscan color.
    ;
    ; Init "extra" parts to NONE
    ;
	xor	ax,ax
	mov	RightWid,ax
	mov	BottomHig,ax
    ;
    ; Invert the selection, if any, in the screen bitmap
    ;
	call	InvSel
    ;
    ; Blit the screen bitmap into the display paint rect
    ;
	push	[si.Pstruct.psHdc]	  ; hDestDC
	mov	ax,[si.Pstruct.PSrcPaint.rcLeft]
	mov	cx,[si.Pstruct.PSrcPaint.rcRight]
	sub	cx,ax
	push	ax			; X
	mov	ax,[si.Pstruct.PSrcPaint.rcTop]
	push	ax			; Y
	mov	bx,GrxBitWid
	sub	bx,[si.PGVDRect.rcLeft]
	cmp	cx,bx
	ja	short AdjWid
widok:
	push	cx			; nWidth
	mov	cx,[si.Pstruct.PSrcPaint.rcBottom]
	sub	cx,ax
	mov	bx,GrxBitHeight
	sub	bx,[si.PGVDRect.rcTop]
	cmp	cx,bx
	ja	short AdjHig
HighOK:
	push	cx			; nHeight
	push	ScreenDC		; hSrcDC
	mov	ax,[si.PGVDRect.rcLeft]
	sub	ax,DDRCLeft
	push	ax			; XSrc
	mov	ax,[si.PGVDRect.rcTop]
	sub	ax,DDRCTop
	push	ax			; YSrc
	push	SRCCOPY_H		; dwRop
	push	SRCCOPY_L
	cCall	BitBlt
    ;
    ; Un-Invert the selection rectangle, if any
    ;
    ;	 call	 InvSel2     ; We don't need to do this since we will shortly
    ;			     ;	 Discard the screen DC and bitmap
    ; Is there any "extra" area?
    ;
	mov	ax,RightWid
	or	ax,BottomHig
	jz	short CleanUp
DoExtra:
    ;
    ; PatBlt the overscan color into the "extra" areas.
    ;
	call	CreateOVBrGrx
	jz	short CleanUp           ; Hmmm.... Barfed don't complain though
	push	ax			; Save old brush
	cmp	RightWid,0		; Extra on right?
	jz	short TryBot		; No

	push	[si.Pstruct.psHdc]	  ; hDestDC
	push	RightXPos		; X
	push	[si.Pstruct.PSrcPaint.rcTop] ; Y
	push	RightWid		; nWidth
	mov	ax,[si.Pstruct.PSrcPaint.rcBottom]
	sub	ax,[si.Pstruct.PSrcPaint.rcTop]
	push	ax			; nHeight
	push	PATCOPY_H
	push	PATCOPY_L
	cCall	PatBlt

TryBot:
	cmp	BottomHig,0		; Extra on bottom?
	jz	short Selback		; No

	push	[si.Pstruct.psHdc]	  ; hDestDC
	push	[si.Pstruct.PSrcPaint.rcLeft] ; X
	push	BottomYPos		; Y
	mov	ax,[si.Pstruct.PSrcPaint.rcRight]
	sub	ax,[si.Pstruct.PSrcPaint.rcLeft]
	push	ax			; nWidth
	push	BottomHig		; nHeight
	push	PATCOPY_H
	push	PATCOPY_L
	cCall	PatBlt
    ;
    ; Put back the previous brush
    ;
Selback:
	pop	ax			; Recover previous brush
	push	[si.Pstruct.psHdc]	  ; hDestDC
	push	ax
	cCall	SelectObject
	jmp	short CleanUp

SetProbBit:
	bts	[si.EPStatusFlags],fGrbProbBit	; Tell caller we can't deal
InvalidRect:
        clc
        ret     

CleanUp:
        call    DeAllocRes
        mov     ax,ScreenBitmap
	clc
	ret
MGError:
        call    DeAllocRes
        mov     ax,0
	stc
        ret

ModeGrfx endp


;**
;
; CreateOVBrGrx - Create the overscan brush for graphics mode and select it
;
; ENTRY:
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	SS:BP -> Graphics mode paint frame
; EXIT:
;	Zero flag clear
;	    Correct brush selected in display DC
;	    AX is old brush handle from display DC
;	Zero flag set
;	    Attempt to select brush failed
;	OvscnBrsh is handle to brush (zero if not created)
; USES:
;	ALL but DS,SI,DI,BP
;
; Overscan always black on Hercules
;
CreateOVBrGrx proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
    ;
    ; Create a brush in the Overscan color.
    ;
	mov	ax,[si.BlkBrshH]
	mov	OvscnBrsh,ax		; Brush handle
SelB2:
	push	[si.Pstruct.psHdc]	  ; hDestDC
	push	ax
	cCall	SelectObject		; Select the brush
	or	ax,ax
	ret

CreateOVBrGrx endp

;**
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
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	AH = Mode byte from above
; EXIT:
;	Carry Clear
;	    Screen Painted
;	Carry Set
;	    Screen not painted, probably low Windows memory problem
; USES:
;	ALL but DS,SI,BP
;
; NOTE:
;	This routine may set the fGrbProb bit in EPStatusFlags in the
;	Extended paint structure to indicate that this app is in a mode
;	which cannot be rendered into a Window. This routine must return with
;	Carry clear in this case.
;
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

ModeText proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing

        call    InitTextFrameVars       ; Initialize frame variables
        jc      SetProbBitText
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
                                ; or GetVidSel failed(Problem bit set)
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
        add     edi,eax
	mov	es,VidSel
	mov	ax,word ptr es:[edi]	; Get attribute of first char in AH
	call	AdjustSelText		; Do adjustment if needed
	not	ah			; Make color "different" to cause
					;  initial set
	and	ah,FgTxtCol+BkTxtCol
	mov	Currcols,ah
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
	cmp	[si.EPGrabDTA.CntrlSt.VDA_HERC_Mode],HERC_Mode_text
	jnz	short NoMode30

	push	esi
	push	edx
	mov	esi,edi
	movzx	edx,BytesToRight
	sub	esi,edx
	push	ds
	push	es
	pop	ds
	call	IsDBCSMiddleText    ;left bound on DBCS 2nd byte?
	pop	ds
	pop	edx
	pop	esi
	mov	cx,WidPaint
	mov	ax,[bx.TPXpos]		    ; X
	jc	short @F
	dec	edi		    ;yes, extend the left bound to DBCS 1st byte
	dec	edi		    ;char/att pair
	inc	cx		    ;increase line width
	sub	ax,[bx.TPFntWid]    ;and adjust x starting position
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
    ; Get the next char and adjust it if it's in the selection rectangle.
    ;
	mov	ax,word ptr es:[edi]
	call	AdjustSelText
	inc	edi
	inc	edi
    ;
    ; Set new colors if needed
    ;
	cmp	ah,Currcols		; Colors changed?
	jz	short StoreChar
        call    SetNewBkFg              ;  Yes
StoreChar:
	push	edi
	mov	di,pToutBuf		; Char goes here
	mov	byte ptr [di],al
	inc	pToutBuf		; Next char
	pop	edi
ifdef	DBCS
	cmp	[si.EPGrabDTA.CntrlSt.VDA_HERC_Mode],HERC_Mode_Text
	jnz	short @F
	xor	ah,ah
	push	bx
	push	cx
	cCall	IsDBCSLeadByte,<ax>	;the char is DBCS 1st byte ?
	pop	cx
	pop	bx
	or	ax,ax
	jz	short @F
	mov	ax,word ptr es:[edi]	;yes, sel the 2nd byte too

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
	dec	cx
ifdef	DBCS
;cx might be negative if a DBCS sit on the right bound
	jg	NextChar
else
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

SetProbBitText:
	bts	[si.EPStatusFlags],fGrbProbBit ; Tell caller we can't deal with this
        jmp     short MTDone

ModeText endp

;*****************************************************************************
;
; InitTextFrameVars - Initialize text frame vars
;
; ENTRY:
;
; EXIT: Text Frame Vars initialized, 
;       CX = # of cols
;       Carry set if invalid mode
;
;*****************************************************************************

InitTextFrameVars proc near

	xor	bx,bx 
	mov	BkTxtBrsh,bx
	mov	WidRgt,bx
	mov	HigBot,bx
	mov	OldBrshHand,bx
	mov	OldFontHand,bx
	cmp	ah,HERC_MODE_48KText
	je	short ITError
	mov	cx,80

        clc
        ret
ITError:
        stc
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
	mov	cx,25			; 25 lines on HERC
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
	call	CreateOVBrTxt
	lea	di,[si.Pstruct.PSrcPaint]
	mov	ax,[di.rcRight]
	sub	ax,[di.rcLeft]
	mov	dx,[di.rcBottom]
	sub	dx,[di.rcTop]
	cCall	PatBlt,<[si.Pstruct.psHdc],[di.rcLeft],[di.rcTop],ax,dx,PATCOPY_H,PATCOPY_L>
InvalidRectText:
        stc
        ret

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

	mov	ax,[si.AltFnt1.FontHand]	; Assume 25 line
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
	call	CreateOVBrTxt
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
	mov	bh,Currcols
	mov	bl,ah			; BH is old bk, BL is new bk
	and	bx,(BkTxtCol shl 8) + BkTxtCol	; BH is old back, BL is new back
	cmp	bh,bl			; Changed?
	jz	short SameBk		; No
GetBk:
	mov     ax,0FFFFh
	mov	dx,00FFH		; Assume new color is white
	test	bl,BkTxtCol
	jnz	short SetBk
	xor	ax,ax			; New color is black
	mov	dx,ax
SetBk:
	cCall	SetBkColor,<[si.Pstruct.psHdc],dx,ax>
SameBk:
    ;
    ; Set new foreground color
    ;
	pop	ax
	push	ax
	mov	bh,Currcols
	mov	bl,ah			; BH is old foreground, BL new
	and	bx,(FgTxtCol shl 8) + FgTxtCol	; BH is old Fgnd, BL is new
	cmp	bh,bl			; Changed?
	jz	short SameFg		; no
GetFg:
	mov     ax,0FFFFh
	mov	dx,00FFH		; Assume new color is white
	test	bl,FgTxtCol
	jnz	short SetFg
	xor	ax,ax			; New color is black
	mov	dx,ax
SetFg:
	cCall	SetTextColor,<[si.Pstruct.psHdc],dx,ax>
SameFg:
	pop	ax
	pop	cx
	pop	bx
	pop	es
	mov	Currcols,ah	        ; Set current colors

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
; AdjustSelText - Adjust current char if it is in the selection rect
;
; ENTRY:
;	SS:BP -> Text paint frame
;	DS:SI -> Extended paint structure
;	AX is attr/char of interest
;	DI indicates AX's location in the video buffer
;	   together with StartPg and EPSeg4KPage
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
	xchg	al,ah
	call	GetTxtColor
	xchg	ah,al
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
	and	ah,FgTxtCol+BkTxtCol		; Mask to color bits
	push	ax
NotInSelRect:
	pop	ax
	pop	cx
	pop	dx
	ret

AdjustSelText endp


;**
;
; CreateOVBrTxt - Create the overscan brush for text mode and select it
;
; ENTRY:
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	SS:BP -> Text mode paint frame
; EXIT:
;	Correct brush selected in display DC
; USES:
;	ALL but DS,SI,DI,BP
;
; Overscan always black on Hercules
;
CreateOVBrTxt proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
    ;
    ; Try to create a brush in the overscan color
    ;
	mov	ax,[si.BlkBrshH]
	mov	BkTxtBrsh,ax
    ;
    ; Select the brush into the display DC
    ;
	push	[si.Pstruct.psHdc]	  ; hDC
	push	ax			; hBrush
	cCall	SelectObject
	mov	OldBrshHand,ax
	ret

CreateOVBrTxt endp

;***********************************************************************************
;
; BuildDIBHdr - Build Device Independent Bitmap Header for mono bitmap
;
;***********************************************************************************

BuildDIBHdr proc near

        push    di
        push    eax
   ;
   ; Fill PelColorTable entries
   ;
        lea     di,PelColorTable
        mov     [di.rgbRed],0
        mov     [di.rgbGreen],0
        mov     [di.rgbBlue],0
        mov     [di.rgbResvd],0
        add     di,4
        mov     [di.rgbRed],0FFh
        mov     [di.rgbGreen],0FFh
        mov     [di.rgbBlue],0FFh
        mov     [di.rgbResvd],0

        lea     di,DIBHdr
        mov     [di.biSize],40               ; 40 bytes
        movzx   eax,DDRCWid
        mov     [di.biWidth],eax             ; Width
        movzx   eax,DDRCHig
        mov     [di.biHeight],eax            ; height
        mov     [di.biPlanes],1              ; # of planes

        mov     [di.biCompression],0         ; set to 0
        mov     [di.biSizeImage],0           ; 
        mov     [di.biXPelsPerMeter],0       ; 
        mov     [di.biYPelsPerMeter],0       ; 
        mov     [di.biClrused],0             ; use all colors - default
        mov     [di.biClrImportant],0        ; 

        mov     [di.biBitCount],1            ; assume mode 6 or 11

        pop     eax
        pop     di
        ret

BuildDIBHdr endp

sEnd	code
	end


