;-----------------------------------------------------------------------------
;
;   Screen Grabber for V7 VRAM VGA adaptor
;
;   These routines perform paints and all other Display specific
;	aspects of WINOLDAP (VMDOSAPP)
;
;    (C) Copyright MICROSOFT Corp. 1986-1990
;
;    ************* MICROSOFT CONFIDENTIAL ******************
;
;-----------------------------------------------------------------------------
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

;-----------------------------------------------
;
; External Windows Procedures
;
externFP	PatBlt
externFP	SelectObject
externFP	CreateCompatibleDC
externFP	CreateCompatibleBitmap
externFP	CreateBitmap
externFP	GetNearestColor
externFP	CreateSolidBrush
externFP	BitBlt
externFP	DeleteDC
externFP	DeleteObject
externFP	SetBkColor
externFP	SetTextColor
externFP        TextOut

;-----------------------------------------------
;
; External GRABBER Procedures
;
externNP	ComputeSelTextRect
externNP	InvSel
externNP	InvSel2
externNP	GetVidSel
externNP	ClearVidSel
externNP	SetScreenBMBits
externNP	GetWinRGB

;=============================================================================
;
; Documentation for building DIBs for paints and grabs:
;
; Mode 4,5 : 
;   Packed pixel format - 2 bits per pixel in the Video memory. 
;   DIBs are also packed pixel and can have 1/4/8 bits per pixel. 
;   Hence choose 4. Extend the 2 bit value to a 4 bit value.
;   Pixel doubling occurs in the Horizontal direction. Therefore 4 bits
;   in Video memory are translated into 16 bits. To speed up the translation
;   build a translation Table of 16 words. For a given 4 bit index representing 
;   2 pixels, extend each 2 bit value to a 4 bit value to get a 8 bit value. 
;   Now double each 4 bit value(pixel doubling) to get a 16 bit value. Since 
;   the bytes get reversed during a store operation reverse the bytes in the 
;   Table.
;
;   Algorithm:
;       Fetch a DWORD PQRS, from Video memory.
;       It exists in memory as SRQP. Hence process bytes in the
;       following order S,R,Q,P.
;
;       Within S(S7-S0) process high nibble first.
;
; XTableMode4:                  After Reversing the bytes 
;
; 0000 | 00000000 00000000      00000000 00000000
; 0001 | 00000000 00010001      00010001 00000000
; 0010 | 00000000 00100010      00100010 00000000
; 0011 | 00000000 00110011      00110011 00000000
;
; 0100 | 00010001 00000000      00000000 00010001
; 0101 | 00010001 00010001      00010001 00010001
; 0110 | 00010001 00100010      00100010 00010001
; 0111 | 00010001 00110011      00110011 00010001
;
; 1000 | 00100010 00000000      00000000 00100010
; 1001 | 00100010 00010001      00010001 00100010
; 1010 | 00100010 00100010      00100010 00100010
; 1011 | 00100010 00110011      00110011 00100010
;
; 1100 | 00110011 00000000      00000000 00110011
; 1101 | 00110011 00010001      00010001 00110011
; 1110 | 00110011 00100010      00100010 00110011
; 1111 | 00110011 00110011      00110011 00110011
;
;
; To build the PelColorTable, we need to fill in only the first four entries,
; 0000,0001,0010,0011. Call GetWinRGB to get the xBGR dword value.
;       
; Mode 6 :
;       1 bit per pixel, packed pixel format in the Video memory. 
;       For the DIB choose 1 bpp.
;       There is no pixel doubling in the Hor. direction. Transfer the bits
;       from the source(video memory) to the destination using REP MOVSD.
; To build the PelColorTable, we need to fill only the first two entries.
; Call GetWinRGB to get the entries.
;
; Mode 11 is similar but the format of the video memory is different.
;
;
; Modes E,10 :
;  4 bits per pixel, four bit number formed by concatenating the bits from 
; Planes 3,2,1,0. 
; 
; Mode E,10: Read a word from 
;                          Plane 3 into edx
;                          Plane 2 into ecx
;                          Plane 1 into ebx
;                          Plane 0 into eax
;
; Process the bytes PQRS in the dword Regs from right to left.
; Process the bits in the byte registers from right to left - two bits at a 
; time, the higher one first.
; After forming a 8 bit code OR it into edi and shl by 8. 
; Thus an 8 bit value gives rise to a 32 bit value to be stored in LineBits.
;
; Mode D: Read a word of info into AX,BX,CX,DX
;         Process bytes from right to left,
;         Exchange nibbles in each byte, since higher nibble has to be
;         processed before the lower nibble.
;
; Complex order of processing the bits in the above cases because of byte
; swapping on 8086.
;
;===================================================================================

sBegin	code
	assumes cs,code

	extrn	RGBTable:word
	extrn	RGBTable10:word
	extrn	RGBTableDE:word

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
	public	CreateOVBrGrx

    ;
    ; helper routines for ModeGrfx
    ;
        public  InitFrameVars
        public  CheckMode
        public  PatBltOverScanColor
        public  BuildDisplayRect
        public  BuildDIBHdr
        public  AllocRes
        public  AllocBM
        public  SetParamsCGAMode
        public  SetParamsMode11
        public  SkipBkBits
        public  SetParamsPlanarMode
        public  DeAllocRes
        public  SetupLineBits
	public	GetDwordDataEF10
        public  GetWordDataD

XTableMode4 label dword
        dw      0000000000000000b
        dw      0001000100000000b
        dw      0010001000000000b
        dw      0011001100000000b
        
        dw      0000000000010001b
        dw      0001000100010001b
        dw      0010001000010001b
        dw      0011001100010001b
        
        dw      0000000000100010b
        dw      0001000100100010b
        dw      0010001000100010b
        dw      0011001100100010b
        
        dw      0000000000110011b
        dw      0001000100110011b
        dw      0010001000110011b
        dw      0011001100110011b

;*****************************************************************************
; Initialize frame variables used by grab and paint code
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
; CheckMode - Check for valid EGA/VGA mode
;
; ENTRY: AH = video mode
;        DS:SI -> EXTPAINTSTRUC
;
; EXIT:  Carry set if mode is invalid
;
;*****************************************************************************
CheckMode proc near

	cmp	ah,13H
IFDEF DEBUG
	ja	short BadMode
	je	short GrbProblem
ELSE
	jae	short BadMode
ENDIF
	cmp	ah,7
	jb	short CMDone
	cmp	ah,0DH
	jae	short CMDone
BadMode:
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
	cmp	bx,GrxBitWid640		; Is paint rect "on screen"?
	jae	RectOffScreen		; No
	mov	DDRCLeft,bx
	shr	bx,3			; Divide by 8 for Byte index of this loc
	cmp	ah,0DH			; Q: Mode 0D?
	jne	short SetBI             ;   N:
	shr	bx,1			;   Y: screen is half as wide
SetBI:
	mov	DDRCLeftBI,bx
	mov	bx,[si.PGVDRect.rcRight]
	add	bx,31
	and	bx,1111111111100000B	; Round up to DWORD (32 bit) boundary
	cmp	bx,GrxBitWid640		; Limit to max size of physical screen
	jbe	short OkBWid
	mov	bx,GrxBitWid640
OkBWid:
	mov	DDRCRight,bx
	sub	bx,DDRCLeft
	jbe	short InvalidCoOrd
	mov	DDRCWid,bx
	shr	bx,3			; Divide Width by 8 to get width in Bytes
	mov	DBPWid,bx		; NOTE: For mode 0D display this is width in NIBBLES
	mov	BDsWid,bx
	cmp	ah,0DH			; Q: Mode 0D?
	jne	short NoDv3             ;   N:
	shr	BDsWid,1		;   Y: screen if half as wide
NoDv3:
	shr	bx,2			; Divide Width by 32 to get width in DWORDS
	mov	DDPWid,bx		; NOTE: For mode 0D display this is width in WORDS!
	mov	bx,[si.PGVDRect.rcTop]
    ;
    ; What follows is to round the top and height to a mult of 4 except in mode 10H.
    ;  We do this so that the first line is always an even scan line, and
    ;  the height represents equal numbers of even and odd scan lines. This
    ;  prevents us from having to do wierd boundary checks to make sure we
    ;  don't overflow the full screen bitmap.
    ;
	mov	cx,0FFFFH		; Top, Height round is none
	xor	dx,dx			; Top, Height round is none
	mov	di,GrxBitHeight480	; Max height of mode 11,12H
	cmp	ah,11H
	jae	short GotVals
	mov	di,GrxBitHeight350	; Max height of mode 10H
	cmp	ah,10H
	je	short GotVals
	cmp	ah,0FH
	je	short GotVals
        mov     di,GrxBitHeight400      ; Max height of other modes
	inc	dx			; Top, Height round is 2 modes D and E
	dec	cx			; Top, Height round is 2 modes D and E
	cmp	ah,0DH
	jae	short GotVals		; Modes D,E have round of 2
	mov	cx,1111111111111100B	; Top, Height round is 4 on CGA modes
	mov	dx,3			; Top, Height round is 4 on CGA modes
GotVals:
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
; SetParamsCGAMode - Return index into video memory for the first line and
;                     number of lines to paint, for Modes 4,5,6
;
; ENTRY: DS:SI -> EXTPAINTSTRUC
;
; EXIT: ES:EDI -> first line of paint rect
;       CX = # of lines in paint rect
;
;*****************************************************************************
SetParamsCGAMode proc near

	les	edi,VidAddr             ; es:edi -> Video page 0

	xor	edx,edx 		; Assume doing evens, Vid pgs 0 and 1
	test	GrbFlags,GrbFlgs_GrfxDoOdd
	jz	short AddPageIndex	; Assumption correct
	mov	edx,2 * 4096		; Doing odds, vid pgs 2 and 3
    ;
    ; Note: The even scans run from B8000 -> BA000 which is two pages
    ;	    The odd scans run from BA000 -> BC000 which is two pages
    ;
AddPageIndex:
	add	edi,edx 		; Point to start of evens or odds

	mov	ax,DDRCTop		; Get index to first line
	shr	ax,2			; Convert to phys disp and /2 for even or odd
    ; 
    ; Mode 4,5 - 2 bpp and 320 pels wide
    ; Mode 6   - 1 bpp and 640 pels wide
    ; i.e. in both cases 640 bits/line or (640/8) bytes/line
    ; 
	mov	cx,GrxBitWid640/8		; This many bytes per line
	mul	cx			; This many bytes in

	movzx	eax,ax
	add	edi,eax 		; Point to start of this line
	mov	cx,DDRCHig		; This many lines to do
	shr	cx,2			; Convert to phys disp and evens and odds are half of screen
	movzx	edx,DDRCLeftBI
	add	edi,edx 		; Index to left edge in display mem
        ret

SetParamsCGAMode endp

;*****************************************************************************
;
; SetParamsMode11 - Set params for Mode 11
; 
; ENTRY: 
;
; EXIT: ES:EDI -> first line of paint rect
;       CX = # of lines to paint
;
;*****************************************************************************

SetParamsMode11 proc near

	mov	ax,DDRCTop		; Get index to first line
	mov	cx,GrxBitWid640/8		; This many bytes per line
	mul	cx			; This many bytes in
	les	edi,VidAddr             ; es:edi -> Video page 0
	movzx	eax,ax
	add	edi,eax 		; Point to start of this line
	mov	cx,DDRCHig		; This many lines to do
	movzx	edx,DDRCLeftBI		; Index to left edge in display mem
	add	edi,edx
        ret

SetParamsMode11 endp

;*****************************************************************************
;
; SkipBkBits - Skip across background bits(modes 4,5,6,11 only)
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
    ; Mode 4,5 - convert to phy display(/2) and 2 bpp(*2)
    ; Thus width in mode 4,5 = (DDPWid/2)*2 dwords
    ; Mode 6,11   - 1 bpp and phy disp 640 wide
    ; Mode 6,11 is DDPWid dwords
    ;
	mov	edx,edi 		; Save start of this line
	movzx	ecx,DDPWid		; This many Dwords wide
	xor	eax,eax
	mov	es,VidSel
	cld
	repe	scas dword ptr es:[edi] ; Look for non background
        ret

SkipBkbits endp

;***************************************************************************
;
; XferMonoBits - Transfer bits from Vid mem to LineBits - Mode 6,11 only
;
; ENTRY: None
;
; EXIT: Bits transferred 
;
; USES: es,edi,ds,esi
;
;
;***************************************************************************

XferMonoBits proc near

	push	ax			; Line # of this line

        call    SetupLineBits
        mov     ds,VidSel
        mov     esi,edx                 ; ds:esi -> start of line in video mem
        cld
        movzx   ecx,DDPWid              ; DDPWid*32pixels = DDPWid*32 bits
        rep movs dword ptr es:[edi], dword ptr ds:[esi] ; ds:esi = video mem, es:edi = LineBits

	pop	ax			; Line #
        ret

XferMonoBits endp

;*****************************************************************************
;
; AllocRes - Allocate resources common to paint code and grab code
;
; ENTRY: DS:SI -> EXTPAINTSTRUC
;
; EXIT: carry flag set if error
;
;*****************************************************************************

AllocRes proc near
    ;
    ; Set the background color
    ;
	xor	ax,ax		; The background is color palette 0
	call	GetWinRGB	; Get bkgrnd color
	mov	BcolHigh,dx
	mov	BcolLow,ax
    ;
    ; Create a memory DC for full screen compatible with the display
    ;
	cCall	CreateCompatibleDC, <[si.Pstruct.psHdc]>

	or	ax,ax
	jz	short ACRError
	mov	ScreenDC,ax
    ;
    ; Create a brush in the background color
    ;
	cCall	GetNearestColor,<ScreenDC,BColHigh,BColLow>

	push	dx
	push	ax
	cCall	CreateSolidBrush

	or	ax,ax
	jz	short ACRError
	mov	BkBrush,ax
    ;
    ; Select the background brush into the screen DC
    ;
	cCall	SelectObject,<ScreenDC,ax>
	or	ax,ax                   
        jz      short ACRError
        clc
        ret

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
	bt	GrbFlags,GrbFlgs_GrfxGrabBMBit	; Keep This?
	jc	short TestBkBr  		; Yes
	cCall	DeleteObject,<cx>
TestBkBr:
	bt	GrbFlags,GrbFlgs_GrfxDispBltBit
        jnc     short DelBkBr
    ;
    ; Select back the Old brush into hDC and then delete the BkBrush
    ;
	cCall	SelectObject,<[si.Pstruct.psHdc],SavOldBr>
DelBkBr:
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

;*****************************************************************************
;
; SetParamsPlanarMode - Used only for modes D,E,F,10
; 
; ENTRY:
;
; EXIT: Sets up 
;       VideoIndex = offset of the first line in Video mem
;         LineSkip = Bytes to skip to next line of Paint rect in Vid mem
;               CX = # of lines
;
;*****************************************************************************
SetParamsPlanarMode proc near
    ;
    ; Skip in to first line of paint, map correct pages
    ;
	mov	ax,DDRCTop		; Get index to first line
	mov	cx,GrxBitWid640/8		; This many bytes per line modes E and 10
	cmp	Gmode,0FH
	jae	short NoDv4
	shr	ax,1			; Convert to phys disp modes D and E
	cmp	Gmode,0DH
	jne	short NoDv4
	shr	cx,1			; Mode D screen is half as wide
NoDv4:
	mov	LineSkip,cx		; Save this val
	mul	cx			; This many bytes in
	add	ax,DDRCLeftBI		; Index to left edge in display mem
	movzx	eax,ax
        mov     VideoIndex,eax          ; save index to the left edge
	mov	cx,DDRCHig		; This many lines to do
	cmp	Gmode,0FH
	jae	short NoDv7
	shr	cx,1			; Convert to phys disp
NoDv7:
	mov	ax,BDsWid
	sub	LineSkip,ax		; Value to skip to next line in display
        ret

SetParamsPlanarMode endp

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
; GetDwordDataEF10 - return a dword of pixel info, Modes E,F,10 only
;                   EAX = I bits,EBX = R bits,ECX = G bits,EDX = B bits
; 
; NOTE: Mode F, Plane 3 and 1(I & G planes ) are skipped in the video memory
;       To convert to windows format, 
;               windows I Plane = Plane 2 data
;               windows R,G,B planes = data from Plane 0
;
;*****************************************************************************

GetDwordDataEF10 proc near

        mov     ds,VidSel
	cld

        mov     esi,BPlaneOffset        ; Point at Blue plane
        add     esi,VideoIndex
	lods dword ptr ds:[esi] 	
	mov	edx,eax 		; Get Blue in EDX

        cmp     Gmode,0Fh
        je      SHORT SkipGPlane
        mov     esi,GPlaneOffset        ; Point at green
	add	esi,VideoIndex  	
	lods dword ptr ds:[esi] 	
	mov	ecx,eax 		; Get Green in ECX

SkipGPlane:
        mov     esi,RPlaneOffset        ; Point at Red
	add	esi,VideoIndex  	
	lods dword ptr ds:[esi] 	
        mov     ebx,eax                 ; Get Red in EBX

        cmp     Gmode,0Fh
        je      SHORT SkipIPlane
        mov     esi,IPlaneOffset
        add     esi,VideoIndex          ; Point at Intensity plane
        lods    dword ptr ds:[esi]      ; Get Intensity in EAX
        jmp     SHORT GDE_Exit

SkipIPlane:
        mov     eax,ebx
        mov     ecx,edx
        mov     ebx,edx
GDE_Exit:
        add     VideoIndex,4            ; Advance to next dword for Mode E,F,10
        ret     

GetDwordDataEF10 endp


;*****************************************************************************
; 
; GetWordDataD - return a word of pixel info in AX,BX,CX,DX
;                AX = I bits, BX = R bits, CX = G bits, DX = B bits
;
;*****************************************************************************
GetWordDataD proc near

        mov     ds,VidSel
	cld

        mov     esi,BPlaneOffset        ; Point at Blue plane
        add     esi,VideoIndex
	lods word ptr ds:[esi]		; Get Blue
	mov	dx,ax			; Blue in DX

        mov     esi,GPlaneOffset        ; Point at Green
	add	esi,VideoIndex  	
	lods word ptr ds:[esi]		; Get Green
	mov	cx,ax			; Green in CX

        mov     esi,RPlaneOffset
	add	esi,VideoIndex  	; Point at Red
	lods word ptr ds:[esi]		
        mov     bx,ax                   ; Get Red in BX

        mov     esi,IPlaneOffset        ; Point at Intensity plane
        add     esi,VideoIndex          
        lods    word ptr ds:[esi]       ; Get Intensity in AX

        add     VideoIndex,2            ; Advance to next word 
        ret

GetWordDataD endp

;****************************************************************************
;
; AllocBM - Allocate a bitmap for paint or grab
;
; If (in PaintMode)
;       CreateCompatibleBitmap(...)
;       if (Success)        
;               Select ScreenBM into ScreenDC
;       else
;               Set DispBlt flag
;               Select BkBrush into DisplayDC
;
; else if (in Grab mode)
;       CreateBitmap(..)        /* smaller bitmap may be sufficient for mono Bms*/
;       Select ScreenBM into ScreenDC
;
; ENTRY:
;
; EXIT: Carry set on Error
;       BX contains hBM if allocated
;
;****************************************************************************

AllocBM proc near

	bt	GrbFlags,GrbFlgs_GrfxGrabBMBit	; Grabbing?
        jnc     short PaintMode                 ; No, painting
IF1
%OUT Use CreateBitmap here
ENDIF
    ;
    ; Create the full screen bitmap to the dimensions of the rounded paint rect
    ;
	cCall	CreateCompatibleBitmap, <[si.Pstruct.psHdc],DDRCWid,DDRCHig>
	or	ax,ax
	jz	short ABMError	; Fail.....
	mov	bx,ax
        push    bx
        cCall   SelectObject, <ScreenDC,bx>
        pop     bx
        or      ax,ax
        jz      short ReturnBitmap
	mov	ScreenBitMap,bx

ABMDone:
        clc
        ret

ReturnBitmap:
        mov     ScreenBitmap,0
        cCall   Deleteobject,<bx>
ABMError:
        stc
        ret

PaintMode:
IF1
%OUT Use CreateBitmap here
ENDIF
    ;
    ; Create the full screen bitmap to the dimensions of the rounded paint rect
    ; If this fails (lack of memory) set GrbFlgs_GrfxDispBlt to cause blt 
    ; directly into display DC instead of ScreenDC.
    ;
	cCall	CreateCompatibleBitmap, <[si.Pstruct.psHdc],DDRCWid,DDRCHig>
	or	ax,ax
	jnz	short GotScreenBM

	or	GrbFlags,GrbFlgs_GrfxDispBlt 	; Set Display DC blit mode
    ;
    ; Select the background brush into the Display DC
    ;
	cCall	SelectObject,<[si.Pstruct.psHdc],BkBrush>
	or	ax,ax
	jz	ABMError
        mov     SavOldBr,ax
	jmp	short ABMDone

GotScreenBM:
	mov	bx,ax
    ;
    ; Select the full screen bitmap into the full screen DC
    ;
	push	bx
	cCall	SelectObject,<ScreenDC,bx>
	pop	bx

	or	ax,ax
	jz	short ReturnBitmap
	mov	ScreenBitMap,bx
        jmp     short ABMDone
        
AllocBM endp
        
;*****************************************************************************
;
; ModeGrfx - Paint a graphics mode display
;
;    This routine Paints the old app graphics screen into a window
;
;    Our Strategy is as follows:
;	Construct a "screen DC" as large as the paint rectangle
;	Patblt the background color into the screen DC
;	Look at the physical display bits in the paint rectangle for PELs which
;		are not background
;	    When a non-background PEL is found, build the line the PEL is on
;		in local memory and SetDIBits into the screen DC
;	BitBlt the screen DC onto the Display DC where the paint rectangle is
;
; ENTRY:
;	SS:BP -> Std stack frame
;	DS:SI -> EXTPAINTSTRUC structure of paint
;	AH = Mode byte from controller state
;	CX != 0 if we are building a BITMAP for a display format grab
; EXIT:
; 	If (CX == 0 on input && Carry Clear)
;	    Screen Painted
;	else if (CX == 0 on input && Carry Set)
;	    Screen not painted, probably low Windows memory problem
;       else if (CX != 0 on input and Carry Clear)
;            AX is bitmap handle
;       else if (CX != 0 on input and Carry Set)
;            AX == 0 if problem
; USES:
;	ALL but DS,SI,BP
; NOTE:
;	This routine may set the fGrbProb bit in EPStatusFlags in the
;	Extended paint structure to indicate that this app is in a mode
;	which cannot be rendered into a Window. This routine must return with
;	Carry clear in this case.
;
;*****************************************************************************

ModeGrfx proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
    ;
    ; Initialize frame variables
    ;
        call    InitFrameVars
	or	cx,cx
	jz	short NoGrab
	or	GrbFlags,GrbFlgs_GrfxGrabBM	; Indicate grabbing BitMap
NoGrab:
        call    CheckMode
        jc      SetProbBit

        call    GetVidSel
        jc      GetVidSelFailed
    ;
    ; Build display paint rectangle. This is the same as the paint rect
    ; except the edges are rounded to convenient dword aligned values
    ;
        call    BuildDisplayRect
        jz      InvalidRect     ;rect was off screen or invalid rect CoOrds

        call    AllocRes
	jc	MGError         ; alloc failed

        call    AllocBM         ; allocate bitmap
        jc      MGError         ; Fail ...
    ; 
    ; Build the DIB Hdr depending on the mode
    ;
        call    BuildDIBHdr

	cmp	Gmode,11H		; Mode 11
	jz	short DoBackPat
	cmp	Gmode,0DH		; Plane mode?
	jae	PlaneGrx		; Yes
DoBackPat:
    ;
    ; This is one of the CGA graphics modes (4,5,6) or mode 11
    ; PatBlt the background brush into the full screen DC, or the paint
    ;	rectangle to paint in the background color
    ;
	push	bx
	bt	GrbFlags,GrbFlgs_GrfxDispBltBit
	jnc	short ScrDCPB
	push	[si.Pstruct.psHdc]	  ; hDC
	lea	bx,[si.Pstruct.PSrcPaint]
	mov	ax,[bx.rcRight]
	sub	ax,[bx.rcLeft]
	mov	dx,[bx.rcBottom]
	sub	dx,[bx.rcTop]
	push	[bx.rcLeft]		  ; X
	push	[bx.rcTop]		  ; Y
 	push	ax			; nWidth
	push	dx			; nHeight
	jmp	short ScrDCPBDo

ScrDCPB:
	push	ScreenDC		; hDC
	push	0			; X
	push	0			; Y
	push	DDRCWid 		; nWidth
	push	DDRCHig 		; nHeight
ScrDCPBDo:
	push	PATCOPY_H
	push	PATCOPY_L
	cCall	PatBlt
	pop	bx

        cmp     Gmode, 11h
        je      Mode11Bits

CGAMode:
        call    SetParamsCGAMode
NextLineCGAMode:
    ;
    ; Look for non-background Bits
    ;
	push	edi                     ; push ptr to line in Vid mem
	push	cx                      ; push Line count
        call    SkipBkBits
	jz	LookMoreBits		; All background bits
    ;
    ; Compute line number of line with non-background bits, and backup
    ;	index to the start of this line
    ;
	pop	ax			; Get line left count
	push	ax			; back on stack
	shl	ax,1			; Evens/odds are every other line
	test	GrbFlags,GrbFlgs_GrfxDoOdd
	jz	short EvenLine		; Even line
	dec	ax			; Odd line
EvenLine:
	shl	ax,1			; Convert physical to Bitmap
	sub	ax,DDRCHig
	neg	ax			; Line number
	push	ax			; Line # of this line
    ;
    ; Dispatch to line build code based on mode
    ;
	mov	esi,edi 		; VidSel:esi -> 1 dword past first non-back
	cmp	Gmode,6
	jz	Mode6Line

IFDEF   DEBUG
public Mode45Line
ENDIF
Mode45Line:
    ;
    ; First store background color in LineBits
    ;
        call    SetupLineBits
        push    edi                    ; save start of LineBits
        xor     eax,eax                ; BkColor is 0000b
        cld
        movzx   ecx,DDPWid             ; DDPWid*32 pixels = DDPWid*32*4 bits
        shl     ecx,2                  ; = DDPWid*32*4/32 Dwords
                                       ; = DDPWid*4 Dwords 
        rep stos dword ptr es:[edi]
        pop     edi                    ; recover start of LineBits
    ;
    ; Adjust count to skip bk bits and the current dword
    ;  NOTE: EDX -> start of line 
    ;
	movzx	ecx,DDPWid		; This many dwords in plane
	mov	ebx,edx
	sub	ebx,esi
	neg	bx			; This many bytes to 1 dword past first non-back
	shr	bx,1			; byte cnt -> word cnt
	shr	bx,1			; word cnt -> dword cnt
	sub	cx,bx			; Skip in this many
	xchg	esi,edi
        mov     es,VidSel
    ;
    ; Now es:edi -> one dword past a non Bk dword in vid mem
    ;     es:edx -> start of line
    ;     ds:esi -> LineBits
    ;
IFDEF DEBUG
        public NextDword4
ENDIF
NextDword4:				; Found some non-background bits
	mov	eax,dword ptr es:[edi-4]; Get the data
	add	edx,4			; Adjust start to compute index of this DWORD
	sub	edx,edi
	neg	dx			

; DX is byte index of the first non-bk dword in source buffer(w.r.t. start of
; this line)
; dx*8 is the bit index in source
; dx*8/2 is pixel index in source - 2bpp in video memory
; (dx*8*2)/2 is pixel index in destn., after pixel doubling
; dx*8*4 is bit index in destn. - 4bpp
; dx*8*4/8 is the byte index in destn.

        shl     dx,2
	add	si,dx   		; Skip Bk bits in LineBits

NonBkBits4:        
        push    bx
        push    ecx
        push    dx
        push    edi
        mov     cx,4                   ; four bytes in a dword

NextByte4:
        mov     bl,al                  ; get a byte
        xor     bh,bh                  ; clear rest of bx
        push    bx                     ; save the byte
        rol     bl,4                   ; exchange nibbles, Xlate high nibble first
        and     bl,0fh                 ; look at low 4 bits
        shl     bx,1                   ; word index into XTableMode4
        mov     di,bx                  ; move index into index reg
        mov     dx,word ptr XTableMode4[di]
        mov     [si],dx
        add     si,2

        pop     bx                     ; restore the byte
        and     bl,0fh                 ; look at low nibble
        shl     bx,1                   ; word index into XTableMode4
        mov     di,bx                  ; move index into index reg
        mov     dx,word ptr XTableMode4[di]
        mov     [si],dx
        add     si,2

        ror     eax,8                  ; shift the next byte into AL
        loop    NextByte4

        pop     edi
        pop     dx
        pop     ecx
        pop     bx

        test    cx,cx
        jz      short LineDone

Bit4Loop4:				; More bits
        push    edi
	cld
	xor	eax,eax
	repe scas dword ptr es:[edi]	; Look for more non-backgrnd bits on this line
	pop	edx
	jz	short LineDone		; Done with line
	jmp	NextDword4

Mode6Line:
    ;
    ; when we find a line with non bk bits, we transfer all the bits from the 
    ; video mem to the LineBits array since both the video mem and the DIB 
    ; have the same bit format of 1 bpp
    ; There is no need to first blit in the Bk color in LineBits and then process
    ; the non bk bits as in Mode 4.
    ; 
    ; now, es:edi points to start of LineBits
    ;
        call    XferMonoBits
LineDone:
	pop	ax			; Line #
	push	ss
	pop	ds
	mov	si,SavEXTP
	call	SetScreenBMBits
LookMoreBits:
	pop	cx                      ; recover Line count
	pop	edi			; Recover start of line
	add	edi,GrxBitWid640/8 	; Next line
	dec	cx			; One line done
	jnz	NextLineCGAMode	; More lines
	bts	GrbFlags,GrbFlgs_GrfxDoOddBit
	jc	short DisplayMap        ; Odd lines done?
	jmp	CGAMode	        ; Do odd scans

Mode11Bits:
        call    SetParamsMode11
LookBits11:
    ;
    ; Look for non-background Bits
    ;
	push	edi
	push	cx
        call    SkipBkBits
	jz	short LookMoreBits11		; All background bits
    ;
    ; Compute line number of line with non-background bits, and backup
    ;	index to the start of this line
    ;
	pop	ax			; Get line left count
	push	ax			; back on stack
	sub	ax,DDRCHig
	neg	ax			; Line number
    
        call    XferMonoBits            ; copy bits from vid mem to Linebits
	push	ss
	pop	ds
        mov     si,SavEXTP
	call	SetScreenBMBits

LookMoreBits11:
	pop	cx
	pop	edi			; Recover start
	add	edi,GrxBitWid640/8 	; Next line
	dec	cx			; One line done
	jnz	short LookBits11	; More lines
	jmp	short DisplayMap	; Done

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
	bt	GrbFlags,GrbFlgs_GrfxGrabBMBit  ; Skip This?
	jc	CleanUp		                ; Yes
    ;
    ; Display the paint part of the Screen Bitmap if not GrbFlgs_GrfxDispBlt 
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
    ; Invert the selction, if any, in the screen bitmap, or on the display if
    ;	GrbFlgs_GrfxDispBlt
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
	mov	bx,GrxBitWid640
	sub	bx,[si.PGVDRect.rcLeft]
	cmp	cx,bx
	ja	short AdjWid
widok:
	push	cx			; nWidth
	mov	cx,[si.Pstruct.PSrcPaint.rcBottom]
	sub	cx,ax
	mov	bx,GrxBitHeight480
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],11H
	jae	short GotGHGT
	mov	bx,GrxBitHeight400
	cmp	[si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],0FH
	je	short Ht350
        cmp     [si.EPGrabDTA.CntrlSt.VDA_EGA_Mode],10H
	jne	short GotGHGT
Ht350:
        mov     bx,GrxBitHeight350
GotGHGT:
	sub	bx,[si.PGVDRect.rcTop]
	cmp	cx,bx
	ja	short AdjHig
HighOK:
	bt	GrbFlags,GrbFlgs_GrfxDispBltBit
	jnc	short DoDispMapBlt
	add	sp,4*2			; Clear off BitBlt args pushed so far
	jmp	short DoDispMapExtra

DoDispMapBlt:
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

DoDispMapExtra:
    ;
    ; Is there any "extra" area?
    ;
	mov	ax,RightWid
	or	ax,BottomHig
	jz	CleanUp
DoExtra:
    ;
    ; PatBlt the overscan color into the "extra" areas.
    ;
	call	CreateOVBrGrx
	jz	CleanUp		; Hmmm.... Barfed don't complain though
	push	ax			; Save old brush
	cmp	RightWid,0		; Extra on right?
	jz	short TryBot		; No

	push	[si.Pstruct.psHdc]	  ; hDestDC
	push	RightXPos		; X
	push	[si.Pstruct.PSrcPaint.rcTop]; Y
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

    ;
    ; Do Plane Mode screen. Modes D,E,F,10
    ;
IFDEF DEBUG
public PlaneGrx
ENDIF
PlaneGrx:
        call    SetParamsPlanarMode

NextLineDEF10:
	push	cx                      ; push # of lines still left
        call    SetupLineBits
	mov	cx,DDPWid		; Width in Dwords (words mode D)
    ;
    ; Store background color in LineBits.
    ;
        push    ecx
        push    edi
    ;
    ; Width in bytes of a color plane in LineBits=DBPWid
    ; Width in dwords of a color plane=DBPWid/4
    ; Width in dwords of 4 planes=(DBPWid/4)*4
    ;
        movzx   ecx,DBPWid              ; Width in bytes of a color plane in LineBits
        xor     eax,eax
        cld 
        rep stos dword ptr es:[edi]
        pop     edi
        pop     ecx

NextDwordDEF10:
	push	cx

	cmp	Gmode,0DH
	je	short DoModeD
DoModeEF10:
	call	GetDwordDataEF10
	call	ProcessDwordEF10
	jmp	short ModeEF10Continue
DoModeD:
        call    GetWordDataD
        call    ProcessWordD
ModeEF10Continue:
	pop	cx
	loop	NextDwordDEF10

IFDEF DEBUG
public LineDoneDEF10
ENDIF
LineDoneDEF10:
	movzx	eax,LineSkip
	add	VideoIndex,eax 		; Advance to next line
	pop	ax			; Get line left count
	push	ax
	cmp	Gmode,0FH
	jae	short LinCntOK
	shl	ax,1			; Convert physical to Bitmap
LinCntOK:
	sub	ax,DDRCHig
	neg	ax			; AX is Line number
	push	ss
	pop	ds
	mov	si,SavEXTP
	call	SetScreenBMBits 	; Blast the line
	pop	cx			; Line count
	loop	NextLineDEF10

	jmp	DisplayMap      ; All Done

GetVidSelFailed:
        call    ClearVidSel
SetProbBit:
	bts	[si.EPStatusFlags],fGrbProbBit ; Tell caller we can't deal with this
InvalidRect:
        clc
        ret     

CleanUp:
	bt	GrbFlags,GrbFlgs_GrfxGrabBMBit	; Keep This?
        jnc     short DontClear1
        call    ClearVidSel
DontClear1:
        call    DeAllocRes
        mov     ax,ScreenBitmap
	clc
	ret
MGError:
	bt	GrbFlags,GrbFlgs_GrfxGrabBMBit	; Keep This?
        jnc     short DontClear2        
        call    ClearVidSel
DontClear2:
        call    DeAllocRes
        mov     ax,0
	stc
        ret

ModeGrfx endp

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

        call    InitTextFrameVars
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
        jc      short MTDone    ; Rect is Off screen or invalid rect 
    ;
    ; BytesPerLine,BytesToRight set 
    ; ES:EDI -> first line of paint rect
    ;
        call    SelectTextFont          ; based on mode
        jc      short MTError

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
	dec	cx			; Faster than loop to label which is too far
	jnz	NextChar
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
        mov     Gmode,ah

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
	call	GetVidSel		; Set up
	jc	short GVSFailedText		; Can't do it
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
;	and	al,0111B		; Mask to background color
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
CreateOVBrTxt proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
    ;
    ; Try to create a brush in the overscan color
    ;
	mov	bl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Colr]	; Get overscan color
        xor     bh, bh
        shl     bx, 2
        mov     ax, bx

	push	[si.Pstruct.psHdc]	  ; hDC
	push	[bx.RGBTable10.2]
	push	[bx.RGBTable10]
	cCall	GetNearestColor
	push	dx			; RGBcol
	push	ax
	cCall	CreateSolidBrush
	or	ax,ax
	jz	short DefOvscBr
GotOvscBr:
	mov	BkTxtBrsh,ax
    ;
    ; Select the brush into the display DC
    ;
	push	[si.Pstruct.psHdc]	  ; hDC
	push	ax			; hBrush
	cCall	SelectObject
	mov	OldBrshHand,ax
	ret

DefOvscBr:
	mov	ax,[si.BlkBrshH]	; Black background brush selected
	jmp	short GotOvscBr


CreateOVBrTxt endp

RGBTableText    Label   word
	dd	00000000H			; 0000	Black
	dd	00C40000H			; 0001	Blue
	dd	0000C400H			; 0010	Green
	dd	00C4C400H			; 0011	Cyan
	dd	000000C4H			; 0100	Red
	dd	00C400C4H			; 0101	Magenta
	dd	0000C4C4H			; 0110	Brown
	dd	00C0C0C0H			; 1000	Gray
	dd	00808080H			; 0111	White (low intensity)
	dd	00FF0000H			; 1001	Light Blue
	dd	0000FF00H			; 1010	Light Green
	dd	00FFFF00H			; 1011	Light Cyan
	dd	000000FFH			; 1100	Light Red
	dd	00FF00FFH			; 1101	Light Magenta
	dd	0000FFFFH			; 1110	Yellow
	dd	00FFFFFFH			; 1111	White (high intensity)

;************************************************************************************
;
; GetWinRGBText - Get XBGR dword value for the given Palette index.
;
; ENTRY: DS:SI -> EXTPAINTSTRUC, 
;        AL = 4 bit Palette index
; EXIT:  xBGR value returned in dx:ax
;
; Alg:
; Mode 1,2,3 - b3-b0 is a 4 bit index into RGBTableText
; The dword value in the RGBTable gives the required RGBvalue.
;
;*************************************************************************************

GetWinRGBText proc near
    assume ds:nothing,es:nothing,ss:nothing

	push	bx
	and	ax,01111B		; Mask to bits of interest
	mov	bx,ax
	shl	bx,2				; BX now has the index into table
	mov	ax,[bx.RGBTableText]
	mov	dx,[bx.RGBTableText.2]
	pop	bx
	ret

GetWinRGBText endp

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
CreateOVBrGrx proc near
    assumes ds,nothing
    assumes es,nothing
    assumes ss,nothing
    ;
    ; Create a brush in the Overscan color.
    ;
        xor     bh,bh
	mov	bl,[si.EPGrabDTA.CntrlSt.VDA_EGA_Colr]	; Get overscan color
	push	[si.Pstruct.psHdc]	  ; hDestDC

    ;
    ; BX has the 6 bit value for modes D,E,F,10,12
    ;
        cmp     Gmode, 0Dh
        je      short COBGModeDE
        cmp     Gmode, 0Eh
        je      short COBGModeDE
	cmp	Gmode, 0Fh
        je      short COBGMode10
        cmp     Gmode, 10h
        je      short COBGMode10
        cmp     Gmode, 12h
        je      short COBGMode10

    ;
    ; modes 4,5,6,11 Palette contents in bl(=xxxIxRGB)
    ;
    ; % Mode 11 how to interpret Palette contents? Looks like it is 6 bit value.

	mov	bh,bl
	and	bl,00000111B		; Mask to RGB values
	shl	bl,1
	and	bh,00010000B		; Mask to intensity bit
	or	bl,bh
	xor	bh,bh			; bx is IRGB color * 2
	shl	bx,1			; * 4 for table

   ; 
   ; creating the overscan brush,
   ; For picking up the Windows RGBColor, use RGBTable10 for Mode 10,12;
   ; RGBTableDE for D and E, RGBTable for 4,5,6,11
   ; 
	push	[bx.RGBTable.2]
	push	[bx.RGBTable]
        jmp     short COBGModeDone        
COBGModeDE:
	push	[bx.RGBTableDE.2]
	push	[bx.RGBTableDE]
        jmp     short COBGModeDone        

COBGMode10:
	push	[bx.RGBTable10.2]               ; Modes 10, 12
	push	[bx.RGBTable10]

COBGModeDone:
	cCall	GetNearestColor

	push	dx
	push	ax
	cCall	CreateSolidBrush
	or	ax,ax			; Worked?
	jnz	short SelB		; Yes
	mov	ax,BkBrush		; Try to use the background brush
	or	ax,ax			; got one?
	jnz	short SelB2		; Yes, use it
	mov	ax,[si.WhtBrshH]	; No, use white brush
	jmp	short SelB2

SelB:
	mov	OvscnBrsh,ax		; Brush handle
SelB2:
	push	[si.Pstruct.psHdc]	  ; hDestDC
	push	ax
	cCall	SelectObject
	or	ax,ax
	ret

CreateOVBrGrx endp

;***********************************************************************************
;
; BuildDIBHdr - Build Device Independent Bitmap Header
; 
;       In the PelColorTable, need only 
;               4 entries for Mode 4,5 since 2 bpp
;               2 entries for Mode 6,11 since 1 bpp
;		16 entries for Mode D,E,F,10,12 since 4 bpp
;
;***********************************************************************************

;
; Maps Mode to # of entries in PelColorTable
; ModeToEntries[Mode] = # of entries in PelColorTable
; 
ModeToEntries label byte
        db      0,0,0,0,4,4,2,0,0
        db      0,0,0,0,16,16,0,16
        db      2,16,0

; Pel Color Table for mode F DIBs
ModeF_PCT label dword
        dd      0,0,0,0,0,0,0,00C0C0C0H,00808080H,0,0,0,0,0,0,00FFFFFFH

BuildDIBHdr proc near

        push    eax
        push    bx
        push    cx
        push    dx
        push    di
        push    si
        
        cmp     Gmode,0Fh               ; Q: Mode 0f ?
        jne     SHORT BDH_BuildTable    ;   N:Build Table from Palette
        mov     cx,16                   ;   Y: Special PelColorTable
        push    cs
        pop     ds
        mov     si, codeOffset ModeF_PCT
        push    ss
        pop     es
        lea     di,PelColorTable
        rep     movsd
        push    ss
        pop     ds
        jmp     SHORT BDH_00
BDH_BuildTable:
   ;
   ; Fill PelColorTable entries
   ;
        xor     ax,ax                  ; color index, start with 0
        lea     di,PelColorTable
        xor     bh,bh
        mov     bl,Gmode
        xor     ch,ch
        mov     cl,ModeToEntries[bx]   ; 2/4/16 entries
BDH_Loop:
        push    ax
        call    GetWinRGB
        mov     [di.rgbRed],al
        mov     [di.rgbGreen],ah
        mov     [di.rgbBlue],dl
        mov     [di.rgbResvd],0
        add     di,4
        pop     ax
        inc     ax
        loop    BDH_Loop

BDH_00:
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
        cmp     Gmode,6
        je      short BDH_Done          ; assumption correct
        cmp     Gmode,011h
        je      short BDH_Done          ; assumption correct
        mov     [di.biBitCount],4            ; Mode 4,5,d,e,f,10

BDH_Done:        
        pop     si
        pop     di
        pop     dx
        pop     cx
        pop     bx
        pop     eax

        ret
BuildDIBHdr endp

;****************************************************************************
;
; ProcessDwordEF10 - process a dword of data in Mode E,F and 10
;
; ENTRY: Dword of data from Planes 0,1,2,3 in edx, ecx, ebx and eax respectively
;        ES:EDI -> LineBits
;
; EXIT: Set 32*4bpp bits in LineBits, update edi
;       eax, ebx, ecx, edx destroyed
;
;****************************************************************************

ProcessDwordEF10 proc near

    ; 
    ; check if all are background bits
    ;
        push    eax
        or      eax,ebx
        or      eax,ecx
        or      eax,edx
        pop     eax
        jz      short PDBkBits
	call	ProcessWordEF10
    ;
    ; upper words preserved across the above call
    ;
        shr     eax,16
        shr     ebx,16
        shr     ecx,16
        shr     edx,16
	call	ProcessWordEF10

        ret

PDBkBits:
    ;
    ; All background bits, skip 32*4bpp bits or 16 bytes in LineBits
    ;
        add     edi,16
        ret

ProcessDwordEF10    endp

;***********************************************************************************
;
; ProcessWordEF10 - process a word in mode E,F and 10
;
; ENTRY: word in dx, cx, bx, ax, ptr to LineBits in edi
;
; EXIT: Set 16*4bpp bits in LineBits,edi updated. 
;        ax, bx, cx and dx destroyed. 
;        upper words in eax, ebx, ecxand edx preserved.
;
;**************************************************************************************

ProcessWordEF10     proc near

        push    ax
        or      ax,bx
        or      ax,cx
        or      ax,dx                   
        pop     ax
        jz      short PWBkBits          ; Background bits?

	call	ProcessByteEF10
	call	ProcessByteEF10
        ret

PWBkBits:
        add     edi,8                   ; skip 8 bytes in LineBits
        ret

ProcessWordEF10  endp

;***************************************************************************
;
; Form4BitCode - Macro to create a 4bit code in DI - Mode D only
;                using a bit from planes 3,2,1,0
;                Plane 3,2,1,0 = I,R,G,B. AX has the MSB
;
;
;***************************************************************************

Form4BitCode    MACRO

        xor     edi,edi
        shr     ax,1                   ;ax has the MSB
        rcl     di,1
        shr     bx,1
        rcl     di,1
        shr     cx,1
        rcl     di,1
        shr     dx,1
        rcl     di,1                   ; 4 bit code in DI

        ENDM        

;***************************************************************************
;
; Form8BitCode - Macro to create a 8bit code in DI - Mode E,F,10 only
;
; Concatenate bits from LSB1 to form a 4 bit code. Store it as high nibble of DI
; Concatenate bits from LSB0 to form another 4 bits code. Store this
; in DI as the low nibble
;
; (Plane 3,2,1,0 = I,R,G,B. AX has the MSB)
;
;
;***************************************************************************

Form8BitCode    MACRO

        xor     edi,edi
    ;
    ; Prepare to form 4 bit code from LSB1        
    ;
        ror     ax,1                   
        ror     bx,1                   
        ror     cx,1                   
        ror     dx,1                   
    ;
    ; form 4 bit code from LSB1
    ;
        shr     ax,1                    
        rcl     di,1
        shr     bx,1                    
        rcl     di,1
        shr     cx,1                    
        rcl     di,1
        shr     dx,1                    
        rcl     di,1
    ;
    ; Prepare to form 4 bit code from LSB0
    ;
        rol     ax,2 
        rol     bx,2 
        rol     cx,2 
        rol     dx,2 

    ;
    ; LSB 0 is back in place
    ; form 4 bit code from LSB0
    ;
        shr     ax,1
        rcl     di,1
        shr     bx,1
        rcl     di,1
        shr     cx,1
        rcl     di,1
        shr     dx,1
        rcl     di,1                   ; 4 bit code in DI
    ;
    ; 8 bit code in DI
    ;
    ; now throw away LSB1
    ;
        shr     ax,1
        shr     bx,1
        shr     cx,1
        shr     dx,1

        ENDM        


                
;******************************************************************************
;
; ProcessByteEF10 - Process a byte in mode E,F and 10
;
; Entry - Byte in dl, cl, bl, al, ptr to LineBits in es:edi
;
; Exit  - edi updated. al, bl, cl and dl destroyed and replaced with original
;         ah, bh, ch, dh. Upper words in eax, ebx, ecx and edx preserved.
;
; Algorithm:
;       Processing a byte
;       Process 2 bits at a time from right to left.
;       Within each two bits, process higher bit first.
;       Form8BitCode macro returns two 4 bit codes in DI - the code from LSB0
;       is in the high nibble of DI, code formed from LSB1 is in the low
;       nibble of DI. This weird order is because of Intel's byte swapped
;       memory.
;       
;**************************************************************************************

ProcessByteEF10     proc near
        
        push    ax                      
        or      al,bl
        or      al,cl
        or      al,dl                   
        pop     ax                      ; Q: Background bits?
        jnz     short NonBkByteE        ;  N: 
        add     edi,4                   ;  Y: skip 4 bytes in LineBits
    ;
    ; may need to prepare al, bl, cl, dl for the next call.
    ; need to shift the other high byte into low byte
    ;
        mov     al,ah
        mov     bl,bh
        mov     cl,ch
        mov     dl,dh
        ret

NonBkByteE:
        push    edi                     ; push ptr to LineBits
        xor     esi,esi
        
        Form8BitCode                    ; 8 bit code in DI
        or      esi,edi                 ; mov the code into ESI
        shl     esi,8                   ; make room for the next byte

        Form8BitCode                    ; 8 bit code in DI
        or      esi,edi                 ; mov the code into ESI
        shl     esi,8                   ; make room for the next byte

        Form8BitCode                    ; 8 bit code in DI
        or      esi,edi                 ; mov the code into ESI
        shl     esi,8                   ; make room for the next byte

        Form8BitCode                    ; 8 bit code in DI
        or      esi,edi                 ; mov the code into ESI

        pop     edi                     ; get back Ptr to LineBits
        mov     dword ptr es:[di],esi   ; store the 8 four bit codes in LineBits
        add     di,4
        ret

ProcessByteEF10 endp


;******************************************************************************
;
; ProcessWordD - Process a word in Mode D
;
; Entry - ax, bx, cx, dx contain a word from Planes 3,2,1,0 respectively.
;         es:edi - ptr into LineBits
; Exit  - ax, bx, cx, dx destroyed. edi updated.
;
;*****************************************************************************

ProcessWordD     proc near

        push    ax
        or      ax,bx
        or      ax,cx
        or      ax,dx                   ; Q: BackGround bits ?
        pop     ax
        jnz     short NonBkWordD        ;   N: 
        add     edi,16                  ;   Y: skip 16 bytes in LineBits.
        ret

NonBkWordD:
    ;
    ; while processing bytes in al,bl,cl,dl the upper bytes get destroyed.
    ; Therefore save it for later use.
    ;
        push    ax
        push    bx
        push    cx
        push    dx

        call    ProcessByteD
    ;
    ; restore upper bytes
    ; 
        pop     dx
        pop     cx
        pop     bx
        pop     ax

        mov     al,ah
        mov     bl,bh
        mov     cl,ch
        mov     dl,dh

        call    ProcessByteD
        ret

ProcessWordD  endp

;***************************************************************************
;
; ProcessByteD - Process a byte in al, bl, cl, dl in Mode D
;
; Entry - byte of data from Planes 0,1,2,3 in dl, cl, bl, al
;         edi = ptr to LineBits
;
; Exit  - destroys ax, bx, cx, dx. Updates edi
;
; Algorithm:
;
;     if PQ exists in the 16 bit registers process byte Q and then P
;     since in memory they exist as QP. 
;     Within a byte process higher nibble first
;     Within a nibble process bits from right to left.
;     For pixel doubling, store each four bit code twice.
;
;****************************************************************************

ProcessByteD     proc near
        
        push    ax                      ; preserve data in AL
        or      al, bl
        or      al,cl
        or      al,dl                   
        pop     ax                         
        jnz     short NonBkByteD        ; Non Background bits ?
        add     edi,8                   ; N: skip 8 bytes in LineBits
        ret

NonBkByteD:
        
        push    edi                     ; save ptr into LineBits
        xor     esi,esi

    ; 
    ; 8086 is byte swapped, exchange nibbles for processing the pixels in
    ; the right order.
    ; 
        rol     al,4
        rol     bl,4
        rol     cl,4
        rol     dl,4


        Form4BitCode                    ; get 4 bit code in DI
        or      si,di                   ; mov it into si
        shl     esi,4                   ; make room for the next nibble
        or      si,di                   ; store the same code again - pixel doubling
        shl     esi,4 


        Form4BitCode                    
        or      si,di                  
        shl     esi,4                   
        or      si,di                  
        shl     esi,4 

        Form4BitCode                    
        or      si,di                  
        shl     esi,4                   
        or      si,di                  
        shl     esi,4 

        Form4BitCode                    
        or      si,di                  
        shl     esi,4                   
        or      si,di                  

        pop     edi                     ; get ptr to LineBits
        mov     dword ptr es:[di],esi   ; store the 8 four bit codes in LineBits
        add     di,4

        push    edi                     ; save ptr into LineBits
        xor     esi,esi

        Form4BitCode                    
        or      si,di                  
        shl     esi,4                   
        or      si,di                  
        shl     esi,4 

        Form4BitCode                    
        or      si,di                  
        shl     esi,4                   
        or      si,di                  
        shl     esi,4 

        Form4BitCode                    
        or      si,di                  
        shl     esi,4                   
        or      si,di                  
        shl     esi,4 

        Form4BitCode                    
        or      si,di                  
        shl     esi,4                   
        or      si,di                  
        
        pop     edi             ; get ptr to LineBits
        mov     dword ptr es:[di],esi     ; store the 8 four bit codes in LineBits
        add     di,4
        ret

ProcessByteD endp


sEnd	code
	end

