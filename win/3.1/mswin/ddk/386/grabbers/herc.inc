;--------------------------------------------------------------------
;
;   Screen Grabber for HERCULES adaptor
;
;    INCLUDE FILE FOR HERCULES GRABBER
;
;    (C) Copyright MICROSOFT Corp. 1986-1990
;    (C) Copyright APRICOT Computers 1987
;
;
;-----------------------------------------------------------------------------

;
; Grabber version EQUs
;
GRB_VER_HIGH	EQU	3
GRB_VER_LOW	EQU	0ah

GrxBitWid	equ	720	; Width of graphics bitmaps
GrxBitHeight	equ	348	; Height of graphics bitmaps

;
; Definition of layout for GrabArea in selection structure
;	See GrabArea in Selection.inc
;
MySelectStruc	struc
	StartPointX	DW	?	; Start X screen coord (fixed end point)
	StartPointY	DW	?	; Start Y screen coord (fixed end point)
	EndPointX	DW	?	; End X screen coord (active end point)
	EndPointY	DW	?	; End Y screen coord (active end point)
	Irect		DB	(size RECT) dup(?) ; Selection rect
	SelecFlags	DW	?	; Various flags
MySelectStruc	ends

;
; Bit defs for SelecFlags flag word
;
SelectOn	equ	0000000000000001B

IF2
    IF (SIZE MySelectStruc) GT (SIZE GrabArea)
	%out ******* ERROR !!!!! GrabArea Overflowed!!!!!
	Syntax error
    ENDIF
ENDIF

;
; Template for accessing data at EPGrabDTA in EXTPAINTSTRUC. 
; This piece of data is managed by the grabber only.
; 
;
GrbData STRUC
CntrlSt 	db	(SIZE VDA_HERC_State) dup (?)
CurCursBeg	dw	?
CurCursEnd	dw	?
CurCursMode	db	?

DispMod         db      SIZE VDD_MOD_STATE dup (?)
GrbData ENDS

IF2
    IF (SIZE GrbData) GT (SIZE EPGrabDTA)
	%out ****** ERROR !!!!!!! EPGrabDTA overflowed!!!!!!
	Syntax error
    ENDIF
ENDIF

;
; Template for accessing data Global Grabber Area, GGrbArea in EXTPAINTSTRUC
; GLOBALGRBSIZEMAX(=12) bytes long
; Data stored in this area(e.g. VddApiProcAdr got in GetGrbVersion) is 
; propagated to future instances of the grabber.
; This should not be combined with EPGrabDTA
;
GGrbAreaStruc   struc
        VddApiProcAdr      dd  ?
; 8 unused bytes
GGrbAreaStruc   ends
        
;
; DIB related structure - should be consistent with the definition in GDI
;
BitmapInfoHeader    struc
  biSize            dd  0
  biWidth           dd  0
  biHeight          dd  0
  biPlanes          dw  0
  biBitCount        dw  0

  biCompression     dd  0
  biSizeImage       dd  0
  biXPelsPerMeter   dd  0
  biYPelsPerMeter   dd  0
  biClrUsed         dd  0
  biClrImportant    dd  0
BitmapInfoHeader    ends
;
; RGBTriple structure for PelColorTable entries in DIBs
;
MyRGBTriple     struc
        rgbBlue         db      ?
        rgbGreen        db      ?
        rgbRed          db      ?
        rgbResvd     db      0
MyRGBTriple     ends

;
; Masks for GrbFlags
; Note: GrxFlags_Intrlv MUST be in bits 0 and 1!  (ModeGrfx depends on it.)
;
GrbFlags_GrfxIntrlv	equ	0000000000000011B	; two LSB indicate scan line
						; currently in process
GrbFlags_GrfxGrabBM	equ	0000000000000100B	; display format bitmap grab
GrbFlags_GrfxGrabBMBit	equ	2

GrbFlags_GrfxRevCol	equ	0000000000001000B	; Bitmap is Black on White
GrbFlags_GrfxRevColBit	equ	3

;
; Causes us to do Get_GrbMem instead of Get_Mem unless
;  GrbFlags_DoingWindowSelGrab is set (doing a selection via VMDOSAPP
;  on a VM in a window)
;
GrbFlags_DoingGrab		equ	0000000000100000B
GrbFlags_DoingGrabBit		equ	5
GrbFlags_DoingWindowSelGrab	equ	0000000001000000B
GrbFlags_DoingWindowSelGrabBit	equ	6

InitBasicFrame	MACRO
	mov	GrbFlags,0
	mov	VidSel,0
	ENDM

;
; Common frame variables
;
GrbFlags	equ	word ptr [bp-4]   
VidSel		equ	word ptr [bp-6]   ; Video memory selector
PgOffst 	equ	dword ptr [bp-10] ; Offset in vid mem of start
VidAddr 	equ	fword ptr [bp-10] ; Above as an fword
Gmode		equ	byte ptr [bp-12]  ; Storage for mode byte
MemState	equ	byte ptr [bp-12-(SIZE VDA_Mem_State)]

CFrm		equ	(12+(SIZE VDA_Mem_State))

;
; Define the graphics stack frame
;
SzGrxFrm	equ	CFrm+80+4+(GrxBitWid/8)

;; **** Following six frame variables are declared above. They are at the same
;;	location on both the graphics and text frames.
;;
;GrbFlags	equ	word ptr [bp-4]   
;VidSel		equ	word ptr [bp-6]   ; Video memory selector
;PgOffst 	equ	dword ptr [bp-10] ; Offset in vid mem of start
;VidAddr 	equ	fword ptr [bp-10] ; Above as an fword
;Gmode		equ	byte ptr [bp-12]  ; Storage for mode byte
;MemState	equ	byte ptr [bp-12-(SIZE VDA_Mem_State)]
ScreenDC	equ	word ptr [bp-CFrm-02]  ; hMemDC for whole screen
ScreenBitMap	equ	word ptr [bp-CFrm-04]  ; hMemBitmap for whole screen
BkBrush 	equ	word ptr [bp-CFrm-06]  ; hBrush for background
RightWid	equ	word ptr [bp-CFrm-08]  ; Width of "extra" on right
RightXpos	equ	word ptr [bp-CFrm-10]  ; X coord of "extra" on right
BottomHig	equ	word ptr [bp-CFrm-12]  ; Height of "extra" on bottom
BottomYpos	equ	word ptr [bp-CFrm-14]  ; Y coord of "extra" on bottom
DDRCLeft	equ	word ptr [bp-CFrm-16]  ; Left of paint rect rounded down DD
DDRCTop 	equ	word ptr [bp-CFrm-18]  ; Left of paint rect rounded down DD
DDRCRight	equ	word ptr [bp-CFrm-20]  ; Right of paint rect rounded up DD
DDRCWid 	equ	word ptr [bp-CFrm-22]  ; Width of Screen Bitmap
DDRCHig 	equ	word ptr [bp-CFrm-24]  ; Height of Screen Bitmap
DDPWid		equ	word ptr [bp-CFrm-26]  ; Width of screen Bitmap in Dwords
DDRCLeftBI	equ	word ptr [bp-CFrm-28]  ; Byte index of DDRCLeft
OvscnBrsh	equ	word ptr [bp-CFrm-30]  ; Brush in overscan color
SavEXTP         equ     word ptr [bp-CFrm-32]  ; save area for ptr to EXTP

PelColorTable   equ     [bp-CFrm-40]           ; mono - 2 colors = 8 bytes
DIBHdr          equ     [bp-CFrm-40-SIZE BitmapInfoHeader]  

LineBits	equ	byte ptr [bp-SzGrxFrm] ; One line of bits

;
; Define the text stack frame
;
SzTxtFrm equ	CFrm+30+4+84

;; **** Following six frame variables are declared above. They are at the same
;;	location on both the graphics and text frames.
;
;GrbFlags	equ	word ptr [bp-4]   
;VidSel		equ	word ptr [bp-6]   ; Video memory selector
;PgOffst 	equ	dword ptr [bp-10] ; Offset in vid mem of start
;VidAddr 	equ	fword ptr [bp-10] ; Above as an fword
;Gmode		equ	byte ptr [bp-12]  ; Storage for mode byte
;MemState	equ	byte ptr [bp-12-(SIZE VDA_Mem_State)]
StartBuf	equ	word ptr [bp-CFrm-02]	; Save for pointer to save area
BytesPerLine	equ	word ptr [bp-CFrm-04]	; Bytes per text line in vid mem
WidPaint	equ	word ptr [bp-CFrm-06]	; Width in chars of paint
HigPaint	equ	word ptr [bp-CFrm-08]	; Height in char lines of paint
BytesToRight	equ	word ptr [bp-CFrm-10]	; Index in vid buf to left char
PToutBuf	equ	word ptr [bp-CFrm-12]	; Pointer to text buffer
BkTxtBrsh	equ	word ptr [bp-CFrm-14]	; Background brush
WidRgt		equ	word ptr [bp-CFrm-16]	; Width of extra on right
XRgt		equ	word ptr [bp-CFrm-18]	; X of extra
HigBot		equ	word ptr [bp-CFrm-20]	; Height of extra on Bottom
YBot		equ	word ptr [bp-CFrm-22]	; Y of extra
TLineXPos	equ	word ptr [bp-CFrm-24]	; X pos in line
OldBrshHand	equ	word ptr [bp-CFrm-26]	; Save for DC brush
OldFontHand	equ	word ptr [bp-CFrm-28]	; Save for stock font
Currcols	equ	byte ptr [bp-CFrm-30]	; Current text colors set
ToutBuf 	equ	byte ptr [bp-SzTxtFrm]	; Text buffer

;
; Masks for Currcols
; WARNING!!!! DO NOT CHANGE WITHOUT LOOKING AT TxtColMapTable!!!!
;
FgTxtCol	equ	00000001B	; 1 = white, 0 = Black
FgTxtColBit	equ	0
BkTxtCol	equ	00000010B	; 1 = white, 0 = Black
BkTxtColBit	equ	1

StdGrbFrameSz	equ	SzGrxFrm

.erre	SzGrxFrm GE SzTxtFrm
