;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;----------------------------------------------------------------------------
; ENABLE.ASM
;----------------------------------------------------------------------------
        .xlist
DOS5 = 1
        include cmacros.inc
        incDevice = 1
        include gdidefs.inc
        include macros.inc
        include dibeng.inc
        .list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
DRV_VERSION	= 0400h
STOP_IO_TRAP    = 4000h		        ; stop io trapping
START_IO_TRAP   = 4007h			; re-start io trapping

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externFP DIB_Enable
        externFP DIB_BeginAccess
        externFP DIB_EndAccess
        externFP GetPrivateProfileInt   ; Kernel!GetProfileInt
        externNP HookInt2Fh             ;Hook into multiplexed interrupt
        externFP SetPaletteTranslate    ;in palette.asm
        externFP SetRAMDAC_far          ;in palette.asm
        externNP PhysicalEnable         ;Enable routine
        externNP RestoreInt2Fh          ;Restore multiplexed interrupt
	externNP GetScreenSettings
        externW  wDpi
        externW  wScreenHeight
        externW  wScreenWidth
        externW  wScreenWidthBytes
        externW  ScreenSelector         ;selector for the screen

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
DeclareGDIINFO  macro name,x,y,EngLoResX,EngLoResY,EngHiResX,EngHiResY,TwipX,TwipY
name label      byte
        dw      DRV_VERSION             ;version number = 4.00
        dw      DT_RASDISPLAY           ;Device classification
        dw      208                     ;Horizontal size in millimeters
        dw      156                     ;Vertical size in millimeters
        dw      x                       ;Horizontal width in pixels
        dw      y                       ;Vertical width in pixels
        dw      ?                       ;Number of bits per pixel (PATCHED)
        dw      1                       ;Number of planes
        dw      ?                       ;Number of brushes (PATCHED)
        dw      ?                       ;Number of pens (PATCHED)
        dw      0                       ;Reserved
        dw      0                       ;Number of fonts the device has
        dw      ?                       ;Number of colors (PATCHED)
        dw      ?                       ;size of PDevice (PATCHED)
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?
        dw      ?

;Metric  Lo res WinX,WinY,VptX,VptY
        dw      2080                    ;  HorzSize * 10
        dw      1560                    ;  VertSize * 10
        dw      x                       ;  HorizRes
        dw      -y                      ;  -VertRes

;Metric  Hi res WinX,WinY,VptX,VptY
        dw      20800                   ;  HorzSize * 100
        dw      15600                   ;  VertSize * 100
        dw      x                       ;  HorizRes
        dw      -y                      ;  -VertRes

;English Lo res WinX,WinY,VptX,VptY
        dw      325                     ;  HorzSize * 1000
        dw      325                     ;  VertSize * 1000
        dw      EngLoResX               ;  HorizRes * 254
        dw      EngLoResY               ;  -VertRes * 254

;English Hi res WinX,WinY,VptX,VptY
        dw      1625                    ;  HorzSize * 10000
        dw      1625                    ;  VertSize * 10000
        dw      EngHiResX               ;  HorizRes * 254
        dw      EngHiResY               ;  -VertRes * 254

;Twips  WinX,WinY,VptX,VptY
        dw      2340                    ;  HorzSize * 14400
        dw      2340                    ;  VertSize * 14400
        dw      TwipX                   ;  HorizRes * 254
        dw      TwipY                   ;  -VertRes * 254

        dw      96                      ;Logical Pixels/inch in X (PATCHED)
        dw      96                      ;Logical Pixels/inch in Y (PATCHED)
        dw      DC_IgnoreDFNP           ;dpDCManage
        dw      ?                       ;CAPS1
        dw      0
        dw      0
        dw      0
        dw      0
        dw      ?                       ; number of palette entries (PATCHED)
        dw      ?                       ; number of reserved entries (PATCHED)
        dw      ?                       ; DAC resolution for RGB (PATCHED)
name&Size = $-name
        endm

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
        globalD lpDriverPDevice,0		;-->our PDEV we pass to GDI
        globalD lpColorTable,0
	externD RepaintAddr
        externB bReEnable
        externW wResolution
        externW wPalettized
        externW wBpp
        externW wPDeviceFlags
        externW wCurrentMode
        globalW wDIBPdevSize,0
	globalW	wLastValidRes,1
	globalW	wLastValidBpp,8
	globalW	wLastValidMode,101h
	globalW wEnabled,0
sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
        .386
;----------------------------------------------------------------------------
; Color Table used for non-palettized 1 bpp format.
;----------------------------------------------------------------------------
DIB1ColorTable  label   dword
;               blue  green red   flags
 DIBColorEntry <0000, 0000, 0000, 0000      >  ; 0
 DIBColorEntry <0FFh, 0FFh, 0FFh, MAPTOWHITE>  ; 1
DIB1ColorTableSize = $ - DIB1ColorTable

;----------------------------------------------------------------------------
; Color Table used for non-palettized 4 bpp format.
;----------------------------------------------------------------------------
DIB4ColorTable  label   dword
;               blue  green red   flags
 DIBColorEntry <0000, 0000, 0000, 0000      >  ; 0
 DIBColorEntry <0000, 0000, 080h, 0000      >  ; 1
 DIBColorEntry <0000, 080h, 0000, 0000      >  ; 2
 DIBColorEntry <0000, 080h, 080h, 0000      >  ; 3
 DIBColorEntry <080h, 0000, 0000, 0000      >  ; 4
 DIBColorEntry <080h, 0000, 080h, 0000      >  ; 5
 DIBColorEntry <080h, 080h, 0000, 0000      >  ; 6
 DIBColorEntry <0C0h, 0C0h, 0C0h, MAPTOWHITE>  ; 7  Light Grey
 DIBColorEntry <080h, 080h, 080h, MAPTOWHITE>  ; 8  Dark Grey
 DIBColorEntry <0000, 0000, 0FFh, 0000      >  ; 9
 DIBColorEntry <0000, 0FFh, 0000, MAPTOWHITE>  ; a
 DIBColorEntry <0000, 0FFh, 0FFh, MAPTOWHITE>  ; b
 DIBColorEntry <0FFh, 0000, 0000, 0000      >  ; c
 DIBColorEntry <0FFh, 0000, 0FFh, 0000      >  ; d
 DIBColorEntry <0FFh, 0FFh, 0000, MAPTOWHITE>  ; e
 DIBColorEntry <0FFh, 0FFh, 0FFh, MAPTOWHITE>  ; f
DIB4ColorTableSize = $ - DIB4ColorTable

;----------------------------------------------------------------------------
; Color Table used for palettized 8 bpp format.
;----------------------------------------------------------------------------
DIB8ColorTable  label   dword
;               blue  green red   flags
 DIBColorEntry <0000, 0000, 0000, 0000      >  ; 0
 DIBColorEntry <0000, 0000, 080h, 0000      >  ; 1
 DIBColorEntry <0000, 080h, 0000, 0000      >  ; 2
 DIBColorEntry <0000, 080h, 080h, 0000      >  ; 3
 DIBColorEntry <080h, 0000, 0000, 0000      >  ; 4
 DIBColorEntry <080h, 0000, 080h, 0000      >  ; 5
 DIBColorEntry <080h, 080h, 0000, 0000      >  ; 6
 DIBColorEntry <0C0h, 0C0h, 0C0h, MAPTOWHITE>  ; 7  Light Grey
 DIBColorEntry <0C0h, 0dch, 0C0h, NONSTATIC+MAPTOWHITE>  ; Money Green
 DIBColorEntry <0F0h, 0CAh, 0A6h, NONSTATIC+MAPTOWHITE>  ; cool blue
REPT 236
 DIBColorEntry <0000, 0000, 0000, NONSTATIC >  ; Palette Manager colors
ENDM
 DIBColorEntry <0F0h, 0FBh, 0FFh, NONSTATIC+MAPTOWHITE>  ; off white
 DIBColorEntry <0A4h, 0A0h, 0A0h, NONSTATIC+MAPTOWHITE>  ; med grey
 DIBColorEntry <080h, 080h, 080h, MAPTOWHITE>  ; 8  Dark Grey
 DIBColorEntry <0000, 0000, 0FFh, 0000      >  ; 9
 DIBColorEntry <0000, 0FFh, 0000, MAPTOWHITE>  ; a
 DIBColorEntry <0000, 0FFh, 0FFh, MAPTOWHITE>  ; b
 DIBColorEntry <0FFh, 0000, 0000, 0000      >  ; c
 DIBColorEntry <0FFh, 0000, 0FFh, 0000      >  ; d
 DIBColorEntry <0FFh, 0FFh, 0000, MAPTOWHITE>  ; e
 DIBColorEntry <0FFh, 0FFh, 0FFh, MAPTOWHITE>  ; f
DIB8ColorTableSize = $ - DIB8ColorTable

;----------------------------------------------------------------------------
; The GDIInfo data Structures for multiple resolutions.  The specifics
; of the display driver are passed to GDI via this structure.
; Usage:
; DeclareGDIINFO name,x,y,EngLoResX,EngLoResY,EngHiResX,EngHiResY,TwipX,TwipY
;----------------------------------------------------------------------------
DeclareGDIINFO  GDIINFO0,320,200, 127,-127,64,-64,64,-64
DeclareGDIINFO  GDIINFO1,640,480, 254,-254,127,-127,127,-127
DeclareGDIINFO  GDIINFO2,800,600, 318,-318,159,-159,159,-159
DeclareGDIINFO  GDIINFO3,1024,768,407,-407,203,-203,203,-203
DeclareGDIINFO  GDIINFO4,1280,1024,509,-509,254,-254,254,-254

GDIINFOTable    label   word
        dw      GDIINFO0        ;For 320x200   mode
        dw      GDIINFO1        ;For 640x480   mode
        dw      GDIINFO2        ;For 800x600   mode
        dw      GDIINFO3        ;For 1024x768  mode
        dw      GDIINFO4        ;For 1280x1024 mode

;----------------------------------------------------------------------------
; ReEnable
;   Called by GDI to change the display resolution dynamically. This function
;   will retrieve the new resolution value from the register, map it into
;   a resolution ID (0 = 320x200, 1=640x480, 2=800x600, 3=1024x768 will set
;   set wResolution to this value. It will then call Enable twice to set up
;   the GDIInfo block and the PDevice block appropriately.
;
;   The return value is FALSE if the function failed, else it is TRUE
;----------------------------------------------------------------------------
cProc ReEnable,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        parmD   lpPDevice               ;-> devices PDevice
        parmD   lpGDIInfo               ;-> GDI Info block
cBegin
	mov	ax,DGROUP
	mov	ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
	mov	ax,wResolution		;save just in case we fail.
	mov	wLastValidRes,ax
	mov	ax,wBpp
	mov	wLastValidBpp,ax
	mov	ax,wCurrentMode
	mov	wLastValidMode,ax
	call	GetScreenSettings	;Get new res, etc.
        mov     bReEnable,1             ;this is a reenable call
	push	lpPDevice
	xor	eax,eax
	push	eax			;Left, Top = 0,0
	mov	ax,wScreenWidth
	dec	ax
	push	ax			;Right
	mov	ax,wScreenHeight
	dec	ax
	push	ax			;Bottom
	push	CURSOREXCLUDE
	call	DIB_BeginAccess
@@:
;
; call enable to create a new PDevice and init the h/w.
;
        xor     eax,eax
        cCall   Enable,<lpPDevice,0,eax,eax,eax>
	push	ax			;save results from Enable.
	push	lpPDevice
	push	CURSOREXCLUDE
	call	DIB_EndAccess
	pop	ax
@@:	test	ax,ax
	jz	RE_Fail
;
; call enable to fill in the new GDIINFO Block
;
        xor     eax,eax
        cCall   Enable,<lpGDIInfo,1,eax,eax,eax>
	mov	ax,1
PLABEL RE_Exit
	mov	bReEnable,0
cEnd

PLABEL RE_Fail
	mov	ax,wLastValidRes	;Restore last mode.
	mov	wResolution,ax
	mov	ax,wLastValidBpp
	mov	wBpp,ax
	mov	ax,wLastValidMode
	mov	wCurrentMode,ax
        xor     eax,eax
        cCall   Enable,<lpPDevice,0,eax,eax,eax>
	call	RepaintAddr
	xor	ax,ax
	jmp	RE_Exit

;----------------------------------------------------------------------------
; Enable
;   Called twice by GDI at boot time.  1st time to retrieve GDIINFO
;   structure, 2nd time to initialize video hardware.
;----------------------------------------------------------------------------
cProc   Enable,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
        parmD   lpDevice                ;Physical device or GDIinfo destination
        parmW   style                   ;Style, Enable Device, or Inquire Info
        parmD   lpDeviceType            ;Device type (i.e FX80, HP7470, ...)
        parmD   lpOutputFile            ;DOS output file name (if applicable)
        parmD   lpStuff                 ;Device specific information
cBegin
	mov	ax,DGROUP
	mov	ds,ax
        assumes ds,Data
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        test    style,1                 ;Get GDIINFO?
	jnz	E_GetGDIInfo		;yes.

;----------------------------------------------------------------------------
; Init the PDEVICE.
;----------------------------------------------------------------------------
	mov	eax,lpDevice
	mov	lpDriverPDevice,eax	;save ptr to my pDevice.
        call    PhysicalEnable          ;Attempt to init the h/w.
	jc	E_ErrorExit		;did not work. Fail the call.
        cmp     bReEnable,0             ;re-enable being done ?
        jnz     short @f                ;yes, bypass int 2f call
        mov     ax,STOP_IO_TRAP
        int     2fh
@@:     cCall   DIB_Enable,<lpDevice,style,lpDeviceType,lpOutputFile,lpStuff>
        cmp     wPalettized,0           ;Is the display palettized?
        je      short @f                ;no.
        xor     ax,ax                   ;yes.
        farPtr  <lpNULL>,ax,ax          ;set up a null pointer
        arg     lpNULL
        cCall   SetPaletteTranslate     ;initialize the palette trans. table
@@:     les     di,lpDevice             ;--> dest: PDEVICE or GDIINFO struct.
        assumes es,nothing

; Build the driver's PDevice

        mov     es:[di].deType,'RP'
        mov     ax,wScreenWidth             ;ax = width of screen in pels.
        mov     es:[di].deWidth,ax          ;store it.
        mov     ax,wScreenHeight            ;ax = height of screen in scans.
        mov     es:[di].deHeight,ax         ;store it.
        movzx   eax,wScreenWidthBytes       ;ax = width of screen in bytes.
        mov     es:[di].deWidthBytes,ax     ;store off the byte screen width.
        mov     es:[di].deDeltaScan,eax     ;store off the delta scan width.
        mov     bx,wBpp                     ;bx = bpp.
        inc     bh                          ;bh = 1
        xchg    bl,bh                       ;bl = planes (always 1), bh = bpp
        mov     word ptr es:[di].dePlanes,bx;store off planes, bpp.
        xor     eax,eax                     ;offset to surface bits is zero.
        mov     es:[di].deReserved1,eax     ;make this zero.
        mov     es:[di].delpPDevice,eax     ;make this zero.
        mov     dword ptr es:[di].deBits,eax;store it.
        mov     ax,ScreenSelector           ;ax = selector to the surface bits.
        mov     word ptr es:[di].deBits+4,ax;store it.
        mov     ax,wPDeviceFlags            ;determined in vga.asm
        cmp     wPalettized,0               ;Is the surface palettized?
        je      short @f                    ;no.
        or      ax,PALETTIZED               ;yes. Set the PALETTIZED bit too.
@@:     mov     es:[di].deFlags,ax          ;store PDevice flags.
        mov     bx,di
        add     bx,wDIBPdevSize             ;es:bx -->surface's BitmapInfoHeader
        mov     word ptr es:[di].deBitmapInfo,bx  ;store it.
        mov     word ptr es:[di].deBitmapInfo+2,es;es = selector of BitmapInfoHeader
        mov     es:[di].deVersion,DRV_VERSION   ;Show that we are version 4.

; Copy the call back address of the Begin/End access functions

	mov	ax,seg DIB_BeginAccess
	shl	eax,16
	mov	ax,offset DIB_BeginAccess
        mov     dword ptr es:[di].deBeginAccess,eax
	mov	ax,seg DIB_EndAccess
	shl	eax,16
	mov	ax,offset DIB_EndAccess
        mov     dword ptr es:[di].deEndAccess,eax

; Now fill in the BitmapInfoHeader following the PDevice.

        mov     es:[bx].biSize,(size BitmapInfoHeader)  ;store header size.
        movzx   eax,es:[di].deWidth             ;get pel width.
        mov     es:[bx].biWidth,eax             ;store it.
        mov     ax,es:[di].deHeight             ;get # of scans.
        mov     es:[bx].biHeight,eax            ;store it.
        mov     ax,wBpp                         ;ax = bpp.
        rol     eax,16                          ;eax.msw = bpp
        inc     ax                              ;eax.lsw = 1
        mov     dword ptr es:[bx].biPlanes,eax  ;store planes, bpp.
        xor     eax,eax
        mov     es:[bx].biCompression,eax       ;zero these for now.
        mov     es:[bx].biSizeImage,eax
        mov     es:[bx].biXPelsPerMeter,eax
        mov     es:[bx].biYPelsPerMeter,eax
        mov     es:[bx].biClrUsed,eax
        mov     es:[bx].biClrImportant,eax

; Now copy in the color table (if there is one). If this is a re-enable call,
; we will not need to copy the color table, the PDevice would already have
; one.

        mov     ax,wBpp                         ;ax = bpp.  Assume 8 bpp.
        mov     si,offset DIB8ColorTable        ;cs:si-->DIB8 Color table.
        mov     cx,DIB8ColorTableSize/4         ;cx = size of DIB8 color table.
        cmp     wPalettized,0                   ;Are we palettized?
        jne     short @f                        ;yes.
;;;     mov     si,offset DIB8ColorTable_NoPal  ;cs:si-->DIB8 non-palettized
@@:     cmp     ax,8                            ;is bpp 8?
        jg      E_Install2FHandler              ;It is bigger. No color table.
        je      E_CopyColorTable                ;yes. Go copy the table.
        mov     si,offset DIB1ColorTable        ;no. cs:si-->DIB1 color table.
        mov     cx,DIB1ColorTableSize/4         ;cx = size of DIB1 tolor table.
        cmp     ax,1                            ;is bpp 1?
        je      E_CopyColorTable                ;yes. Go copy the table.
        mov     si,offset DIB4ColorTable        ;no.
        mov     cx,DIB4ColorTableSize/4         ;bpp must be 4 then.

PLABEL E_CopyColorTable
        lea     di,[bx].biClrImportant+4        ;es:di-->start of color table.
        mov     word ptr lpColorTable,di
        mov     word ptr lpColorTable+2,es      ;remember address of color table.
        cmp     bReEnable,0                     ;ReEnable call ?
        jnz     short @f                        ;yes, colors are already there
        rep     movs dword ptr es:[di],cs:[si]
@@:
	mov	cx,2
	cmp	wBpp,1
	je	short @f
	mov	cx,16
	cmp	wBpp,4
	je	short @f
	mov	cx,256
@@:	xor	ax,ax
	push	ds
	lds	si,lpColorTable
	call	SetRAMDAC_far			;set up initial palette
	pop	ds

PLABEL E_Install2FHandler
        cmp     bReEnable,0                     ;re-enable call ?
        jnz     short @f                        ;yes, no need of int 2f hooking
        call    HookInt2Fh                      ;Hook into multiplexed interrupt
@@:     mov     ax,1
	mov	wEnabled,ax
        jmp     E_Exit

;----------------------------------------------------------------------------
; Return GDIINFO to GDI.
;----------------------------------------------------------------------------
PLABEL E_GetGDIInfo
        assumes ds,Data
        assumes es,nothing
        assumes fs,nothing
        assumes gs,nothing
        cCall   DIB_Enable,<lpDevice,style,lpDeviceType,lpOutputFile,lpStuff>

        movzx   ebx,wResolution
        les     di,lpDevice             ;--> dest: GDIINFO struct.
        assumes es,nothing

        mov     si,GDIINFOTable[ebx*2]
        mov     cx,24/4                 ;copy 1st 24 bytes
        rep     movs dword ptr es:[di], dword ptr cs:[si]
        add     di,24                   ;skip past DIB Engine capabilities
        add     si,24
        mov     cx,40/4                 ;copy the rest of our GDIINFO.
        rep     movs dword ptr es:[di], dword ptr cs:[si]
        errnz   GDIINFO1Size-110
        mov     di,word ptr lpDevice    ;es:di-->GDIINFO structure
        mov     ax,wDpi                 ;Patch up LogPixels X and Y with wDpi.
        mov     es:[di].dpLogPixelsX,ax
        mov     es:[di].dpLogPixelsY,ax
        mov     bx,wBpp
        mov     es:[di].dpBitsPixel,bx  ;patch up bpp.
        mov     es:[di].dpDCManage,DC_IgnoreDFNP
        or      es:[di].dpCaps1,C1_REINIT_ABLE+C1_BYTE_PACKED+C1_GLYPH_INDEX+C1_COLORCURSOR+C1_SLOW_CARD
        mov     ax,es:[di].dpDEVICEsize
        mov     wDIBPdevSize,ax         ;save DIB Eng. PDevice size.

        cmp     bx,8
        jl      E_BPPLessThan8
        jg      E_BPPGreaterThan8

PLABEL E_BPPIs8
        mov     es:[di].dpNumBrushes,-1         ;# of brushes (a big value)
        cmp     wPalettized,0                   ;is this driver palettized?
        je      short @f                        ;no.
        mov     es:[di].dpNumPens,16            ;# of pens this driver realizes
        mov     es:[di].dpNumColors,20          ;# colors in color table
        mov     es:[di].dpNumPalReg,256
        mov     es:[di].dpPalReserved,20
        mov     es:[di].dpColorRes,18
        or      es:[di].dpRaster,RC_PALETTE+RC_DIBTODEV+RC_SAVEBITMAP
        mov     ax,(size BitmapInfoHeader) + 256*4
        add     es:[di].dpDEVICEsize,ax         ;GDI will allocate this for us.
        mov     ax,GDIINFO1Size                 ;return size of GDIINFO
        jmp     E_Exit

@@:     mov     es:[di].dpNumPens,256           ;# of pens this driver realizes
        mov     es:[di].dpNumColors,256         ;# colors in color table
        xor     ax,ax
        mov     es:[di].dpNumPalReg,ax
        mov     es:[di].dpPalReserved,ax
        mov     es:[di].dpColorRes,ax
        or      es:[di].dpRaster,RC_DIBTODEV
        mov     ax,(size BitmapInfoHeader) + 256*4
        add     es:[di].dpDEVICEsize,(size BitmapInfoHeader) + 256*4
                                                ;GDI will allocate this for us.
        mov     ax,GDIINFO1Size                 ;return size of GDIINFO
        jmp     short E_Exit


PLABEL E_BPPLessThan8
        mov     cl,bl
        mov     ax,1
        shl     ax,cl                           ;bpp 4 = 16  bpp 1 = 2
        mov     es:[di].dpNumPens,ax            ;# of pens this driver realizes
        mov     es:[di].dpNumColors,ax          ;# colors in color table
        mov     es:[di].dpNumBrushes,-1         ;# of brushes (a big value)
        xor     cx,cx
        mov     es:[di].dpNumPalReg,cx
        mov     es:[di].dpPalReserved,cx
        mov     es:[di].dpColorRes,cx
        or      es:[di].dpRaster,RC_DIBTODEV
        shl     ax,2                            ;ax = size of color table.
        add     ax,(size DIBENGINE) + 8 + (size BitmapInfoHeader) ;Add in PDEVICE
        mov     es:[di].dpDEVICEsize,ax         ;GDI will allocate this for us.
        mov     ax,GDIINFO1Size                 ;return size of GDIINFO
        jmp     short E_Exit

PLABEL E_BPPGreaterThan8
        mov     ax,-1
        mov     es:[di].dpNumPens,ax            ;# of pens this driver realizes
        mov     es:[di].dpNumBrushes,ax         ;# of brushes (a big value)
        mov     es:[di].dpNumColors,ax          ;# colors in color table
        inc     ax
        mov     es:[di].dpNumPalReg,ax
        mov     es:[di].dpPalReserved,ax
        mov     es:[di].dpColorRes,ax
        or      es:[di].dpRaster,RC_DIBTODEV
        add     es:[di].dpDEVICEsize,(size BitmapInfoHeader) ;GDI will allocate
                                                             ; this for us.
        mov     ax,GDIINFO1Size                 ;return size of GDIINFO

PLABEL E_Exit
cEnd

PLABEL E_ErrorExit
	xor	ax,ax
	jmp	E_Exit

sEnd    InitSeg
end
