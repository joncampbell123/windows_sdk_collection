page            ,132
title           Enable, Disable, and Inquire Routines
.286c


.xlist
include         CMACROS.INC
incDevice = 1
include         GDIDEFS.INC
include         8514.INC
include		DEBUG.INC
.list

NUMCOLORS	equ	16

externFP    AllocSelector		; imported from KERNEL for protect mode
externA     __WinFlags			;LSB set --> we're in protect mode
externA     ExtraPalLength
externA     NbrofXColors
externA     PaletteLength
externA     NbrofColours
DefaultPalLen	equ	PaletteLength-ExtraPalLength
DefaultPalCol	equ	NbrofColours-NbrofXColors

RC_DI_BITMAP	equ	0000000010000000b   ; to be put into gdidefs.inc
RC_DIBTODEV	equ	0000001000000000b   ; Device can handle DevInd. Bitmaps
					    ; Add to other RC_XXXXXX stuff
RC_PALETTE	equ	0000000100000000b   ; can do palette management
RC_BIGFONT	equ	0000010000000000b   ;can do greater that 64k fonts

subttl          General Data Used by the Driver
page +

sBegin	Data
externB ShadowMemoryTrashed		;in SAVESCRN.ASM
externB SystemFontLoaded		;in STRBLT.ASM
externW OldFontHdr			;in TEXTOUT.ASM
externW BigOldFontHdr			;in TEXTOUT.ASM
externB SystemPaletteColors		;in DATA.ASM
externB PhysicalIndexTable

externW FullScreenClip			;all items in data segment are defined
externW InvisibleScreenClip		; in DATA.ASM
externW VisibleScreenClip
externB WriteEnable
externB BitsPixel
externB SystemPaletteColors
externB bLines				;in TEXTOUT.ASM to force word alignment

ife	GreyScale
externB Palette
endif
externB f96DPI                          ;in SSWITCH.ASM (charlkin 4/10/91)
sEnd	Data


subttl          Code Segment Definitions
page +
sBegin          Code
assumes         cs,Code
assumes         ds,Data


externNP        SetScreenClip           ;in ROUTINES.ASM

cProc		HardwareEnable,<FAR,PUBLIC,NODATA>
cBegin

;This function is called when it is time to really get going.  The first time
;Enable() is called is to return the GDIINFO block, to determine the
;environment, and to and to fix-up some far pointers as needed.  This function
;is called the second time Enable() was called.

	assumes ds, Data

	push	bp			;save stack frame pointer
	mov	ax, 1130h		;get the line mode
	sub	bx, bx
	int	10h			;now DL: # of lines-1
	inc	dl
	mov	bLines, dl		;save the current # of lines
	pop	bp

;Since ENInitBoard is performed upon returning from an OldApp (badly behaved),
;we need to assure ourselves that we will reload the system font and not think
;that we have stuff saved in off-screen memory.  Thus, we trash a few flags to
;assure ourselves of the state upon reentry.  This also benefits us when we
;are returning to Windows running in the OS/2 compatibility box.

	sub	ax, ax
	mov	ShadowMemoryTrashed, al

	mov	SystemFontLoaded, al
	mov	cx, size OldFontHdr
	push	ds
	pop	es
	assumes	ds,Data
	lea	di, OldFontHdr
rep	stosb

;Now, set the adapter into Hi-Res mode, setting some states as we go along:

	mov	dx,2eah 		;get "open adapter" port
	xor	ax,ax			;send out a zero
	out	dx,al
	mov	dx,42e8h		;set state in control port
	mov	ax,9000h
	out	dx,ax
	mov	ax,5000h
	out	dx,ax
	mov	ax,53h
	mov	dx,22e8h		;set CRTC state to "normal"
	out	dx,ax
	mov	ax,07h			;set misc I/O register
	mov	dx,4ae8h		;to advanced function mode
	out	dx,ax
	mov	ax,660h 		;set frame total length
	mov	dx,12e8h
	out	dx,ax
	mov	ax,600h 		;set frame sync position
	mov	dx,1ae8h
	out	dx,ax
	mov	ax,5fbh 		;set frame blank start
	mov	dx,16e8h
	out	dx,ax
	mov	ax,9dh			;set line total length
	mov	dx,2e8h
	out	dx,ax
	mov	ax,81h			;set line sync position
	mov	dx,0ae8h
	out	dx,ax
	mov	ax,7fh			;set line blank start
	mov	dx,6e8h
	out	dx,ax
	mov	ax,16h			;set line sync width
	mov	dx,0ee8h
	out	dx,ax
	mov	ax,08h			;set frame sync width
	mov	dx,1ee8h
	out	dx,ax
	mov	ax,33h			;reset CRTC mode
	mov	dx,22e8h
	out	dx,ax

ENStatusCheck:     
	MakeEmptyFIFO
	mov	ax,5006h		;set config register
	mov	dx,0bee8h
	out	dx,ax
	xor	ax,ax			;set colour compare reg to 0
	mov	dx,0b2e8h
	out	dx,ax
	mov	al,WriteEnable		;set write enable to all on
	mov	dx,WRITE_ENABLE_PORT
	out	dx,ax

	sub	ax, ax
	mov	dx, Srcx_PORT
	out	dx, ax
	mov	dx, Srcy_PORT
	out	dx, ax
	mov	dx, COLOUR_0_PORT
	out	dx, ax
	mov	ax, X_SIZE-1
	mov	dx, RECT_WIDTH_PORT
	out	dx, ax
	mov	ax, Y_SIZE-1
	mov	dx, RECT_HEIGHT_PORT
	out	dx, ax
	MakeEmptyFIFO
	mov	al, 7
	mov	dx, FUNCTION_0_PORT
	out	dx, ax
	mov	dx, FUNCTION_1_PORT
	out	dx, ax
	mov	dx, MODE_PORT
	mov	ax, 0a000h
	out	dx, ax
	mov	dx, COMMAND_FLAG_PORT
	mov	ax, 040b3h
	out	dx, ax

	mov	al,WriteEnable		;set pixel mask in palette DAC
	mov	dx,2eah
	out	dx,al

;Let's define our new 16 colour palette!
ife	GreyScale

	mov	cx, 256 		;make the palette initially all black
MakeAllBlackLoop:
	mov	dx, 2ech
	mov	al, cl
	out	dx, al
	sub	al, al
	mov	dx, 2edh
	out	dx, al
	out	dx, al
	out	dx, al
	loop	MakeAllBlackLoop

	push	ds
	pop	es
	assumes es, Data
	mov	di, DataOFFSET PhysicalIndexTable
	xor	ax,ax			;let AX be our colour index
	mov	cx, ax			;CX=0
	mov	bx, 0bf80h
	mov	cl, [SystemPaletteColors]   ;define all system colours
	shr	cx, 1			;load only half the palette at a time
	mov	si,DataOFFSET Palette	;now DS:SI points at our palette
	push	cx			;load the low intensity colors first
	call	LoadPaletteEntries

	sub	ax, ax
	pop	cx
	mov	al, [WriteEnable]
	inc	ax
	sub	ax, cx
	call	LoadPaletteEntries	;now load high intensity colors

else					;GreyScale
	xor	bx,bx			;let BX be our colour index
	mov	cx,NUMCOLORS		;we want to define 256 colours

public  ENPaletteLoadLoop
ENPaletteLoadLoop:
	cli
	mov	ax,bx			;get next colour to do
	mov	dx,2ech 		;this is index port
	out	dx,al
	mov	dx,2edh 		;now set the colours
	shr	al,2			;divide it by 4
	out	dx,al			;set it into all RGB colours
	out	dx,al
	out	dx,al
	sti				;restore interrupts
	add	bx, GREYINCR
	dec	bx
	loop	ENPaletteLoadLoop
endif					;GreyScale

;Now set the scissor clip to full screen (1024 by 1024):

	mov	si,DataOFFSET FullScreenClip	;set clip to full screen
	cCall	SetScreenClip			;go set the clip
cEnd

ife	GreyScale
LoadPaletteEntries  proc near
	cli				;totally disable interrupts while
ENPaletteLoadLoop:			;loading color palette
	push	ax			;save our colour index
	mov	dx,2ech 		;this is index port
	out	dx,al
	mov	dx,2edh 		;now set the colours
	lodsw				;get red & green
	cmp	al, bl
	jne	RedOkay
	cmp	ah, bl
	je	SpecialGrey
NotSpecialGrey:
	mov	al, bh
RedOkay:
	cmp	ah, bl
	jne	GreenOkay
	mov	ah, bh
GreenOkay:
	shr	al,2			;colour must be in low 6 bits
	out	dx,al
	mov	al,ah			;get green
	shr	al,2			;colour must be in low 6 bits
	out	dx,al
	lodsb				;get blue
	cmp	al, bl
	jne	BlueOkay
	mov	al, bh
BlueOkay:
	shr	al,2			;colour must be in low 6 bits
	out	dx,al
	pop	ax			;restore saved counter
	stosb				;store the associated physical index
	inc	ax			;bump to next colour
	loop	ENPaletteLoadLoop
	sti				;restore interrupts
	ret
SpecialGrey:
	cmp	[si], bl
	jne	NotSpecialGrey
	mov	bh, bl
	dec	bx
	jmp	short RedOkay
LoadPaletteEntries  endp
endif

sEnd	Code


dpXRate                 equ     1       ;horizontal MICKY to PIXEL ratio
dpYRate                 equ     2       ;vertical MICKY to PIXEL ratio


createSeg       _INIT,InitSeg,word public,CODE
sBegin          InitSeg
assumes         cs,InitSeg
assumes         ds,Data


externNP        hook_int_2fh            ;in SSWITCH.ASM
externNP	restore_int_2fh 	;in SSWITCH.ASM
externNP	GetHardwareConfig	;in data.asm


subttl          Enable Process
page +
cProc   Enable,<FAR,PUBLIC>,<si,di>

        parmD   lpDevice
        parmW   Style
        parmD   lpDeviceType
        parmD   lpOutputFile
        parmD   lpData

cBegin
	assumes ds, Data
	assumes cs, InitSeg
	cld
	mov	cx,Style		;get the requested function
	jcxz	ENInitBoard		;it's an init hardware

public  ENReturnGDIInfo
ENReturnGDIInfo:                                       
	call	GetHardwareConfig	;Set global configuration varibles
	les	di,lpDevice		;get the return area into ES:DI
	mov	si,InitSegOFFSET GDIInfoDataStructure
	mov	cx,SIZE GDIINFO 	;get nbr of bytes to move in CX
	push	cx			;get it into AX for return
	push	di
rep	movs	byte ptr es:[di], byte ptr cs:[si]

; Naughty, naughty....  Think of those poor souls with 286's
; Besides, this type of REP MOV doesn't have the 386 bug
;       db      67h                     ;fix bug in early 80386's

	pop	di			;now its time to perform fixups
;-----------------------------------------------------------------------
; Are we 96dpi? (charlkin 4/10/91 mutlires)
; if (f96DPI) 
;    set logical pixels to 96
;-----------------------------------------------------------------------
        cmp     BYTE PTR f96DPI, MULTIRES_96DPI
        jne     short Not96DPI
        mov     es:[di].dpLogPixelsX, 96 ;Set up GDIINFO for 96 dpi
        mov     es:[di].dpLogPixelsY, 96
Not96DPI:
	sub	ah, ah			;depending on the number of bits per
	mov	al, BitsPixel		;pixel in GDI Info
	mov	es:[di].dpBitsPixel, ax
	mov	cl, 01h
	xchg	al, cl
	shl	ax, cl
	mov	es:[di].dpPalColors, ax
	cmp	ax, 256
	jne	NonPaletteDevice
	add	es:[di].dpRaster, RC_PALETTE
	sub	ah, ah
	mov	al, [SystemPaletteColors]
	mov	es:[di].dpNumColors, ax ;set the number of system colors
	mov	es:[di].dpPalReserved, ax
	mov	es:[di].dpNumPens, ax	;account for those 4 extra system colors
NonPaletteDevice:
	mov	ax, __WinFlags		;enable bigfonts only in protect mode
if DEBUG_286
	mov	ax,3
endif
	test	al, 01h 		;are we in protect mode?
	jz	RealAddressMode
	test	al, 02h 		;are we on a 286 processor?
	jnz	RealAddressMode		;Yes.  Don't enable bigfonts.
	add	es:[di].dpRaster, RC_BIGFONT
RealAddressMode:
	pop	ax
	jmp	short ENExit		;do the move and we're done

public  ENInitBoard
ENInitBoard:                                        
	call	hook_int_2fh		;go hook this in case we're
					;running OS/2
	les	di,lpDevice		;we need to put our PDEVICE
	mov	ax, 2000h		;bmType=2000h==>this is the device ID
	stosw				;store that
	mov	ax, X_SIZE		;width of device surface
	stosw
	mov	ax, Y_SIZE		;height of device surface
	stosw
	sub	ax, ax			;store 0 for bmWidthBytes
	stosw
	mov	ah, [BitsPixel] 	;AH: bmBitsPixel
	mov	al, 1			;AL: bmPlanes
	stosw
	cCall	HardwareEnable		;go enable the hardware
	mov	ax,1			;set success return code

public  ENExit
ENExit:                                                                    
cEnd


subttl          Disable Process
page +
cProc   Disable,<FAR,PUBLIC>,<si>

        parmD   lpDevice

cBegin

;We return to VGA (text) mode and leave gracefully:

	cli
	mov	ax,53h			;set CRTC mode
	mov	dx,22e8h
	out	dx,ax
	mov	al,2			;set misc I/O back to VGA
	mov	dx,4ae8h
	out	dx,ax
	mov	ax,0ah			;set CRTC status
	mov	dx,2e8h
	out	dx,ax
	mov	ax,33h			;set CRTC mode
	mov	dx,22e8h
	out	dx,ax

	cmp	bLines, 25		;25 line mode ?
	jz	LinesOkay		;yes.
	mov	ax,1201h		;350 scan line mode
	cmp	bLines, 50		;50 line mode?
	jne	@f			;no.
	inc	ax			;set 400 scans
@@:
	mov	bl,30h			;set scan lines
	int	10h
LinesOkay:

	mov	ax, 1200h		;enable default palette loading
	mov	bl, 31h
	int	10h
	mov	ax, 03h 		;set standard text mode
	int	10h

; restore 43 line mode if it existed at start

	cmp	bLines, 25
	je	LineOk
	mov	ax,1112h
	mov	bl,0
	int	10h
	mov	ax,0100h
	mov	cx,0407h
	int	10h
LineOk:

	call	restore_int_2fh 	;unhook ourselves from INT 2FH
					;(ES & DS are destroyed by this)
cEnd


subttl          The Inquire Function
page +
cProc           Inquire,<FAR,PUBLIC>,<di>

        parmD   lpCursorInfo

cBegin
	les	di,lpCursorInfo 	;get address of CURSORINFO into ES:DI
	mov	ax,dpXRate		;get horizontal MICKY to PIXEL ratio
	stosw				;into return string
	mov	ax,dpYRate		;get vertical MICKY to PIXEL ratio
	stosw				;into return string
	mov	ax,4			;return nbr of bytes passed in AX
cEnd	Inquire


subttl          The GDI Information Data Structure
page +
public  GDIInfoDataStructure
GDIInfoDataStructure             label   word

	dw	30AH			;Version = 030Ah now

        dw      DT_RASDISPLAY           ;Device classification

        dw      280                     ;width of display in mm (8514 COLOUR)
        dw      210                     ;height of display in mm

        dw      X_SIZE                  ;width of display in pixels
        dw      Y_SIZE                  ;height of display in raster lines

	dw	0			;Number of consecutive bits (within
                                        ;the same plane) required to make up
                                        ;a pixel

        dw      1                       ;Number of bit planes

	dw	0ffffh			;Number of brushes the device has

	dw	16			;Number of pens the device has

        dw      0                       ;Reserved

        dw      0                       ;Number of fonts the device has

	dw	NUMCOLORS		;Number of colors in color table

	dw	32			;Size required for device descriptor
 
        dw      CC_NONE                 ;Curves capabilities (none)

        dw      LC_POLYLINE             ;Lines capabilities (solid polylines)

	dw	PC_SCANLINE+PC_INTERIORS+PC_RECTANGLE
                                        ;polygonal capabilities
					;(rectangles, scanlines)

        dw      TC_RA_ABLE+TC_CP_STROKE ;Text capabilities
 
        dw      CP_RECTANGLE            ;Clipping capabilities for OUTPUT 
                                        ;(can do rectangular clipping)

	dw	RC_BITBLT+RC_SAVEBITMAP+RC_GDI20_OUTPUT+RC_BITMAP64+RC_DI_BITMAP+RC_DIBTODEV
                                        ;Raster Capabilities (bitblt able and
                                        ;supports ExtTextOut etc, large bitmaps,
					;SaveScreenBitmap, and device ind. bmp.)

        dw      10                      ;Distance moving X only

        dw      10                      ;Distance moving Y only

        dw      14                      ;Distance moving X and Y

        dw      28                      ;Length of segment for line styles
                                        ;(2 * Distance moving X and Y)
;Metric Lo-Res
        dw      2800                    ;  HorzSize * 10
        dw      2100                    ;  VertSize * 10
        dw      1024                    ;  HorizRes
        dw      -768                    ;  -VertRes
;Metric Hi-Res
        dw      28000                   ;  HorzSize * 100
        dw      21000                   ;  VertSize * 100
        dw      1024                    ;  HorizRes
        dw      -768                    ;  -VertRes
;English Lo-Res
        dw      4375                    ;  HorzSize * 1000 scaled  (/64)
        dw      4375                    ;  VertSize * 1000 scaled  (/48)
        dw      4064                    ;  HorzRes * 254 scaled    (/64)
        dw      -4064                   ;  -VertRes * 254 scaled  (/48)
;English Hi-Res
        dw      21875                   ;  HorzSize * 10000 scaled (/128)
        dw      21875                   ;  VertSize * 10000 scaled (/96)
        dw      2032                    ;  HorizRes * 254 scaled   (/128)
        dw      -2032                   ;  -VertRes * 254 scaled   (/96)
;TWIP
        dw      7875                    ;  HorzSize * 14400 scaled (/512)
        dw      7875                    ;  VertSize * 14400 scaled (/384)
        dw      508                     ;  HorizRes * 254 scaled   (/512)
        dw      -508                    ;  -VertRes * 254 scaled   (/384)


	dw	120			;Logical Pixels/inch in X

	dw	120			;Logical Pixels/inch in Y

        dw      DC_IgnoreDFNP           ;dpDCManage

        dw      0                       ;Reserved fields
        dw      0
        dw      0
        dw      0
	dw	0

	dw	0 ;NUMCOLORS		   ; Number of entries in device's palette
	dw	16			; Number of colors reserved for system
	dw	18;BITSPIXEL		   ; Bits of color resolution

sEnd            InitSeg
end
