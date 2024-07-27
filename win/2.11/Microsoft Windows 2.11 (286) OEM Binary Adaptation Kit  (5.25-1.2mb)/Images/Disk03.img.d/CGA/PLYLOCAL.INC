
	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: plylocal.inc
;
; Brief Description: Local variables for polyline routines.
;
; Created: 3/31/87
; Author: **** ******
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
;-----------------------------------------------------------------------;


	parmD	lp_dst_dev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lp_points		;--> to a set of points
	parmD	lp_phys_pen		;--> to physical pen
	parmD	lp_phys_brush		;--> to physical brush
	parmD	lp_draw_mode		;--> to a Drawing mode
	parmD	lp_clip_rect		;--> to a clipping rectange if <> 0


	localB	LineStyle		; what it says
	localB	FillByte		; necessary ???
	localW	AddVertStep		; stepping constant, up or down
	localW	SubVertStep		; minus AddVertStep
	localW	DDAstandardProcTable	; address of dda_solid or dda_styled
	localW	DDAsimpleProcTable	; dda_solid_final or dda_styled_final
	localW	DDAcurrentProcTable	; either standard or simple table.
	localW	HFirst			; length of first run.
	localW	HLast			; length of final run.
	localW	DDAcount		; number of intermediate runs
	localW	DDAtmpcount		; temp storage for above number.
	localW	ErrAdj			; DDA error adjustment
	localW	ErrReset		; DDA error reset
	localW	BitCount		; number of bits to write
	localW	StyledProc		; address of styled output procedure.
	localW	BitmapProc		; address of bitmap output procedure.
	localW	OverflowProc		; address of segment overflow proc.

	localW	XMajorDist
	localW	YMajorDist
	localW	XMinorDist
	localW	YMinorDist
	localW	Hypot
	localW	StyleLength		; error term for style DDA.
	localW	TmpStyleLength		; error term reset value.
	localW	StyleLengthReset	; errot term reset value.
	localW	StyleFlags		; styled line misc flags.
	localW	TmpStyleFlags		; styled flags reset value.
	localW	StyleFlagsReset		; reset value for above flags.
	localB	SingleFlag		; single pass flag.
	localB	PenANDFlag		; AND flags for the pen.
	localB	PenXORFlag		; XOR flags for the pen.
	localB	DataROTFlag		; data rotation flag.
	localW	CaseFlags		; directional case flags.
	localD	CurByte 		; address of current point
	localW	TmpCurByte		; current byte offset reset number.
	localW	TmpSegment		; current segment reset value.
	localW	BitmapOffset		; offset of start of bitmap.
	localW	BitmapSegment		; segment of start of bitmap.
	localW	DrawModeIndex		; drawing Mode index
	localW	NextScan		; index to next scan of a plane
	localW	NextScanXor		; interlace wrap/xor mask
ifdef	IBM_CGA
	localW	NextScanSub		; index to next scan for SubVertStep
	localW	NextScanXorSub		; xor mask for SubVertStep
endif
	localW	SegmentNum 		; segment counter for huge bitmaps
	localW	ScansSeg		; number of scans per 64K segment
	localW	SegIndex		; index to next segment
	localW	FillBytes		; number of fill bytes per segment
	localB	RotBitMask		; rotating-bit mask, current bit
	localB	TmpBitMask		; temp storage for above.
	localB	DeviceFlags		; some flags
	localB	BackMode		; backGround mode
	localB	OVDetected		; overflow detected by line code
	localW	WidthAllPlanes		; #Planes * WidthBytes for huge bitmaps

	localB	TmpColor		; temporary color storage.
	localB	CurStyle		; current line style.
	localB	CurPlane		; current plane mask
	localV	BackColor,4		; background color
	localV	CurPen,4		; current Pen

	localB	NeedExclusion		; flag: 0 --> no, ~0 --> yes

ifdef	HERCULES
	localW	TmpAddVertStep
endif
ifdef	IBM_CGA
	localW	TmpAddVertStep
	localW	TmpSubVertStep
endif
