;-----------------------------------------------------------------------------
;
;   Screen Grabber for IBM 8514/GAD adaptor 
;
;    INCLUDE FILE FOR 8514/GAD GRABBER
;
;    (C) Copyright MICROSOFT Corp. 1986-1990
;
;    ************* MICROSOFT CONFIDENTIAL ******************
;
;-----------------------------------------------------------------------------

IF1
    %out VGA MONO version
ENDIF

;
; Grabber version EQUs
;
GRB_VER_HIGH	EQU	3
GRB_VER_LOW	EQU	0

GrxBitWid640	equ	640	; Width of graphics bitmaps
GrxBitHeight400	equ	400	; Height of graphics bitmaps (200*2)
GrxBitHeight350	equ	350	; Height of graphics bitmaps (350)
GrxBitHeight480	equ	480	; Height of graphics bitmaps (480)

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
CntrlSt 	db	(SIZE VDA_EGA_State) dup (?)
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
        rgbResvd        db      0
MyRGBTriple     ends

;
; Structure for storing line numbers corresponding to various video pages
; Used in UpdateScreen
;
LineTabStruc    struc
        StartLine       dw      ?
        EndLine         dw      ?
LineTabStruc    ends

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

InitBasicFrame	MACRO
	mov	GrbFlags,0
	mov	VidSel,0
	ENDM

;
; Masks for GrbFlgs
;
GrbFlgs_GrfxDoOdd	        equ	0000000000000001B
GrbFlgs_GrfxDoOddBit	        equ	0
GrbFlgs_GrfxGrabBM	        equ	0000000000000010B
GrbFlgs_GrfxGrabBMBit	        equ	1
GrbFlgs_GrfxInvPal	        equ	0000000000000100B
GrbFlgs_GrfxInvPalBit	        equ	2
GrbFlgs_GrfxDispBlt 	        equ	0000000000001000B
GrbFlgs_GrfxDispBltBit	        equ	3
GrbFlgs_GrfxInvSelReComp        equ	0000000000010000B
GrbFlgs_GrfxInvSelReCompBit     equ	4
;
; Causes us to do Get_GrbMem instead of Get_Mem unless
;  GrbFlgs_DoingWindowSelGrab is set (doing a selection via VMDOSAPP
;  on a VM in a window)
;
GrbFlgs_DoingGrab		equ	0000000000100000B
GrbFlgs_DoingGrabBit		equ	5
GrbFlgs_DoingWindowSelGrab	equ	0000000001000000B
GrbFlgs_DoingWindowSelGrabBit	equ	6

GrbFlgs_StrOdd			equ	0000000010000000B
GrbFlgs_StrOddBit		equ	7
GrbFlgs_SecScns 		equ	0000000100000000B
GrbFlgs_SecScnsBit		equ	8
GrbFlgs_ExtrOdd 		equ	0000001000000000B
GrbFlgs_ExtrOddBit		equ	9
GrbFlgs_ExtrEvn 		equ	0000010000000000B
GrbFlgs_ExtrEvnBit		equ	10

;
; Define the graphics stack frame
;       Width of LineBits = (# of pels)*(# of bits/pel)*(# of planes)/8 bytes
;
SzGrxFrm	equ	CFrm+174+4+((GrxBitWid640*1*1)/8)

;**** Following six frame variables are declared above. They are at the same
;	location on both the graphics and text frames.
;
;;GrbFlags	equ	word ptr [bp-4] 
;;VidSel	equ	word ptr [bp-6]   ; Video memory selector
;;PgOffst 	equ	dword ptr [bp-10]       ; Offset in vid mem of start
;;VidAddr 	equ	fword ptr [bp-10]       ; Above as an fword
;;Gmode		equ	byte ptr [bp-12]        ; Storage for mode byte
;;MemState	equ	[bp-12-(SIZE VDA_Mem_State)]
BcolHigh	equ	word ptr [bp-CFrm-02]  ; High word of background RGB color
BcolLow 	equ	word ptr [bp-CFrm-04]  ; Low word of background RGB color
ScreenDC	equ	word ptr [bp-CFrm-06]  ; hMemDC for whole screen
ScreenBitMap	equ	word ptr [bp-CFrm-08]  ; hMemBitmap for whole screen
BkBrush 	equ	word ptr [bp-CFrm-10]  ; hBrush for background
IPlaneNum       equ     byte ptr [bp-CFrm-11]  
RPlaneNum       equ     byte ptr [bp-CFrm-12]
GPlaneNum       equ     byte ptr [bp-CFrm-13]
BPlaneNum       equ     byte ptr [bp-CFrm-14]
RightWid	equ	word ptr [bp-CFrm-16]  ; Width of "extra" on right
RightXpos	equ	word ptr [bp-CFrm-20]  ; X coord of "extra" on right
BottomHig	equ	word ptr [bp-CFrm-22]  ; Height of "extra" on bottom
BottomYpos	equ	word ptr [bp-CFrm-24]  ; Y coord of "extra" on bottom
DDRCLeft	equ	word ptr [bp-CFrm-26]  ; Left of paint rect rounded down DD
DDRCTop 	equ	word ptr [bp-CFrm-28]  ; Top of paint rect rounded down DD
DDRCRight	equ	word ptr [bp-CFrm-30]  ; Right of paint rect rounded up DD
DDRCWid 	equ	word ptr [bp-CFrm-32]  ; Width of Screen Bitmap
DDRCHig 	equ	word ptr [bp-CFrm-34]  ; Height of Screen Bitmap
DDPWid		equ	word ptr [bp-CFrm-36]  ; Width of screen Bitmap in Dwords
DDRCLeftBI	equ	word ptr [bp-CFrm-38]  ; Byte index of DDRCLeft
DBPWid		equ	word ptr [bp-CFrm-40]  ; Width of screen Bitmap in bytes
OvscnBrsh	equ	word ptr [bp-CFrm-42]  ; Brush in overscan color
LineSkip	equ	word ptr [bp-CFrm-44]  ; Bytes of line not in paint rect
BDsWid		equ	word ptr [bp-CFrm-46]  ; Bytes of line in display
VideoIndex      equ     dword ptr [bp-CFrm-50] ; ptr into Video mem
SavEXTP         equ     word ptr [bp-CFrm-52]  ; save area for ptr to EXTP
SavOldBr        equ     word ptr [bp-CFrm-54]  ; Old Br in hDC - direct Blt code only
;
; Offsets to the 4 planes in the video mem are stored only for Planar modes 
; D,E,10. .
;
IPlaneOffset    equ     dword ptr [bp-CFrm-58]  ; I Plane Offset - Plane 3
RPlaneOffset    equ     dword ptr [bp-CFrm-62]  ; R Plane Offset - Plane 2
GPlaneOffset    equ     dword ptr [bp-CFrm-66]  ; G Plane Offset - Plane 1
BPlaneOffset    equ     dword ptr [bp-CFrm-70]  ; B Plane Offset - Plane 0

PelColorTable   equ     [bp-CFrm-134]           ; 16 * 4 bytes pre color
DIBHdr          equ     [bp-CFrm-134-SIZE BitmapInfoHeader]  ; DIB Hdr
LineBits	equ	byte ptr [bp-SzGrxFrm]  ; One line of bits

;
; Define the text stack frame
;
SzTxtFrm equ	CFrm+32+84

;; **** Following six frame variables are declared above. They are at the same
;;	location on both the graphics and text frames.
;;
;GrbFlags	equ	word ptr [bp-4]         ;
;VidSel		equ	word ptr [bp-6]         ; Video memory selector
;PgOffst 	equ	dword ptr [bp-10]       ; Offset in vid mem of start
;VidAddr 	equ	fword ptr [bp-10]       ; Above as an fword
;Gmode		equ	byte ptr [bp-12]        ; Storage for mode byte
;MemState	equ	[bp-12-(SIZE VDA_Mem_State)]
StartBuf	equ	word ptr [bp-CFrm-02]	; Save for pointer to save area
BytesPerLine	equ	word ptr [bp-CFrm-04]	; Bytes per text line in vid mem
WidPaint	equ	word ptr [bp-CFrm-06]	; Width in chars of paint
HigPaint	equ	word ptr [bp-CFrm-08]	; Height in char lines of paint
BytesToRight	equ	word ptr [bp-CFrm-10]	; Index in vid buf to left char
PToutBuf	equ	word ptr [bp-CFrm-12]	; Pointer to text buffer
Currcols	equ	[bp-CFrm-14] 	        ; Current text colors set
BkTxtBrsh	equ	word ptr [bp-CFrm-16]	; Background brush
WidRgt		equ	word ptr [bp-CFrm-18]	; Width of extra on right
XRgt		equ	word ptr [bp-CFrm-20]	; X of extra
HigBot		equ	word ptr [bp-CFrm-22]	; Height of extra on Bottom
YBot		equ	word ptr [bp-CFrm-24]	; Y of extra
TLineXPos	equ	word ptr [bp-CFrm-26]	; X pos in line
OldBrshHand	equ	word ptr [bp-CFrm-28]	; Save for DC brush
TrueColor	equ	byte ptr [bp-CFrm-29]	; True color in selection invert
Unused		equ	byte ptr [bp-CFrm-30]
OldFontHand	equ	word ptr [bp-CFrm-32]	; Save for stock font
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


