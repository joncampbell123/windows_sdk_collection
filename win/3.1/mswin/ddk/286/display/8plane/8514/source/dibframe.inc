	parmD	lpPdevice		; device dependent bmp
	parmW	wSetGet 		; 0 -> set, 1 -> get
	parmW	iStart			; first scan to get/set
	parmW	nNumScans		; the number of scans to process
	parmD	lpBits			; the DIBmp
	parmD	lpInfoBlock		; all the goodies about the DIBmp
	parmD	lpConversionInfo	;how to treat colors on a get bits

	localD	pDest			; pointer to our bmp
	localD	lpPalette
	localD	lpDeviceColorMatch
	localW	wTmpSrc
	localW	wTmpDst
	localW	wTranslate		; pointer to the pixels translate rout.
	localW	ColorTableFunction
	localW	nColors 		; number of colors in DIBmp
	localW	wDIByteScanline 	; # bytes/scanline of DIBmp
	localW	wBmByteScanline 	; # bytes/scan of int. Bmp
	localW	nDIBitsPixel		; number of bits per pixel of DIBmp
	localW	nWidth			; width of bmp to process
	localW	nLinesDone		; want to return this value
	localW	nFill
	localW	nSegIndex
	localW	oCount
	localW	wStackTop
	localW	pdwIndexToRGB
	localW	wTranslatePartial
	localB	iCount
	localB	bRem
	localB	bShiftCount
	localB	bMask			; Pixel mask
	localB	bLast
	localB	bType24 		; nonzero if 24 bit bmp involved
	localB	oRed
	localB	oGreen
	localV	bColorTable,256 	; color translation table
