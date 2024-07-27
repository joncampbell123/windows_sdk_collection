;***************************************************************************
;                                                                          *
;   Copyright (C) 1984 by Microsoft Inc.                                   *
;                                                                          *
;***************************************************************************

        title   Epson Specific Code and Routine
        %out    epson
        page    ,132



;       This file contains the GDIINFO for both landscape and portrait
;       mode.
;

;       Define the portions of gdidefs.inc that will be needed

incDevice = 1


        include cmacros.inc
        include gdidefs.inc
        include epson.inc

sBegin  data
assumes ds,data

        public  port_infobase  ;EPSON portrait GDIInfo table base address
        public  land_infobase  ;EPSON landscape info table base address

ifdef WIDE_PG_SUPPORT
        public  wide_port_infobase  ;EPSON portrait GDIInfo table base address
        public  wide_land_infobase  ;EPSON landscape info table base address
endif

land_Infobase label byte


        dw      100H                    ;Version = 0100h for now
        errnz   dpVersion

        dw      DT_RASPRINTER           ;Device classification
        errnz   dpTechnology-dpVersion-2

        dw      MM_VSIZE                ;landscape Horizontal size in millimeters

        errnz   dpHorzSize-dpTechnology-2

        dw      MM_HSIZE                ;Vertical size in millimeters

        errnz   dpVertSize-dpHorzSize-2

        dw      PG_DOWN                 ;Horizontal width in pixels
        errnz   dpHorzRes-dpVertSize-2

        dw      PG_ACROSS               ;Vertical width in pixels
        errnz   dpVertRes-dpHorzRes-2

        dw      1                       ;Number of bits per pixel
        errnz   dpBitsPixel-dpvertRes-2

        dw      1                       ;Number of planes
        errnz   dpPlanes-dpBitsPixel-2

        dw      17                      ;Number of brushes thdevice.has
        errnz   dpNumBrushes-dpPlanes-2

        dw      8                       ;Number of pens thdevice.has
        errnz   dpNumPens-dpNumBrushes-2

        dw      0                       ; future use

        dw      0                       ; Number of fonts thdevice.has
        errnz   dpNumFonts-dpNumPens-4

        dw      2                       ;Number of colors in color table
        errnz   dpNumColors-dpNumFonts-2

        dw      0                       ;Size required for device descriptor
        errnz   dpDEVICEsize-dpNumColors-2

        dw      CC_NONE                 ;Curves capabilities
        errnz   dpCurves-dpDEVICEsize-2

        dw      LC_NONE                 ;Line capabilities
        errnz   dpLines-dpCurves-2

        dw      PC_SCANLINE             ;Polygonal capabilities
        errnz   dpPolygonals-dpLines-2

        dw      TC_RA_ABLE
                                        ;raster font able
        errnz   dpText-dpPolygonals-2

        dw      CP_NONE                 ;Clipping capabilities
        errnz   dpClip-dpText-2

        dw      RC_BITBLT OR RC_BANDING ;Bitblt capabilities
        errnz   dpRaster-dpClip-2

        dw      yMajorDist              ;Distance moving X only
        errnz   dpAspectX-dpraster-2

        dw      xMajorDist              ;Distance moving Y only
        errnz   dpAspectY-dpAspectX-2

        dw      Hypotenuse              ;Distance moving X and Y
        errnz   dpAspectXY-dpAspectY-2

        dw      MaxStyleErr             ;Length of segment for line styles
        errnz   dpStyleLen-dpAspectXY-2

; landscape
        errnz   dpMLoWin-dpStyleLen-2   ;Metric  Lo res WinX,WinY,VptX,VptY
        dw      MM02                    ;HorzSize * 10
        dw      MM01                    ;VertSize * 10
        dw      MM04                    ;HorizRes
        dw      -MM03                   ;-VertRes

        errnz   dpMHiWin-dpMLoWin-8     ;Metric  Hi res WinX,WinY,VptX,VptY
        dw      MM002                   ;HorzSize * 100
        dw      MM001                   ;VertSize * 100
        dw      MM004                   ;HorizRes
        dw      -MM003                  ;-VertRes

; landscape
        errnz   dpELoWin-dpMHiWin-8     ;English Lo res WinX,WinY,VptX,VptY
        dw      EnglishLo2              ;  HorzSize * 1000
        dw      EnglishLo1              ;  VertSize * 1000
        dw      EnglishLo4              ;  HorizRes * 254
        dw      -EnglishLo3             ;  -VertRes * 254


        errnz   dpEHiWin-dpELoWin-8     ;English Hi res WinX,WinY,VptX,VptY
        dw      EnglishHi2              ;  HorzSize * 10000
        dw      EnglishHi1              ;  VertSize * 10000
        dw      EnglishHi4              ;  HorizRes * 254
        dw      -EnglishHi3             ;  -VertRes * 254

        errnz   dpTwpWin-dpEHiWin-8     ;Twips          WinX,WinY,VptX,VptY
        dw      Twips2                  ;  HorzSize * 14400
        dw      Twips1                  ;  VertSize * 14400
        dw      Twips4                  ;  HorizRes * 254
        dw      -Twips3                 ;  -VertRes * 254

;landscape
        errnz   dpLogPixelsX-dpTwpWin-8
        dw      VDPI

        errnz   dpLogPixelsY-dpLogPixelsX-2
        dw      HDPI

        errnz   dpDCManage-dpLogPixelsY-2
        dw      DC_SPDevice

        dw      0
        dw      0
        dw      0
        dw      0
        dw      0


        errnz   <(OFFSET $)-(OFFSET land_infobase)-(SIZE GDIINFO)>


port_infobase label byte


        dw      100H                    ;Version = 0100h for now
        errnz   dpVersion

        dw      DT_RASPRINTER           ;Device classification
        errnz   dpTechnology-dpVersion-2

        dw      MM_HSIZE                ;Horizontal size in millimeters
                                        ;8 inches - portrait
        errnz   dpHorzSize-dpTechnology-2

        dw      MM_VSIZE                ;Vertical size in millimeters
                                        ;10 inches - portrait
        errnz   dpVertSize-dpHorzSize-2

        dw      PG_ACROSS               ;Horizontal width in pixels
        errnz   dpHorzRes-dpVertSize-2

        dw      PG_DOWN                 ;Vertical width in pixels
        errnz   dpVertRes-dpHorzRes-2

        dw      1                       ;Number of bits per pixel
        errnz   dpBitsPixel-dpvertRes-2

        dw      1                       ;Number of planes
        errnz   dpPlanes-dpBitsPixel-2

        dw      17                      ;Number of brushes thdevice.has
        errnz   dpNumBrushes-dpPlanes-2

        dw      8                       ;Number of pens thdevice.has
        errnz   dpNumPens-dpNumBrushes-2

        dw      0                       ; future use

        dw      0                       ; Number of fonts thdevice.has
        errnz   dpNumFonts-dpNumPens-4

        dw      2                       ;Number of colors in color table
        errnz   dpNumColors-dpNumFonts-2

        dw      0                       ;Size required for device descriptor
        errnz   dpDEVICEsize-dpNumColors-2

        dw      CC_NONE                 ;Curves capabilities
        errnz   dpCurves-dpDEVICEsize-2

        dw      LC_NONE                 ;Line capabilities
        errnz   dpLines-dpCurves-2

        dw      PC_SCANLINE             ;Polygonal capabilities
        errnz   dpPolygonals-dpLines-2

        dw      TC_RA_ABLE
                                        ;raster font able
        errnz   dpText-dpPolygonals-2

        dw      CP_NONE                 ;Clipping capabilities
        errnz   dpClip-dpText-2

        dw      RC_BITBLT OR RC_BANDING ;Bitblt capabilities
        errnz   dpRaster-dpClip-2

        dw      xMajorDist              ;Distance moving X only
        errnz   dpAspectX-dpraster-2

        dw      yMajorDist              ;Distance moving Y only
        errnz   dpAspectY-dpAspectX-2

        dw      Hypotenuse              ;Distance moving X and Y
        errnz   dpAspectXY-dpAspectY-2

        dw      MaxStyleErr             ;Length of segment for line styles
        errnz   dpStyleLen-dpAspectXY-2

; portrait
        errnz   dpMLoWin-dpStyleLen-2   ;Metric  Lo res WinX,WinY,VptX,VptY
        dw      MM01                    ;HorzSize * 10
        dw      MM02                    ;VertSize * 10
        dw      MM03                    ;HorizRes
        dw      -MM04                   ;-VertRes

        errnz   dpMHiWin-dpMLoWin-8     ;Metric  Hi res WinX,WinY,VptX,VptY
        dw      MM001                   ;HorzSize * 100
        dw      MM002                   ;VertSize * 100
        dw      MM003                   ;HorizRes
        dw      -MM004                  ;-VertRes

; portrait
        errnz   dpELoWin-dpMHiWin-8     ;English Lo res WinX,WinY,VptX,VptY
        dw      EnglishLo1              ;  HorzSize * 1000
        dw      EnglishLo2              ;  VertSize * 1000
        dw      EnglishLo3              ;  HorizRes * 254
        dw      -EnglishLo4             ;  -VertRes * 254


        errnz   dpEHiWin-dpELoWin-8     ;English Hi res WinX,WinY,VptX,VptY
        dw      EnglishHi1              ;  HorzSize * 10000
        dw      EnglishHi2              ;  VertSize * 10000
        dw      EnglishHi3              ;  HorizRes * 254
        dw      -EnglishHi4             ;  -VertRes * 254

        errnz   dpTwpWin-dpEHiWin-8     ;Twips          WinX,WinY,VptX,VptY
        dw      Twips1                  ;  HorzSize * 14400
        dw      Twips2                  ;  VertSize * 14400
        dw      Twips3                  ;  HorizRes * 254
        dw      -Twips4                 ;  -VertRes * 254

;portrait
        errnz   dpLogPixelsX-dpTwpWin-8
        dw      HDPI

        errnz   dpLogPixelsY-dpLogPixelsX-2
        dw      VDPI

        errnz   dpDCManage-dpLogPixelsY-2
        dw      DC_SPDevice

        dw      0
        dw      0
        dw      0
        dw      0
        dw      0

ifdef WIDE_PG_SUPPORT
wide_land_Infobase label byte


        dw      100H                    ;Version = 0100h for now
        errnz   dpVersion

        dw      DT_RASPRINTER           ;Device classification
        errnz   dpTechnology-dpVersion-2

        dw      MM_VSIZE                ;landscape Horizontal size in millimeters

        errnz   dpHorzSize-dpTechnology-2

        dw      WIDE_MM_HSIZE                ;Vertical size in millimeters

        errnz   dpVertSize-dpHorzSize-2

        dw      PG_DOWN                 ;Horizontal width in pixels
        errnz   dpHorzRes-dpVertSize-2

        dw      WIDE_PG_ACROSS               ;Vertical width in pixels
        errnz   dpVertRes-dpHorzRes-2

        dw      1                       ;Number of bits per pixel
        errnz   dpBitsPixel-dpvertRes-2

        dw      1                       ;Number of planes
        errnz   dpPlanes-dpBitsPixel-2

        dw      17                      ;Number of brushes thdevice.has
        errnz   dpNumBrushes-dpPlanes-2

        dw      8                       ;Number of pens thdevice.has
        errnz   dpNumPens-dpNumBrushes-2

        dw      0                       ; future use

        dw      0                       ; Number of fonts thdevice.has
        errnz   dpNumFonts-dpNumPens-4

        dw      2                       ;Number of colors in color table
        errnz   dpNumColors-dpNumFonts-2

        dw      0                       ;Size required for device descriptor
        errnz   dpDEVICEsize-dpNumColors-2

        dw      CC_NONE                 ;Curves capabilities
        errnz   dpCurves-dpDEVICEsize-2

        dw      LC_NONE                 ;Line capabilities
        errnz   dpLines-dpCurves-2

        dw      PC_SCANLINE             ;Polygonal capabilities
        errnz   dpPolygonals-dpLines-2

        dw      TC_RA_ABLE
                                        ;raster font able
        errnz   dpText-dpPolygonals-2

        dw      CP_NONE                 ;Clipping capabilities
        errnz   dpClip-dpText-2

        dw      RC_BITBLT OR RC_BANDING ;Bitblt capabilities
        errnz   dpRaster-dpClip-2

        dw      yMajorDist              ;Distance moving X only
        errnz   dpAspectX-dpraster-2

        dw      xMajorDist              ;Distance moving Y only
        errnz   dpAspectY-dpAspectX-2

        dw      Hypotenuse              ;Distance moving X and Y
        errnz   dpAspectXY-dpAspectY-2

        dw      MaxStyleErr             ;Length of segment for line styles
        errnz   dpStyleLen-dpAspectXY-2

; landscape
        errnz   dpMLoWin-dpStyleLen-2   ;Metric  Lo res WinX,WinY,VptX,VptY
        dw      MM02                    ;HorzSize * 10
        dw      MM01               ;VertSize * 10
        dw      MM04                    ;HorizRes
        dw      -MM03                ;-VertRes

        errnz   dpMHiWin-dpMLoWin-8     ;Metric  Hi res WinX,WinY,VptX,VptY
        dw      MM002                   ;HorzSize * 100
        dw      MM001               ;VertSize * 100
        dw      MM004                   ;HorizRes
        dw      -MM003              ;-VertRes

; landscape
        errnz   dpELoWin-dpMHiWin-8     ;English Lo res WinX,WinY,VptX,VptY
        dw      EnglishLo2              ;  HorzSize * 1000
        dw      EnglishLo1          ;  VertSize * 1000
        dw      EnglishLo4              ;  HorizRes * 254
        dw      -EnglishLo3         ;  -VertRes * 254


        errnz   dpEHiWin-dpELoWin-8     ;English Hi res WinX,WinY,VptX,VptY
        dw      EnglishHi2              ;  HorzSize * 10000
        dw      EnglishHi1          ;  VertSize * 10000
        dw      EnglishHi4              ;  HorizRes * 254
        dw      -EnglishHi3         ;  -VertRes * 254

        errnz   dpTwpWin-dpEHiWin-8     ;Twips          WinX,WinY,VptX,VptY
        dw      Twips2                  ;  HorzSize * 14400
        dw      Twips1              ;  VertSize * 14400
        dw      Twips4                  ;  HorizRes * 254
        dw      -Twips3             ;  -VertRes * 254

;landscape
        errnz   dpLogPixelsX-dpTwpWin-8
        dw      VDPI

        errnz   dpLogPixelsY-dpLogPixelsX-2
        dw      HDPI

        errnz   dpDCManage-dpLogPixelsY-2
        dw      DC_SPDevice

        dw      0
        dw      0
        dw      0
        dw      0
        dw      0


        errnz   <(OFFSET $)-(OFFSET wide_land_infobase)-(SIZE GDIINFO)>


wide_port_infobase label byte


        dw      100H                    ;Version = 0100h for now
        errnz   dpVersion

        dw      DT_RASPRINTER           ;Device classification
        errnz   dpTechnology-dpVersion-2

        dw      WIDE_MM_HSIZE            ;Horizontal size in millimeters

        errnz   dpHorzSize-dpTechnology-2

        dw      MM_VSIZE                ;Vertical size in millimeters
                                        ;10 inches - portrait
        errnz   dpVertSize-dpHorzSize-2

        dw      WIDE_PG_ACROSS               ;Horizontal width in pixels
        errnz   dpHorzRes-dpVertSize-2

        dw      PG_DOWN                 ;Vertical width in pixels
        errnz   dpVertRes-dpHorzRes-2

        dw      1                       ;Number of bits per pixel
        errnz   dpBitsPixel-dpvertRes-2

        dw      1                       ;Number of planes
        errnz   dpPlanes-dpBitsPixel-2

        dw      17                      ;Number of brushes thdevice.has
        errnz   dpNumBrushes-dpPlanes-2

        dw      8                       ;Number of pens thdevice.has
        errnz   dpNumPens-dpNumBrushes-2

        dw      0                       ; future use

        dw      0                       ; Number of fonts thdevice.has
        errnz   dpNumFonts-dpNumPens-4

        dw      2                       ;Number of colors in color table
        errnz   dpNumColors-dpNumFonts-2

        dw      0                       ;Size required for device descriptor
        errnz   dpDEVICEsize-dpNumColors-2

        dw      CC_NONE                 ;Curves capabilities
        errnz   dpCurves-dpDEVICEsize-2

        dw      LC_NONE                 ;Line capabilities
        errnz   dpLines-dpCurves-2

        dw      PC_SCANLINE             ;Polygonal capabilities
        errnz   dpPolygonals-dpLines-2

        dw      TC_RA_ABLE
                                        ;raster font able
        errnz   dpText-dpPolygonals-2

        dw      CP_NONE                 ;Clipping capabilities
        errnz   dpClip-dpText-2

        dw      RC_BITBLT OR RC_BANDING ;Bitblt capabilities
        errnz   dpRaster-dpClip-2

        dw      xMajorDist              ;Distance moving X only
        errnz   dpAspectX-dpraster-2

        dw      yMajorDist              ;Distance moving Y only
        errnz   dpAspectY-dpAspectX-2

        dw      Hypotenuse              ;Distance moving X and Y
        errnz   dpAspectXY-dpAspectY-2

        dw      MaxStyleErr             ;Length of segment for line styles
        errnz   dpStyleLen-dpAspectXY-2

; portrait
        errnz   dpMLoWin-dpStyleLen-2   ;Metric  Lo res WinX,WinY,VptX,VptY
        dw      MM01                ;HorzSize * 10
        dw      MM02                    ;VertSize * 10
        dw      MM03                ;HorizRes
        dw      -MM04                   ;-VertRes

        errnz   dpMHiWin-dpMLoWin-8     ;Metric  Hi res WinX,WinY,VptX,VptY
        dw      MM001               ;HorzSize * 100
        dw      MM002                   ;VertSize * 100
        dw      MM003               ;HorizRes
        dw      -MM004                  ;-VertRes

; portrait
        errnz   dpELoWin-dpMHiWin-8     ;English Lo res WinX,WinY,VptX,VptY
        dw      EnglishLo1          ;  HorzSize * 1000
        dw      EnglishLo2              ;  VertSize * 1000
        dw      EnglishLo3          ;  HorizRes * 254
        dw      -EnglishLo4             ;  -VertRes * 254


        errnz   dpEHiWin-dpELoWin-8     ;English Hi res WinX,WinY,VptX,VptY
        dw      EnglishHi1          ;  HorzSize * 10000
        dw      EnglishHi2              ;  VertSize * 10000
        dw      EnglishHi3          ;  HorizRes * 254
        dw      -EnglishHi4             ;  -VertRes * 254

        errnz   dpTwpWin-dpEHiWin-8     ;Twips          WinX,WinY,VptX,VptY
        dw      Twips1              ;  HorzSize * 14400
        dw      Twips2                  ;  VertSize * 14400
        dw      Twips3              ;  HorizRes * 254
        dw      -Twips4                 ;  -VertRes * 254

;portrait
        errnz   dpLogPixelsX-dpTwpWin-8
        dw      HDPI

        errnz   dpLogPixelsY-dpLogPixelsX-2
        dw      VDPI

        errnz   dpDCManage-dpLogPixelsY-2
        dw      DC_SPDevice

        dw      0
        dw      0
        dw      0
        dw      0
        dw      0

        errnz   <(OFFSET $)-(OFFSET wide_port_infobase)-(SIZE GDIINFO)>
endif 		; WIDE_PG_SUPPORT

sEnd    data

sBegin  code

assumes cs,code

;       return:  (ax) > 0 if the resulting rectangle is not empty

cProc   OffsetClipRect,<NEAR,PUBLIC>

        ParmD       lpRect
        ParmW       Xoffset
        ParmW       Yoffset

cBegin
        push        ds
        lds         bx,lpRect           ;load rectangle
        mov         cx,Xoffset
        mov         dx,Yoffset
        xor         ax,ax

        sub         left[bx],cx
        jns         shift1
        mov         left[bx],ax
shift1:
        sub         top[bx],dx
        jns         shift2
        mov         top[bx],ax

shift2:
        mov         ax,right[bx]
        sub         ax,cx
        mov         right[bx],ax
        sub         ax,left[bx]
        jle         done
        sub         bottom[bx],dx
        mov         ax,bottom[bx]
        sub         ax,top[bx]
done:
        pop         ds

cEnd    OffsetRect

cProc   Copy, <FAR, PUBLIC>, <si,di>

        parmd   dst             ;long destination pointer
        parmd   src             ;long source pointer
        parmw   cnt             ;cnt of bytes

cBegin  Copy
        push    ds
        mov     cx,cnt
        les     di,dst
        lds     si,src
        cld
        rep     movsb
        pop     ds
cEnd    Copy

cProc   FillBuffer, <FAR, PUBLIC>, <di>

        parmd   dst             ;long destination pointer
        parmw   pattern         ;word pattern to fill the buffer
        parmw   cnt             ;cnt of words

cBegin  FillBuffer
        mov     cx,cnt
        mov     ax,pattern
        les     di,dst
        cld
        rep     stosw
cEnd    FillBuffer

sEnd    code
end
