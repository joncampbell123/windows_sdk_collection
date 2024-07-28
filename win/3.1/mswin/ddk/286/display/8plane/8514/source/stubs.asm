page            ,132
title           Unimplemented GDI Feature Stubs
;
;
.xlist
include         CMACROS.INC
.list
;
;
sBegin          Code
assumes         cs,Code
assumes         ds,Data
;
;       In MS-WINDOWS 2.0, there are a number of unimplemented GDI calls to
;the device driver.  The GDI expects the device driver to return these calls
;to it with an appropriate return code.  This file simply exists to do that
;task in a convenient and non-intrusive manner.
;
subttl          DeviceBitmap Function
page +
cProc           DeviceBitmap,<FAR,PUBLIC,WIN,PASCAL>
        parmD   lpDstDev
        parmW   Command
        parmD   lpBitmap
        parmD   lpBits
cBegin
xor             ax,ax                   ;return failure return code
cEnd                                    ;and return to caller
;
;
subttl          SetAttribute Function
page +
cProc           SetAttribute,<FAR,PUBLIC,WIN,PASCAL>
        parmD   lpDstDev
        parmW   StateNum
        parmW   Index
        parmW   Attribute
cBegin                                              
xor     ax,ax                           ;return failure return code
cEnd                                    ;and return to caller
;
;
subttl          Fast Border Function
page +
cProc           FastBorder,<FAR,PUBLIC,WIN,PASCAL>
;
        parmD   lpRect
        parmW   HorizBorderThick
        parmW   VertBorderThick
        parmD   ROP
        parmD   lpDstDev
        parmD   lpPBrush
        parmD   lpDrawMode
        parmD   lpClipRect
;
cBegin
xor             ax,ax                   ;set failure code
cEnd                                    ;and return it to caller
;
;
cProc   DeviceMode,<FAR,PUBLIC,WIN,PASCAL>
;
        parmW   hWnd
        parmW   hInst
        parmD   lpDeviceType
        parmD   lpOutputFile
;
cBegin  DeviceMode
mov     ax,-1                   ;Show success
cEnd    DeviceMode
;                                        
;

cProc	CreateBitmap, <FAR, PUBLIC, WIN, PASCAL, NODATA>

cBegin
	xor	ax, ax			; just return a zero in AX
cEnd

cProc	Spare2, <FAR, PUBLIC, WIN, PASCAL, NODATA>
cBegin
	sub	ax, ax
cEnd

cProc	Spare3, <FAR, PUBLIC, WIN, PASCAL, NODATA>
cBegin
	sub	ax, ax
cEnd

;cProc	 DeviceBitmapBits, <FAR, PUBLIC, WIN, PASCAL, NODATA>
;cBegin
;	 sub	 ax, ax
;cEnd
;
;Proc	DIBToScreen, <FAR, PUBLIC, WIN, PASCAL, NODATA>
;cBegin
;	 sub	 ax, ax
;cEnd

sEnd            Code
end
