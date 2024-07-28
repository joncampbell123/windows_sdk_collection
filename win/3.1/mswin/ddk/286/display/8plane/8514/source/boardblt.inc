	parmD	lpDstDev		;pointer to destination's PDEVICE
        parmW   DstxOrg                 ;X-origin on destination bitmap
        parmW   DstyOrg                 ;Y-origin on destination bitmap
        parmD   lpSrcDev                ;pointer to source's PDEVICE
        parmW   SrcxOrg                 ;X-origin on source bitmap
        parmW   SrcyOrg                 ;Y-origin on source bitmap
        parmW   xExt                    ;length in X
        parmW   yExt                    ;length in Y
        parmD   Rop                     ;tertiary raster operation code
        parmD   lpPBrush                ;pointer to physical brush
        parmD   lpDrawMode              ;pointer to DRAWMODE data structure

;These are exclusively for BoardBlt:

	localW	ParseStringOffset
	localW	ParseStringLength
	localW	LocDstyOrg
	localW	LocFreeSpaceyExt
	localW	DestType
	localW	SrcType
	localW	PatXOrg
	localW	PatYOrg
	localW	YIncr
	localW	YAdjust
	localB	BoardBltFlags
	localB	BackgroundColour
	localB	ForegroundColour
	localB	NotBrush
	localV	bbl_dst, %(size BITMAP) ;local copy of destination bmp header
	localV	bbl_src, %(size BITMAP) ;local copy of source bmp header

;These are for BlockWrite and ReadScreen:

	localD	WriteColor
	localD	ReadColor
        localW  SegmentAdder
        localW  ScanlinesLeftInSegment
        localW  StartingScanline
        localW  ScanlinesPerSegment
	localW	SrcxOrgByteOffset
	localW	wMono2ColorLUT
        localB  FirstByteShiftFactor
        localB  LoopShiftFactor
        localB  LoopCounter
	localB	SrcAlignedFlag
	localB	PaletteXlateFlag

;These are exclusively for ReadScreen:

	localW	wByteOffset
	localW	wByteCount
	localW	wBytesPerScanline
	localW	wSegmentIndex
	localB	bFirstByteKeep
	localB	bFirstByteErase
	localB	bFirstBytePixels
	localB	bFirstByteShift
	localB	bLastByteKeep
	localB	bLastByteShift
	localB	bLastBytePixels
	localD	ToRing0
	localD	ToRing3
