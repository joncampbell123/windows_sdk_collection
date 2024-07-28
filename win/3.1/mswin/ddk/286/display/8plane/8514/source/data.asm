;This module contains ALL data objects used in the 8514 display driver

include cmacros.inc
include brush.inc
incDrawMode	equ	1		; enable drawing mode definitions
include gdidefs.inc
include drvpal.inc
include 8514.inc
include	debug.inc

externFP    GetExePtr			;imported from KERNEL
externFP    GetProcAddress		;imported from KERNEL

externA     __WinFlags			;imported from KERNEL

public	TempSelector
public  FreeSpaceyOrg
public  FreeSpaceyExt
public  FullScreenClip
public  InvisibleScreenClip
public  VisibleScreenClip
public	Rop2TranslateTable
public	WriteEnable
public	BitsPixel
public	TranslateTable
public	ITranslateTable
public	BrushCopy
public	PaletteFlags
public	NewDrawMode
public	PenCopy
public	WriteScreenColor
public	ReadScreenColor
public	PixelAddress
public	MemoryScanline
public	MemoryPolyline
public	TextOutFunction
public	SystemPaletteColors
public	HardwarePaletteRead
public	GetHardwareConfig

sBegin	Data
	even

	TempSelector	dw  0
	FreeSpaceyOrg	dw  Y_SIZE+64
	FreeSpaceyExt	dw  192
	FullScreenClip	dw  0,0,1024,1024
	InvisibleScreenClip	dw	0,768,1024,1024
	VisibleScreenClip	dw	0,0,1024,768

Rop2TranslateTable	label	byte
	db	01h	;Rop2=1     DDx
	db	0fh	;Rop2=2     DPon
	db	0eh	;Rop2=3     DPna
	db	04h	;Rop2=4     Pn
	db	0dh	;Rop2=5     PDna
	db	00h	;Rop2=6     Dn
	db	05h	;Rop2=7     DPx
	db	08h	;Rop2=8     DPan
	db	0ch	;Rop2=9     DPa
	db	06h	;Rop2=10    DPxn
	db	03h	;Rop2=11    D
	db	09h	;Rop2=12    DPno
	db	07h	;Rop2=13    P
	db	0ah	;Rop2=14    PDno
	db	0bh	;Rop2=15    DPo
	db	02h	;Rop2=16    DDxn

	WriteEnable	db	?
	BitsPixel	db	?
ife	GreyScale
.xlist
include Palette.dat
.list
endif

TranslateTable	    db	    256 dup(?)
ITranslateTable     db	    256 dup(?)
BrushCopy	    db	    (Size NewBrush) dup(?)
DriverInitialized   db	    0
HardwarePaletteRead db	    0
PaletteFlags	    db	    NOMEMSTRBLT
NewDrawMode	    db	    (Size DRAWMODE) dup (?)
PenCopy 	    dw	    ?
		    db	    ?
SystemPaletteColors db	    ?

WriteScreenColor    dd	    ?
ReadScreenColor     dd	    ?
PixelAddress	    dd	    ?
MemoryScanline	    dd	    ?
MemoryPolyline	    dd	    ?
TextOutFunction     dd	    ?
if 0
ProtectTable         label   word
        db      0ffh,0
        db      7fh,80h
        db      3fh,0c0h
        db      1fh,0e0h
        db      0fh,0f0h
        db      07h,0f8h
        db      03h,0fch
	db	01h,0feh
endif
public	Ring3ToRing0
Ring3ToRing0	dd	0
public	Ring0ToRing3
Ring0ToRing3	dd	0
sEnd	Data

createSeg       _INIT,InitSeg,word public,CODE
sBegin          InitSeg
assumes         cs,InitSeg
assumes 	ds,Data

externNP    LUTInit			;in TBINIT.ASM

DefaultPalLen	equ	PaletteLength-ExtraPalLength
DefaultPalCol	equ	NbrofColours-NbrofXColors

GET386API	equ	1684h		;Get API entry point from VxD.
MULTIPLEX	equ	2fh		;Multiplex interrupt.
VDD		equ	000Ah		;id of Virtual Display Driver.
RingAPI		equ	000Ah		;Ring Transition API of VDD.

;at the time GetHardwareConfig() is called, the 8514 has to be switched into
;hi-res mode.

cProc	GetHardwareConfig, <NEAR, PUBLIC>
	localW	ModuleHandle
	localD	lpfnVDD
	
cBegin
	cmp	[DriverInitialized], 0
	je	GHCInitialize
	jmp	GetHardwareConfigEnd

GHCInitialize:
	inc	[DriverInitialized]
	cCall	LUTInit 		;initialize translate table
	cCall	GetExePtr, <ds>
	mov	ModuleHandle, ax
	mov	bx, 240 		;real mode ExtTextOut module
	mov	dx, __WinFlags
if DEBUG_286
	mov	dx,3
endif
	test	dl, 01h 		;are we in protect mode?
	jz	GHCGetTextOutFunction
	test	dl, 02h 		;are we on a 286 processor?
	jnz	GHCGetTextOutFunction	;yes. Use the real-mode code.
	inc	bx			;make it protect mode ExtTextOut
GHCGetTextOutFunction:
	sub	di, di
	cCall	GetProcAddress, <ax, di, bx>	;call with handle, 0, ord. #
	mov	word ptr TextOutFunction, ax
	mov	word ptr TextOutFunction+2, dx

	push	ds
	pop	es
	assumes es, Data
	mov	cx, NbrofColours

	mov	dx, 042e8h		;dx=Subsys. Status register
	in	ax, dx			;al=hardware configuration
if DEBUG_4PLANE				
	mov	al,0			;Debug for testing 4plane driver.
endif	
	and	al, 080h		;4 plane or 8 plane config.?
	jz	SetFourPlaneDriver

	mov	[SystemPaletteColors], cl	;8 plane configuration
	mov	[WriteEnable], 0ffh	;set constants specific to 8 plane
	mov	[BitsPixel], 08h	;mode
	mov	si, 200 		;get all 8 plane specific modules
	jmp	short CommonFixup	;loaded

SetFourPlaneDriver:
	mov	dx, DefaultPalCol	;DX: 16
	mov	[SystemPaletteColors], dl   ;make it the # of system colors
	sub	cx, dx			;CX: was 20 and is now 4
	jcxz	PaletteDataMoved	;sanity check
	mov	bx, cx			;save # of entries to skip in BX
	mov	ax, dx			;AX: 16
	shr	dx, 1			;DX: 8
	add	dx, ax			;DX, 24
	mov	di, DataOFFSET Palette	;DS:SI-->palette
	add	di, dx			;ES:DI-->index 8 in system palette
	mov	ax, cx			;AX: 4
	shl	cx, 1			;we want to skip all the extra colors
	add	ax, cx			;AX: 12
	mov	si, di			;that may be in our palette, so we
	add	si, ax			;copy the default colors in place of
	mov	cx, dx			;the extra system colors  CX=24
rep	movsb				;shrink the physical palette

	mov	di, DataOFFSET ColorFlagTable
	mov	cx, DefaultPalCol
	shr	cx, 1			;CX: size of color flag table/2
	add	di, cx			;ES:DI-->8th entry in color flag table
	lea	si, [di][bx]		;DS:SI-->12th entry in color flag table
	shr	cx, 1			;move 2 entries at once
rep	movsw				;shrink the color flag table

PaletteDataMoved:
	mov	[WriteEnable], 00fh
	mov	[BitsPixel], 04h
	mov	si, 220
CommonFixup:
	mov	ax,__WinFlags
if DEBUG_286
	mov	ax,3
endif
	test	al, 01h 		;are we in protect mode?
	jz	short For286
	test	al, 02h 		;are we on a 286 processor?
	jnz	short For286		;yes. Use the real-mode code.
	call	Get386Procs
	jnc	short GetOtherProcs
For286:
	mov	di, ModuleHandle
	sub	ax, ax
	cCall	GetProcAddress, <di, ax, si>		;200,220
	mov	word ptr WriteScreenColor, ax
	mov	word ptr WriteScreenColor+2, dx
	inc	si
	sub	ax, ax
	cCall	GetProcAddress, <di, ax, si>		;201,221
	mov	word ptr ReadScreenColor, ax
	mov	word ptr ReadScreenColor+2, dx
	inc	si
GetOtherProcs:
	sub	ax, ax
	cCall	GetProcAddress, <di, ax, si>		;202,222
	mov	word ptr PixelAddress, ax
	mov	word ptr PixelAddress+2, dx
	inc	si
	sub	ax, ax
	cCall	GetProcAddress, <di, ax, si>		;203,223
	mov	word ptr MemoryScanline, ax
	mov	word ptr MemoryScanline+2, dx
	inc	si
	sub	ax, ax
	cCall	GetProcAddress, <di, ax, si>		;204,224
	mov	word ptr MemoryPolyline, ax
	mov	word ptr MemoryPolyline+2, dx
GetHardwareConfigEnd:
cEnd	GetHardwareConfig

Get386Procs	proc	near
;--------------------------------------------------------------------
;Call into the VDD and get the Ring3<-->Ring0 entry points.
;If the entry point does not exist, then fail (with carry) and
;old BitBlt code (w/o ring transition code) will be loaded instead.
;
;Entry:
; si: 200 or 220
;Exit:
; If (no carry)      //success 
;   si = 202 or 222
; else	             //fail
;   si = 200 or 220
;--------------------------------------------------------------------
	push	es
	add	si,5			;205 or 225
	xor	di,di
	mov	es,di
	mov	ax,GET386API
	mov	bx,VDD
	int	MULTIPLEX
	mov	ax,es
	or	ax,ax
	jz	short G3P_NoVDD		;will be zero if no VDD is present.

	mov	word ptr lpfnVDD,di
	mov	word ptr lpfnVDD+2,es


;--------------------------------------------------------------------
;Loads 386 specific procs with ring transition logic.
;--------------------------------------------------------------------
	mov	di, ModuleHandle
	sub	ax, ax
	cCall	GetProcAddress, <di, ax, si>	;205,225
	mov	word ptr WriteScreenColor, ax	;offset
	mov	word ptr WriteScreenColor+2, dx	;selector
	inc	si
	sub	ax, ax
	cCall	GetProcAddress, <di, ax, si>	;206,226
	mov	word ptr ReadScreenColor, ax
	mov	word ptr ReadScreenColor+2, dx
	inc	si				;207,227

;--------------------------------------------------------------------
;If we've made it this far, then we know the VDD is present. 
;Get the ring transition entry point from the VDD.  If this fails,
;then fail (with carry) and the old BitBlt code (w/o ring transition 
;code) will be loaded.
;--------------------------------------------------------------------
	xor	dx,dx
	xor	cx,cx
	mov	ax,RingAPI
	mov	cx,word ptr WriteScreenColor+2	;desired code segment.
	call	lpfnVDD
	sub	si,2				;Assume error exit.
	or	dx,dx
	jz	short G3P_NoVDD			;RingAPI not present.
	add	si,2
	mov	word ptr Ring3ToRing0,ax	;offset
	mov	word ptr Ring3ToRing0+2,dx	;selector
	mov	word ptr Ring0ToRing3,bx	;offset
	mov	word ptr Ring0ToRing3+2,cx	;selector
	sub	si,5				;202 or 225
	clc
	pop	es
	ret
G3P_NoVDD:
	sub	si,5				;200 or 220
	stc
	pop	es
	ret

Get386Procs	endp

sEnd	InitSeg
end
