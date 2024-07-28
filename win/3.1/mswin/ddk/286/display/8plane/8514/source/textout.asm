.286
.xlist
include cmacros.inc
include 8514.inc
.list

public  OldFaceName
public	OldFaceNameLength
public  OldFontHdr
public  SystemFontLoaded
public	SystemFontPointSize
public	SystemFontEndPlane
public	FontFreeSpaceXOrg
public	FontFreeSpaceYOrg
public	BigOldFaceName
public	BigOldFaceNameLength
public  BigOldFontHdr

public  SystemFontyExt
public  FontInfoTable
public	FontInfoTableLength
public	SystemFontInfoTable
public	BigFontInfoTable
public	BigFontXOrg
public	BigFontLRU
public	BigFontWritePlane
public	bLines
public  BigCacheDisabled

sBegin	Data
externD TextOutFunction 			;fixed up in DATA.ASM
OldFaceName		db	32 dup(0)	;32 bytes should be sufficient
BigOldFaceName		db	32 dup(0)
OldFontHdr		db	32 dup(0)
BigOldFontHdr		db	32 dup(0)
OldFaceNameLength	dw	0
BigOldFaceNameLength	dw	0
;OldFontSize		dw	0
;OldFontHeight		dw	0
;BigOldFontSize		dw	0
;BigOldFontHeight	dw	0
SystemFontPointSize	dw	0
SystemFontEndPlane	db	1
FontFreeSpaceYOrg	db	0		;is measured from top of invsbl
FontFreeSpaceXOrg	dw	0

SystemFontyExt		dw	0
SystemFontLoaded	db	0		;set by LoadFont

;The FontInfoTable is a local pointer into our fonts that we cache on the
;board.  The first byte of each 5 byte entry is the width in pixels of the
;font.  The second word length entry is the X-coordinate of the start of
;this character.  The third word lengthed entry is the Y-coordinate of the
;start of this character.  We have a special one for the system font since
;it's kept resident at all times.

bLines				db	0   ;put here to force word alignment

FontInfoTable			db	256 dup(0,0,0,0,0,0)
FontInfoTableLength		equ	($-DataOFFSET FontInfoTable) / 2
SystemFontInfoTable		db	224 dup(0,0,0,0,0,0)
BigFontInfoTable		db	256 dup(0,0,0,0,0,0)
BigFontXOrg			dw	0
BigFontLRU			db	4 dup(0)
BigFontWritePlane		dw	0

BigCacheDisabled	db	0	;See PGETFONT.ASM.
					;0=Not disabled, 1=Disabled.
sEnd	Data

sBegin	Code
assumes cs,Code
assumes ds,Data

cProc	ExtTextOut, <FAR,PUBLIC,WIN,PASCAL>, <si, di>

	include strblt.inc

cBegin
	jmp	dword ptr [TextOutFunction]
cEnd	<nogen> 			;don't generate any exit code either


subttl          StrBlt Entry Point
page +
cProc           StrBlt,<FAR,PUBLIC>

        parmD   lpDstDev
        parmW   DstxOrg
        parmW   DstyOrg
        parmD   lpClipRect
        parmD   lpString
        parmW   Count
        parmD   lpFont
	parmD	lpDrawMode	; includes background mode and bkColor
        parmD   lpTextForm         

cBegin  <nogen>                         ;don't fool with the stack until
                                        ;we get to ExtTextOut

;First, we must save our caller's far return address. Use CX & BX for this.

	assumes ds,Data
	pop	cx
	pop	bx

;Now dummy up null parameters for the extra parameters needed by ExtTextOut:

	xor	ax,ax
	push	ax			;push a dword for lpCharWidths
	push	ax
	push	ax			;push a dword for lpOpaqueRect
	push	ax
	push	ax			;push a word for Options
	push	bx			;push the caller's return address
	push	cx
	jmp	ExtTextOut		;now go do the StrBlt using ExtTextOut!
cEnd    <nogen>                         ;don't generate exit code either

sEnd	Code
end
